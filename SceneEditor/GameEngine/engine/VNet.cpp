//VNet.cpp

#include "VNet.h"
#include "VNetTCPLink.h"
#include "string.h"

#include "VComDataRec.h"
#ifdef _MZPhone
// 独占网络连接资源
//static VLock mz_tcp_lock;
#endif

struct TNetLinkShare{
private:
    VLock                lock;
    long                 ShareCount;
    bool                 isCanceldLink;
    virtual long changeShare(int dValue){
        VAutoLock _autoLock(lock);
        {
            ShareCount+=dValue;
            return ShareCount;
        }
    }
public:
    INetLink*	netLink;
    TNetLinkShare(INetLink*	_netLink):ShareCount(0),isCanceldLink(false),netLink(_netLink){ 
        assert(_netLink!=0);
    }   
    virtual TNetLinkShare* newShare(){
        changeShare(1);
        return this;
    }
    virtual long decShare(){
        return changeShare(-1);
    }
    static bool tryDeleteShare(TNetLinkShare* share){
        bool isDel=(share->changeShare(0)==0);
        if (isDel)
            delete share; //堆中创建
        return isDel;
    }
    static void canceldLink(TNetLinkShare* share){
        share->netLink->setOnEventCallBackProc(0,0);
        share->isCanceldLink=true;
    }
    static bool getIsCanceldLink(TNetLinkShare* share){
        return share->isCanceldLink;
    }
    
protected:
    ~TNetLinkShare(){
        delete netLink;
    }
};


//线程安全的数据表
class TDataList{
public:
    struct TData:public TNetData{
        inline void clearData(){
            if (data!=0){
                delete []data;
                data=0;
                dataLength=0;
            }
        }
    };
    
private:
    Deque<TData>	m_list;
    VLock			m_lock;
public:
    long size(){
        long result=0;
        VAutoLock _autoLock(m_lock);
        {
            result= (long)m_list.size();
        }
        return result;
    }
    
    void push_back(const TData& new_data){
        VAutoLock _autoLock(m_lock);
        {
            m_list.push_back(new_data);
        }
    }
    
    void pop_front(TNetData& data){
        pop_front(data.data,data.dataLength);
    }
    void pop_front(UInt8*& data,long& dataLength){
        VAutoLock _autoLock(m_lock);
        {
            if (m_list.size()>0){
                TData front_data=m_list.front();
                m_list.pop_front();
                
                data=front_data.data;
                dataLength=front_data.dataLength;
            }else{
                data=0;
                dataLength=0;
            }
        }
    }
    
    void clear(){
        VAutoLock _autoLock(m_lock);
        {
            for (long i=0;i<(long)m_list.size();++i){
                m_list[i].clearData();
            }
            m_list.clear();
        }
    }
    ~TDataList(){
        clear();
    }
};

struct TDataAuto:public TDataList::TData{
    inline void resize(long _dataLength){
        if (dataLength!=_dataLength){
            clearData();
            data=new UInt8[_dataLength];
            dataLength=_dataLength;		
        }
    }
    inline void copyDataAddHead(const UInt8* _data,long _dataLength){
        resize(_dataLength+VNet::comHeadSize);
        dataLength=_dataLength;		
        memcpy(&data[VNet::comHeadSize],_data,_dataLength);
    }
    inline void swap(TDataList::TData& ndata){
        STD::swap(data,ndata.data);
        STD::swap(dataLength,ndata.dataLength);
    }
    inline ~TDataAuto(){ clearData(); }
};

//读线程
class TReadThread:public VThread{
private:
    TNetLinkShare*	m_netLinkShare;
    TDataList		m_list;
    volatile bool   m_isReadError;
    
    bool doRead(){
        if (!m_netLinkShare->netLink->getIsReceivedData()){
            sleep_s(0.001);
            return true;
        }
        else
            sleep_s(0);
        
        UInt8 headData[VNet::comHeadSize];
        if (!m_netLinkShare->netLink->readData(headData,VNet::comHeadSize)) return false;
        if (!VNet::checkHeadData(headData)) return false;
        long dataLength=VNet::headDataToComDataLength(headData);
        if ((dataLength<=0)||(dataLength>VNet::getMaxComDataLength())) return false;
        
        TDataAuto newDataAuto;
        newDataAuto.resize(dataLength);
        if (!m_netLinkShare->netLink->readData(newDataAuto.data,dataLength)) return false;
        
        TDataList::TData newData;
        newDataAuto.swap(newData);
        m_list.push_back(newData);
        return true;
    }
public:
    explicit TReadThread(TNetLinkShare* netLinkShare):m_isReadError(false),m_netLinkShare(netLinkShare->newShare()){}
    inline TDataList& getDataList(){ return m_list; }
protected:
    virtual void run(){
        m_isReadError=false;
        while (!getIsWantExit()){
            if (m_isReadError)
                sleep_s(0.01); //只等着退出了
            else
                m_isReadError=!doRead();
        }	
        m_netLinkShare->decShare();
        m_netLinkShare=0;		
    }
public:
    virtual bool getIsReadError()const{
        return m_isReadError;
    }
};

//写线程
class TWriteThread:public VThread{
private:
    TNetLinkShare*	m_netLinkShare;
    TDataList		m_list;
    volatile bool   m_isWriteError;
    bool doWrite(){
        TDataList::TData data;
        m_list.pop_front(data);
        if (data.data==0){
            sleep_s(0.01);
            return true;
        }else
            sleep_s(0);
        
        long sendDataLength=data.dataLength+VNet::comHeadSize;
        if (sendDataLength>VNet::getMaxComDataLength()) return false;
        
        TDataAuto dataAuto;
        dataAuto.swap(data);
        if (!VNet::createHead(dataAuto.data,dataAuto.dataLength)) return false;
        if (!m_netLinkShare->netLink->sendData(dataAuto.data,sendDataLength)) return false;
        return true;
    }
public:
    explicit TWriteThread(TNetLinkShare* netLinkShare):m_isWriteError(false),m_netLinkShare(netLinkShare->newShare()){}
    inline TDataList& getDataList(){ return m_list; }
protected:
    virtual void run(){	
        m_isWriteError=false;
        while (!getIsWantExit()){
            if (m_isWriteError)
                sleep_s(0.01);//只等着退出了
            else
                m_isWriteError=!doWrite();
        }	
        m_netLinkShare->decShare();
        m_netLinkShare=0;		
    }
public:
    virtual bool getIsWriteError()const{
        return m_isWriteError;
    }
};


//连接线程
class TConnectThread:public VThread{
private:
    TNetLinkShare*	m_netLinkShare;
    String			m_hostName;
    long			m_port;
    bool            m_isConnect;
    bool doConnect(){
        if (TNetLinkShare::getIsCanceldLink(m_netLinkShare)) return false;
        bool result=m_netLinkShare->netLink->connect(m_hostName.c_str(),m_port);
        m_isConnect=true;
        
#ifdef DEBUG_PACKDATA
        VComDataRec::recConnectData(m_hostName.c_str(),m_port);
#endif
        
        
        return result;	
    }
protected:
    virtual void run(){
#ifdef _MZPhone
        //VAutoLock autoLock(mz_tcp_lock);
#endif
        
        if (!getIsWantExit())
            doConnect();
        m_netLinkShare->decShare();
        
        while (!getIsWantExit()){
            sleep_s(0.1);
        }	
        m_netLinkShare->netLink->close();
        
        while (!TNetLinkShare::tryDeleteShare(m_netLinkShare)){
            sleep_s(0.1);//等待
        }	
        m_netLinkShare=0;
    }
public:
    explicit TConnectThread(TNetLinkShare* netLinkShare,const char* hostName,long port)
    :m_netLinkShare(netLinkShare->newShare()),m_hostName(hostName),m_port(port),m_isConnect(false){}
    virtual bool getIsConnect()const{
        return m_isConnect;
    }
};



void VNet::setNetLinkState(TNetLinkState netLinkState) { 
    if (netLinkState==m_linkState) return;
    m_linkState=netLinkState; 
    
    if (m_connectThread!=0){
        if (((TConnectThread*)m_connectThread)->getIsConnect()){
            VThread::tryExitThread(m_connectThread);
        }
    }
    switch (netLinkState){ 
        case nls_UnLinked:{
            if (m_netIOListener!=0) 
                m_netIOListener->disposeEventNetIOUnLinked(this);
            close();
        }  break;
        case nls_Linked:{
            assert(m_readThread!=0);
            assert(m_writeThread!=0);
            
            m_readThread->start();
            m_writeThread->start();	
            if (m_netIOListener!=0) 
                m_netIOListener->disposeEventNetIOLinked(this);	
        } break;
        case nls_Error:{
            if (m_netIOListener!=0) 
                m_netIOListener->disposeEventNetIOError(this);	
            //close();
        }break;
        case nls_ReadError:{
            if (m_netIOListener!=0) 
                m_netIOListener->disposeEventNetIOReadError(this);	
            //close();
        }break;
        case nls_WriteError:{
            if (m_netIOListener!=0) 
                m_netIOListener->disposeEventNetIOWriteError(this);	
            //close();
        }break;
        default:
            break;
    }
}

VNet::VNet()
:m_Import(0),m_readThread(0),m_writeThread(0),m_connectThread(0),
m_linkState(nls_UnLinked),m_netIOListener(0){
}

class VNet_:public VNet{
public:
    must_inline void toSetNetLinkState(TNetLinkState netLinkState){
        VNet::setNetLinkState(netLinkState);
    }	
public:
    static must_inline void setNetLinkState_(VNet* net,TNetLinkState netLinkState){
        ((VNet_*)net)->toSetNetLinkState(netLinkState);
    }
};

static void netLinkEventCallBack(TNetLinkImportHandle sender,void* userData,TNetLinkEventType netLinkEventType,long netLinkEventData){
    VNet* net=(VNet_*)userData;
    if (netLinkEventType==net_Linked)
        VNet_::setNetLinkState_(net,nls_Linked);
    else if  (netLinkEventType==net_Error)
        VNet_::setNetLinkState_(net,nls_Error);
    else if (netLinkEventType==net_UnLinked)
        VNet_::setNetLinkState_(net,nls_UnLinked);
}

void VNet::initNet(INetLink* netLink){
    if (netLink==0)
        netLink=new VNetTCPLink();//默认创建TCP链接
    TNetLinkShare* netLinkShare=new TNetLinkShare(netLink);
    netLinkShare->netLink->setOnEventCallBackProc(netLinkEventCallBack,this);
    assert(m_Import==0);
    m_Import=netLinkShare->newShare();
}

VNet::~VNet(){
    close();
}

void VNet::connect(const char* hostName,long port,INetLink* netLinkType){
    close();
    initNet(netLinkType);
    TNetLinkShare* netLinkShare=(TNetLinkShare*)m_Import;
    
    m_linkState=nls_Connecting;
    m_readThread=new TReadThread(netLinkShare);
    m_writeThread=new TWriteThread(netLinkShare);
    m_connectThread=new TConnectThread(netLinkShare,hostName,port);	
    m_connectThread->start();
}
void VNet::close(){
    
    TNetLinkShare* netLinkShare=(TNetLinkShare*)m_Import;	
    m_Import=0;
    
    bool isDeleteLinkSelf=true;
    if (m_connectThread!=0){
        isDeleteLinkSelf=false;
        VThread::tryExitThread(m_connectThread);
    }
    m_linkState=nls_UnLinked;
    
    
    VThread::tryExitThread(m_readThread);
    VThread::tryExitThread(m_writeThread);
    
    if (netLinkShare!=0){
        TNetLinkShare::canceldLink(netLinkShare);
        netLinkShare->decShare();
        if (isDeleteLinkSelf)
            TNetLinkShare::tryDeleteShare(netLinkShare);
        netLinkShare=0;
    }
}

void VNet::sendData(const UInt8* data,long length){
    if (m_writeThread!=0){
        TDataAuto newDataAuto;
        newDataAuto.copyDataAddHead(data, length);
        
        TDataList::TData newData;
        newDataAuto.swap(newData);
        ((TWriteThread*)m_writeThread)->getDataList().push_back(newData);
    }
}

bool VNet::receivedData(TNetData& netData){
    if (m_readThread==0) return false;
    ((TReadThread*)m_readThread)->getDataList().pop_front(netData);
    bool result=netData.data!=0;
    if (!result){
        bool isReadError= ((TReadThread*)m_readThread)->getIsReadError();
        bool isWriteError= ((TWriteThread*)m_writeThread)->getIsWriteError();
        if (isReadError)
            this->setNetLinkState(nls_ReadError);
        if (isWriteError)
            this->setNetLinkState(nls_WriteError);
        if (isReadError||isWriteError)
            this->setNetLinkState(nls_UnLinked);
    }
    return result;
}

void VNet::releaseReceivedData(TNetData& netData){
    ((TDataList::TData*)&netData)->clearData();
}

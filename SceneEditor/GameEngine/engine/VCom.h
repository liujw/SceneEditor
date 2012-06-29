//
//  VCom.h
//
//  Created by housisong on 08-5-6.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VCom_h_
#define _VCom_h_

#include "INetIO.h"

class ClientPackProtocol{
protected:	
    int     m_packID;
    bool    m_isUseGBK;
    inline unsigned char getAPakID(){
        int result=m_packID;
        ++m_packID;
        const int MaxPackID =255;
        if (m_packID> MaxPackID) initPakID();
        return (unsigned char)result;
    }
public:
    ClientPackProtocol(bool isUseGBK):m_isUseGBK(isUseGBK){ initPakID(); }
    typedef Vector<unsigned char> TDataBuf;
    void packComData(const StringList& datas,TDataBuf& out_dataBuf,long& out_datalength);
    bool unPackComData(const unsigned char* data,long length,StringList& out_datas);
    inline void initPakID(){
        const long MinPackID=1;
        m_packID=MinPackID; 
    }
};


class VCom;

class IComListener{
public:
    virtual ~IComListener() {}
    virtual void disposeEventComLinked(VCom* sender){}
    virtual void disposeEventComUnLinked(VCom* sender){}
    virtual void disposeEventComError(VCom* sender){}
    virtual void disposeEventComReadError(VCom* sender){}
    virtual void disposeEventComWriteError(VCom* sender){}
    virtual void disposeEventComReadDataError(VCom* sender){}
    
    virtual void disposeEventComReadedData(VCom* sender,const StringList& datas){}
};

//通讯解析
class VCom:private INetIOListener{
public:
    typedef ClientPackProtocol::TDataBuf TDataBuf;
protected:
    INetIO*				m_netIO;
    INetIOFactory*		m_netIOFactory;
    TDataBuf			m_tmp_dataBuf;
    ClientPackProtocol	m_packProtocol;
    IComListener*		m_comListener;
    Deque<StringList> m_readedDataList;
protected:
    //INetIOListener
    virtual void disposeEventNetIOLinked(INetIO* sender)	{
        if (m_comListener!=0) m_comListener->disposeEventComLinked(this); 
    }
    virtual void disposeEventNetIOUnLinked(INetIO* sender)	{ 
        if (m_comListener!=0) m_comListener->disposeEventComUnLinked(this); 
    }
    virtual void disposeEventNetIOError(INetIO* sender)		{
        if (m_comListener!=0) m_comListener->disposeEventComError(this); 
    }
    virtual void disposeEventNetIOReadError(INetIO* sender)	{ 
        if (m_comListener!=0) m_comListener->disposeEventComReadError(this); 
    }
    virtual void disposeEventNetIOWriteError(INetIO* sender){ 
        if (m_comListener!=0) m_comListener->disposeEventComWriteError(this); 
    }
    //
    virtual void readDataErrorEvent() { if (m_comListener!=0) m_comListener->disposeEventComReadDataError(this); }
    virtual void readedDataEvent(const StringList& datas) { if (m_comListener!=0) m_comListener->disposeEventComReadedData(this,datas); }
public:
    explicit VCom(INetIOFactory* netIOFactory);
    virtual ~VCom();
    
    TNetLinkState getNetLinkState()const { return m_netIO->getNetLinkState(); }
    void connect(const char* hostName,long port) { m_packProtocol.initPakID(); m_netIO->connect(hostName, port); }
    void close() { m_netIO->close(); }
    void sendData(const StringList& datas);
    bool readData(StringList& datas);
    void setComListener(IComListener* comListener) { 
        m_comListener=comListener;
    }
    
    void pushReadData(const StringList& datas){
        m_readedDataList.push_back(datas);
    }
};



#endif //_VCom_h_

//VCom.cpp

#include "VCom.h"
#include "../base/MyMD5Import.h"

#include "VComDataRec.h"

static const bool com_isUseGBK=true;

VCom::VCom(INetIOFactory* netIOFactory)
:m_netIO(0),m_netIOFactory(netIOFactory),m_comListener(0),m_packProtocol(com_isUseGBK) { 
    m_netIO=m_netIOFactory->newNetIO();  
    m_netIO->setNetIOListener(this);
}

VCom::~VCom(){
    if (m_netIO!=0){
        m_netIO->setNetIOListener(0);
        m_netIOFactory->deleteNetIO(m_netIO);
        m_netIO=0;
    }
}

void VCom::sendData(const StringList& datas) { 
    long length=0; 
    m_packProtocol.packComData(datas,m_tmp_dataBuf,length);
    if (length>0) m_netIO->sendData(&m_tmp_dataBuf[0],length);
}
bool VCom::readData(StringList& datas){
    if (!m_readedDataList.empty()){
        datas.swap(m_readedDataList.front());
        m_readedDataList.pop_front();
        return true;
    }
    
    TNetData netData;
    if (!m_netIO->receivedData(netData)) return false;
    if (!m_packProtocol.unPackComData(netData.data,netData.dataLength,datas)){
        //ERROR DATA
        m_netIO->releaseReceivedData(netData);
        readDataErrorEvent();
        return false; 
    }else{
        readedDataEvent(datas);
        m_netIO->releaseReceivedData(netData);
        return true;
    }
}



inline void push_String(VCom::TDataBuf& out_dataBuf,const String& str){
    const unsigned char* itBegin=(const unsigned char*)str.c_str();
    long count=(long)str.size();
    for (long i=0;i<count;++i)
        out_dataBuf.push_back(itBegin[i]);
}

const char cs_MsgValue_ListSeparator = 13;//'\r';
const char cs_MsgIDBegin_ListSeparator = '<';
const char cs_MsgIDEnd_ListSeparator = '>';


void ClientPackProtocol::packComData(const StringList& datas,TDataBuf& out_dataBuf,long& out_datalength){
    out_dataBuf.clear();
    out_datalength=0;
    long count=(long)datas.size();
    if (count<=0) return;
    
    String msgStr;
    msgStr.resize(1);
    msgStr[0]=' ';//给消息帧id占位
    
    //消息内容
    msgStr+=datas[0]; //msgID
    for (long i=1;i<count;++i){
        msgStr+=datas[i];
        if (i!=(count-1)) 
            msgStr+=cs_MsgValue_ListSeparator;
    }
    if ((m_isUseGBK)&&(!isAsciiChars(msgStr))){
        utf8ToGbk(msgStr.c_str(),msgStr.c_str()+msgStr.size(),&msgStr);
    }
    msgStr[0]=getAPakID();//消息帧id
    push_String(out_dataBuf,msgStr);
    
    String md5Str;
    md5Str.resize(8);
    getMyMD5StrBin8(msgStr.c_str(),(long)msgStr.size(),&md5Str[0]); //MD5
    push_String(out_dataBuf,md5Str); 
    out_datalength=(long)out_dataBuf.size();
    
#ifdef DEBUG_PACKDATA
    VComDataRec::recPackData(&out_dataBuf[0],out_datalength);
#endif
    
}


bool ClientPackProtocol::unPackComData(const unsigned char* data,long length,StringList& out_datas){
    out_datas.clear();
    if (length<=0) return false;
    
    {//去掉前缀字符
        long it0=pos((const char*)data,length,cs_MsgIDBegin_ListSeparator);
        if (it0>0){
            data+=it0;
            length-=it0;
        }
    }
    
    String tmpBuf;
    if (m_isUseGBK&&(!isAsciiChars((const char*)data,length))){
        gbkToUtf8((const char*)data,(const char*)data+length,&tmpBuf);
        data=(const unsigned char*)tmpBuf.c_str();
        length=tmpBuf.size();
    }
    
    long it0=pos((const char*)data,length,cs_MsgIDBegin_ListSeparator);
    if (it0<0) return false;
    long it1=posEx((const char*)data,length,it0+1,cs_MsgIDEnd_ListSeparator);
    if (it1<0) return false;
    
    //ID
    String tmpStr;
    dataCopyToString(data,it0,it1+1,tmpStr);
    out_datas.push_back(tmpStr);
    it0=it1+1;
    
    while (it0<length){
        it1=posEx((char*)data,length,it0,cs_MsgValue_ListSeparator);
        if (it1<0) it1=length;
        
        dataCopyToString(data,it0,it1,tmpStr);
        out_datas.push_back(tmpStr);
        if (it1==length-1){
            out_datas.push_back(""); //特殊处理最后一个空消息项		
            it0=length;
        }
        else
            it0=it1+1;
    }
    
    /*//消息内容
     String msgStr=out_datas[0]; //msgID
     for (long i=1;i<(long)out_datas.size();++i){
     msgStr+=out_datas[i];
     if (i!=(out_datas.size()-1)) 
     msgStr+=cs_MsgValue_ListSeparator;
     }*/
    
#ifdef DEBUG_PACKDATA    
    VComDataRec::recPackData(data,length);
#endif    
    
    
    return true;
}

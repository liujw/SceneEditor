//
//  VNet.h
//
//  Created by housisong on 08-4-18.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VNet_h_
#define _VNet_h_

#include "INetIO.h"
#include "VThread.h"

//数据头协议
struct ComHeadProtocol_Hex{
    enum { comHeadSize=4 };
    must_inline static long getMaxComDataLength(){ return ( 1 << (comHeadSize*4) ) - 1; }
    must_inline static bool checkHeadData(const UInt8* head){
        return checkHexStr((const char*)head,comHeadSize);
    }
    must_inline static long headDataToComDataLength(const UInt8* head){
        return HexToInt((const char*)head,comHeadSize);
    }
    must_inline static bool createHead(UInt8* head,long comDataLength){
        if (comDataLength<0) return false;
        if (comDataLength>getMaxComDataLength()) return false;
        IntToHex(comDataLength,(char*)head,comHeadSize);
        return true;
    }
};

//支持更大的数据长度 到 2M
struct ComHeadProtocol_G{
    enum { comHeadSize=4, comHeadTag='G' };
    must_inline static long getMaxComDataLength(){ return (1<<21) - 1; }
    inline static bool checkHeadData(const UInt8* head){
        if (ComHeadProtocol_Hex::checkHeadData(head)) return true;
        if (head[0]!=comHeadTag) return false;
        return headDataToComDataLength(head)<=getMaxComDataLength();
    }
    must_inline static long headDataToComDataLength(const UInt8* head){
        return head[1] | (head[2]<<8) | (head[3]<<16);
    }
    inline static bool createHead(UInt8* head,long comDataLength){
        if (comDataLength<=ComHeadProtocol_Hex::getMaxComDataLength())
            return ComHeadProtocol_Hex::createHead(head,comDataLength);
        
        if (comDataLength>getMaxComDataLength()) return false;
        head[0]=comHeadTag;
        head[1]=(UInt8)(comDataLength);
        head[2]=(UInt8)(comDataLength>>8);
        head[3]=(UInt8)(comDataLength>>16);
        return true;
    }
};

//一个TCP的内部实现
//内部实现了异步的读写线程
class VNet:public INetIO
#ifdef IS_NEED_COM_BIG_DATA
,public ComHeadProtocol_G
#else
,public ComHeadProtocol_Hex
#endif
{
protected:
    void*		m_Import;
    VThread*	m_readThread;
    VThread*	m_writeThread;
    VThread*    m_connectThread;
    INetIOListener* m_netIOListener;
    volatile TNetLinkState  m_linkState;
    
    virtual void setNetLinkState(TNetLinkState netLinkState);
protected:
    void initNet(INetLink* netLink);
public:
    explicit VNet(); 
    virtual ~VNet();
    virtual void setNetIOListener(INetIOListener* netIOListener) { m_netIOListener=netIOListener; }
    virtual TNetLinkState getNetLinkState()const { return m_linkState; }
    virtual void connect(const char* hostName,long port,INetLink* netLinkType=0);//默认使用TCP链接
    virtual void close();
    virtual void sendData(const UInt8* data,long length);
    virtual bool receivedData(TNetData& netData);
    virtual void releaseReceivedData(TNetData& netData);
};


class VNetIOFactory:public INetIOFactory{
public:
    virtual ~VNetIOFactory(){}
    virtual INetIO* newNetIO() { return new VNet(); }
    virtual void deleteNetIO(INetIO* netIO) { if (netIO!=0) delete netIO; }
};


#endif //_VNet_h_

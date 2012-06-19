//
//  VNetTCPLink.h
//
//  VNetTCPLink 一个TCP链接的实现
//
#ifndef _VNetTCPLink_h_
#define _VNetTCPLink_h_

#include "INetLink.h"

class VNetTCPLink:public INetLink{
protected:
    void*		m_Import;
public:
    explicit VNetTCPLink();
    virtual ~VNetTCPLink();
    virtual bool connect(const char* hostName,long port);
    virtual void setOnEventCallBackProc(TNetLinkEventCallBackProc callBackProc,void* userData);
    virtual bool sendData(const unsigned char* data,long length);
    virtual bool getIsReceivedData();
    virtual bool readData(unsigned char* out_data,long read_length);
    virtual void close();
};

//class VNetTCPHostLink:public INetHostLink

#endif //_VNetTCPLink_h_

//
//  INetIO.h
//
//  Created by housisong on 08-5-6.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _INetIO_h_
#define _INetIO_h_

#include "../base/SysImportType.h"
#include "INetLink.h"

struct TNetData{
    UInt8* data;
    long   dataLength;
    TNetData():data(0),dataLength(0){}
};


class INetIO;

class INetIOListener{
public:
    virtual void disposeEventNetIOLinked(INetIO* sender){}
    virtual void disposeEventNetIOUnLinked(INetIO* sender){}
    virtual void disposeEventNetIOError(INetIO* sender){}
    virtual void disposeEventNetIOReadError(INetIO* sender){}
    virtual void disposeEventNetIOWriteError(INetIO* sender){}
    virtual ~INetIOListener(){}
};

enum TNetLinkState{ nls_UnLinked=0,nls_Connecting,nls_Linked,nls_Error,nls_ReadError,nls_WriteError };
class INetIO{
public:
    virtual ~INetIO() {}
    virtual void setNetIOListener(INetIOListener* netIOListener)=0;
    virtual TNetLinkState getNetLinkState()const=0;
    virtual void connect(const char* hostName,long port,INetLink* netLinkType=0)=0;
    virtual void close()=0;
    virtual void sendData(const UInt8* data,long length)=0;
    virtual bool receivedData(TNetData& netData)=0;
    virtual void releaseReceivedData(TNetData& netData)=0;
};

class INetIOFactory{
public:
    virtual ~INetIOFactory(){}
    virtual INetIO* newNetIO()=0;
    virtual void deleteNetIO(INetIO* netIO)=0;
};

#endif //_INetIO_h_

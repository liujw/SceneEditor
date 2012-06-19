/*
 *  DownLoader.h
 *
 *  Created by housisong on 08-10-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DownLoader_h_
#define _DownLoader_h_
#include "SysImportType.h"
#include "hStream.h"

class IDownLoadListener;

class IDownLoader{
public:
    virtual void downLoadByURL(IDownLoadListener* downLoadListener,const String& url)=0;
    virtual long getDownLoadedDataSize()=0;
    virtual long readDownLoadedData(void* dst,long readPos,long readLength)=0;
    virtual void clearDownLoadedData()=0;
    virtual ~IDownLoader(){}
};

class IDownLoadListener{
public:
    virtual void downLoader_start(IDownLoader* downLoader,long fileByteLength){}
    virtual void downLoader_process(IDownLoader* downLoader,long curPos,long fileByteLength){}
    virtual void downLoader_finish(IDownLoader* downLoader){}
    virtual void downLoader_error(IDownLoader* downLoader,const char* errorInfo){}
    virtual ~IDownLoadListener(){}
};

class VDownLoader:public IDownLoader{
private:
    void* m_import;
    IDownLoadListener* m_downLoadListener;
    long m_receivedLength;
    long m_fileLength;
    Vector<UInt8> m_data;
protected:
    void doOnReceived(const void* data,long dataLength);
    virtual void doOnFinish();
    void doOnStart(long fileByteLength);
public:
    VDownLoader();
    virtual ~VDownLoader();
    virtual void downLoadByURL(IDownLoadListener* downLoadListener,const String& url);
    virtual IDownLoadListener* getListener(){ return m_downLoadListener; }
    virtual void setListener(IDownLoadListener* downLoadListener){ m_downLoadListener=downLoadListener; }
    
    virtual long getDownLoadedDataSize() { return (long)m_data.size(); }
    virtual long readDownLoadedData(void* dst,long readPos,long readLength);
    virtual void clearDownLoadedData() { m_data.clear(); }
};

//下载成功后保存为一个文件
class VDownLoadToFile:public VDownLoader{
private:
    IOutputStream* m_dstStream;
    bool           m_isOwnerStream;
    void setSteam(IOutputStream* dstStream,bool isOwnerStream){
        clear();
        m_dstStream=dstStream;
        m_isOwnerStream=isOwnerStream;
    }
protected:
    virtual void doOnFinish();
    void clear(){
        if ((m_isOwnerStream)&&(m_dstStream!=0)){
            delete m_dstStream;
            m_dstStream=0;
            m_isOwnerStream=false;
        }
    }
public:
    VDownLoadToFile(const String& dstFileName);
    VDownLoadToFile(IOutputStream* dstStream);
    virtual ~VDownLoadToFile(){ clear(); }
};


#endif //_DownLoader_h_
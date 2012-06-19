/*
 *  DownLoader.cpp
 *
 *  Created by housisong on 08-10-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DownLoader.h"
#include "../../import/importInclude/WebImport.h"


class VDownLoader_:public VDownLoader{
public:
    must_inline void doOnFinish_(){
        VDownLoader::doOnFinish();
    }
    must_inline void doOnReceived_(const void* data,long dataLength){
        VDownLoader::doOnReceived(data,dataLength);
    }
    must_inline void doOnStart_(long fileByteLength){
        VDownLoader::doOnStart(fileByteLength);
    }
};

static void URLDownLoad_Finish(void* callBackData){
    VDownLoader* downLoader=(VDownLoader*)callBackData;
    assert(downLoader!=0);
    
    ((VDownLoader_*)downLoader)->doOnFinish_();
}

static void URLDownLoad_Received(void* callBackData,const void* data,long dataLength){
    VDownLoader* downLoader=(VDownLoader*)callBackData;
    assert(downLoader!=0);
    
    ((VDownLoader_*)downLoader)->doOnReceived_(data,dataLength);
}

static void URLDownLoad_Start(void* callBackData,long fileByteLength){
    VDownLoader* downLoader=(VDownLoader*)callBackData;
    assert(downLoader!=0);
    
    ((VDownLoader_*)downLoader)->doOnStart_(fileByteLength);
}


static void URLDownLoad_Error(void* callBackData,const char* errorInfo){
    VDownLoader* downLoader=(VDownLoader*)callBackData;
    assert(downLoader!=0);
    IDownLoadListener* downLoadListener=downLoader->getListener();
    
    if (downLoadListener)
        downLoadListener->downLoader_error(downLoader,errorInfo);
}

VDownLoader::VDownLoader():m_import(0),m_downLoadListener(0),m_receivedLength(0),m_fileLength(0){
    m_import=URLDownLoad_create(this,URLDownLoad_Received,URLDownLoad_Start,URLDownLoad_Finish,URLDownLoad_Error);
}
VDownLoader::~VDownLoader(){
    if (m_import){
        URLDownload_free(m_import);
        m_import=0;
    }
}

void VDownLoader::downLoadByURL(IDownLoadListener* downLoadListener,const String& url){
    m_downLoadListener=downLoadListener;
    URLDownLoad_downLoad(m_import,url.c_str());
}

void VDownLoader::doOnReceived(const void* data,long dataLength){
    const UInt8* src=(const UInt8*)data;
    m_data.insert(m_data.end(),src,&src[dataLength]);
}


long VDownLoader::readDownLoadedData(void* dst,long readPos,long readLength){
    if (readPos<0) return 0;
    if (readLength<=0) return 0;
    long maxPos=(long)(m_data.size());
    if (readPos>=maxPos) return 0;
    if (readPos+readLength>maxPos) readLength=maxPos-readPos;
    memcpy(dst,&m_data[0],readLength);
    return readLength;
}


void VDownLoader::doOnFinish(){
    if (m_downLoadListener)
        m_downLoadListener->downLoader_finish(this);
}


void VDownLoader::doOnStart(long fileByteLength){
    //m_data.reserve(fileByteLength);
    if (m_downLoadListener)
        m_downLoadListener->downLoader_start(this, fileByteLength);	
}

///////////


void VDownLoadToFile::doOnFinish(){
}

VDownLoadToFile::VDownLoadToFile(const String& dstFileName):m_dstStream(0),m_isOwnerStream(false){
    IOutputStream* dstStream=new MyDataFileOutputStream(dstFileName.c_str());
    setSteam(dstStream,true);
}

VDownLoadToFile::VDownLoadToFile(IOutputStream* dstStream):m_dstStream(0),m_isOwnerStream(false){
    setSteam(dstStream,false);	
}



//
//  NetLinkImport.h
//
//  TCP的import
//
#ifndef _NetLinkImport_h_
#define _NetLinkImport_h_

#include "../../GameEngine/engine/INetLink.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    //客户端
    TNetLinkImportHandle netLink_create();
    void netLink_setOnEventCallBackProc(TNetLinkImportHandle netLinkImportHandle,TNetLinkEventCallBackProc callBackProc,void* userData);
    bool netLink_connect(TNetLinkImportHandle netLinkImportHandle,const char* hostName,long port);
    
    bool netLink_sendData(TNetLinkImportHandle netLinkImportHandle,const unsigned char* data,long length);
    bool netLink_getIsReceivedData(TNetLinkImportHandle netLinkImportHandle);
    bool netLink_readData(TNetLinkImportHandle netLinkImportHandle,unsigned char* out_data,long read_length);
    
    void netLink_close(TNetLinkImportHandle netLinkImportHandle);
    void netLink_delete(TNetLinkImportHandle netLinkImportHandle);
    
    //////////
    //服务器
    typedef void* TNetHostImportHandle;

    TNetHostImportHandle netHost_create();
    const char* netHost_getDefaultMyHostName();
    void netHost_setOnEventCallBackProc(TNetHostImportHandle netHostImportHandle,TNetLinkEventCallBackProc callBackProc,void* userData);
    bool netHost_listen(TNetHostImportHandle netHostImportHandle,const char* myHostName,long hostPort,long queueLinkPeerCount=5);
    TNetLinkImportHandle netHost_checkNewPeerLink(TNetHostImportHandle netHostImportHandle,const char*& out_peerName,long& peerPort);
    void netHost_close(TNetHostImportHandle netHostImportHandle);
    void netHost_delete(TNetHostImportHandle netHostImportHandle);
    
#ifdef __cplusplus 
}
#endif




#endif //_NetLinkImport_h_
//WebImport.h

#ifndef _WebImport_h_
#define _WebImport_h_


#ifdef __cplusplus 
extern "C" {
#endif
    
    typedef void* TWebImportHandle;	
    typedef void(*TWebImport_LoadFinishedCallBackProc)(void* callBackData);
    typedef void(*TWebImport_LoadErroredCallBackProc)(void* callBackData);
    
    //网页显示
    TWebImportHandle web_create(long x0,long y0,long width,long height,long isShare);
    void web_setListener(TWebImportHandle webImportHandle,void* callBackData,TWebImport_LoadFinishedCallBackProc finishProc,TWebImport_LoadErroredCallBackProc errorProc);
    void web_delete(TWebImportHandle webImportHandle);
    void web_setPosAndSize(TWebImportHandle webImportHandle,long x0,long y0,long width,long height);
    void web_setBackGroundColor(TWebImportHandle webImportHandle,unsigned char R,unsigned char G,unsigned char B,unsigned char A);
    void web_setVisible(TWebImportHandle webImportHandle,long isVisible);
    void web_openURL(TWebImportHandle webImportHandle,const char* url);
    void web_openHtmlFile(TWebImportHandle webImportHandle,const char* htmlFileName,const char* htmlFilePath);
    
    //调用系统的浏览器打开网页
    void web_openURLWithSystem(const char* url);
    //发短信
    void web_openSmsWithSystem(const char* phoneNumber,const char* text);
    //打电话
    void web_openPhoneWithSystem(const char* phoneNumber);
    //发邮件
    void web_openMailWithSystem(const char* mailAddress,const char* title,const char* text);
    
    ///////////////////////////////////////////////////////////////////////
    
    
    typedef void(*TURLDownLoad_ReceivedDataCallBackProc)(void* callBackData,const void* data,long dataLength);
    typedef void(*TURLDownLoad_StartCallBackProc)(void* callBackData,long fileByteLength);
    typedef void(*TURLDownLoad_FinishCallBackProc)(void* callBackData);
    typedef void(*TURLDownLoad_ErrorCallBackProc)(void* callBackData,const char* errorInfo);
    
    //下载
    void* URLDownLoad_create(void* callBackData,TURLDownLoad_ReceivedDataCallBackProc ReceivedDataProc,TURLDownLoad_StartCallBackProc startProc,
                             TURLDownLoad_FinishCallBackProc finishProc,TURLDownLoad_ErrorCallBackProc errorProc);
    void URLDownload_free(void* URLDownLoadHandle);
    
    void URLDownLoad_downLoad(void* URLDownLoadHandle,const char* url);
    
#ifdef __cplusplus 
}
#endif
#endif //_WebImport_h_
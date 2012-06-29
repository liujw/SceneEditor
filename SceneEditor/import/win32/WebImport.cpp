//WebImport.cpp

#include "../importInclude/WebImport.h"
#include "SysImportType.h"
#ifdef Set
	#undef Set
#endif
#ifdef WINDOWS
	#include "windows.h"
	const void* getMainFormHandle();
#endif
#ifdef _MZPhone
	//#include <sms.h>
	//#pragma comment(lib,"sms.lib")
#endif


TWebImportHandle web_create(long x0,long y0,long width,long height,long isShare){
    return 0;
}
void web_setListener(TWebImportHandle webImportHandle,void* callBackData,TWebImport_LoadFinishedCallBackProc finishProc,TWebImport_LoadErroredCallBackProc errorProc){
}

void web_delete(TWebImportHandle webImportHandle){
}

void web_setPosAndSize(TWebImportHandle webImportHandle,long x0,long y0,long width,long height){
}

void web_setBackGroundColor(TWebImportHandle webImportHandle,unsigned char R,unsigned char G,unsigned char B,unsigned char A){
}
void web_setVisible(TWebImportHandle webImportHandle,long isVisible){
}
void web_openURL(TWebImportHandle webImportHandle,const char* url){
}
void web_openHtmlFile(TWebImportHandle webImportHandle,const char* htmlFileName,const char* htmlFilePath){
}

void _openURLWithSystem(const char* openType,const char* url){
#ifdef WINDOWS
	SHELLEXECUTEINFO   ExecuteInfo;
	memset(&ExecuteInfo,0,sizeof(ExecuteInfo));
	ExecuteInfo.cbSize   =   sizeof(  ExecuteInfo  );
	ExecuteInfo.fMask   =   SEE_MASK_NOCLOSEPROCESS;  
	ExecuteInfo.hwnd   =   NULL;
	ExecuteInfo.lpDirectory   =   NULL;
	ExecuteInfo.nShow   =   SW_SHOWNORMAL; 

	#ifdef _MZPhone
		if ((openType==0)||(openType[0]==0)){
			openType ="open";  
		}
	#endif

#ifdef WINCE
	wchar_t* wOpenType=0;
#endif
	if ((openType!=0)&&(openType[0]!=0)){
		#ifdef WINCE
			wOpenType=ansiCharToUnicode(openType);
			ExecuteInfo.lpVerb  = wOpenType; 
		#else
			ExecuteInfo.lpVerb  = openType;  
		#endif
	}
#ifdef WINCE
	wchar_t* wUrl=ansiCharToUnicode(url); 
	#ifdef _MZPhone
		ExecuteInfo.lpFile   =  TEXT("\\Program Files\\WebBrowser\\WebBrowser.exe"); 
		ExecuteInfo.lpParameters   =   wUrl;
	#else
		ExecuteInfo.lpFile=wUrl;
	#endif
#else
	ExecuteInfo.lpFile=url;
#endif

	BOOL   bResult   =  ::ShellExecuteEx(&ExecuteInfo); 

#ifdef WINCE
	delete[]wUrl;
	if (wOpenType!=0)
		delete[]wOpenType;
#endif

	if(bResult){  
		#ifdef WINCE
			exitApp();
		#endif
	}else{
		//运行失败!
	}
#endif
}

void web_openURLWithSystem(const char* url){
	_openURLWithSystem("",url);
}

void web_openSmsWithSystem(const char* phoneNumber,const char* text){
#ifdef _MZPhone
/************************************
* 调用范例:
* SendSMS(_T("+8613xxxxxxxxx"),_T("测试~"));
*
* Author:LOJA
* Version: 1.0.1.0
* Date: 2009/07/30
*
* Smartphone Platforms: Smartphone 2002 and later
* OS Versions: Windows CE 3.0 and later
*
*************************************/
/*
bool SendSMS(IN LPCTSTR lpNumber,IN LPCTSTR lpszMessage)
{

        HRESULT hRes;
        SMS_HANDLE   smsHandle=NULL;
        SMS_ADDRESS   smsaDestination;
        SMS_MESSAGE_ID   smsmidMessageID=0;
        TEXT_PROVIDER_SPECIFIC_DATA   tpsd;

        hRes=SmsOpen(SMS_MSGTYPE_TEXT,SMS_MODE_SEND,&smsHandle,NULL);
        if   (FAILED(hRes))
        {
                return   false;
        }

        //地址方式注意国内号码前加"+86"
        smsaDestination.smsatAddressType = SMSAT_INTERNATIONAL;
        _tcsncpy(smsaDestination.ptsAddress, lpNumber,SMS_MAX_ADDRESS_LENGTH);

        tpsd.dwMessageOptions  = PS_MESSAGE_OPTION_NONE;
        //tpsd.dwMessageOptions =  PS_MESSAGE_OPTION_STATUSREPORT;//表示需要状态报告
        tpsd.psMessageClass  = PS_MESSAGE_CLASS1;
        //PS_MESSAGE_CLASS0表示短信在被接收后立即显示且不存储在收件箱(称为闪信)
        //PS_MESSAGE_CLASS1表示一般的情况，被接收后存储到收件箱并发送一个确认回短信中心，发送方收到一个已被接收的状态报告。

        ZeroMemory(tpsd.pbHeaderData, sizeof(tpsd.pbHeaderData));         
        tpsd.dwHeaderDataSize = 0;
        tpsd.fMessageContainsEMSHeaders = FALSE;
        tpsd.dwProtocolID = SMS_MSGPROTOCOL_UNKNOWN;
        tpsd.psReplaceOption = PSRO_NONE;


        hRes= SmsSendMessage(smsHandle,
                NULL,   
                &smsaDestination,   
                NULL,
                (PBYTE)lpszMessage,   
                _tcslen(lpszMessage) *  sizeof(TCHAR),   
                (PBYTE)&tpsd,
                sizeof(TEXT_PROVIDER_SPECIFIC_DATA),   
                SMSDE_OPTIMAL,   
                SMS_OPTION_DELIVERY_NONE,
                &smsmidMessageID);

        SmsClose(smsHandle);

        if   (SUCCEEDED(hRes))
        {
                return true;
        }
        else
        {
                return false;
        }
} */
//
#endif
}

void web_openPhoneWithSystem(const char* phoneNumber){
}

void web_openMailWithSystem(const char* mailAddress,const char* title,const char* text){
	String str="mailto:";
	str+=mailAddress;
	if (((title!=0)&&(title[0]!=0))||((text!=0)&&(text[0]!=0))){
		str+="?";
		if ((title!=0)&&(title[0]!=0)){
			str+="subject=";
			str+=title;
		}
		if ((text!=0)&&(text[0]!=0)){
			if ((title!=0)&&(title[0]!=0))
				str+="&";
			str+="body=";
			str+=text;
		}
	}
	_openURLWithSystem("open",str.c_str());
}


///////////////////////////////////////////////////////////////////////

//下载
void* URLDownLoad_create(void* callBackData,TURLDownLoad_ReceivedDataCallBackProc ReceivedDataProc,TURLDownLoad_StartCallBackProc startProc,
                         TURLDownLoad_FinishCallBackProc finishProc,TURLDownLoad_ErrorCallBackProc errorProc){
     return 0;
}
void URLDownload_free(void* URLDownLoadHandle){
}
		
void URLDownLoad_downLoad(void* URLDownLoadHandle,const char* url){
}

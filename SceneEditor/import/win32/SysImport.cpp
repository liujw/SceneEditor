//SysImport.cpp

#include "../importInclude/SysImport.h"
#include "../../GameEngine/base/SysImportType.h"
#include "../../GameEngine/base/hStream.h"
#include "MyMD5Import.h"
#ifdef Set
	#undef Set
#endif
#include "Windows.h"
#include "stdio.h"

	#if (defined(_DEMO) || defined(_MZMyStore))
		bool _isRunAsDemo=true;
	#else	
		bool _isRunAsDemo=false;
	#endif

#ifdef WINCE
#include "WinIoCtl.h"//
//#include "Pkfuncs.h"//
	extern "C" __declspec(dllimport)
	BOOL KernelIoControl(
		DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize,
		LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned
	);
#endif
#ifdef _MZPhone
	#include "tapi.h"
	#include "extapi.h"
	#define TAPI_API_HIGH_VERSION   0x00020000
	#define EXT_API_LOW_VERSION     0x00010000
	#define EXT_API_HIGH_VERSION    0x00010000
	
	#pragma comment (lib, "cellcore.lib")

	#ifdef _MZMyStore
		#include "MyStoreLib.h"
		#pragma comment (lib, "MyStoreLib.lib")

		bool check_MyStoreVerify(){
			wchar_t pszFileName[MAX_PATH] = {0};
			GetModuleFileName(0, pszFileName, MAX_PATH);            // 当前进程EXE的文件名
			MYSTORE_VERIFY_CONTEXT mystore = {0};
			DWORD dwRet = MyStoreVerify(pszFileName, &mystore);     // 验证此文件是否合法
			return ((dwRet==0)&&(!mystore.Expired));
		}
	#endif
#endif

int g_isVerticalScreenImport=0;

const UInt8 MonthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static inline bool _isLeapYear(long Year){
	return ((Year%4)==0) && ( ((Year%100)!=0) || ((Year%400)==0) );
}

static Int64 _clacDays(long Year,long Month,long Day){
	Int64 result=Day;
	for (long i=1;i<Month;++i){
		result+=MonthDays[i];
	}
	if ((Month>2)&&(_isLeapYear(Year)))
		++result;

	long Y=Year-1;
	result += Y*365 + Y/4 - Y/100 + Y/400;
	return result;
}

template<class T0,class T1,class T2,class T3>
must_inline void DivMod(const T0 x,const T1 y,T2& d,T3& m){
	d=(T2)(x/y);
	m=(T3)(x%y);
}


void _daysToYMD(const Int64 days,Int32& Year,Int8& Month,Int8& Day){
	const long D1 = 365;
	const long D4 = D1 * 4 + 1;
	const long D100 = D4 * 25 - 1;
	const long D400 = D100 * 4 + 1;

	Int64 T=days;

    --T;
    long Y = 1;
	while (T >= D400) {
		T-=D400;
		Y+=400;
	}

	long I,D;
    DivMod(T, D100, I, D);
	if (I==4){
      --I;
      D+=D100;
	}
    Y+=I * 100;
    DivMod(D, D4, I, D);
    Y+=I * 4;
    DivMod(D, D1, I, D);
	if (I == 4) {
      --I;
      D+=D1;
	}
    Y+=I;

    bool isLeepYear=_isLeapYear(Y);
    long M= 1;
	while (true){
      I = MonthDays[M];
	  if ((isLeepYear)&&(M==2))
		  ++I;
      if (D < I) break;
      D-=I;
      ++M;
	}

    Year = Y;
    Month= (Int8)M;
    Day  = (Int8)(D + 1);
}

double getNowTime_s(){
	SYSTEMTIME SystemTime; 
	GetLocalTime(&SystemTime);
	return  _clacDays(SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay)*(double)(24.0*3600) 
		   + SystemTime.wHour*3600. + SystemTime.wMinute*60. + SystemTime.wSecond
		   + SystemTime.wMilliseconds*(1.0/1000);
}

void dateTimeToDateTimeData(TDateTime dateTime,TDateTimeData* out_dateTimeData){
	Int64 ms64=(Int64)(dateTime*1000);
	long days,ms;
	DivMod(ms64,24*3600*1000,days,ms);
	_daysToYMD(days,out_dateTimeData->year,out_dateTimeData->month,out_dateTimeData->day);
	
	DivMod(ms,3600*1000,out_dateTimeData->hour,ms);
	DivMod(ms,60*1000,out_dateTimeData->minute,ms);
	out_dateTimeData->second=ms/1000.0;
}


void debug_out(const char* message){
	//printf("%s",message);
	#ifndef WINCE
		const char* str=message;
		const char* cap="error";
	#else
		wchar_t* str=ansiCharToUnicode(message);
		const wchar_t* cap=L"error";
	#endif
	::MessageBox(0,str,cap,MB_OK);
	if ((void*)str!=(void*)message)
		delete[]str;
}

#ifdef _TestMemAlloc
	extern void VGame_outDebugInfo(const char* text,bool isClearOld);
	long g_memSize=0;
	long g_memSize_max=0;
	bool g_memSize_max_lock=false;

	HashMap<void*,void*> g_memP;
	void outMemSize(){
		if (!g_memSize_max_lock){
			g_memSize_max_lock=true;
			String s; itoa(g_memSize_max,s);
			VGame_outDebugInfo(s.c_str(),true);
			itoa(g_memSize,s);
			VGame_outDebugInfo(s.c_str(),false);
			g_memSize_max_lock=false;
		}
	}
	void* allocPixelsMemery(long memSize){
		if (memSize>2*1024*1024){
			++g_memSize;
		}
		g_memSize+=memSize;
		if (g_memSize>15*1024*1024){
			++g_memSize;
		}
		if (g_memSize>g_memSize_max) {
			g_memSize_max=g_memSize;
			outMemSize();
		}
		UInt32* p=(UInt32*)(new UInt8[memSize+4]);//VirtualAlloc(0,memSize+4,MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,PAGE_READWRITE);
		memset(p,0,memSize+4);
		*p=memSize;
		if (g_memP[p]!=0){
			++g_memSize;
		}
		g_memP[p]=p;
		return p+1;
	}
	void  freePixelsMemery(void* pMem){
		UInt32* p=((UInt32*)pMem)-1;
		if (g_memP[p]!=p){
			++g_memSize;
		}
		g_memP[p]=0;
		long memSize=*p;
		g_memSize-=memSize;
		delete[](UInt8*)p;//VirtualFree(p,0,MEM_RELEASE);
		outMemSize();
	}
#endif




DWORD getDiskSerialnum(const char* diskPath){

	#ifndef WINCE
	const long nameLength=256;
	char	volumename[nameLength];
	DWORD	serialnumber=0;
	DWORD	flags;
	char	filesystemname[nameLength];
	DWORD	compleng;
		GetVolumeInformation(diskPath, volumename, nameLength, &serialnumber,&compleng, &flags, filesystemname, nameLength);
		return serialnumber;
	#else
		return 0;
	#endif
}

	static void hashUID(char* uid){
			unsigned long r=387254239;
			String str(uid);
			long length=str.size();

			//映射
			for (int i=0;i<length;++i){
				char c=str[i];
				r=((r*1423235437) >>8)+342543431;
				if ( (('0'<=c)&&(c<='9')) || (('a'<=c)&&(c<='z')) || (('A'<=c)&&(c<='Z')) ){
					const long azCount=('z'-'a'+1);
					long ix=0;
					if (('0'<=c)&&(c<='9'))
						ix=c-'0';
					else if (('a'<=c)&&(c<='z'))
						ix=10+c-'a';
					else
						ix=10+azCount+c-'A';
					ix=( ((unsigned long)(ix+r))%(10+azCount*2) );

					if (ix<10)
						c=(char)(ix+'0');
					else if (ix<10+azCount)
						c=(char)(ix-10+'a');
					else
						c=(char)(ix-10-azCount+'A');
				}
				uid[i]=c;
			}
			
			//交换
			for (int i=0;i<length-1;++i){
				r=((r*432435341) >>8)+17893325;
				int s=i + r%(unsigned long)(length-i);
				//swap
				char tmp=uid[i];
				uid[i]=uid[s];
				uid[s]=tmp;
			}
	}

const char* getMachineUniqueIdentifier(){
	static char* g_uniqueIdentifier=0;
	if ((g_uniqueIdentifier!=0)&&(g_uniqueIdentifier[0]!=0))
		return g_uniqueIdentifier;

#ifdef _DEBUG
	/*const char* fileName="DEBUG_MUID.vol";
	if (file_exists(fileName)){
		FileInputStream idFile(fileName);
		unsigned long length=idFile.getDataSize();
		char* uniqueIdentifier=new char[length+1];	
			if (length>0){
			idFile.read(uniqueIdentifier,length);
			uniqueIdentifier[length]=0;
			g_uniqueIdentifier=uniqueIdentifier;
			return g_uniqueIdentifier;
		}
	}*/
#endif

#ifdef WINCE
	
	#ifdef _MZPhone
		//Ge DeviceSIMInfo    
		const int nBuffSize = MAX_PATH*2;
		char buff[nBuffSize+1]={0};
		DWORD dwOutBytes=0;

		HLINEAPP hLineApp = 0;
		HLINE hLine = 0;
		DWORD dwNumDevs;
		DWORD dwAPIVersion = TAPI_API_HIGH_VERSION;
		DWORD dwExtVersion = 0;
		DWORD dwMediaMode = LINEMEDIAMODE_DATAMODEM | LINEMEDIAMODE_INTERACTIVEVOICE;
		

		LINEINITIALIZEEXPARAMS lineInitializeExParams;
		lineInitializeExParams.dwTotalSize = sizeof(lineInitializeExParams);
		lineInitializeExParams.dwOptions =  LINEINITIALIZEEXOPTION_USEEVENT; //The application desires to use the Event Handle event notification mechanism
		LONG tapiresult = lineInitializeEx(&hLineApp, 0, 0,TEXT("SimTry"), &dwNumDevs, &dwAPIVersion,&lineInitializeExParams); //returns 0 (SUCCESS)

		LINEGENERALINFO* lineGeneralInfo = (LINEGENERALINFO*)malloc(sizeof(LINEGENERALINFO));//保存设备序列号的信息的结构体
		memset(lineGeneralInfo,0,sizeof(LINEGENERALINFO));
		lineGeneralInfo->dwTotalSize = sizeof(LINEGENERALINFO);

		bool bRes=false;
		for (DWORD dwDeviceID = 0; dwDeviceID < dwNumDevs;++dwDeviceID) {
			tapiresult = lineNegotiateExtVersion(hLineApp, dwDeviceID, dwAPIVersion, EXT_API_LOW_VERSION,EXT_API_HIGH_VERSION, &dwExtVersion); //returns 0 (SUCCESS)
			tapiresult = lineOpen(hLineApp, dwDeviceID,&hLine, dwAPIVersion, 0, 0,LINECALLPRIVILEGE_OWNER, dwMediaMode, 0);           //returns 0 (SUCCESS)
			tapiresult = lineGetGeneralInfo(hLine,lineGeneralInfo); //returns 0 (SUCCESS)
			if((tapiresult == 0) && (lineGeneralInfo->dwNeededSize > lineGeneralInfo->dwTotalSize)){
				//重新获取最新的值
				DWORD dwNeededSize = lineGeneralInfo->dwNeededSize;
				free(lineGeneralInfo);
				lineGeneralInfo = (LINEGENERALINFO*)malloc(dwNeededSize);
				lineGeneralInfo->dwTotalSize = dwNeededSize;
				tapiresult = lineGetGeneralInfo(hLine, lineGeneralInfo);
			}

			if (tapiresult==0){
				//成功 保存设备的IMEI
				lstrcpy((TCHAR*)&buff[0], (TCHAR*)((char*)lineGeneralInfo+ lineGeneralInfo->dwSerialNumberOffset));
				dwOutBytes=2*lstrlen((TCHAR*)&buff[0]);
				//IMSI
				//if(lineGeneralInfo->dwSubscriberNumberSize > 2) {
				//	for(int j=0;j<(long)lineGeneralInfo->dwSubscriberNumberSize/2;j++)
				//		szIMSI[j] = *((unsigned short *)(lineGeneralInfo) + j + lineGeneralInfo->dwSubscriberNumberOffset/2);
				//}
				bRes=true;
				break;
			}
		}

		//回收资源
		free(lineGeneralInfo);
		if(hLine)
			lineClose(hLine);
		if(hLineApp)
			lineShutdown(hLineApp);

		//com9 AT指令
		if (!bRes){
			HANDLE hCOM9 = CreateFile( L"COM9:" , GENERIC_READ|GENERIC_WRITE , FILE_SHARE_READ|FILE_SHARE_WRITE , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL );
			if ( hCOM9 != INVALID_HANDLE_VALUE ){
				DWORD dwBytes;
				char p = 0x0D;//cr
				char q = 0x0A;//lf
				WriteFile( hCOM9 , "AT+CGSN;+CGMM" , 13 , &dwBytes , NULL );
				WriteFile( hCOM9 , &p , 1 , &dwBytes , NULL );
				WriteFile( hCOM9 , &q , 1 , &dwBytes , NULL );
				Sleep(100);
				if (ReadFile( hCOM9 , buff , nBuffSize , &dwBytes , NULL )){
					long idPos0=pos(buff,nBuffSize,p);
					if (idPos0<0) idPos0=pos(buff,nBuffSize,q);
					while ((buff[idPos0]==p)||(buff[idPos0]==q)) 
						++idPos0;
					long idPos1=posEx(buff,nBuffSize,idPos0+1,p);
					if (idPos1<0) idPos1=posEx(buff,nBuffSize,idPos0+1,q);
					while ((idPos1>0)&&((buff[idPos1-1]==p)||(buff[idPos1-1]==q)))
						--idPos1;
					if (idPos1>idPos0){
						buff[idPos1]=0;
						strcpy(buff,&buff[idPos0]);
						dwOutBytes=strlen(buff);
						bRes=true;
					}
				}

				CloseHandle( hCOM9 );
			}
		}
	#else
		//get DEVICEID
		#define IOCTL_HAL_GET_DEVICEID CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

		DWORD dwOutBytes;
		const int nBuffSize = 4096;
		char buff[nBuffSize+1]={0};
		bool bRes= 0!= KernelIoControl(IOCTL_HAL_GET_DEVICEID,0, 0,buff,nBuffSize,&dwOutBytes);
	#endif

	if ((bRes)&&(dwOutBytes>0)) {
		static char uniqueIdentifier[nBuffSize*2+1];
		long insertIndex=0;
		for (long i=0;i<(long)dwOutBytes;++i){
			UInt8 c=buff[i];
			if (c!=0){
				if ( (('0'<=c)&&(c<='9')) || (('a'<=c)&&(c<='z')) || (('A'<=c)&&(c<='Z')) ){
					uniqueIdentifier[insertIndex]=c;
					++insertIndex;
				}else{
					IntToHex(c,&uniqueIdentifier[insertIndex],2);
					insertIndex+=2;
				}
			}
		}
		uniqueIdentifier[insertIndex]=0;
		//hashUID(uniqueIdentifier);
		g_uniqueIdentifier=uniqueIdentifier;
	}else{
		static char str[]="";
		g_uniqueIdentifier=str;
	}

#else
	//读入C盘硬件码
	//todo: 不唯一！
	DWORD cDiskSerialnum=getDiskSerialnum("C:\\");
	if (cDiskSerialnum!=0){
		const long length=8;
		static char uniqueIdentifier[length+1];
		IntToHex(cDiskSerialnum,uniqueIdentifier,length);
		uniqueIdentifier[length]=0;
		//hashUID(uniqueIdentifier);
		g_uniqueIdentifier=uniqueIdentifier;
	}else{
		static char str[]="";
		g_uniqueIdentifier=str;
	}
#endif

#ifdef _IS_IN_APP_PURCHASE
	if ((g_uniqueIdentifier==0)||(g_uniqueIdentifier[0]==0)){
		exitApp();
	}
#endif
	return g_uniqueIdentifier;
}



DWORD getDefFontCharSet(){
	DWORD result = DEFAULT_CHARSET;
	HDC DisplayDC = GetDC(0);
	if (DisplayDC!=0){
		HFONT StockFont = (HFONT)GetStockObject(SYSTEM_FONT);
		if (StockFont!=0){
			if (SelectObject(DisplayDC, StockFont)!=0){
				TEXTMETRIC TxtMetric;
				if (GetTextMetrics(DisplayDC,&TxtMetric))
					result = TxtMetric.tmCharSet;
			}
			DeleteObject(StockFont);
		}
		ReleaseDC(0, DisplayDC);
	}
	return result;
}


const char* getLanguageID(){
	//GetSystemDefaultLangID ?
	static char* languageID=0;
	if (languageID==0){
		//0x0404 Chinese (Taiwan Region)
		//0x0804 Chinese (PRC)
		//0x0c04 Chinese (Hong Kong SAR, PRC)
		//0x1004 Chinese (Singapore)
		DWORD lgID=GetSystemDefaultLangID();
		if (lgID== 0x0804 )  
			languageID="zh-Hans";//old is "zh-CHS"
		else if ((lgID==0x0404)||(lgID==0x0c04)||(lgID==0x1004))  
			languageID="zh-Hant";//old is "zh-CHT"
		else
			languageID="en";
		//TODO ;other CharSet
	}
	return languageID;
}


const char* getDefaultFontName(){
#ifdef WINCE
	return "";
#else
	return "宋体";
#endif
}

void utf8ToGbk(const char* str,const char* strEnd,String* outStr) //liujw
{
}
void gbkToUtf8(const char* str,const char* strEnd,String* outStr)
{

}

bool dateTimeDataToDataTime(const TDateTimeData& dateTimeData, TDateTime& dt)
{
	return true;
}

const void* getMainFormHandle()
{
	return NULL;
}
#ifdef WINDOWS
#define CHAR_CODE_PAGE 0x000003A8
wchar_t* ansiCharToUnicode(const char *buffer){
    int len = strlen(buffer);
    int wlen = MultiByteToWideChar(CHAR_CODE_PAGE, 0, buffer, len, 0, 0);
    wchar_t* wBuf = new wchar_t[wlen + 1];
    MultiByteToWideChar(CHAR_CODE_PAGE, 0,buffer, len, wBuf, wlen);
	wBuf[wlen]=0;
    return wBuf;
}

char * unicodeToAnsiChar(wchar_t * wbuffer){
	// Calculate unicode string length.
	/*UINT len = wcslen(wbuffer)*2;
	char *buf = new char[len];
	UINT i = wcstombs(buf,wbuffer,len);
	return buf;*/
    int wlen = wcslen(wbuffer);
	int len = WideCharToMultiByte(CHAR_CODE_PAGE, 0, wbuffer, wlen, 0, 0, 0, 0);
	char* buf = new char[len+1];
	WideCharToMultiByte(CHAR_CODE_PAGE, 0, wbuffer, wlen, buf, len, 0, 0); 
	buf[len]=0;
	return buf;
}
#endif

#ifdef WINCE
	#include "../importInclude/SoundImport.h"
	#ifdef _MZPhone
		#include <mzfc_inc.h>
		#include <ShellNotifyMsg.h>
	#else
		//#define INITGUID
		#include <aygshell.h> //SHFullScreen
		#pragma comment (lib, "aygshell.lib")
		//#include "shobjidl.h" //ITaskbarList2 fullscreen
		//#include "shlguid.h"
		

		#ifndef __ITaskbarList_FWD_DEFINED__
		#define __ITaskbarList_FWD_DEFINED__
		typedef interface ITaskbarList ITaskbarList;
		#endif 	// __ITaskbarList_FWD_DEFINED__

		EXTERN_C const CLSID CLSID_TaskbarList;

		//EXTERN_C const IID IID_ITaskbarList;
	    
		MIDL_INTERFACE("56FDF342-FD6D-11d0-958A-006097C9A090")
		ITaskbarList : public IUnknown
		{
		public:
			virtual HRESULT STDMETHODCALLTYPE HrInit( void) = 0;
	        
			virtual HRESULT STDMETHODCALLTYPE AddTab( 
				 HWND hwnd) = 0;
	        
			virtual HRESULT STDMETHODCALLTYPE DeleteTab( 
				 HWND hwnd) = 0;
	        
			virtual HRESULT STDMETHODCALLTYPE ActivateTab( 
				 HWND hwnd) = 0;
	        
			virtual HRESULT STDMETHODCALLTYPE SetActiveAlt( 
				 HWND hwnd) = 0;
	        
		};

		#ifndef __ITaskbarList2_FWD_DEFINED__
		#define __ITaskbarList2_FWD_DEFINED__
		typedef interface ITaskbarList2 ITaskbarList2;
		#endif 	// __ITaskbarList2_FWD_DEFINED__ 


		EXTERN_C const IID IID_ITaskbarList2;

		MIDL_INTERFACE("602D4995-B13A-429b-A66E-1935E44F4317")
		ITaskbarList2 : public ITaskbarList
		{
		public:
			virtual HRESULT STDMETHODCALLTYPE MarkFullscreenWindow( 
				 HWND hwnd,
				 BOOL fFullscreen) = 0;
	        
		};

	#endif

	const char* volumeSetfileName="4avol_VolumeSet.vol";
	UInt8 oldVolumeSet=127;
	void loadVolumeSet(){
		if (file_exists(volumeSetfileName)){
			FileInputStream volumeFile(volumeSetfileName);
			unsigned long length=volumeFile.getDataSize();
			if (length>=1){
				length=1;
				volumeFile.read(&oldVolumeSet,length);
				setWavOutVolume(oldVolumeSet/255.0f);
			}
		}
	}
	void saveVolumeSet(){
		UInt8  iVolume=(UInt8)(getWavOutVolumeValue()*255.0f+0.001);
		if (oldVolumeSet!=iVolume){
			FileOutputStream volumeFile(volumeSetfileName);
			volumeFile.write(&iVolume,1);
		}
	}


#define IOCTL_HAL_SET_DVSONOFF		CTL_CODE(FILE_DEVICE_UNKNOWN, 2103, METHOD_BUFFERED, FILE_ANY_ACCESS)

bool setDVS(bool bOn){
		HANDLE hPWC = CreateFile(TEXT("PWC0:"),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,0);
        if(hPWC != INVALID_HANDLE_VALUE) {
			BOOL ret = DeviceIoControl (hPWC, IOCTL_HAL_SET_DVSONOFF,&bOn,sizeof(bOn), NULL,0, NULL, NULL);
            CloseHandle(hPWC);
            return (ret!=0);
        }else
			return false;
}

bool rotateScreen(long dwRotaion,unsigned long& old_dwRotaion){//0 1 2 4
	DEVMODE settings;
    memset(&settings, 0, sizeof(DEVMODE));
    settings.dmSize = sizeof(DEVMODE);

    settings.dmFields = DM_DISPLAYORIENTATION;
    ChangeDisplaySettingsEx(NULL, &settings, NULL, CDS_TEST, NULL);
    old_dwRotaion = settings.dmDisplayOrientation;
	if (dwRotaion<0) return true;
    if (dwRotaion == old_dwRotaion)
        return true;

   settings.dmDisplayOrientation = dwRotaion;
   return (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsEx(NULL, &settings, NULL, 0, NULL));
}


long _autoRotate_is_rotaion=false;
long _autoRotate_old_rotaion;
bool autoRotateScreen(){
	bool isVerticalScreen=(g_isVerticalScreenImport!=0);
	long screenWidth=GetSystemMetrics(SM_CXSCREEN);
	long screenHeight=GetSystemMetrics(SM_CYSCREEN);
	if (  (!isVerticalScreen &&(screenWidth<screenHeight))
		||((isVerticalScreen)&&(screenWidth>screenHeight))){
		unsigned long old_rotaion;
		unsigned long new_rotaion;
		if (!rotateScreen(-1,old_rotaion))
			return false;
		if ((old_rotaion==DMDO_0)||(old_rotaion==DMDO_180))
			new_rotaion=DMDO_270;
		else if ((old_rotaion==DMDO_90)||(old_rotaion==DMDO_270))
			new_rotaion=DMDO_0;
		else
			return false;

		if (!_autoRotate_is_rotaion){
			_autoRotate_is_rotaion=true;
			_autoRotate_old_rotaion=old_rotaion;
		}
		return rotateScreen(new_rotaion,old_rotaion);
	}else
		return true;
}

void autoRotateScreenRestore(){
	if (_autoRotate_is_rotaion){
		unsigned long rotaion;
		rotateScreen(_autoRotate_old_rotaion,rotaion);
		_autoRotate_is_rotaion=false;
	}
}


#ifdef WINCE
void setFullScreen(HWND hWnd,bool isFull){
	#ifdef _MZPhone
		if (isFull){
			HideMzTopBar();
			//EnableNeverQuit(true);
		}else
			ShowMzTopBar();
	#else
		ITaskbarList2* pTaskbarList2 = NULL;
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		bool isOk=false;
		HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList2, (void**)&pTaskbarList2);
		if(SUCCEEDED(hr) && pTaskbarList2 != NULL) {
			pTaskbarList2->MarkFullscreenWindow(hWnd,isFull);
			pTaskbarList2->Release();
			isOk=true;
		}
		CoUninitialize();

		if (!isOk){
			#ifdef IS_NEED_CTRL_HHTaskBar
				HWND  tbhWnd = ::FindWindow(TEXT("HHTaskBar"),NULL);  
				::ShowWindow(tbhWnd,SW_HIDE);   //警告：程序异常退出的时候，工具条将得不到恢复 ！！！
			#endif
			::SHFullScreen(hWnd,SHFS_HIDETASKBAR | SHFS_HIDESTARTICON | SHFS_HIDESIPBUTTON);
			//::SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong((HWND)winMainFormHandle,GWL_EXSTYLE) | WS_EX_TOPMOST);
			//::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
			isOk=true;
		}
	#endif
}
#endif

	HWND _curAtctiveWinMainFormHandle=0;
	void setActiveStatus(void* winMainFormHandle,bool isActive){
		if (isActive){
			
			#ifdef _MZMyStore
				static bool isInitMyStoreVerify=false;
				static bool isMyStoreVerify=false;
				if (!isInitMyStoreVerify){
					isMyStoreVerify=check_MyStoreVerify();
					isInitMyStoreVerify=true;
				}

				_isRunAsDemo=!isMyStoreVerify;

				//if (!isMyStoreVerify){
					//::PostQuitMessage(0);
					//return;
				//}

			#endif

			_curAtctiveWinMainFormHandle=(HWND)winMainFormHandle;
			#ifdef WINCE
				setFullScreen(_curAtctiveWinMainFormHandle,true);
				::Sleep(1);
			#endif
			autoRotateScreen();
			::Sleep(1);
		}else{
			#ifdef WINCE
				#ifdef _MZPhone
					//not need show bar
				#else
					setFullScreen(_curAtctiveWinMainFormHandle,false);
				#endif
			#endif 
		}
	}

	bool _isOpenKeyboarding=false;
	bool isOpenKeyboard(){
		#ifdef WINCE
			#ifdef _MZPhone		
				return MzIsSipOpen();
			#else
				//SHSipInfo
				return _isOpenKeyboarding;
			#endif
		#endif
	}
	void openKeyboard(){
		#ifdef WINCE
			#ifdef _MZPhone
				MzOpenSip(IM_SIP_MODE_GEL_PY,-1);
				SetSipWindowPos(-1);
			#else
				//SHSipPreference(_curAtctiveWinMainFormHandle, SIP_UP);
				BOOL fRes = FALSE;
				SIPINFO si;
				memset( &si, 0, sizeof( si ) );
				si.cbSize = sizeof( si );
				if( SHSipInfo( SPI_GETSIPINFO, 0, &si, 0 ) ) {
					si.fdwFlags |= SIPF_ON;
					fRes = SHSipInfo( SPI_SETSIPINFO, 0, &si, 0 );
				}
			#endif
			_isOpenKeyboarding=true;
		#endif
	}
	void closeKeyboard(){
		#ifdef WINCE
			#ifdef _MZPhone
				MzCloseSip();
			#else
				//SHSipPreference(_curAtctiveWinMainFormHandle, SIP_DOWN);
				BOOL fRes = FALSE;
				SIPINFO si;
				memset( &si, 0, sizeof( si ) );
				si.cbSize = sizeof( si );
				if( SHSipInfo( SPI_GETSIPINFO, 0, &si, 0 ) ){
					si.fdwFlags &= ~SIPF_ON;
					fRes = SHSipInfo( SPI_SETSIPINFO, 0, &si, 0 );
				}
			#endif
			_isOpenKeyboarding=false;
		#endif
	}
	long getKeyboardSpaceHeight(){
		//long r=MzGetVisibleDesktopRect().bottom;
		//return r; // A4 170  点讯 320
		return 170+39;//mapGamePos(170);
	}


#ifdef _MZPhone

	void setScreenAutoOff(){
		//SetScrennAutoOff();
		SetScreenAutoOff();
	}

	bool getIsLockPhoneStatus(){
		return 0!=IsLockPhoneStatus();
	}

	void setSomeHostKey(void* winMainFormHandle){
		SetScreenAlwaysOn((HWND)winMainFormHandle);
		RegisterShellMessage((HWND)winMainFormHandle,WM_MZSH_CLICK_M_KEY);
		HoldShellUsingSomeKeyFunction((HWND)winMainFormHandle,MZ_HARDKEY_PLAY | MZ_HARDKEY_VOLUME_UP | MZ_HARDKEY_VOLUME_DOWN);
		
	}

	void unSetSomeHostKey(void* winMainFormHandle){
		setScreenAutoOff(); 
		UnRegisterShellMessage((HWND)winMainFormHandle,WM_MZSH_CLICK_M_KEY);
		UnHoldShellUsingSomeKeyFunction((HWND)winMainFormHandle,MZ_HARDKEY_PLAY | MZ_HARDKEY_VOLUME_UP | MZ_HARDKEY_VOLUME_DOWN);
	}


	extern void VGame_outDebugInfo(const char* text,bool isClearOld);

	String getFileHash(const char* fileName){
		FileInputStream smFile(fileName);
		unsigned long fSize=smFile.getDataSize();
		String strMD5;
		strMD5.resize(32);
		getMD5StdHex32((void*)smFile.read(fSize), fSize, &strMD5[0]);
		::VGame_outDebugInfo(strMD5.c_str(),false);
		return strMD5; 
	}
#endif//_MZPhone

#endif//WINCE

extern const void* getMainFormHandle();
void exitApp(){
	static bool isClosed=false;
	if (isClosed) return;
	isClosed=true;
	#ifdef WINCE
		autoRotateScreenRestore();
	#endif
	#ifdef _MZPhone
		ShowMzTopBar();
		::Sleep(200);
	#endif
	//::PostQuitMessage(0);
	::PostMessage((HWND)getMainFormHandle(),WM_CLOSE,0,0);
}

#ifdef WINDOWS

	//struct _TAutoInitApp_sysImport
	_TAutoInitApp_sysImport::_TAutoInitApp_sysImport(){ 
		static bool isInit=false;
		if (isInit) return;
		isInit=true;

		#ifdef WINCE
			CoInitializeEx(0, COINIT_MULTITHREADED);
			//setDVS(false); 
			loadVolumeSet();
			//setActiveStatus(0,true);
		#endif 
	}
	_TAutoInitApp_sysImport::~_TAutoInitApp_sysImport(){ 
		static bool isInit=false;
		if (isInit) return;
		isInit=true;

		#ifdef WINCE
			setActiveStatus(0,false);
			saveVolumeSet();
			//setDVS(true); 
			CoUninitialize();
		#endif
	}
#endif
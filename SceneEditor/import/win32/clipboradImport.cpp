//clipboradImport.cpp

#include "clipboradImport.h"
#include "SysImportType.h"
#ifdef Set
	#undef Set
#endif
#include "Windows.h"

static String var_text;


bool clipboard_Open(){
	return 0!=::OpenClipboard(0);
}

void clipboard_Close(){
	CloseClipboard();
}

bool clipboard_SetBuffer(WORD Format,const void* Buffer,long size){
	if (!clipboard_Open()) return false;
	HANDLE Data=0;
	void* DataPtr=0;

#ifndef WINCE
	Data = ::GlobalAlloc(GMEM_MOVEABLE+GMEM_DDESHARE, size);
	if (Data==0) return false;
	DataPtr = ::GlobalLock(Data);
	if (DataPtr==0) {
		::GlobalFree(Data);
		return false;
	}
#else
	Data = ::LocalAlloc(LMEM_FIXED, size);
	if (Data==0) return false;
	DataPtr = (void*)Data;
#endif

	memcpy(DataPtr,Buffer, size);
	::SetClipboardData(Format, Data);

#ifndef WINCE
	if (DataPtr!=0)
		::GlobalUnlock(Data);
	if (Data!=0)
		::GlobalFree(Data);
#else
	::LocalFree(Data);
#endif

	clipboard_Close();
	
	return true;
}

bool clipborad_setText(const char* text){
	var_text=text;
#ifndef WINCE
	return clipboard_SetBuffer(CF_TEXT, text, (long)strlen(text) + 1);
#else
	wchar_t* wText=ansiCharToUnicode(text);
	return clipboard_SetBuffer(CF_UNICODETEXT, wText, ((long)wcslen(wText) + 1)*2);
	delete[]wText;
#endif

}

	bool clipborad_getWinClipboradText(String& out_str){
		if (!clipboard_Open()) return false;

		#ifndef WINCE
			HANDLE Data = ::GetClipboardData(CF_TEXT);
			if (Data==0) return false;
			void* DataPtr=::GlobalLock(Data);
			if (DataPtr==0) return false;

			out_str = (const char*)(DataPtr);
			::GlobalUnlock(Data);
		#else
			wchar_t* DataPtr=(wchar_t*)::GetClipboardData(CF_UNICODETEXT);
			char* text=unicodeToAnsiChar(DataPtr);
			out_str = text;
			delete[]text;
		#endif

		clipboard_Close();
		return true;
	}
const char* clipborad_getText(){
	String text;
	if (clipborad_getWinClipboradText(text)){
		var_text=text;
	}
	return 	var_text.c_str();
}


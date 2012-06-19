//hPixels32ImportBase.cpp

#include "../importInclude/SysImport.h"
#include "../importInclude/hPixels32ImportBase.h"
#ifdef Set
	#undef Set
#endif
#include <Windows.h>
#include <assert.h>
#ifdef WINCE
    #include <aygshell.h> //SHGetUIMetrics
    #pragma comment (lib, "aygshell.lib")
#endif
#ifdef _MZPhone
	#include <FontHelper.h>
#endif

HBITMAP	create_DIB(long nWidth , long nHeight,long nBitCount,void*& out_pBits,long& out_WidthBytes)
{
	out_WidthBytes=0;
	out_pBits=0;

	BITMAPINFOHEADER	_BI;
	memset(&_BI,0,sizeof(BITMAPINFOHEADER));
	_BI.biSize= sizeof(BITMAPINFOHEADER);
	_BI.biWidth=nWidth;
	_BI.biHeight=nHeight;
	_BI.biPlanes=1;
	_BI.biBitCount=(WORD)nBitCount;
	_BI.biCompression=0;
	_BI.biSizeImage=0;
	_BI.biXPelsPerMeter=0;
	_BI.biYPelsPerMeter=0;
	_BI.biClrUsed=0;
	_BI.biClrImportant=0;
	HBITMAP _hBmp=CreateDIBSection(0,(BITMAPINFO*)&_BI,DIB_RGB_COLORS,(void**)&out_pBits,0,0);
	if (_hBmp!=0)
	{
		assert(out_pBits!=0);
		BITMAP bm;	
		::GetObject(_hBmp,sizeof(BITMAP),&bm);
		_BI.biSizeImage=bm.bmHeight*bm.bmWidthBytes;
		_BI.biClrUsed=1<<nBitCount;
		out_WidthBytes=bm.bmWidthBytes;
	}
	else
		assert(out_pBits==0);

	return _hBmp;
}

void delete_DIB(HBITMAP hDIB)
{
	if (hDIB!=0) DeleteObject(hDIB);
}

HDC create_DIB_DC(HBITMAP hDIB,HGDIOBJ& out_OldObject)
{
	assert(hDIB!=0);
	HDC MemDC=0;

	MemDC=CreateCompatibleDC(0);
	#ifndef WINCE
		SetMapMode(MemDC,MM_TEXT);
	#endif
	assert(out_OldObject==0);
	out_OldObject=SelectObject(MemDC,hDIB);

	assert(MemDC!=0);
	return MemDC;
}

void delete_DIB_DC(HDC MemDC,HGDIOBJ& out_OldObject)
{
	if (MemDC!=0)
	{
		SelectObject(MemDC,out_OldObject);
		out_OldObject=0;
		DeleteDC(MemDC);
	}
}



//////////////////////////////////////////


struct CDIB32
{
	HDC		m_MemDC;  //必须在第一个位置

	HBITMAP m_hDIB;
	HGDIOBJ	m_OldObject;

	void*   m_DIBMem;
	long    m_byte_width;
	long    m_width;
	long    m_height;
public:
	explicit CDIB32(const long width,const long height,long colorBit)
	:m_MemDC(0),m_hDIB(0),m_OldObject(0), m_DIBMem(0),m_byte_width(0),m_width(width),m_height(height){
	
		assert(m_MemDC==0);
		assert(m_hDIB==0);

		m_hDIB=create_DIB(m_width,m_height,colorBit,m_DIBMem,m_byte_width);
		assert(m_hDIB!=0);
		lock_DC();
		//result.reversal();
	}

	~CDIB32(){
		unloack_DC();
		assert(m_MemDC==0);
		if (m_hDIB!=0){
			delete_DIB(m_hDIB);
			m_hDIB=0;
			m_DIBMem=0; 
			m_byte_width=0;
			m_width=0;
			m_height=0;
		}
	}

	HDC  lock_DC(){
		assert(m_hDIB!=0);
		assert(m_MemDC==0);

		m_MemDC=create_DIB_DC(m_hDIB,m_OldObject);
		return m_MemDC;
	}

	HDC  getDC(){
		if (m_MemDC==0)
			lock_DC();
		return m_MemDC;
	}

	void unloack_DC(){
		if (m_MemDC!=0)
		{
			delete_DIB_DC(m_MemDC,m_OldObject);
			m_MemDC=0;
		}
	}


};


void getNewPixels_ImportEx(long width,long height,void** pColorData,long* byte_width,void** out_contextHandleImport,long colorBit){
	CDIB32* dib=new CDIB32(width,height,colorBit);
	*pColorData=dib->m_DIBMem;
	*byte_width=dib->m_byte_width;
	*out_contextHandleImport=dib; 
}
void getNewPixels_Import(long width,long height,void** pColorData,long* byte_width,void** out_contextHandleImport){
	getNewPixels_ImportEx(width,height,pColorData,byte_width,out_contextHandleImport,32);
}


void deletePixels_Import(void* contextHandleImport,void* pColorData){
	CDIB32* dib=(CDIB32*)(contextHandleImport);
	if (dib!=0)
		delete dib;
}

TImageHandleImport _loadImageFile(const char* fileName,const char* pathName,long* out_width,long* out_height){
	return 0;
}


void deleteImageHandle(TImageHandleImport imageHandleImport){
	assert(false); //can not call
}

void drawImage(void* contextHandleImport,TImageHandleImport imageHandleImport){	
	assert(false); //can not call
}

void ContextHandle_GetDrawTextSize(void* contextHandleImport,const char* text,long* out_width,long* out_height){
	CDIB32* dib=(CDIB32*)(contextHandleImport);
	HDC dc=dib->getDC();

	SIZE TextSize;
	TextSize.cx=0; TextSize.cy=0;
#ifndef WINCE
	GetTextExtentPoint32(dc,text,(int)strlen(text),&TextSize);
#else
	wchar_t* wText=ansiCharToUnicode(text);
	GetTextExtentPoint32(dc,wText,(int)wcslen(wText),&TextSize);
	delete[]wText;
#endif
    *out_width=TextSize.cx;
    *out_height=TextSize.cy;
}

void ContextHandle_GetDrawATextSize(void* contextHandleImport,int aChar,long* out_width,long* out_height){
	unsigned char text[3];
    if (aChar<=asciiCharMaxValue){
		text[0]=aChar;
		text[1]=0;
	}
	else{
		text[0]=aChar;
		text[1]=aChar>>8; 
		text[2]=0;
	}
	ContextHandle_GetDrawTextSize(contextHandleImport,(const char*)&text[0],out_width,out_height);
}


const BYTE	 hFont_DefultCharset=DEFAULT_CHARSET; // GB2312_CHARSET; //

void ContextHandle_setFont(void* contextHandleImport,const char* fontName,long fontSize,int isBlod){

	double fFontSize=mapGamePos(fontSize);
#ifndef WINCE
	fFontSize*=0.8;
#endif

	CDIB32* dib=(CDIB32*)(contextHandleImport);
	HDC dc=dib->getDC();

#ifdef _MZPhone
	//FontHelper::ClearCachedFonts();
	int lfWeight	= isBlod ? FW_BOLD : FW_NORMAL;
	//BOOL lfUnderline	= isUnderline ? 1 : 0;
	HFONT hFont=FontHelper::GetFont(fFontSize,lfWeight,0,0,FONT_ROTATION_FLAG_0,FONT_QUALITY_DEFAULT);
	if (hFont){
		HGDIOBJ hOldFont= SelectObject(dc, hFont);
	}
#else
	LOGFONT LogFont;
	memset(&LogFont,0,sizeof(LOGFONT));
	LONG  dwFontHeight = (long)(fFontSize*LOGPIXELSY*(1.0/72) +0.5);

	LogFont.lfHeight			= dwFontHeight;  //MulDiv(fontSize,GetDeviceCaps(dc,LOGPIXELSY),72);
	LogFont.lfWidth				= 0;
	LogFont.lfEscapement		= 0;
	LogFont.lfOrientation		= 0;
	LogFont.lfWeight			= isBlod ? FW_BOLD : FW_NORMAL;
	LogFont.lfItalic			= 0;//m_IsItalic ? 1 : 0;
	LogFont.lfUnderline			= 0;//isUnderline ? 1 : 0;
	LogFont.lfStrikeOut			= 0;//m_IsStrikeOut ? 1 : 0;
	LogFont.lfCharSet			= hFont_DefultCharset;//m_Charset;
	LogFont.lfOutPrecision		= OUT_DEFAULT_PRECIS; 
	LogFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS; 
	LogFont.lfQuality			= DEFAULT_QUALITY;
	LogFont.lfPitchAndFamily	= FIXED_PITCH;
	if (fontName==0) fontName=getDefaultFontName();
#ifdef WINCE
	wchar_t* wFontName=ansiCharToUnicode(fontName);
	size_t sl=wcslen(wFontName)+1;
	assert(sl<=LF_FACESIZE);
	memcpy(&LogFont.lfFaceName[0],wFontName,sl*sizeof(wchar_t));
	delete []wFontName;
#else
	size_t sl=strlen(fontName)+1;
	assert(sl<=LF_FACESIZE);
	memcpy(&LogFont.lfFaceName[0],fontName,sl);
#endif
	HFONT hFont = CreateFontIndirect(&LogFont);
	if (hFont){
		HGDIOBJ hOldFont= SelectObject(dc, hFont);
		if (hOldFont!=0)
			DeleteObject(hOldFont);
	}
#endif
	SetTextColor(dc, RGB(255, 255, 255));
	SetBkColor(dc, RGB(0, 0, 0));
}


void ContextHandle_DrawText(void* contextHandleImport,const char* text){
	CDIB32* dib=(CDIB32*)(contextHandleImport);
	HDC dc=dib->getDC();

    /*test 
    dc=GetWindowDC(0);
	SetTextColor(dc, RGB(255, 255, 255));
	SetBkColor(dc, RGB(0, 0, 0));//*/
#ifndef WINCE
	TextOut(dc,0,0,text,(int)strlen(text));
#else
	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=dib->m_width;
	rect.bottom=dib->m_height;
	wchar_t* wText=ansiCharToUnicode(text);
	DrawText(dc,wText,(int)wcslen(wText),&rect,0);
	//or ExtTextOut
	delete[]wText;
#endif

}

void ContextHandle_DrawAText(void* contextHandleImport,int aChar){
	unsigned char text[3];
    if (aChar<=asciiCharMaxValue){
		text[0]=aChar;
		text[1]=0;
	}
	else{
		text[0]=aChar;
		text[1]=aChar>>8; 
		text[2]=0;
	}
	ContextHandle_DrawText(contextHandleImport,(const char*)&text[0]);
}

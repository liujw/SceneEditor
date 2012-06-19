
#include "DDrawImport.h"
#include "hDraw.h"
#ifdef Set
	#undef Set
#endif
#include <ddraw.h>

//#define  IS_DOUBLE_BUFFER

typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned long  UInt32;

#ifndef _MZPhone

#ifdef WINCE
	#define DDFLIP_WAIT 0
	#define DDLOCK_WAIT 1

	static HRESULT PASCAL EnumFunction(LPDIRECTDRAWSURFACE pSurface,
				 LPDDSURFACEDESC lpSurfaceDesc,  LPVOID  lpContext) {
		static BOOL bCalled = FALSE;
		if (!bCalled) {
			*((LPDIRECTDRAWSURFACE *)lpContext) = pSurface;
			bCalled = TRUE;
			return DDENUMRET_OK;
		}
		else {
			OutputDebugString(L"DDEX2: Enumerated more than surface?");
			pSurface->Release();
			return DDENUMRET_CANCEL;
		}
	}
	extern void getNewPixels_ImportEx(long width,long height,void** pColorData,long* byte_width,void** out_contextHandleImport,long colorBit);



#endif

void copy_32bit_to_16bit565(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount);

class CDDraw
{
private:
	typedef   LONG   (*DIRECTDRAWCREATE)(LPGUID,   LPDIRECTDRAW*,   LPUNKNOWN   *);  
	LPDIRECTDRAW			m_ddraw;
    LPDIRECTDRAWSURFACE		m_ddsprimary; // dd 主表面
    LPDIRECTDRAWSURFACE		m_ddsback;	// dd 缓冲表面
    DDSURFACEDESC			m_ddsd;		// 一个 Direct Draw 表面 描述 struct
	bool					m_IS_USER_BACK_BUFFER;
	HINSTANCE				m_pDLL;
	DIRECTDRAWCREATE		m_DirectDrawCreate;

	long m_ScreenWidth;
	long m_ScreenHeight;
	long m_ScreenColorBit;
	void * m_main_window_handle;

	bool flip(long bltWidth,long bltHeight){
        if (m_ddsback==0) return true;
		if (m_IS_USER_BACK_BUFFER){
			if ((bltWidth>0)&&(bltHeight>0)){
				RECT rt; rt.left=0; rt.top=0;
				rt.right=bltWidth; rt.bottom=bltHeight;
				#ifdef WINCE
					HRESULT ddrval=m_ddsprimary->Blt(&rt,m_ddsback, &rt,DDBLT_WAITNOTBUSY, NULL);
				#else
					HRESULT ddrval=m_ddsprimary->BltFast(0,0, m_ddsback, &rt, NULL);
				#endif
				return (ddrval == DD_OK);
			}else
				return true;
		}else{ //flip
			while( true ) {
				HRESULT ddrval= m_ddsprimary->Flip( NULL, DDFLIP_WAIT );// 交换表面
				if( ddrval == DD_OK )
					return true;
				if( ddrval == DDERR_SURFACELOST ) {
					ddrval = m_ddsprimary->Restore();
					if( ddrval!= DD_OK )
						return false;
				}
				if( ddrval != DDERR_WASSTILLDRAWING ){
					return false;
				}
				::Sleep(0);
			}	
		}
	}
	bool lock(LPDIRECTDRAWSURFACE dd,_TPixelsRef& out_pixelsRef){
		DDSURFACEDESC ddsdDest; 
		memset(&ddsdDest,0,sizeof(ddsdDest));
		ddsdDest.dwSize=sizeof(ddsdDest);
		//ddsdDest.dwFlags=DDSD_LPSURFACE | DDSD_PITCH | DDSD_WIDTH | DDSD_HEIGHT;
		HRESULT ddrval=dd->Lock(NULL,&ddsdDest,DDLOCK_WAIT,NULL);
		if( ddrval == DDERR_SURFACELOST ) {
			ddrval = m_ddsprimary->Restore();
			if( ddrval!= DD_OK )
				return false;
			dd->Lock(NULL,&ddsdDest,DDLOCK_WAIT,NULL);
		}

		if (ddsdDest.lpSurface==0) return false;

		if (ddsdDest.lPitch<=4){
			out_pixelsRef.pdata=ddsdDest.lpSurface;//获得页面首地址
			out_pixelsRef.width=ddsdDest.dwHeight;
			out_pixelsRef.height=ddsdDest.dwWidth;
			out_pixelsRef.byte_width=-out_pixelsRef.width*ddsdDest.lPitch;
			((Pixels32Ref&)out_pixelsRef).reversal();
		}else{
			out_pixelsRef.pdata=ddsdDest.lpSurface;//获得页面首地址
			out_pixelsRef.width=ddsdDest.dwWidth;
			out_pixelsRef.height=ddsdDest.dwHeight;
			out_pixelsRef.byte_width=ddsdDest.lPitch;
		}

		return true;
	}
	void unLock(LPDIRECTDRAWSURFACE dd){
		dd->Unlock(NULL);//解锁页面
	}
public:
    CDDraw():m_ddraw(0),m_ddsprimary(0),m_ddsback(0),m_pDLL(0),m_DirectDrawCreate(0){
		m_ScreenWidth=0;
		m_ScreenHeight=0;
		m_ScreenColorBit=0;
		m_main_window_handle=0;
		m_pDLL=::LoadLibrary(
			#ifdef WINCE
				_T("ddraw.dll"));
			#else
				("ddraw.dll"));
			#endif
		if (m_pDLL!=0)
			m_DirectDrawCreate=(DIRECTDRAWCREATE)GetProcAddress(m_pDLL,
				#ifdef WINCE
					_T("DirectDrawCreate"));
				#else
					("DirectDrawCreate"));
				#endif
				
	}
    ~CDDraw()  { 
		clear(); 
		if (m_pDLL!=0)
			::FreeLibrary(m_pDLL);
	}

    void  clear(){
        if (m_ddsback!=0){ // 释放缓冲表面
            m_ddsback->Release();
            m_ddsback = 0;
        }
        if (m_ddsprimary!=0){ //释放主表面
            m_ddsprimary->Release();
            m_ddsprimary = 0;
        }
        if (m_ddraw!=0){ //释放DDraw
            m_ddraw->Release();
            m_ddraw = 0;
        }
    }

    bool setScreenMode(long ScreenWidth,long ScreenHeight,long ScreenColorBit){
		#ifdef WINCE
			HRESULT result=m_ddraw->SetDisplayMode(ScreenWidth,ScreenHeight, ScreenColorBit,0,0);
		#else
			HRESULT result=m_ddraw->SetDisplayMode(ScreenWidth,ScreenHeight, ScreenColorBit);
		#endif
        return !(FAILED(result));
    }

    bool initDDraw(long ScreenWidth,long ScreenHeight,long ScreenColorBit,void * main_window_handle){
        clear();		
		m_ScreenWidth=ScreenWidth;
		m_ScreenHeight=ScreenHeight;
		m_ScreenColorBit=ScreenColorBit;
		m_main_window_handle=main_window_handle;

		m_IS_USER_BACK_BUFFER=true;//getIsCanSafeUseDDraw();

        // 该函数在初次创建完窗体和进入事件循环时调用
        // 进行DDraw的初始化

        // 创建基本 IDirectDraw 界面
		if (m_DirectDrawCreate==0) return false;
        if (FAILED(m_DirectDrawCreate(0, &m_ddraw, 0)))
            return false;
        // 查询 IDirectDraw4 界面
        //if (FAILED(lpdd_temp->QueryInterface(IID_IDirectDraw4,(LPVOID *)&lpdd4)))
        //    return false;

        // 设置协作等级为全屏幕
		if ( FAILED(m_ddraw->SetCooperativeLevel((HWND)main_window_handle,DDSCL_NORMAL))){ // DDSCL_FULLSCREEN
            ;//return false;  不用推出
		}
        //设置显示模式
		if (!setScreenMode(ScreenWidth,ScreenHeight,ScreenColorBit)){//设置默认模式
            ;//return false;不用推出
        }

        //清除Direct Draw 表面 描述，设置大小
        memset(&m_ddsd,0,sizeof(m_ddsd)); 
        m_ddsd.dwSize=sizeof(m_ddsd); 
        //设置正确的属性标记
        m_ddsd.dwFlags = DDSD_CAPS ;// |DDSD_HEIGHT | DDSD_WIDTH;
		#ifdef IS_DOUBLE_BUFFER
			m_ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
			m_ddsd.dwBackBufferCount = 1;//设置缓冲表面数目（不包括主表面）
		#endif
		//m_ddsd.dwWidth = ScreenWidth;
		//m_ddsd.dwHeight = ScreenHeight;
        m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE ;//| DDSCAPS_FLIP;
        //创建主表面
        if (FAILED(m_ddraw->CreateSurface(&m_ddsd,&m_ddsprimary, 0)))
            return false;
		#ifdef IS_DOUBLE_BUFFER
			#ifndef WINCE
				//现在请求主表面的附加表面
				m_ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
				//得到附加的缓冲表面  不是必需的
				if (FAILED(m_ddsprimary->GetAttachedSurface(&m_ddsd.ddsCaps, &m_ddsback)))
					m_ddsback=0; //附加表面申请不成功 但还可以执行
			#else
					//通过遍历与Primary Surface相连接的Surface来获得backBuffer Surface的指针
				if (FAILED(m_ddsprimary->EnumAttachedSurfaces(&m_ddsback,EnumFunction))){
						m_ddsback=0;
				}
			#endif
		#endif
		if ((m_ddsback==0)&& m_IS_USER_BACK_BUFFER){
		    DDSURFACEDESC ddsd;
			memset(&ddsd, 0, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
			ddsd.dwWidth = ScreenWidth;
			ddsd.dwHeight = ScreenHeight;
			if (FAILED(m_ddraw->CreateSurface( &ddsd, &m_ddsback,  NULL ))){
				m_ddsback=0;
			}
		}
        return true;
    }

    bool drawFrame(const _TPixelsRef* src32,long* pDstRect,long rectCount){
        if (m_ddsprimary==0) return false;

        //if (m_ddraw->TestCooperativeLevel()==DD_OK)//是否需要恢复表面
	    //    m_ddraw->RestoreAllSurfaces(); 
        //else
        //    return false;

        LPDIRECTDRAWSURFACE dd=0;
        if (m_ddsback!=0)
            dd=m_ddsback;
        else
            dd=m_ddsprimary;
        
		//*
		_TPixelsRef  tDst; 
		if (!lock(dd,tDst)) return false;

		copy_32bit_to_16bit565(tDst,*src32,pDstRect,rectCount);
		unLock(dd);//解锁离屏页面
		//*/
		return flip(src32->width,src32->height);
	}
	bool setActive(bool isActive){
		if (!isActive){
			clear();
			return true;
		}else{		
			if (m_ddsprimary!=0)
				return true;
			else
				return initDDraw(m_ScreenWidth,m_ScreenHeight,m_ScreenColorBit,m_main_window_handle);
		}
	}
};


#else

/////////////////////////////////////////////////////////////////////////////////////////
#include <Mzfc/MzDDrawOverlay.h>
void copy_32bit_to_32bit(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount);
void copy_32bit_to_32bit_rotary(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount);
void copy_32bit_to_32bit_2To3(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount);

bool isUseDeviceZoom=false;
long DeviceZoomWidth=720;
long DeviceZoomHeight=480;

class MzOverlayExt : public MzDDrawOverlay
{
public:
	void SetDrawRect(const RECT &destRect, const RECT &srcRect);
};


void MzOverlayExt::SetDrawRect(const RECT &destRect, const RECT &srcRect)
{
	m_rcSrc = srcRect;
	m_rcDst = destRect;

	DDCAPS 	ddcaps;
	memset(&ddcaps, 0, sizeof(ddcaps));
	ddcaps.dwSize = sizeof(ddcaps);
	m_lpDDraw->GetCaps(&ddcaps, NULL);
	if (ddcaps.dwAlignSizeSrc != 0)
	{
		m_rcSrc.right += m_rcSrc.right % ddcaps.dwAlignSizeSrc;
	}

	int stretchFactor = ddcaps.dwMinOverlayStretch > 1000 ? ddcaps.dwMinOverlayStretch : 1000;
	m_rcDst.left = (m_rcDst.left * stretchFactor + 999) / 1000;
	m_rcDst.top = m_rcDst.top * stretchFactor / 1000;
	m_rcDst.right  = (m_rcDst.right * stretchFactor + 999) / 1000;
	m_rcDst.bottom = m_rcDst.bottom * stretchFactor / 1000;
	if (ddcaps.dwAlignSizeDest != 0)
	{
		m_rcDst.right = (int)((m_rcDst.right + ddcaps.dwAlignSizeDest - 1) / ddcaps.dwAlignSizeDest) * ddcaps.dwAlignSizeDest;
	}

	m_x = m_rcDst.left;
	m_y = m_rcDst.top;

	bool visible = m_visible;
	if (visible)
	{
		HideOverlay();
		Sleep(20);
	}
	m_lpDDSOverlay->UpdateOverlay(&m_rcSrc, m_lpDDSPrimary, &m_rcDst, 0, NULL);
	if (visible)
	{
		ShowOverlay();
	}
}


class CDDraw{
private:
	MzOverlayExt* m_ddrawOverlay;
	long	m_oldWidth;
	long	m_oldHeight;
	void *	m_main_window_handle;
	bool lock(_TPixelsRef& out_pixelsRef){
		if (m_ddrawOverlay==0) return false;

		DDSURFACEDESC ddsdDest; 
		memset(&ddsdDest,0,sizeof(ddsdDest));
		ddsdDest.dwSize=sizeof(ddsdDest);
		//ddsdDest.dwFlags=DDSD_LPSURFACE | DDSD_PITCH | DDSD_WIDTH | DDSD_HEIGHT;
		if (!m_ddrawOverlay->LockData(&ddsdDest)) return false;
		if (ddsdDest.lpSurface==0) return false;

		if (ddsdDest.lPitch<=4){
			out_pixelsRef.pdata=ddsdDest.lpSurface;//获得页面首地址
			out_pixelsRef.width=ddsdDest.dwHeight;
			out_pixelsRef.height=ddsdDest.dwWidth;
			out_pixelsRef.byte_width=-out_pixelsRef.width*4;//ddsdDest.lPitch;
			((Pixels32Ref&)out_pixelsRef).reversal();
		}else{
			out_pixelsRef.pdata=ddsdDest.lpSurface;//获得页面首地址
			out_pixelsRef.width=ddsdDest.dwWidth;
			out_pixelsRef.height=ddsdDest.dwHeight;
			out_pixelsRef.byte_width=ddsdDest.lPitch;
		}
		return true;
	}
	void unLock(){
		if (m_ddrawOverlay!=0)
			m_ddrawOverlay->UnLockData(false);
	}
public:
	CDDraw():m_ddrawOverlay(0){ }
    ~CDDraw()  { setActive(false); }
	bool initDDraw(long ScreenWidth,long ScreenHeight,long ScreenColorBit,void * main_window_handle){
		m_oldWidth=ScreenWidth;
		m_oldHeight=ScreenHeight;
		m_main_window_handle=main_window_handle;
		return true;// return setActive(true);
	}
    bool drawFrame(const _TPixelsRef* src32,long* pDstRect,long rectCount){
		_TPixelsRef  tDst;
		if (!lock(tDst))
			return false;

		Vector<long> rectList;
		if ((rectCount>0)&&(tDst.width*3==m_oldWidth*2)&&(tDst.height*3==m_oldHeight*2)){
			rectList.resize(rectCount*4);
			for (long i=0;i<rectCount*4;++i){
				rectList[i]=pDstRect[i]*2/3;
			}
			pDstRect=&rectList[0];
		}
		//拷贝数据到显存
		#ifdef CANVAS_COLOR_RGB16
			//copy_16bit_to_16bit(tDst,*src16,pDstRect,rectCount);
		#else
			if ((src32->width*3==tDst.width*2)&&(src32->height*3==tDst.height*2)){
				copy_32bit_to_32bit_2To3(tDst,*src32,pDstRect,rectCount);
			} else  if ((tDst.width!=tDst.height)&&(tDst.width==src32->height)&&(tDst.height==src32->width)){
				copy_32bit_to_32bit_rotary(tDst,*src32,pDstRect,rectCount);
			}else {
				copy_32bit_to_32bit(tDst,*src32,pDstRect,rectCount);
			}
		#endif
		unLock();
		return true;
	}
	bool setActive(bool isActive){
		if (!isActive){
			if (m_ddrawOverlay==0)
				return true;
			m_ddrawOverlay->HideOverlay();
			m_ddrawOverlay->Clear();
			delete m_ddrawOverlay;
			m_ddrawOverlay=0;
			return true;
		}else{
			if (m_ddrawOverlay!=0)
				return true;
			m_ddrawOverlay=new MzOverlayExt();

			#ifdef CANVAS_COLOR_RGB16
				OverlayPixelFormat pfType=PixFmtRGB565;
			#else
				OverlayPixelFormat pfType=PixFmtRGB24;
			#endif
			if (isUseDeviceZoom){
				long sw=DeviceZoomWidth; long sh=DeviceZoomHeight;
				if (g_isVerticalScreenImport){
					long t=sh; sh=sw; sw=t;
				}
				if (!m_ddrawOverlay->Init((HWND)m_main_window_handle,0,0,sw,sh,0,15,0,false,pfType))
					return false;
				RECT destRect; destRect.left=0; destRect.top=0; destRect.right=m_oldWidth; destRect.bottom=m_oldHeight;
				RECT srcRect; srcRect.left=0; srcRect.top=0; srcRect.right=sw; srcRect.bottom=sh;
				m_ddrawOverlay->SetDrawRect(destRect,srcRect);
			}else{
				if (!m_ddrawOverlay->Init((HWND)m_main_window_handle,0,0,m_oldWidth,m_oldHeight,0,15,0,false,pfType))
					return false;
			}
			
			m_ddrawOverlay->ShowOverlay();
			return true;
		}
	}
};

#endif


/////////////////////////////////////////////////////////////////////////////////////////

CDDrawImprot::CDDrawImprot() 
: m_Import(0) {
}


bool CDDrawImprot::intiScreen(long ScreenWidth,long ScreenHeight,long ScreenColorBit,void * main_window_handle){
    if (m_Import==0)
        m_Import=new CDDraw();
    bool result=((CDDraw*)m_Import)->initDDraw(ScreenWidth,ScreenHeight,ScreenColorBit,main_window_handle);
    if (!result)
        clearImportPointer();
    return result;
}

void CDDrawImprot::clearImportPointer(){
    if (m_Import!=0){
        CDDraw* Import=((CDDraw*)m_Import);
        m_Import=0;
        delete Import;
    }
}

CDDrawImprot::~CDDrawImprot(){
    clearImportPointer();
}

bool CDDrawImprot::drawFrame(const _TPixelsRef* src,long* pDstRect,long rectCount){
    if (m_Import!=0)
        return ((CDDraw*)m_Import)->drawFrame(src,pDstRect,rectCount);
	else
		return false;
}

bool CDDrawImprot::setActive(bool isActive){
    if (m_Import!=0)
        return ((CDDraw*)m_Import)->setActive(isActive);
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////////

void copy_32bit_to_16bit565(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount){
	if ((pDstRect==0)&&(rectCount==0)){
		copy_32bit_to_16bit565_ErrorDiffuse((Pixels32Ref&)dstRef,16,true,(Pixels32Ref&)srcRef);
		_TPixelsRef ref=dstRef;
		ref.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*srcRef.height;
		ref.height-=srcRef.height;
		//fill((ref,);
	}else{
		_TPixelsRef sRef;
		_TPixelsRef dRef;
		sRef.byte_width=srcRef.byte_width;
		dRef.byte_width=dstRef.byte_width;
		for (long i=0;i<rectCount;++i){
			RECT& r=*(RECT*)pDstRect;
			sRef.pdata=(UInt8*)srcRef.pdata+srcRef.byte_width*(r.top)+4*(r.left);
			sRef.width=r.right-r.left;
			sRef.height=r.bottom-r.top;
			dRef.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*(r.top)+2*(r.left);
			dRef.width=r.right-r.left;
			dRef.height=r.bottom-r.top;
			copy_32bit_to_16bit565_ErrorDiffuse((Pixels32Ref&)dRef,16,true,(Pixels32Ref&)sRef);
			pDstRect+=4;
		}
	}
}



	void copy_16bit_to_16bit_fast_line(UInt16* dst,const UInt16* src,long width){
		UInt16* dstEnd =&dst[width];
		UInt16* dstEndFast =&dst[(width & (~3))];
		for (; dst != dstEndFast; dst+=4,src+=4){
			dst[0]=src[0];
			dst[1]=src[1];
			dst[2]=src[2];
			dst[3]=src[3];
		}
		for (; dst != dstEnd; ++dst,++src)
			*dst=*src;
    }

template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
void tm_copy_32bit_to_16bit_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
	long width=dstRef.width; if (width>srcRef.width) width=srcRef.width;
	long height=dstRef.height; if (height>srcRef.height) height=srcRef.height;
	const UInt32* sline=(const UInt32*)srcRef.pdata;
	TDstColor16* dline=(TDstColor16*)dstRef.pdata;

    for (long y=0;y<height;++y){
        tm_copy_32bit_to_16bit_fast_line<TDstColor16,isPack,rbit,gbit,bbit>(dline,sline,width);
        (UInt8*&)sline+=srcRef.byte_width;
        (UInt8*&)dline+=dstRef.byte_width;
    }
}

void copy_16bit_to_16bit(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef){
	//long width=min(dstRef.width,srcRef.width);
	//long height=min(dstRef.height,srcRef.height);
	//void*
	//for 
}

void copy_16bit_to_16bit(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount){
	if ((pDstRect==0)&&(rectCount==0)){
		copy_16bit_to_16bit(dstRef,srcRef);
		_TPixelsRef ref=dstRef;
		ref.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*srcRef.height;
		ref.height-=srcRef.height;
		//fill((ref,);
	}else{
		_TPixelsRef sRef;
		_TPixelsRef dRef;
		sRef.byte_width=srcRef.byte_width;
		dRef.byte_width=dstRef.byte_width;
		for (long i=0;i<rectCount;++i){
			RECT& r=*(RECT*)pDstRect;
			sRef.pdata=(UInt8*)srcRef.pdata+srcRef.byte_width*(r.top)+2*(r.left);
			sRef.width=r.right-r.left;
			sRef.height=r.bottom-r.top;
			dRef.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*(r.top)+2*(r.left);
			dRef.width=r.right-r.left;
			dRef.height=r.bottom-r.top;
			copy_16bit_to_16bit(dRef,sRef);
			pDstRect+=4;
		}
	}
}


void copy_32bit_to_32bit(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount){
	if ((pDstRect==0)&&(rectCount==0)){
		zoom1To1_fast((Pixels32Ref&)dstRef,(Pixels32Ref&)srcRef,true);
		_TPixelsRef ref=dstRef;
		ref.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*srcRef.height;
		ref.height-=srcRef.height;
		fill((Pixels32Ref&)ref,cl32Empty);
	}else{
		_TPixelsRef sRef;
		_TPixelsRef dRef;
		sRef.byte_width=srcRef.byte_width;
		dRef.byte_width=dstRef.byte_width;
		for (long i=0;i<rectCount;++i){
			RECT& r=*(RECT*)pDstRect;
			sRef.pdata=(UInt8*)srcRef.pdata+srcRef.byte_width*(r.top)+4*(r.left);
			sRef.width=r.right-r.left;
			sRef.height=r.bottom-r.top;
			dRef.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*(r.top)+4*(r.left);
			dRef.width=r.right-r.left;
			dRef.height=r.bottom-r.top;
			#ifdef CANVAS_COLOR_RGB16
				
			#endif
			zoom1To1_fast((Pixels32Ref&)dRef,(Pixels32Ref&)sRef,true);
			pDstRect+=4;
		}
	}
}

void copy_32bit_to_32bit_rotary(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount){
	copy_rotary270((Pixels32Ref&)dstRef,(Pixels32Ref&)srcRef,true);
}

void copy_32bit_to_32bit_2To3(const _TPixelsRef& dstRef,const _TPixelsRef& srcRef,long* pDstRect,long rectCount){
	long tmpRect[4];
	if ((pDstRect==0)&&(rectCount==0)){
		tmpRect[0]=0;
		tmpRect[1]=0;
		tmpRect[2]=dstRef.width;
		tmpRect[3]=dstRef.height;
		pDstRect=&tmpRect[0];
		rectCount=1;

		_TPixelsRef ref=dstRef;
		long dH=(srcRef.height*3/2);
		ref.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*dH;
		ref.height-=dH;
		fill((Pixels32Ref&)ref,cl32Empty);
	}

	_TPixelsRef sRef;
	_TPixelsRef dRef;
	sRef.byte_width=srcRef.byte_width;
	dRef.byte_width=dstRef.byte_width;
	for (long i=0;i<rectCount;++i){
		RECT& r=*(RECT*)pDstRect;
		sRef.pdata=(UInt8*)srcRef.pdata+srcRef.byte_width*(r.top/3*2)+4*(r.left/3*2);
		sRef.width=r.right/3*2-r.left/3*2;
		sRef.height=r.bottom/3*2-r.top/3*2;
		dRef.pdata=(UInt8*)dstRef.pdata+dstRef.byte_width*(r.top)+4*(r.left);
		dRef.width=r.right-r.left;
		dRef.height=r.bottom-r.top;
		zoom2To3_fast((Pixels32Ref&)dRef,(Pixels32Ref&)sRef,true);
		pDstRect+=4;
	}
}
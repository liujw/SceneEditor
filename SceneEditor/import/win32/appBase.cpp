//appBase.cpp
#pragma warning(disable:4996)
#include "appBase.h"
#include "../importInclude/hPixels32ImportBase.h"
#include "../importInclude/SysImport.h"
#ifdef Set
	#undef Set
#endif
#include "hDraw.h" 
#include "mmTimer.h"
#include "DDrawImport.h" 
#include "ThreadImport.h" 

#include <assert.h>
// Windows 头文件:
#include <windows.h>

#ifdef WINCE
	const double FPS=30;
#else
	const double FPS=60;
#endif

const void* getMainFormHandle();


extern void init_MyApplication();
extern void exit_MyApplication();

extern void VGame_outDebugInfo(const char* text,bool isClearOld);

	long DDScreenWidth=0;
	long DDScreenHeight=0;

	enum TMapPosType{ mp_2to1,mp_3to2,mp_4to3,mp_1to1,mp_3to4,mp_2to3,mp_1to2 };
	long MapPosVOld[]={ 2,3,4,1,3,2,1 };
	long MapPosVNew[]={ 1,2,3,1,4,3,2 };
	TZoomMapProc ZoomMapProcV[]={zoom2To1_fast,zoom3To2_fast,zoom4To3_fast,zoom1To1_fast,zoom3To4_fast,zoom2To3_fast,zoom1To2_fast};
#ifdef WINCE
	#include "SoundImport.h"
	extern void setActiveStatus(void* winMainFormHandle,bool isActive);
	extern void getNewPixels_ImportEx(long width,long height,void** pColorData,long* byte_width,void** out_contextHandleImport,long colorBit);
	extern bool isOpenKeyboard();
	extern long getKeyboardSpaceHeight();
	extern void g_netLinkClear();

#endif
	static inline long getScreenMoveHeight(){
		#ifdef WINCE
		if (isOpenKeyboard()){
			const long spaceHeight=getKeyboardSpaceHeight();
			long dMoveHeight=DDScreenHeight/2-spaceHeight;
			if (dMoveHeight<0) dMoveHeight=0;
			return dMoveHeight;
		}else
		#endif
			return 0;
	}

#ifdef _MZPhone
	#include <mzfc_inc.h> 
	#include <UsbNotifyApi.h> 
	#include <CallNotifyApi.h> 

	#define MSG_MZ_HOME_KEY_CLICK 0xC000 
	#define MSG_MZ_CTRLMUSIC_KEY_CLICK 129 
	int g_iUsbNotifyMsg = 0;
    int g_iCallMsg = 0;
    int g_iSmsMsg = 0;
	bool isInUsbAttach=false;
	extern bool getIsLockPhoneStatus();
	extern void setSomeHostKey(void* winMainFormHandle);
	extern void unSetSomeHostKey(void* winMainFormHandle);
#endif


struct TCanvasBaseData:public _TCanvasBaseData{
	TContextDataBase* m_contextDataBase;
	long	m_colorBit;
#ifdef CANVAS_COLOR_RGB16
TCanvasBaseData(long colorBit=16){
#else
TCanvasBaseData(long colorBit=32){
#endif
        pdata=0;
        byte_width=0;
        width=0;
        height=0;
		m_contextDataBase=0;
		m_colorBit=colorBit;
    } 
	must_inline bool isEmpty()const{
		return (pdata==0);
	}
    void resizeFast(long aWidth,long aHeight){
        if ((width!=aWidth)||(height!=aHeight)){
            clear();
			#ifdef WINCE
				getNewPixels_ImportEx(aWidth,aHeight,&pdata,&byte_width,(void **)&m_contextDataBase,m_colorBit);
			#else
				getNewPixels_Import(aWidth,aHeight,&pdata,&byte_width,(void **)&m_contextDataBase);
			#endif
            if (pdata!=0){
                width=aWidth;
                height=aHeight;

				(BYTE*&)pdata+=(height-1)*byte_width;
				byte_width*=-1;
            }
        }
    }
    void clear(){
        if ((m_contextDataBase!=0)||(pdata!=0)){
            if (pdata!=0)
				(BYTE*&)pdata+=(height-1)*byte_width; 
            deletePixels_Import(m_contextDataBase,pdata);
			m_contextDataBase=0;
            pdata=0;
        }
        byte_width=0;
        width=0;
        height=0;
    }
	must_inline HDC getDC()const{
		if (m_contextDataBase==0)
			return 0;
		else
			return (HDC)m_contextDataBase->context;
	}
    ~TCanvasBaseData(){ clear(); }
};


static TApplicationBase* gApplication;
static HINSTANCE  gethInstance();

LRESULT CALLBACK gWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct TFormPrivateData;
TFormPrivateData* gCteateHWND_FormPrivateData=0;
const DWORD MSG_MMTimerMSG_UPDATE=WM_APP+1;
const DWORD MSG_MMTimerMSG_TRY   =MSG_MMTimerMSG_UPDATE+1;


void gFormOnTimerProc(TAbstractTimer* sender,const void* callBackData);

void TApplicationBase::getFullScreenSize(long& out_screenWidth,long& out_screenHeight){
	out_screenWidth=GetSystemMetrics(SM_CXSCREEN);
	out_screenHeight=GetSystemMetrics(SM_CYSCREEN);
}


struct TFormPrivateData{
    String     m_caption;
#ifdef WINCE
    WString    m_caption_w;
#endif
	HWND            m_hWnd;
	bool            m_isColosed;
    IUpdateListenerBase m_updateListener;
    TMMTimer        m_updateTimer;  //改为 thread 来循环 并Post消息 和 Sleep
    bool            m_isUpdateTimerActived;
    TCanvasBaseData m_canvasBase;
    TDateTime       m_updateTime;
	unsigned short  m_wideCharHalf;
	bool			m_isCanFarmeUpdate;
	bool            m_isUseCanvasBack;
	bool            m_isUseMapPos;
	TCanvasBaseData m_canvasBack;
	TMapPosType		m_mapPosType;
	long			m_mapPosMvX0;
	long			m_mapPosMvY0;
	long			m_userWidth;
	long			m_userHeight;
#ifdef WINCE
	bool			m_IS_USE_DDRAW;
	CDDrawImprot    m_ddrawImprot;
	bool            m_isNeedGDIDrawFull;
#endif
    TFormPrivateData():m_hWnd(0),m_isCanFarmeUpdate(true),m_isColosed(false)
#ifdef WIN_TIMER
	,m_updateTimer(0)
#endif
	{
#ifdef WINCE
		m_IS_USE_DDRAW=true;
		m_isNeedGDIDrawFull=false;
		TApplicationBase::getFullScreenSize(DDScreenWidth,DDScreenHeight);

	#ifdef _MZPhone
		/*//复制屏幕
		//TCanvasBaseData m_screenBackCanvas;
		HDC scDC=::GetDC(NULL);
		if (scDC!=0){
			TCanvasBaseData tmpCanvas;
			tmpCanvas.resizeFast(DDScreenWidth,DDScreenHeight);
			::BitBlt(tmpCanvas.getDC(),0,0,tmpCanvas.width,tmpCanvas.height,scDC,0,0,SRCCOPY);

			m_screenBackCanvas.resizeFast(DDScreenHeight,DDScreenWidth);
			::copy_rotary270((Pixels32Ref&)m_screenBackCanvas,(Pixels32Ref&)tmpCanvas);
		}*/
	#endif
#endif
		m_isUseCanvasBack=false;

		m_isUseMapPos=false;
		m_mapPosType=mp_1to1;
		m_mapPosMvX0=0;
		m_mapPosMvY0=0;

	    m_updateTimer.setInterval((long)(1000/FPS));
	    m_updateTimer.setOnTimer(&gFormOnTimerProc,this);
	    m_updateTimer.setEnabled(false);
        m_isUpdateTimerActived=false;
        m_updateTime=0;
		m_wideCharHalf=0;
		initUpdateListener(&m_updateListener);
    }
	void setUpdateListener(const IUpdateListenerBase& updateListener){
        m_updateListener=updateListener;
	}
    void clearForm(){
		m_wideCharHalf=0;
	    m_updateTimer.setEnabled(false);			 
        m_isUpdateTimerActived=false;
        if (m_hWnd!=0){
			#ifdef _MZPhone
				unSetSomeHostKey(m_hWnd);
			#endif
            HWND  hWnd=m_hWnd;
            m_hWnd=0;
		    DestroyWindow(hWnd);
        }
    }
    ~TFormPrivateData(){ 	
		m_isColosed=true;
        clearForm();  
        if (m_updateListener.setNullUpdateListener!=0){
           //todo: 警告 代码写的有bug,主动释放时容易出发异常 让系统自己回收 
			//m_updateListener.setNullUpdateListener(&m_updateListener);
        }
    }

    void createForm(const char* caption,long clientWidth,long clientHeight,long nCmdShow,const char* icon,const char* cursor){
        clearForm();
        m_caption=caption;
		#ifdef WINCE
			wchar_t* wText=ansiCharToUnicode(caption);
			m_caption_w=wText;
			delete[]wText;
		#endif 

		#ifndef WINCE
			if (icon==0) icon=IDI_APPLICATION;
		#endif
		if (cursor==0) cursor=(const char*)IDC_ARROW;

        WNDCLASS wc;
        memset(&wc,0,sizeof(WNDCLASS));
        wc.style=CS_VREDRAW | CS_HREDRAW 
			#ifndef WINCE 
				| CS_OWNDC
			#endif 
			;
        wc.lpfnWndProc=(WNDPROC)gWndProc;
        wc.hInstance=gethInstance();
        wc.lpszMenuName = 0;
		#ifndef WINCE
			wc.lpszClassName =m_caption.c_str();
		#else
			wc.lpszClassName =m_caption_w.c_str();
		#endif

		#ifndef WINCE
			wc.hIcon=LoadIcon(gethInstance(),icon);
			wc.hCursor=LoadCursor(gethInstance(),cursor);
		#else			
			wc.hIcon=LoadIcon(gethInstance(),(LPCWSTR)icon);
			wc.hCursor=LoadCursor(gethInstance(),(LPCWSTR)cursor);
		#endif

        RegisterClass(&wc);

        gCteateHWND_FormPrivateData=this;
		#ifndef WINCE
			m_hWnd = CreateWindow(
				wc.lpszClassName,
				m_caption.c_str(),
				WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU,//WS_OVERLAPPEDWINDOW ;, 
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT, 
				0, 0, wc.hInstance, 0);
		#else
			m_hWnd = CreateWindow(
					wc.lpszClassName,
					m_caption_w.c_str(),
					WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_POPUP,
					0,
					0,
					DDScreenWidth,
					DDScreenHeight, 
					0, 0, wc.hInstance, 0);	

			ShowWindow(m_hWnd,nCmdShow);
			//设定屏幕方向
			setActiveStatus(0,true);
			TApplicationBase::getFullScreenSize(DDScreenWidth,DDScreenHeight);
			setClientRect(0,0,DDScreenWidth,DDScreenHeight);
			#ifdef WINCE
				if (m_IS_USE_DDRAW){
					if (!m_ddrawImprot.intiScreen(DDScreenWidth,DDScreenHeight,16,m_hWnd))
						m_IS_USE_DDRAW=false;
				}
			#endif
			#ifdef _MZPhone
				setSomeHostKey(m_hWnd);
			#endif
			//只运行一个实例
			HANDLE hMutex = NULL;
			hMutex = CreateMutex(NULL, FALSE,(TEXT("4avol_")+m_caption_w).c_str());
			if ((hMutex == NULL)||(GetLastError() == ERROR_ALREADY_EXISTS)) {
				m_isCanFarmeUpdate=false;
				::PostMessage(m_hWnd,WM_CLOSE,0,0);
			}
		#endif
        gCteateHWND_FormPrivateData=0;

        long x0=(DDScreenWidth-clientWidth)/2;
		if (x0<0) x0=0;
		long y0=(DDScreenHeight-clientHeight)/2;
		if (y0<0) y0=0;
        setClientRect(x0,y0,x0+clientWidth,y0+clientHeight);
#ifdef WIN_TIMER
		m_updateTimer.setWinHandle(m_hWnd);
#endif
        ShowWindow(m_hWnd,nCmdShow);
	    m_updateTimer.setEnabled(true);
    }
    void setCaption(const char* caption){
        if (m_caption!=caption){
            m_caption=caption;
            SendMessage(m_hWnd,WM_SETTEXT, 0,(LONG_PTR)(m_caption.c_str()));
        }
    }

	void setIconRes(const char*  resID){
		//HICON hIcon=(HICON) LoadImage( gethInstance(), MAKEINTRESOURCE(resID), IMAGE_ICON,32,32,LR_DEFAULTSIZE); 
        HICON hIcon=LoadIcon(gethInstance(),MAKEINTRESOURCE(resID));
		if (hIcon!=0){
			SendMessage(m_hWnd, WM_SETICON, ICON_BIG,(LPARAM)hIcon); 
		}
	}

	void setCursorRes(const char*  resID){
        HCURSOR hCursor=LoadCursor(gethInstance(),MAKEINTRESOURCE(resID));
		if (hCursor!=0){
			::SetCursor(hCursor);		
		}
	}

    void getClientRect(long& x0,long& y0,long& x1,long& y1) const{
        RECT WinRect;
        ::GetClientRect(m_hWnd,&WinRect);
        x0=WinRect.left;
        y0=WinRect.top;
        x1=WinRect.right;
        y1=WinRect.bottom;
    }
    void setClientRect(long x0,long y0,long x1,long y1){
		m_userWidth=x1-x0;
		m_userHeight=y1-y0;
		#ifdef WINCE
			x0=0;
			y0=0;
			x1=DDScreenWidth;
			y1=DDScreenHeight;
			m_isUseMapPos=setBestMap(m_userWidth,m_userHeight,DDScreenWidth,DDScreenHeight);
			m_isUseCanvasBack=m_isUseMapPos||(m_mapPosType!=mp_1to1);
			if (m_isUseCanvasBack)
				m_canvasBack.resizeFast(DDScreenWidth,DDScreenHeight);
			else
				m_canvasBack.clear();
		#endif
        RECT clientRect;
        ::GetClientRect(m_hWnd,&clientRect);
		if ((clientRect.left!=x0)||(clientRect.right!=x1)||(clientRect.top!=y0)||(clientRect.bottom!=y1)){
			RECT winRect;
			::GetWindowRect(m_hWnd,&winRect);

			long left=x0+(winRect.left-clientRect.left);
			long top =y0+(winRect.top -clientRect.top );
			long width = (x1-x0) + ((winRect.right-winRect.left)-(clientRect.right-clientRect.left));
			long height= (y1-y0) + ((winRect.bottom-winRect.top)-(clientRect.bottom-clientRect.top));
			#ifdef WINCE
			   left=0;
			   top=0;
			   width=DDScreenWidth;
			   height=DDScreenHeight;
			#endif
			MoveWindow(m_hWnd,left,top,width,height,true);
		}
    }

   inline void getClientSize(long& width,long& height) const{
        long x0,y0,x1,y1;
        getClientRect(x0,y0,x1,y1);
        width=x1-x0;
        height=y1-y0;
    }
    inline long getClientWidth() const{
        long width,height;
        getClientSize(width,height);
		return width;
    }
    inline long getClientHeight() const{
        long width,height;
        getClientSize(width,height);
		return height;
    }
	bool tryResizeBuf(){
		if (m_hWnd==0) return false;
		#ifndef WINCE
			if (::IsIconic(m_hWnd)) return false;
		#endif

		long dstWidth,dstHeight;
		#ifdef WINCE
			dstWidth=m_userWidth;
			dstHeight=m_userHeight;
		#else
			getClientSize(dstWidth,dstHeight);
		#endif
		if (m_mapPosType==mp_1to2){ //为了边界访问安全
			++dstWidth;
			++dstHeight;
		}
		if ((m_canvasBase.width!=dstWidth)||(m_canvasBase.height!=dstHeight)){
			m_canvasBase.resizeFast(dstWidth,dstHeight);
			return true;
		}else
			return false;

	}
	bool setBestMap(long userScreenWidth,long userScreenHeight,long DDScreenWidth,long DDScreenHeight){
		TMapPosType i=mp_1to2;
		for (;i>=mp_2to1;i=(TMapPosType)(i-1)){
			if (isCanSetMap(i,userScreenWidth,userScreenHeight,DDScreenWidth,DDScreenHeight)){
				m_mapPosType=i;
				//居中
				m_mapPosMvX0=(DDScreenWidth-(userScreenWidth*MapPosVNew[m_mapPosType])/MapPosVOld[m_mapPosType])/2;
				m_mapPosMvY0=(DDScreenHeight-(userScreenHeight*MapPosVNew[m_mapPosType])/MapPosVOld[m_mapPosType])/2;
				return ((m_mapPosType!=mp_1to1)||(m_mapPosMvX0!=0)||(m_mapPosMvY0!=0));
			}
		}

		m_mapPosType=mp_2to1;
		//靠右下
		m_mapPosMvX0=(DDScreenWidth-(userScreenWidth*MapPosVNew[m_mapPosType])/MapPosVOld[m_mapPosType]);
		m_mapPosMvY0=(DDScreenHeight-(userScreenHeight*MapPosVNew[m_mapPosType])/MapPosVOld[m_mapPosType]);
		return true;
	}
	static bool isCanSetMap(TMapPosType mp,long userScreenWidth,long userScreenHeight,long DDScreenWidth,long DDScreenHeight){
		return ( (userScreenWidth*MapPosVNew[mp] <= DDScreenWidth*MapPosVOld[mp]) 
			&&   (userScreenHeight*MapPosVNew[mp] <= DDScreenHeight*MapPosVOld[mp]) );
	}
	inline long mapPos(long x){
		return (x*MapPosVOld[m_mapPosType])/MapPosVNew[m_mapPosType];
	}
	inline long mapX(long x){
		return mapPos(x-m_mapPosMvX0);
	}
	inline long mapY(long y){
		return mapPos(y+getScreenMoveHeight()-m_mapPosMvY0);
	}
	inline long rMapMin(long x){
		return (x/MapPosVOld[m_mapPosType])*MapPosVNew[m_mapPosType];
	}
	inline long rMapMax(long x){
		return ((x+MapPosVOld[m_mapPosType]-1)/MapPosVOld[m_mapPosType])*MapPosVNew[m_mapPosType];
	}


    void doUpdate(long** ppUpdateRect,long* pRectCount){

	    double nowTime=getMMTimerCount()*0.001;
	    double stepTime_s;
	    if (m_updateTime==0){
		    stepTime_s=0;
		    m_updateTime=nowTime;
	    }
	    else{
		    stepTime_s=(nowTime-(double)m_updateTime);
		    m_updateTime=nowTime;
		    if (stepTime_s<=0) return; 
	    }

		bool isScreenSizeChanged=tryResizeBuf();
    	
	    //update
	    if (m_updateListener.update!=0)
			m_updateListener.update(m_updateListener.callBackHandle,stepTime_s,isScreenSizeChanged,m_canvasBase.width,m_canvasBase.height);
	    if (m_updateListener.draw!=0){
			_TCanvasBaseData dst;
			if (m_isUseCanvasBack&&(m_mapPosType==mp_1to1)){
				dst=*(_TCanvasBaseData*)&( ((Pixels32Ref*)&m_canvasBack)->getSubRef(m_mapPosMvX0,m_mapPosMvY0,m_mapPosMvX0+m_canvasBase.width,m_mapPosMvY0+m_canvasBase.height) );
			}else
				dst=m_canvasBase;
		    m_updateListener.draw(m_updateListener.callBackHandle,&dst,isScreenSizeChanged,ppUpdateRect,pRectCount);
	    }
    }

	void zoomMap(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,long* pRect,long rectCount,bool isErrorDiffuse=false){
		for (long i=0;i<rectCount;++i){
			Pixels32Ref dref=dstRef.getSubRef(pRect[0],pRect[1],pRect[2],pRect[3]);
			if ((dstRef.pdata!=srcRef.pdata)||(m_mapPosType!=mp_1to1)){
				Pixels32Ref sref=srcRef.getSubRef(mapPos(pRect[0]-m_mapPosMvX0),mapPos(pRect[1]-m_mapPosMvY0),mapPos(pRect[2]-m_mapPosMvX0),mapPos(pRect[3]-m_mapPosMvY0));
				ZoomMapProcV[m_mapPosType](dref,sref,false);
			}
			#ifdef WINCE
				if (isErrorDiffuse)
					copy_32bit_to_16bit565_ErrorDiffuse(*(Pixels32Ref*)&dref,32,false,*(Pixels32Ref*)&dref);
			#endif
			pRect+=4;
		}
	}

    void onTimerUpdate(){
		if (!m_isCanFarmeUpdate) return;
	    long* pRect=0;
	    long rectCount=0;
	    doUpdate(&pRect,&rectCount);

		if ((pRect==0)||(rectCount<=0)) return;

		if (m_isUseMapPos){
			for (int i=0;i<rectCount*4;i+=4){
				pRect[i+0]=rMapMin(pRect[i+0])+m_mapPosMvX0;
				pRect[i+1]=rMapMin(pRect[i+1])+m_mapPosMvY0;
				pRect[i+2]=rMapMax(pRect[i+2])+m_mapPosMvX0;
				pRect[i+3]=rMapMax(pRect[i+3])+m_mapPosMvY0;
			}
		}

		if (m_isUseCanvasBack&&(m_mapPosType!=mp_1to1)){
			#ifdef WINCE 
				#ifdef _MZPhone 
					if (m_IS_USE_DDRAW && (!isOpenKeyboard()))
						;//do nothing
					else
						zoomMap(*(Pixels32Ref*)&m_canvasBack,*(Pixels32Ref*)&m_canvasBase,pRect,rectCount,true);
				#else
					if (m_IS_USE_DDRAW && (!isOpenKeyboard()))
						zoomMap(*(Pixels32Ref*)&m_canvasBack,*(Pixels32Ref*)&m_canvasBase,pRect,rectCount,false);
					else
						zoomMap(*(Pixels32Ref*)&m_canvasBack,*(Pixels32Ref*)&m_canvasBase,pRect,rectCount,true);
				#endif
			#else
				zoomMap(*(Pixels32Ref*)&m_canvasBack,*(Pixels32Ref*)&m_canvasBase,pRect,rectCount,false);
			#endif
		}else if (m_mapPosType==mp_1to1){
			#ifdef WINCE 
				TCanvasBaseData* dst;
				if (m_isUseCanvasBack)
					dst=&m_canvasBack;
				else
					dst=&m_canvasBase;

				if (m_IS_USE_DDRAW && (!isOpenKeyboard()))
					;//do nothing
				else
					zoomMap(*(Pixels32Ref*)dst,*(Pixels32Ref*)dst,pRect,rectCount,true);//利用其误差扩散
			#endif
		}

	#ifdef WINCE
		if (m_IS_USE_DDRAW && (!isOpenKeyboard()))
			doUpdateToView(pRect,rectCount);
		else
	#endif
		{
			//标记脏区域 从而触发PAINT事件
			#ifdef WINCE
				static long old_dMoveHeight=0;
				long dMoveHeight=getScreenMoveHeight();

				if (dMoveHeight!=old_dMoveHeight){
					old_dMoveHeight=dMoveHeight;
					long iRect[4];
					iRect[0]=0;
					iRect[1]=0;
					getClientSize(iRect[2],iRect[3]);
					InvalidateRect(m_hWnd,(const RECT*)iRect,false);
				}else{
					for (long i=0;i<rectCount*4;i+=4){
						pRect[i+1]-=dMoveHeight;
						pRect[i+3]-=dMoveHeight;
						InvalidateRect(m_hWnd,(const RECT*)&pRect[i],false);
					}
				}
			#else
				for (long i=0;i<rectCount*4;i+=4){
					InvalidateRect(m_hWnd,(const RECT*)&pRect[i],false);
				}
			#endif
		}
    }

    void doUpdateToView(long* pRect=0,long rectCount=0){
		if (!m_isCanFarmeUpdate) return;

		if (tryResizeBuf())
			onTimerUpdate();
		if (m_canvasBase.isEmpty()) return;
			
		long dMoveHeight=getScreenMoveHeight();
		#ifdef WINCE
			if (m_IS_USE_DDRAW){
				m_ddrawImprot.setActive(!isOpenKeyboard());
			}

			if (m_IS_USE_DDRAW && (isOpenKeyboard())){
				if (m_isNeedGDIDrawFull){
					m_isNeedGDIDrawFull=false;
					if (m_isUseCanvasBack&&(m_mapPosType!=mp_1to1)){
						long iRect[4];
						iRect[0]=rMapMin(0)+m_mapPosMvX0;
						iRect[1]=rMapMin(0)+m_mapPosMvY0;
						iRect[2]=rMapMax(m_canvasBase.width)+m_mapPosMvX0;
						iRect[3]=rMapMax(m_canvasBase.height)+m_mapPosMvY0;
						zoomMap(*(Pixels32Ref*)&m_canvasBack,*(Pixels32Ref*)&m_canvasBase,iRect,1,true);
					}

					HDC srcDC=0;
					if(m_isUseCanvasBack)
						srcDC=m_canvasBack.getDC();
					else
						srcDC=m_canvasBase.getDC();
					if (srcDC!=0) {
						HDC hDC=GetDC(m_hWnd);
						BitBlt(hDC,0,-dMoveHeight,getClientWidth(),getClientHeight()-dMoveHeight,srcDC,0,0,SRCCOPY);
						RECT rt;
						rt.left=0;  rt.top=getClientHeight()-dMoveHeight;
						rt.right=getClientWidth(); rt.bottom=getClientHeight();
						HBRUSH hbr= (HBRUSH)GetStockObject(BLACK_BRUSH); 
						FillRect(hDC,&rt,hbr);
						ReleaseDC(m_hWnd,hDC);
					}
				}
			}
			
			if (m_IS_USE_DDRAW && (!isOpenKeyboard())){
				#ifdef _MZPhone 
					_TPixelsRef* srcRef=(_TPixelsRef*)&m_canvasBase;
				#else
					_TPixelsRef* srcRef=(_TPixelsRef*)&m_canvasBack;
				#endif
				if (m_isNeedGDIDrawFull)
					m_ddrawImprot.drawFrame(srcRef,pRect,rectCount);
				else{
					m_ddrawImprot.drawFrame(srcRef);
					m_isNeedGDIDrawFull=true;
				}
			}else
		#endif
			{
				HDC srcDC=0;
				if(m_isUseCanvasBack)
					srcDC=m_canvasBack.getDC();
				else
					srcDC=m_canvasBase.getDC();
				if (srcDC!=0) {
					PAINTSTRUCT  ps;
					HDC hDC= BeginPaint(m_hWnd, &ps);
					if (hDC!=0){
						const RECT& updateRrect=ps.rcPaint;
						BitBlt(hDC,updateRrect.left,updateRrect.top-dMoveHeight,(updateRrect.right-updateRrect.left),
							(updateRrect.bottom-updateRrect.top+dMoveHeight), srcDC,updateRrect.left,updateRrect.top,SRCCOPY);
					}
 					EndPaint(m_hWnd,&ps);
				}
			}

		/*
		RECT updateRrect;			
		::GetUpdateRect(m_hWnd,&updateRrect,false); //todo: GetUpdateRgn?
        //HDC hDC=GetWindowDC(0);
        HDC hDC=GetDC(m_hWnd);
		BitBlt(hDC,updateRrect.left,updateRrect.top,updateRrect.right-updateRrect.left,updateRrect.bottom-updateRrect.top,
				m_canvasBase.getDC(),updateRrect.left,updateRrect.top,SRCCOPY);
        ReleaseDC(m_hWnd,hDC);
        //ReleaseDC(0,hDC);
		//*/

		/*
        _TCanvasBaseData tSrc=m_canvasBase;
        if (tSrc.byte_width<0){
          ((BYTE*&)tSrc.pdata)+=tSrc.byte_width*(tSrc.height-1);
          tSrc.byte_width=-tSrc.byte_width;
        }

        BITMAPINFO bmpi;
        memset(&bmpi,0,sizeof(bmpi));
        bmpi.bmiHeader.biSize=sizeof(bmpi.bmiHeader);
        bmpi.bmiHeader.biWidth=tSrc.width;
        bmpi.bmiHeader.biHeight=tSrc.height;
        bmpi.bmiHeader.biBitCount=32;
        bmpi.bmiHeader.biSizeImage=0;//BackBuf.ByteWidth*BackBuf.Height;
        bmpi.bmiHeader.biPlanes=1;
        bmpi.bmiHeader.biCompression=BI_RGB;
        HDC hDC=GetDC(m_hWnd);
        SetDIBitsToDevice(hDC,0,0,tSrc.width,tSrc.height,0,0,0,tSrc.height,tSrc.pdata,&bmpi,DIB_RGB_COLORS);
        ReleaseDC(m_hWnd,hDC);
		//*/
    }

	static long getHostKeyState(){
	#ifndef WINCE
		BYTE keyStates[256];
		if (GetKeyboardState(keyStates)){
			long result =(keyStates[VK_SHIFT]>>7);
			result|=(keyStates[VK_CONTROL]>>7) <<1;
			result|=(keyStates[VK_MENU]>>7) <<2;
			return result;
		}else
	#endif
			return 0;
	}

	void disposeAppActive(bool isActive){
		m_isCanFarmeUpdate = isActive;
#ifdef WINCE
		if (isActive) {
			long iRect[4];
			iRect[0]=0;
			iRect[1]=0;
			getClientSize(iRect[2],iRect[3]);
			InvalidateRect(m_hWnd,(const RECT*)iRect,false);
		}
		setWavOutActive(isActive);
		setActiveStatus(m_hWnd,isActive);
		#ifdef WINCE
			if (m_IS_USE_DDRAW) m_ddrawImprot.setActive(isActive);
		#endif
#endif
	}
    LRESULT dispatchWinMsg(UINT message, WPARAM wParam, LPARAM lParam) {
		bool isMsgDisposed=true;
		bool isTestUpdateTimerActive=true;
	    switch (message) {
			#ifdef WIN_TIMER
			case WM_TIMER:
			#endif
			case MSG_MMTimerMSG_TRY: {
				MSG lpMsg;
				PeekMessage(&lpMsg,m_hWnd,MSG_MMTimerMSG_TRY,MSG_MMTimerMSG_TRY,PM_REMOVE);
				if (!PeekMessage(&lpMsg,m_hWnd,0,0,PM_NOREMOVE)){
					m_isUpdateTimerActived=false;
					PostMessage(m_hWnd,MSG_MMTimerMSG_UPDATE,0,0);
				}else{
					m_isUpdateTimerActived=true;
				}
				isTestUpdateTimerActive=false;
				break;
            }
			case MSG_MMTimerMSG_UPDATE:{
				#ifdef _MZPhone
					if (getIsLockPhoneStatus()){
						m_isCanFarmeUpdate=false;
						::PostMessage(m_hWnd,WM_CLOSE,0,0);
						break;
					}
				#endif
				onTimerUpdate();
				break;
			}
			case WM_PAINT:{
				doUpdateToView();
			    break;
		    }
			case WM_MOUSEMOVE:{
				if (m_updateListener.disposeEventMouseMoved!=0){
					m_updateListener.disposeEventMouseMoved(m_updateListener.callBackHandle,
						mapX((short)(lParam & 0xFFFF)),mapY((short)(lParam>>16)),getHostKeyState());
				}
				break;
			}
			case WM_LBUTTONDOWN:{
				::SetCapture(m_hWnd);
				if (m_updateListener.disposeEventMouseDown!=0)
					m_updateListener.disposeEventMouseDown(m_updateListener.callBackHandle,
						mapX((short)(lParam & 0xFFFF)),mapY((short)(lParam>>16)),getHostKeyState());
				break;
			}
			case WM_LBUTTONUP:{
				::ReleaseCapture();
				if (m_updateListener.disposeEventMouseUp!=0)
					m_updateListener.disposeEventMouseUp(m_updateListener.callBackHandle,
						mapX((short)(lParam & 0xFFFF)),mapY((short)(lParam>>16)),getHostKeyState());
				break;
			}
			case WM_KEYDOWN:{
				if (m_updateListener.disposeEventKeyDown!=0)
					m_updateListener.disposeEventKeyDown(m_updateListener.callBackHandle,(short)wParam,getHostKeyState());
				break;
			}
			case WM_KEYUP:{
				#ifdef WINCE
					if (wParam==VK_VOLUME_UP)
						addWavOutVolumeStep();
					else if (wParam==VK_VOLUME_DOWN)
						subWavOutVolumeStep();
				#endif

				if (m_updateListener.disposeEventKeyUp!=0)
					m_updateListener.disposeEventKeyUp(m_updateListener.callBackHandle,(short)wParam,getHostKeyState());
				break;
			}
			case WM_CHAR:{
				//处理半个字符的问题
				unsigned short aChar=(unsigned short)wParam;
				if (m_wideCharHalf>0){
					aChar=(aChar<<8) | m_wideCharHalf;
					m_wideCharHalf=0;
				}else if (aChar>=128){
					#ifdef WINCE
						wchar_t wChar[2]; wChar[0]=aChar; wChar[1]=0;
						char * ansiChar=unicodeToAnsiChar(wChar);
						aChar=((UInt8)ansiChar[0])|(((UInt8)ansiChar[1])<<8);
						delete[]ansiChar;
					#else
						m_wideCharHalf=aChar;
						break;
					#endif
				}

				if (m_updateListener.disposeEventKeyChar!=0)
					m_updateListener.disposeEventKeyChar(m_updateListener.callBackHandle,aChar,getHostKeyState());
				break;
			}
			case WM_CLOSE:{
				//m_isCanFarmeUpdate=false;
				disposeAppActive(false);
				break;
			}
#ifdef WINCE
			//case WM_HIBERNATE:{ //内存不足
			//	break;
			//}
			case WM_ACTIVATE:{
				bool isActive = ((wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE));
				#ifdef _MZPhone
					isActive = isActive&&(!isInUsbAttach);
				#endif
				disposeAppActive(isActive);
				if (!isActive)
					exitApp();
				break;
			}
	#ifdef _MZPhone
			case MSG_MZ_HOME_KEY_CLICK:{
				if (m_isCanFarmeUpdate){//&&(lParam==0)
					disposeAppActive(false);
					exitApp();
				}
				break;
			}
			case WM_HOTKEY:{
				switch (LOWORD(wParam)){
					case 	WPARAM_KEY_EVENT_CLICK_HOME:
					case 	WPARAM_KEY_EVENT_DBLCLICK_HOME:
					case 	WPARAM_KEY_EVENT_LONGCLICK_HOME:{
						if (m_isCanFarmeUpdate){
							disposeAppActive(false);
							exitApp();
						}
						break;
					}
					case 	WPARAM_KEY_EVENT_CLICK_POWER:
					case 	WPARAM_KEY_EVENT_DBLCLICK_POWER:
					case 	WPARAM_KEY_EVENT_LONGCLICK_POWER:{
							disposeAppActive(false);
							exitApp();
						//if (m_IS_USE_DDRAW) m_ddrawImprot.setActive(isActive);
						break;
					}
				}
				break;
			}
	#endif
#endif
			default:{
				#ifdef _MZPhone
					if (message==g_iUsbNotifyMsg){
						if((INT)wParam == USB_MASSSTORAGE_ATTACH){
							isInUsbAttach=true;
							disposeAppActive(false);
							exitApp();
							//::PostQuitMessage(0);
							//::MessageBox(0,(m_caption_w+L"程序无法工作在U盘模式！").c_str(),L"错误",MB_OK);
							//::MzMessageBoxEx(0,(m_caption_w+L"程序不能工作在U盘模式！").c_str(),L"错误",MB_OK);
							//::MzMessageBoxV2(0,(m_caption_w+L"程序不能工作在U盘模式！").c_str());
							//::Sleep(1800);
						}//else if((isInUsbAttach)&&((INT)wParam == USB_FUNCTION_DETACH)){
						//	isInUsbAttach=false;
						//	dispatchWinMsg(WM_ACTIVATE,WA_ACTIVE,0);
						//}
						break;
					}else if (message == g_iCallMsg || message == g_iSmsMsg) {
						if (wParam == CALL_BEGIN){
							if (m_isCanFarmeUpdate){
								disposeAppActive(false);
								::PostMessage(m_hWnd,WM_CLOSE,0,0);
							}							
						}
						else if (wParam==SMS_BEGIN){ 
							disposeAppActive(false);
						}
					}
				#endif
				isMsgDisposed=false;
				break;
			}
	    }

	    LRESULT rt=DefWindowProc(m_hWnd,message,wParam,lParam);
		if (isTestUpdateTimerActive&&m_isUpdateTimerActived)
			doUpdateTimerTime();
		return rt;
    }

    void doUpdateTimerTime() {
		if (m_hWnd==0) return;
		MSG lpMsg;
		if (!PeekMessage(&lpMsg,m_hWnd,MSG_MMTimerMSG_TRY,MSG_MMTimerMSG_TRY,PM_NOREMOVE)){
			m_isUpdateTimerActived=false;
			PostMessage(m_hWnd,MSG_MMTimerMSG_TRY,0,0);
		}
    }
};

void gFormOnTimerProc(TAbstractTimer* sender,const void* callBackData){
    ((TFormPrivateData*)callBackData)->doUpdateTimerTime();
}

//----------------------------------------------------------------------------------------------------------

TFormBase::TFormBase(const char* caption,long clientWidth,long clientHeight,long nCmdShow,const char* icon,const char* cursor):m_data(0){
    assert(gApplication!=0);
    m_data=new TFormPrivateData();

    gApplication->registForm(this);
    ((TFormPrivateData*)m_data)->createForm(caption,clientWidth,clientHeight,nCmdShow,icon,cursor);
}

void TFormBase::setUpdateListener(const IUpdateListenerBase& updateListener){
	((TFormPrivateData*)m_data)->setUpdateListener(updateListener);
}

TFormBase::~TFormBase(){
    if (gApplication!=0)
		gApplication->unRegistForm(this);

    if (m_data!=0){
        TFormPrivateData* data=(TFormPrivateData*)m_data;
        m_data=0;
        delete data;
    }
};

const char* TFormBase::getCaption()const{
    TFormPrivateData* data=(TFormPrivateData*)m_data;
    return data->m_caption.c_str();
}
void TFormBase::setCaption(const char* caption){
    TFormPrivateData* data=(TFormPrivateData*)m_data;
    data->setCaption(caption);
}

void TFormBase::getClientRect(long& x0,long& y0,long& x1,long& y1) const{
    TFormPrivateData* data=(TFormPrivateData*)m_data;
    data->getClientRect(x0,y0,x1,y1);
}
void TFormBase::setClientRect(long x0,long y0,long x1,long y1){
    TFormPrivateData* data=(TFormPrivateData*)m_data;
    data->setClientRect(x0,y0,x1,y1);
}

void TFormBase::setIconRes(const char*  resID){
    TFormPrivateData* data=(TFormPrivateData*)m_data;
    data->setIconRes(resID);
}

void TFormBase::setCursorRes(const char*  resID){
    TFormPrivateData* data=(TFormPrivateData*)m_data;
    data->setCursorRes(resID);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
    class TFormBaseEx:public TFormBase{
    public:
        static inline TFormPrivateData* getFormData(TFormBase* form){
            return (TFormPrivateData*)(((TFormBaseEx*)form)->getPrivateData());
        }
    };

struct TAppPrivateData{
    HINSTANCE	m_hInstance;
    HINSTANCE   m_hPrevInstance;
    Vector<TFormBase*> m_formList;
    const char* m_cmdLine;
    int         m_nCmdShow;
    
    TAppPrivateData():m_cmdLine(0),m_hInstance(0),m_hPrevInstance(0),m_nCmdShow(0){}
    ~TAppPrivateData(){ clear(); }
    void clear(){
        long size=(long)m_formList.size();
        for (long i=size-1;i>=0;--i){
			if (m_formList[i]!=0){
				TFormBase* form=m_formList[i];
				m_formList[i]=0;
				delete form;
			}
        }
        m_formList.clear();
    }

    TFormBase* getMainForm(){
        long size=(long)m_formList.size();
        if (size<=0)
            return 0;
        else
            return m_formList[0];
    }

    void addForm(TFormBase* aForm){
        assert(aForm!=0);
        m_formList.push_back(aForm);
    }
    long indexOf(TFormBase* aForm){
        long size=(long)m_formList.size();
        for (long i=0;i<size;++i){
            if (aForm==m_formList[i])
                return i;
        }
        return -1;
    }
    void outForm(TFormBase* aForm,bool isDoFree){
        assert(aForm!=0);
        long index=indexOf(aForm);
		if (index<0){
			if (isDoFree)
				delete aForm;
			return;
		}

		m_formList[index]=0;
		long insertIndex=0;
        long size=(long)m_formList.size();
        for (long i=0;i<size-1;++i){
			if (m_formList[i]!=0){
				m_formList[insertIndex]=m_formList[i];
				++insertIndex;
			}
        }
        m_formList.resize(insertIndex);

		if (isDoFree)
			delete aForm;
		if (m_formList.empty())
			PostQuitMessage(0); //exit app
    }


    LRESULT transmitWinMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        long size=(long)m_formList.size();
	    for(long i=0;i<size;++i)
	    {
            TFormPrivateData* formData=TFormBaseEx::getFormData(m_formList[i]);
            if ( ( (formData->m_hWnd!=0) &&(formData->m_hWnd==hWnd) )
                    ||( (formData->m_hWnd==0) && (formData==gCteateHWND_FormPrivateData) ) ){

                if (formData->m_hWnd==0) formData->m_hWnd=hWnd;
                LRESULT result=formData->dispatchWinMsg(message,wParam,lParam);

                if (message==WM_CREATE){
                    m_formList[i]->formCreate();
                }else if (message==WM_DESTROY){
                    m_formList[i]->formDestroy();
					TFormBase* frm=m_formList[i];
					if (m_formList.size()==1){
						#ifdef WINCE 
							g_netLinkClear();
						#endif
						PostQuitMessage(0); //exit app
					}else
						outForm(frm,true); 
                }
                return result;
            }
	    }
	    return DefWindowProc(hWnd,message,wParam,lParam);
    }
};


//----------------------------------------------------------------------------------------------------------

TApplicationBase::TApplicationBase():m_data(0){
    assert(gApplication==0);
    gApplication=this;

    m_data=new TAppPrivateData();
}

TApplicationBase* TApplicationBase::getApplication(){
    return gApplication;
}

TApplicationBase::~TApplicationBase(){
    gApplication=0;
    if (m_data!=0){
        TAppPrivateData* data=(TAppPrivateData*)m_data;
        m_data=0;
        delete data;
    }
}


void TApplicationBase::registForm(TFormBase* aForm){
    TAppPrivateData* data=(TAppPrivateData*)(m_data);
    data->addForm(aForm);
}
    
void TApplicationBase::unRegistForm(TFormBase* aForm){
	if (m_data==0) return;
    TAppPrivateData* data=(TAppPrivateData*)(m_data);
    data->outForm(aForm,false);
}
    
TFormBase* TApplicationBase::getMainForm(){
    if (gApplication==0) return 0;
    TAppPrivateData* data=(TAppPrivateData*)(gApplication->m_data);
    return data->getMainForm();
}

const char* TApplicationBase::getCmdLine(){
    if (gApplication==0) return 0;
    TAppPrivateData* data=(TAppPrivateData*)(gApplication->m_data);
    return data->m_cmdLine;
}


    class TApplicationBaseEx:public TApplicationBase{
    public:
        static inline TAppPrivateData* getAppData(TApplicationBase* app){
            return (TAppPrivateData*)(((TApplicationBaseEx*)app)->getPrivateData());
        }
    };
LRESULT CALLBACK gWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (gApplication==0)  
		return DefWindowProc(hWnd,message,wParam,lParam);
    else{
        TAppPrivateData* appData=TApplicationBaseEx::getAppData(gApplication);
		return appData->transmitWinMsg(hWnd,message,wParam,lParam);
    }
}

HINSTANCE  gethInstance(){
    TAppPrivateData* appData=TApplicationBaseEx::getAppData(gApplication);
    return appData->m_hInstance;
}

const void* getMainFormHandle(){
	if (gApplication==0) return 0;
    TAppPrivateData* appData=TApplicationBaseEx::getAppData(gApplication);
	TFormBase* form=appData->getMainForm();
	if (form==0) return 0;
	
	TFormPrivateData* formData=TFormBaseEx::getFormData(form);
	if (formData==0) return 0;
	return  formData->m_hWnd;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//init_MyApplication(); //liujw
    if (gApplication==0) return 0;
    TAppPrivateData* appData=TApplicationBaseEx::getAppData(gApplication);
    appData->m_hInstance=hInstance;
    appData->m_hPrevInstance=hPrevInstance;
	#ifndef WINCE
		appData->m_cmdLine=lpCmdLine;
	#else
		char* text=unicodeToAnsiChar(lpCmdLine);
		appData->m_cmdLine=text;
		delete[]text;
	#endif
    appData->m_nCmdShow=nCmdShow;

    gApplication->applicationCreate();

#ifdef _MZPhone
	g_iUsbNotifyMsg = RegisterUsbNotifyMsg();
    g_iCallMsg = GetCallRegisterMessage();
    g_iSmsMsg = GetSmsRegisterMessage();
#endif


	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	/*while (true){
		if (PeekMessage(&msg,0,0,0,PM_REMOVE)) {
			TranslateMessage(&msg);
			bool isExitApp=(msg.message==WM_DESTROY)&&((HWND)getMainFormHandle()==msg.hwnd);
			DispatchMessage(&msg);
			if (isExitApp)
				break;
		}else{
			//update 
			PostMessage((HWND)getMainFormHandle(),MSG_MMTimerMSG,0,0);
			::Sleep(0);
		}
	}*/

    gApplication->applicationDestroy();
	//exit_MyApplication(); //liujw

	return (unsigned int)msg.wParam;

}

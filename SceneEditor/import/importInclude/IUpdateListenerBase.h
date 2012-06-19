//IUpdateListenerBase.h

#ifndef _IUpdateListenerBase_h_
#define _IUpdateListenerBase_h_

#ifdef __cplusplus 
extern "C" {
#endif
    
    //特定系统平台和游戏代码之间的链接层
    
    //游戏循环\状态更新
    //用于负责循环更新的定时器回调
    
    
    struct _TCanvasBaseData{
        void*		pdata;        
        long        byte_width;   
        long        width;        
        long        height; 
    };
    typedef struct _TCanvasBaseData _TCanvasBaseData;
    typedef _TCanvasBaseData* TCanvasBaseHandle;
    
    
    typedef void (*TSetNullUpdateListenerProc)(void* pIUpdateListenerBase);
    typedef void (*TUpdateProc)(void * callBackHandle, double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight);
    typedef void (*TDrawProc)(void * callBackHandle, TCanvasBaseHandle dst,bool isFullScreenDraw,long** ppRect,long* pRectCount);	
    typedef void (*TDisposeEventKeyEventProc)(void * callBackHandle,int aChar,long keyBoardState);
    typedef void (*TDisposeEventMouseEventProc)(void * callBackHandle,long x, long y,long keyBoardState);
    
    //消息推送
	typedef void (*TPushNotifyProc)(void * callBackHandle,const char* czMessage);
    
    struct IUpdateListenerBase{
        void *						callBackHandle;
        TUpdateProc					update;
        TDrawProc					draw;
        TDisposeEventKeyEventProc	disposeEventKeyChar;	
        TDisposeEventKeyEventProc	disposeEventKeyDown;	
        TDisposeEventKeyEventProc	disposeEventKeyUp;	
        TDisposeEventMouseEventProc disposeEventMouseMoved;	
        TDisposeEventMouseEventProc	disposeEventMouseDown;	
        TDisposeEventMouseEventProc	disposeEventMouseUp;	
        TSetNullUpdateListenerProc  setNullUpdateListener;
        
        TPushNotifyProc     pushNotification;
    };
    
    typedef struct IUpdateListenerBase IUpdateListenerBase;
    void initUpdateListener(IUpdateListenerBase* pIUpdateListenerBase);    
    
 #ifdef __cplusplus 
}
#endif
#endif //_IUpdateListenerBase_h_

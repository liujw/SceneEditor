//VGameCtrl.cpp

#include "VGameCtrl.h"

VGameCtrl* _curGameCtrl=0;

VGameCtrl::VGameCtrl(VGame* game,INetIOFactory* netIOFactory){
    m_game=game;
    m_netIOFactory=netIOFactory;
    m_isFullScreenDraw=true;
    
    _curGameCtrl=this;
}

VGameCtrl::~VGameCtrl(){
    delete m_game;
    delete m_netIOFactory;
}


static void _update(void * callBackHandle, double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->update(stepTime_s,isScreenSizeChanged,newWidth,newHeight);
}

static void _draw(void * callBackHandle, TCanvasBaseHandle dst,bool isScreenSizeChanged,long** ppRect,long* pRectCount){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    Pixels32Ref* dst_ref=(Pixels32Ref*)dst;
    VCanvas  dst_Canvas=VCanvas(*dst_ref);
    gameCtrl->draw(dst_Canvas,isScreenSizeChanged,(TRect**)ppRect,pRectCount);
}

static void _pushNotification(void * callBackHandle,const char* csMessage)
{
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->pushNotification(csMessage);
}

static void _disposeEventKeyChar(void * callBackHandle,int aChar,long keyBoardState){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->disposeEventKeyChar(aChar,TKeyBoardState(keyBoardState));
}

static void _disposeEventKeyDown(void * callBackHandle,int aChar,long keyBoardState){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->disposeEventKeyDown(aChar,TKeyBoardState(keyBoardState));
}

static void _disposeEventKeyUp(void * callBackHandle,int aChar,long keyBoardState){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->disposeEventKeyUp(aChar,TKeyBoardState(keyBoardState));
}

static void _disposeEventMouseMoved(void * callBackHandle,long x, long y,long keyBoardState){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->disposeEventMouseMoved(x,y,TKeyBoardState(keyBoardState));
}
static void _disposeEventMouseDown(void * callBackHandle,long x, long y,long keyBoardState){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->disposeEventMouseDown(x,y,TKeyBoardState(keyBoardState));
}
static void _disposeEventMouseUp(void * callBackHandle,long x, long y,long keyBoardState){
    VGameCtrl* gameCtrl=(VGameCtrl*)callBackHandle;
    gameCtrl->disposeEventMouseUp(x,y,TKeyBoardState(keyBoardState));
}	



static void _setNullUpdateListener(void* pIUpdateListenerBase){
    IUpdateListenerBase* updateListener=(IUpdateListenerBase*)pIUpdateListenerBase;
    VGameCtrl*  gameCtrl=(VGameCtrl*)updateListener->callBackHandle;
    initUpdateListener(updateListener);
    delete gameCtrl;
}

void VGameCtrl::getUpdateListener(IUpdateListenerBase* out_updateListener){
    out_updateListener->callBackHandle=this;
    out_updateListener->update=_update;
    out_updateListener->draw=_draw;
    
    out_updateListener->pushNotification=_pushNotification;
    
    out_updateListener->disposeEventKeyChar=_disposeEventKeyChar;
    out_updateListener->disposeEventKeyDown=_disposeEventKeyDown;
    out_updateListener->disposeEventKeyUp=_disposeEventKeyUp;
    out_updateListener->disposeEventMouseMoved=_disposeEventMouseMoved;	
    out_updateListener->disposeEventMouseDown=_disposeEventMouseDown;
    out_updateListener->disposeEventMouseUp=_disposeEventMouseUp;
    
    out_updateListener->setNullUpdateListener=_setNullUpdateListener;
}


void VGameCtrl::update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight) {
    TInputIORecrdList	curList;
    curList.swap(m_Inputlist);
    long size=(long)curList.size();
    for (long i=0;i<size;++i){
        m_game->getInputIOListener()->disposeInputIOEvent(curList[i]);
    }
    m_game->update(stepTime_s,isScreenSizeChanged,newWidth,newHeight);
}

void VGameCtrl::pushNotification(const char* csMessage)
{
    m_game->pushNotification(csMessage);
}

void VGameCtrl::draw(VCanvas& dst,bool isScreenSizeChanged,TRect** ppRect,long* pRectCount) {
    if (dst.getIsEmpty()) return;
    //to draw 
    m_drawPipe.drawBegin();
	m_game->getSpriteEngine().update(0.0); //liujw
    m_game->draw(m_drawPipe,0,0);
    m_drawPipe.drawOut(dst,0,0,(isScreenSizeChanged||m_isFullScreenDraw),ppRect,pRectCount);
    m_drawPipe.drawEnd();
    
    m_isFullScreenDraw=false;
}

void VGameCtrl::cutScreen_draw(const VCanvas& dst){
    m_drawPipe.drawBegin();
    m_game->draw(m_drawPipe,0,0);
    m_drawPipe.drawOut(dst,0,0, true);
    m_drawPipe.drawEnd();
    
    m_isFullScreenDraw=true;
}

VSurface* VGameCtrl::captureScreen(long nWidth, long nHeight)
{
    VSurface* pSurface = new VSurface();
    pSurface->resizeFast(nWidth, nHeight);
    cutScreenTo(pSurface->getCanvas());
    return pSurface;	
}

void VGameCtrl::cutScreenTo(const VCanvas& dst){
    if (_curGameCtrl!=0)
        _curGameCtrl->cutScreen_draw(dst);
}
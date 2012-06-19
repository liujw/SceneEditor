//
//  VGameCtrl.h
//
//  Created by housisong on 08-4-3.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VGameCtrl_h_
#define _VGameCtrl_h_


#include "../../import/importInclude/IUpdateListenerBase.h"
#include "VGame.h"
#include "VNet.h"
#include "VSurface.h"

class VGameCtrl{
private:
    typedef Vector<InputIORecord> TInputIORecrdList;
    VGame*				m_game;
    VDrawPipe			m_drawPipe;
    TInputIORecrdList	m_Inputlist;
    INetIOFactory*      m_netIOFactory;
    bool				m_isFullScreenDraw;
    void cutScreen_draw(const VCanvas& dst);
public:
    void getUpdateListener(IUpdateListenerBase* out_updateListener);
public:
    VGameCtrl(VGame* game,INetIOFactory* netIOFactory);
    ~VGameCtrl();
    
    void update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight);
    void draw(VCanvas& dst,bool isScreenSizeChanged,TRect** ppRect=0,long* pRectCount=0);
    void pushNotification(const char* csMessage);
    void disposeEventKeyChar(int aChar,const TKeyBoardState& keyBoardState) {
        m_Inputlist.push_back(InputIORecord(INPUT_KEY_CHAR,0,0,aChar,keyBoardState));
    }
    void disposeEventKeyDown(int aChar,const TKeyBoardState& keyBoardState) {
        m_Inputlist.push_back(InputIORecord(INPUT_KEY_DOWN,0,0,aChar,keyBoardState));
    }
    void disposeEventKeyUp(int aChar,const TKeyBoardState& keyBoardState) {
        m_Inputlist.push_back(InputIORecord(INPUT_KEY_UP,0,0,aChar,keyBoardState));
    }
    void disposeEventMouseMoved(long x, long y,const TKeyBoardState& keyBoardState) {
        m_Inputlist.push_back(InputIORecord(INPUT_MOUSE_MOVED,x,y,0,keyBoardState));
    }
    void disposeEventMouseDown(long x, long y,const TKeyBoardState& keyBoardState) {
        m_Inputlist.push_back(InputIORecord(INPUT_MOUSE_DOWN,x,y,0,keyBoardState));
    }
    void disposeEventMouseUp(long x, long y,const TKeyBoardState& keyBoardState) {
        m_Inputlist.push_back(InputIORecord(INPUT_MOUSE_UP,x,y,0,keyBoardState));
    }
    
    static void cutScreenTo(const VCanvas& dst);
    static VSurface* captureScreen(long nWidth, long nHeight);
};



#endif //_VGameCtrl_h_
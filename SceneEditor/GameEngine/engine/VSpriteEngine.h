//
//  VSpriteEngine.h
//
//  Created by housisong on 08-4-2.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VSpriteEngine_h_
#define _VSpriteEngine_h_

#include "VSprite.h"
#include "IInputIOListener.h"
#include "VDrawPipe.h"

class VSpriteEngine:public IInputIOListener,public VSprite{
protected:
    //VSpriteList&     m_spriteList=VSprite::m_spriteList;
    VSprite*		m_FocusSprite;
    VSprite*		m_MouseEnterSprite;
    VSprite*		m_MouseDownSprite;
    TSpriteMouseEventInfo	m_mouseDownEventInfo;
    long			m_MouseMoveX;
    long			m_MouseMoveY;
    void init(){ 
        m_FocusSprite=0;
        m_MouseEnterSprite=0;
        m_MouseDownSprite=0;
    }
private:
    inline void onKillASprite(VSprite* aSprite){
        if (aSprite==m_FocusSprite) m_FocusSprite=0;
        if (aSprite==m_MouseEnterSprite) m_MouseEnterSprite=0;
        if (aSprite==m_MouseDownSprite) m_MouseDownSprite=0;
    }
    struct TSpriteDeleteEventListener:public IEventListener{
        virtual void event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen){
            VSpriteEngine* engine=(VSpriteEngine*)callBackData;
            engine->onKillASprite((VSprite*)sender);
            isContinueListen=false;
        }
    };
    TSpriteDeleteEventListener m_SpriteDeleteEventListener;
    inline void SetLockSprite(VSprite*& dstOld,VSprite* srcNew){
        if (dstOld!=srcNew) {
            if (dstOld!=0) 
                dstOld->setDeleteEventListener(0,0); //unlock
            if (srcNew!=0) 
                srcNew->setDeleteEventListener(&m_SpriteDeleteEventListener,this); //lock
            dstOld=srcNew;
        }
    }
    
    
    static void sortSpriteList(VSpriteList& spriteList);
    static void delKilledSprite(VSpriteList& spriteList);
    static void updateSpriteList(VSpriteList& spriteList,long x0,long y0,double stepTime_s);
    
    static void drawSpriteList(VDrawPipe& dst,long x0,long y0,VSpriteList& spriteList);
    static VSprite* testHitSpriteList(VSpriteList& spriteList,long x0,long y0);
    
    static bool getSpriteIsCanDisposeKeyEvent(VSprite* sprite);
 
public:
    inline VSprite* testCurSprite(long x, long y) {
        return testHitSpriteList(m_spriteList,x,y);
    }
protected:
    virtual void setSpriteWantFocus(VSprite* srcSprite,bool isWantFocus);
    virtual void spriteWantNextFocus(VSprite* srcSprite);
public:
    VSpriteEngine(){ init(); }
    virtual ~VSpriteEngine(){ kill(); }
    void addASprite(VSprite* aSprite){
        assert(aSprite->getParent()==0);
        VSprite::addASprite(aSprite);
    }
    void update(double stepTime_s);
    void draw(VDrawPipe& dst,long x0,long y0){ 
        drawSpriteList(dst,getLeft()+x0,getTop()+y0,m_spriteList); 
    }
    static void drawSpriteWithChilds(const VCanvas& dst,long x0,long y0,VSprite* srcSprite,bool isNeedUpdate=false);
    
    static bool isInClickRect(long moveX,long moveY);
    
    //IInputIOListener
    virtual void disposeInputIOEvent(const InputIORecord& inputIORecord);
    
    //
    virtual void disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void disposeEventMouseUp(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void disposeEventKeyDown(const TSpriteKeyEventInfo& keyEventInfo);
    virtual void disposeEventKeyUp(const TSpriteKeyEventInfo& keyEventInfo);
    virtual void disposeEventKeyChar(const TSpriteKeyEventInfo& keyEventInfo);
    
};



#endif //_VSpriteEngine_h_
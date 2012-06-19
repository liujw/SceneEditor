//VSpriteEngine.cpp

#include "VSpriteEngine.h"

//#define _DEBUG_SPRITE_POS
#ifdef _DEBUG_SPRITE_POS
#include "../base/Rand.h"
#include "VLabelSprite.h"
#endif

bool VSpriteEngine::isInClickRect(long moveX,long moveY){
    //判断点击范围用
#ifdef WINCE
    static const long CLICK_MAX_MOVE_WIDTH=20;
#else
    static const long CLICK_MAX_MOVE_WIDTH=15;
#endif
    static const long CLICK_MAX_MOVE_HEIGHT=CLICK_MAX_MOVE_WIDTH;
    
    return (MyBase::abs(moveX)<=CLICK_MAX_MOVE_WIDTH) && (MyBase::abs(moveY)<=CLICK_MAX_MOVE_HEIGHT);
}

void VSpriteEngine::sortSpriteList(VSpriteList& spriteList) {
    spriteList.sortSpriteList();
    long spriteCount=spriteList.getSpriteCount();
    for (long i=0;i<spriteCount;++i){
        VSprite* aSprite=(VSprite*)spriteList.getSprite(i);
        sortSpriteList(aSprite->getSpriteList());
    }			
}

void VSpriteEngine::delKilledSprite(VSpriteList& spriteList){
    spriteList.delKilledSprite();
    long spriteCount=spriteList.getSpriteCount();
    for (long i=0;i<spriteCount;++i){
        VSprite* aSprite=(VSprite*)spriteList.getSprite(i);
        delKilledSprite(aSprite->getSpriteList());
    }
}

void VSpriteEngine::updateSpriteList(VSpriteList& spriteList,long x0,long y0,double stepTime_s){
    long spriteCount=spriteList.getSpriteCount();
    for (long i=0;i<spriteCount;++i){
        VSprite* aSprite=(VSprite*)spriteList.getSprite(i);
        if (!aSprite->getIsKilled()){
            aSprite->update(stepTime_s);
            if (!aSprite->getIsKilled()){
                long dx0=x0+aSprite->getLeft();
                long dy0=y0+aSprite->getTop();
                SpriteDrawRectInfo& rectInfo=aSprite->private_getSpriteDrawRectInfo();
                rectInfo.drawX0=dx0;
                rectInfo.drawY0=dy0;
                updateSpriteList(aSprite->getSpriteList(),dx0,dy0,stepTime_s);					
            }
        }
    }		
}

static inline void drawASprite(VDrawPipe& dst,SpriteDrawRectInfo& rectInfo,VSprite* aSprite){
    dst.drawSprite(aSprite);
}

static inline void drawASprite(const VCanvas& dst,SpriteDrawRectInfo& rectInfo,VSprite* aSprite){
    aSprite->draw(dst,rectInfo.drawX0, rectInfo.drawY0);
}

template <class TDrawDstType>
static void tm_drawSpriteList(TDrawDstType& dst,long x0,long y0,VSpriteList& spriteList){
    long spriteCount=spriteList.getSpriteCount();
    for (long i=0;i<spriteCount;++i){
        VSprite* aSprite=(VSprite*)spriteList.getSprite(i);
        if (aSprite!=0&&aSprite->getIsVisible()&&(!aSprite->getIsKilled())){//&&(aSprite->getLayerAlpha()>0.0f)) {
            long dx0=x0+aSprite->getLeft();
            long dy0=y0+aSprite->getTop();
            SpriteDrawRectInfo& rectInfo=aSprite->private_getSpriteDrawRectInfo();
            rectInfo.drawX0=dx0;
            rectInfo.drawY0=dy0;
            drawASprite(dst,rectInfo,aSprite);
            tm_drawSpriteList(dst,dx0,dy0,aSprite->getSpriteList());
        }
    }
}

void VSpriteEngine::drawSpriteList(VDrawPipe& dst,long x0,long y0,VSpriteList& spriteList){
    tm_drawSpriteList<VDrawPipe>(dst,x0,y0,spriteList);
}

static void VSpriteEngine_updateChangeWhithSpriteList(VSpriteList& spriteList){
    long spriteCount=spriteList.getSpriteCount();
    for (long i=0;i<spriteCount;++i){
        VSprite* aSprite=(VSprite*)spriteList.getSprite(i);
        if (!aSprite->getIsKilled()){
            aSprite->updateChange();
            if (!aSprite->getIsKilled()){
                VSpriteEngine_updateChangeWhithSpriteList(aSprite->getSpriteList());					
            }
        }
    }		
}


void VSpriteEngine::drawSpriteWithChilds(const VCanvas& dst,long x0,long y0,VSprite* srcSprite,bool isNeedUpdate){
    VSpriteList spriteList;
    spriteList.addASprite(srcSprite);
    
    if (isNeedUpdate){
        VSpriteEngine_updateChangeWhithSpriteList(spriteList);
        sortSpriteList(spriteList);
    }
    tm_drawSpriteList<const VCanvas>(dst,x0,y0,spriteList);
    
    spriteList.outASprite(srcSprite);
}


VSprite* VSpriteEngine::testHitSpriteList(VSpriteList& spriteList,long x0,long y0){
    long spriteCount=spriteList.getSpriteCount();
    for (long i=spriteCount-1;i>=0;--i){
        VSprite* aSprite=(VSprite*)spriteList.getSprite(i);
        if (aSprite->getIsVisible()&&(!aSprite->getIsKilled())){
            long dx0=x0-aSprite->getLeft();
            long dy0=y0-aSprite->getTop();
            if (aSprite->getIsEnableChild()){
                VSprite* result=testHitSpriteList(aSprite->getSpriteList(),dx0,dy0);
                if (result!=0)
                    return result;
            }
            
            if (aSprite->getIsEnableSelf()){
                if (aSprite->getIsHit(dx0,dy0))
                    return aSprite;
            }
        }
    }	
    return 0;
}

void VSpriteEngine::update(double stepTime_s){
    //sortSpriteList(this.m_spriteList);
    if (m_FocusSprite!=0)
        setSpriteWantFocus(m_FocusSprite,true);
    updateSpriteList(m_spriteList,0,0,stepTime_s);
    delKilledSprite(m_spriteList);
    sortSpriteList(m_spriteList);
}


//////


void VSpriteEngine::disposeInputIOEvent(const InputIORecord& inputIORecord){
    TSpriteMouseEventInfo mouseEventInfo;
    mouseEventInfo.recordTime=inputIORecord.recordTime;
    mouseEventInfo.screenX0=inputIORecord.x;
    mouseEventInfo.screenY0=inputIORecord.y;
    
    TSpriteKeyEventInfo keyEventInfo;
    keyEventInfo.recordTime=inputIORecord.recordTime;
    keyEventInfo.aChar=inputIORecord.aChar;
    keyEventInfo.keyBoardState=inputIORecord.keyBoardState;
    
    switch  (inputIORecord.inputType) {
        case INPUT_MOUSE_MOVED:{
            disposeEventMouseMoved(mouseEventInfo);
        }break;
        case INPUT_MOUSE_DOWN:{
            disposeEventMouseDown(mouseEventInfo);
        }break;
        case INPUT_MOUSE_UP:{
            disposeEventMouseUp(mouseEventInfo);
        }break;
            
        case INPUT_KEY_CHAR:{
            disposeEventKeyChar(keyEventInfo);
        }break;
        case INPUT_KEY_DOWN:{
            disposeEventKeyDown(keyEventInfo);
        }break;
        case INPUT_KEY_UP:{
            disposeEventKeyUp(keyEventInfo);
        }break;
        default:
            break;
    }
}

#ifdef _DEBUG_SPRITE_POS
static VSprite*         debug_back=0;
static bool             debug_isDown=false;
static int              debug_down_x0=-1;
static int              debug_down_y0=-1;
static VLabelSprite*    debug_spritePosLabel_move=0;
static VLabelSprite*    debug_spritePosLabel_down=0;
static VLabelSprite*    debug_spritePosLabel_size=0;
static VRand debug_rand;

class  VDebug_tag:public VSprite{
public:
    VDebug_tag(){ setIsEnableSelf(false); }
    virtual void doDraw(const VCanvas& dst,long x0,long y0){
        dst.rect(x0, y0, x0+getWidth(), y0+getHeight(), 
                 Color32(debug_rand.next()%256,debug_rand.next()%256,debug_rand.next()%256));
    }
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect){
        out_rect.setRect(x0,y0,x0+getWidth(),y0+getHeight());
    }
    virtual void doUpdate(double stepTime_s){
        changed();
    }
};

static VDebug_tag*      debug_tag_size=0;
static VDebug_tag*      debug_tag_selected=0;

#endif


void VSpriteEngine::disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo) {
#ifdef _DEBUG_SPRITE_POS
    {
        if (!debug_back){
            debug_back=new VSprite();
#ifdef _IPAD
            debug_back->setTop(20);
#endif
            debug_back->setZ(1<<30);
            this->addASprite(debug_back);
        }
        if (!debug_spritePosLabel_move){
            debug_spritePosLabel_move=new VLabelSprite();
            debug_back->addASprite(debug_spritePosLabel_move);
            debug_spritePosLabel_move->setWidth(100);
            debug_spritePosLabel_move->setHeight(15);
        }
        debug_spritePosLabel_move->setColor(Color32(debug_rand.next()%256,debug_rand.next()%256,debug_rand.next()%256));
        debug_spritePosLabel_move->setLeft(mouseEventInfo.screenX0+15);
        debug_spritePosLabel_move->setTop(mouseEventInfo.screenY0);
        String posStr="("+intToStr(mouseEventInfo.screenX0)+","+intToStr(mouseEventInfo.screenY0)+")";
        debug_spritePosLabel_move->setText(posStr);
        if (debug_isDown){
            if (!debug_spritePosLabel_size){
                debug_spritePosLabel_size=new VLabelSprite();
                debug_back->addASprite(debug_spritePosLabel_size);
                debug_spritePosLabel_size->setIsEnableSelf(false);
                debug_spritePosLabel_size->setWidth(100);
                debug_spritePosLabel_size->setHeight(15);
            }
            debug_spritePosLabel_size->setColor(Color32(debug_rand.next()%256,debug_rand.next()%256,debug_rand.next()%256));
            debug_spritePosLabel_size->setLeft(mouseEventInfo.screenX0+15);
            debug_spritePosLabel_size->setTop(mouseEventInfo.screenY0-16);
            String posStr="("+intToStr(mouseEventInfo.screenX0-debug_down_x0)+","+intToStr(mouseEventInfo.screenY0-debug_down_y0)+")";
            debug_spritePosLabel_size->setText(posStr);
            
            debug_tag_size->setWidth(mouseEventInfo.screenX0-debug_down_x0);
            debug_tag_size->setHeight(mouseEventInfo.screenY0-debug_down_y0);
        }
    }
#endif
    
    long x=mouseEventInfo.screenX0-getLeft();
    long y=mouseEventInfo.screenY0-getTop();
    
    //如果有鼠标压下选中了一个精灵
    if (m_MouseDownSprite!=0){
        m_MouseMoveX=STD::max(m_MouseMoveX,MyBase::abs(m_mouseDownEventInfo.screenX0-mouseEventInfo.screenX0));
        m_MouseMoveY=STD::max(m_MouseMoveY,MyBase::abs(m_mouseDownEventInfo.screenY0-mouseEventInfo.screenY0));
        if (m_MouseDownSprite->getIsEnableSelf()){
            SpriteDrawRectInfo& rectInfo=m_MouseDownSprite->private_getSpriteDrawRectInfo();
            m_MouseDownSprite->disposeEventMouseMoved(mouseEventInfo.makeMouseEvent(x-rectInfo.drawX0+getLeft(),y-rectInfo.drawY0+getTop(),mouseEventInfo.keyBoardState));
        }
        
        VSprite* curSprite=testCurSprite(x,y);
        if (curSprite!=0){
            SpriteDrawRectInfo& rectInfo=curSprite->private_getSpriteDrawRectInfo();
            curSprite->doSpriteEventMouseOverlayMoved(m_MouseDownSprite,mouseEventInfo.makeMouseEvent(x-rectInfo.drawX0+getLeft(),y-rectInfo.drawY0+getTop(),mouseEventInfo.keyBoardState));
        }
    }else{
        VSprite* curSprite=testCurSprite(x,y);
        if (curSprite!=m_MouseEnterSprite){
            if (m_MouseEnterSprite!=0){
                m_MouseEnterSprite->disposeEventMouseLeave();
                SetLockSprite(m_MouseEnterSprite,0);
            }
            if (curSprite!=0)
                curSprite->disposeEventMouseEnter();
        }else{
            if (curSprite!=0){
                SpriteDrawRectInfo& rectInfo=curSprite->private_getSpriteDrawRectInfo();
                curSprite->disposeEventMouseMoved(mouseEventInfo.makeMouseEvent(x-rectInfo.drawX0+getLeft(),y-rectInfo.drawY0+getTop(),mouseEventInfo.keyBoardState));
            }
        }
        SetLockSprite(m_MouseEnterSprite,curSprite);
        //TODO 鼠标指针替换
    }
}

bool VSpriteEngine::getSpriteIsCanDisposeKeyEvent(VSprite* sprite){
    if (!(sprite->getIsCanDisposeKeyEvent()))  return false;
    if ((sprite->getIsKilled()))  return false;
    if (!(sprite->getIsVisible()))  return false;
    if (!(sprite->getIsEnableSelf())) return false;
    
    VSprite* parent=sprite->getParent();
    while (parent!=0){
        if (!(parent->getIsVisible()))  return false;
        if (!(parent->getIsEnableChild())) return false;
        parent=parent->getParent();
    }
    return true;
}

void VSpriteEngine::disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo) {
#ifdef _DEBUG_SPRITE_POS
    {
        if (!debug_back){
            debug_back=new VSprite();
#ifdef _IPAD
            debug_back->setTop(20);
#endif
            debug_back->setZ(1<<30);
            this->addASprite(debug_back);
        }
        debug_down_x0=mouseEventInfo.screenX0;
        debug_down_y0=mouseEventInfo.screenY0;
        debug_isDown=true;
        if (!debug_spritePosLabel_down){
            debug_spritePosLabel_down=new VLabelSprite();
            debug_back->addASprite(debug_spritePosLabel_down);
            debug_spritePosLabel_down->setWidth(100);
            debug_spritePosLabel_down->setHeight(15);
        }
        debug_spritePosLabel_down->setColor(Color32(debug_rand.next()%256,debug_rand.next()%256,debug_rand.next()%256));
        debug_spritePosLabel_down->setLeft(mouseEventInfo.screenX0-50);
        debug_spritePosLabel_down->setTop(mouseEventInfo.screenY0);
        String posStr="("+intToStr(mouseEventInfo.screenX0)+","+intToStr(mouseEventInfo.screenY0)+")";
        debug_spritePosLabel_down->setText(posStr);
        
        if (!debug_tag_size){
            debug_tag_size=new VDebug_tag();
            debug_back->addASprite(debug_tag_size);
            debug_tag_size->setWidth(0);
            debug_tag_size->setHeight(0);            
        }
        debug_tag_size->setLeft(debug_down_x0);
        debug_tag_size->setTop(debug_down_y0);
        
        //
        
        if (!debug_tag_selected){
            debug_tag_selected=new VDebug_tag();
            debug_back->addASprite(debug_tag_selected);
            debug_tag_selected->setWidth(0);
            debug_tag_selected->setHeight(0);            
        }else{
            debug_tag_selected->setIsVisible(true);
        }
        VSprite* curSprite=testCurSprite(mouseEventInfo.screenX0-getLeft(),mouseEventInfo.screenY0-getTop());
        if (!curSprite){
            debug_tag_selected->setIsVisible(false);
        }else{
            long posX=0; long posY=0;
            curSprite->getInSpriteListTreePos(posX,posY);
            debug_tag_selected->setLeft(posX);
            debug_tag_selected->setTop(posY);
            debug_tag_selected->setWidth(curSprite->getWidth());
            debug_tag_selected->setHeight(curSprite->getHeight());
        }
    }
#endif
    
    long x=mouseEventInfo.screenX0-getLeft();
    long y=mouseEventInfo.screenY0-getTop();
    
    VSprite* curSprite=testCurSprite(x,y);
    if (curSprite!=0){
        m_MouseMoveX=0;
        m_MouseMoveY=0;
        SpriteDrawRectInfo& rectInfo=curSprite->private_getSpriteDrawRectInfo();
        m_mouseDownEventInfo=mouseEventInfo.makeMouseEvent(x-rectInfo.drawX0+getLeft(),y-rectInfo.drawY0+getTop(),mouseEventInfo.keyBoardState);
        curSprite->disposeEventMouseDown(m_mouseDownEventInfo);
        
        //处理键盘焦点
        setSpriteWantFocus(curSprite,true);
    }else{
        if (m_FocusSprite!=0)
            setSpriteWantFocus(m_FocusSprite,false);
    }
    SetLockSprite(m_MouseDownSprite,curSprite);
    SetLockSprite(m_MouseEnterSprite,curSprite);
}

void VSpriteEngine::disposeEventMouseUp(const TSpriteMouseEventInfo& mouseEventInfo) {
#ifdef _DEBUG_SPRITE_POS
    {
        debug_isDown=false;
    }
#endif
    
    long x=mouseEventInfo.screenX0-getLeft();
    long y=mouseEventInfo.screenY0-getTop();
    
    if (m_MouseDownSprite!=0){
        SpriteDrawRectInfo& rectInfo=m_MouseDownSprite->private_getSpriteDrawRectInfo();
        TSpriteMouseEventInfo mouseUpEventInfo=mouseEventInfo.makeMouseEvent(x-rectInfo.drawX0+getLeft(),y-rectInfo.drawY0+getTop(),mouseEventInfo.keyBoardState);
        m_MouseDownSprite->disposeEventMouseUp(mouseUpEventInfo);
        
        m_MouseMoveX=STD::max(m_MouseMoveX,MyBase::abs(m_mouseDownEventInfo.screenX0-mouseEventInfo.screenX0));
        m_MouseMoveY=STD::max(m_MouseMoveY,MyBase::abs(m_mouseDownEventInfo.screenY0-mouseEventInfo.screenY0));
        //生成点击事件
        if(isInClickRect(m_MouseMoveX,m_MouseMoveY)){
            VSprite* curSprite=testCurSprite(x,y);
            if (curSprite==m_MouseDownSprite){
                m_MouseDownSprite->disposeEventMouseClick(m_mouseDownEventInfo,mouseUpEventInfo);
            }
        }
    }
    
    SetLockSprite(m_MouseDownSprite,0); 
    
    if (m_MouseEnterSprite!=0){
        m_MouseEnterSprite->disposeEventMouseLeave();
        SetLockSprite(m_MouseEnterSprite,0);
    }
}

void VSpriteEngine::setSpriteWantFocus(VSprite* srcSprite,bool isWantFocus){
    assert(srcSprite!=0);
    if (!isWantFocus){
        if (srcSprite==m_FocusSprite){
            if (m_FocusSprite!=0)
                m_FocusSprite->disposeEventLostFocus();
            SetLockSprite(m_FocusSprite,0);
        }
    }else{
        if (srcSprite!=m_FocusSprite){
            bool isAllowSetFocus=true;
            if (m_FocusSprite!=0){
                if ((srcSprite!=0)&&(m_FocusSprite->getIsAllowSetFocus(srcSprite)))
                    m_FocusSprite->disposeEventLostFocus();
                else
                    isAllowSetFocus=false;
            }
            if (isAllowSetFocus){
                if (getSpriteIsCanDisposeKeyEvent(srcSprite)){
                    srcSprite->disposeEventSetFocus();
                    SetLockSprite(m_FocusSprite,srcSprite);
                }else{
                    SetLockSprite(m_FocusSprite,0);
                }
            }
        }else{
            if ((m_FocusSprite!=0)&&(!getSpriteIsCanDisposeKeyEvent(m_FocusSprite))){
                m_FocusSprite->disposeEventLostFocus();
                SetLockSprite(m_FocusSprite,0);
            }
        }
    }
}


typedef Vector<VSprite*> TSpriteList;

static void getAllCanDisposeKeyEventSpriteList(VSprite* curSprite,VSprite*& insertSprite,long& out_insertIndex,TSpriteList& out_spriteList){
    VSprite*& back_insertSprite=insertSprite;
    if ((insertSprite!=0)&&(curSprite==insertSprite)){ 
        out_insertIndex=(long)out_spriteList.size();
        out_spriteList.push_back(insertSprite);
        insertSprite=0;
    }
    if ((curSprite->getIsKilled()))  return;
    if (!(curSprite->getIsVisible()))  return;
    
    if ((curSprite->getIsEnableSelf()) && (curSprite->getIsCanDisposeKeyEvent()) && (curSprite!=back_insertSprite)){
        out_spriteList.push_back(curSprite);
    }
    
    if (curSprite->getIsEnableChild()){
        long size=curSprite->getSpriteList().getSpriteCount();
        for (long i=0;i<size;++i){
            getAllCanDisposeKeyEventSpriteList((VSprite*)(curSprite->getSpriteList().getSprite(i)),insertSprite,out_insertIndex,out_spriteList);
        }
    }
}

void VSpriteEngine::spriteWantNextFocus(VSprite* srcSprite){
    VSprite* selectSprite=0;
    if (srcSprite!=0)
        selectSprite=srcSprite;
    else
        selectSprite=m_FocusSprite;
    if (selectSprite==0) return;
    
    TSpriteList allCanDisposeKeyEventSpriteList;
    long insertIndex=-1;
    getAllCanDisposeKeyEventSpriteList(this,selectSprite,insertIndex,allCanDisposeKeyEventSpriteList);
    
    long size=(long)allCanDisposeKeyEventSpriteList.size();
    if (insertIndex<0) return;
    if (insertIndex+1<size)
        setSpriteWantFocus(allCanDisposeKeyEventSpriteList[insertIndex+1],true);
    else if (insertIndex>0)
        setSpriteWantFocus(allCanDisposeKeyEventSpriteList[0],true);
}


void VSpriteEngine::disposeEventKeyDown(const TSpriteKeyEventInfo& keyCharEventInfo){
    if ((m_FocusSprite!=0) &&(m_FocusSprite->getIsEnableSelf())&&(!m_FocusSprite->getIsKilled())){
        m_FocusSprite->disposeEventKeyDown(keyCharEventInfo);
    }
}

void VSpriteEngine::disposeEventKeyUp(const TSpriteKeyEventInfo& keyCharEventInfo){
    if ((m_FocusSprite!=0) &&(m_FocusSprite->getIsEnableSelf())&&(!m_FocusSprite->getIsKilled())){
        m_FocusSprite->disposeEventKeyUp(keyCharEventInfo);
    }
}

void VSpriteEngine::disposeEventKeyChar(const TSpriteKeyEventInfo& keyCharEventInfo){
    if ((m_FocusSprite!=0) &&(m_FocusSprite->getIsEnableSelf())&&(!m_FocusSprite->getIsKilled())){
        m_FocusSprite->disposeEventKeyChar(keyCharEventInfo);
    }
}

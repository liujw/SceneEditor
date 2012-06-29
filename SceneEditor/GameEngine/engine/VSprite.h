//
//  VSprite.h
//
//  Created by housisong on 08-3-21.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VSprite_h_
#define _VSprite_h_

#include "IInputIOListener.h"
#include "IEventListener.h"
#include "VCanvas.h"
#include "../base/Rect.h"
#include "VAction.h"

//对齐方式
enum TAlignType{align_left=0,align_center,align_right};

//给绘制管道使用的数据
//子精灵自己不允许使用
struct SpriteDrawRectInfo
{
    bool  isDrawChanged; //绘图是否发生过改变  绘图引擎用于优化重绘
    long  drawX0;
    long  drawY0;
    TRect wantDrawRect;
    
    inline void init(){
        isDrawChanged=true;
        drawX0=0;
        drawY0=0;
    }
    inline SpriteDrawRectInfo(){ init(); }
};

class ISprite:public IEnableAlpha{
private:
    SpriteDrawRectInfo m_SpriteDrawRectInfo;
public: 
    explicit ISprite(){m_bDrawSpritePos = false;}
    ISprite(const ISprite&); //not allow copy
    ISprite& operator =(const ISprite&); //not allow cop
    virtual ~ISprite()  { disposeEvent_onDelete(); }	
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect){
        out_rect.setEmpty(x0,y0);
    }
	void setDrawSpritePos(bool bDraw){ m_bDrawSpritePos = bDraw;}
    void draw(const VCanvas& dst,long x0,long y0);
    //子类改写这个函数来显示自己  //改写这个函数的类一般都要考虑是否需要改写getWantDrawRect
    virtual void doDraw(const VCanvas& dst,long x0,long y0){}
    SpriteDrawRectInfo& private_getSpriteDrawRectInfo(){ return m_SpriteDrawRectInfo; }
    
protected:
    inline void drawChanged(){
        m_SpriteDrawRectInfo.isDrawChanged=true;
    }
    
private: 

	bool m_bDrawSpritePos;
    //销毁处理 
    VEventDisposer m_deleteEventDisposer;
    void disposeEvent_onDelete() { m_deleteEventDisposer.doEvent(0,0,0);	}
public: 
    //监听点击事件 //警告:引擎自己使用
    void setDeleteEventListener(IEventListener* deleteEventListener,void* callbackData){
        m_deleteEventDisposer.setEventListener(this,deleteEventListener,callbackData);
    }
};
#ifdef __GNUC__
namespace __gnu_cxx {
    template<>
    struct hash<ISprite*>
    {
        must_inline	size_t operator()(const ISprite* sprite) const{
            return  (unsigned long)sprite;
        }
    };
}
#endif


class VSpriteList{
private:
    typedef Vector<ISprite*> TList;
    TList m_list;
    inline void freeSprite(ISprite*& sprite){
        if (sprite!=0){
            ISprite* bck_sprite=sprite;
            sprite=0;
            delete bck_sprite;		
        }
    }
public:
    VSpriteList():m_list(){}
    ~VSpriteList(){  clear();  }
    inline long getSpriteCount()const{ return (long)m_list.size(); }
    inline void addASprite(ISprite* sprite){ m_list.push_back(sprite); }
    void clear();
    must_inline const ISprite* getSprite(long index) const { return (m_list[index]); }
    must_inline ISprite* getSprite(long index) { return (m_list[index]); }
    void sortSpriteList();
    void delKilledSprite();
    void outASprite(ISprite* sprite);
    long findASprite(ISprite* sprite) const;
};


class VSprite;

struct TSpriteMouseEventInfo{
    long		clientX0;
    long		clientY0;
    TKeyBoardState keyBoardState;
    
    long		screenX0;
    long		screenY0;
    TDateTime	recordTime;
    
    inline TSpriteMouseEventInfo makeMouseEvent(long _clientX0,long _clientY0,const TKeyBoardState& _keyBoardState)const{
        TSpriteMouseEventInfo result=*this;
        result.clientX0=_clientX0;
        result.clientY0=_clientY0;
        result.keyBoardState=_keyBoardState;
        return result;
    }
};

long getGeneralIndx();

struct TSpriteKeyEventInfo{
    int             aChar;
    TKeyBoardState	keyBoardState;
    TDateTime		recordTime;
};

struct ISpriteMouseEventListener{
public:
    virtual void doSpriteEventMouseEnter(VSprite* sender) {}
    virtual void doSpriteEventMouseLeave(VSprite* sender) {}
    virtual void doSpriteEventMouseDown(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo) {}
    virtual void doSpriteEventMouseMoved(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo) {}
    virtual void doSpriteEventMouseUp(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo){}
    virtual void doSpriteEventMouseClick(VSprite* sender) {}
    virtual void doSpriteEventMouseOverlayMoved(VSprite* sender,VSprite* dragSrc,const TSpriteMouseEventInfo& mouseEventInfo) {}
    virtual ~ISpriteMouseEventListener(){}
};

class VSprite:public ISprite, public IEnableMove, public IEnableFlare{
private:
    VSprite* m_parent;
    bool m_isVisible;
    bool m_isKilled;
protected:
    bool m_isEnableSelf;
    bool m_isEnableChild; //控制是否让子类接收键盘和鼠标输入
private:
    long m_left;
    long m_top;
    long m_z;//用于遮挡和绘制顺序 z值大的在上面
    long m_width;
    long m_height;
    bool m_isChanged; //状态是否发生过改变
    bool m_isInChanging;
	long m_idx; //
    //float m_layerAlpha; //值域[0..1.0]
    inline void allDrawChanged(VSprite* sprite){
        sprite->drawChanged();
        for (long i=0;i<sprite->m_spriteList.getSpriteCount();++i){
            allDrawChanged((VSprite*)(sprite->m_spriteList.getSprite(i)));
        }
    }
protected:
    friend class VSpriteList;
    VSpriteList m_spriteList;
public:
    long m_dataFlag;
protected:
    inline void init(){
        m_parent=0;
        m_isVisible=true;
        m_isKilled=false;
        m_isEnableSelf=false;
        m_isEnableChild=true;
        m_left=0;
        m_top=0;
        m_z=0;
        m_width=0;
        m_height=0;
        m_isChanged=true;
        m_isInChanging=false;
        m_mouseEventListener=0;
        //m_layerAlpha=1.0f;

		m_idx = getGeneralIndx();
    }
    virtual void setSpriteWantFocus(VSprite* srcSprite,bool isWantFocus){
        if (m_parent!=0)
            m_parent->setSpriteWantFocus(srcSprite,isWantFocus);
    }
    virtual void spriteWantNextFocus(VSprite* srcSprite){
        if (m_parent!=0)
            m_parent->spriteWantNextFocus(srcSprite);
    }
    virtual void doKill(){//子类改写这个函数来做一些必要处理
    }
public:
    VSprite(){ init(); }
    virtual ~VSprite(){}
    
    //不允许直接修改精灵列表
    inline VSpriteList& getSpriteList(){
        return m_spriteList;
    }
    inline const VSpriteList& getSpriteList()const{
        return m_spriteList;
    }
    inline const VSprite* getSubSprite(long index)const{
        return (const VSprite*)(m_spriteList.getSprite(index));
    }
    void drawWidthChild(const VCanvas& dst,long x0,long y0);
    
    inline VSprite* getSubSprite(long index){
        return (VSprite*)(m_spriteList.getSprite(index));
    }
    inline void addASprite(VSprite* aSprite){
        aSprite->outFromParent();
        aSprite->m_parent=this;
        m_spriteList.addASprite(aSprite);
    }	
    inline void outFromParent() {
        if (m_parent!=0){
            m_parent->m_spriteList.outASprite(this);
            m_parent=0;
        }
    }
    inline VSprite* getParent() const{
        return m_parent;
    }
    
    inline bool getIsVisible()const{
        return m_isVisible;		
    }
    inline void setIsVisible(bool isVisible){
        if (m_isVisible!=isVisible){
            m_isVisible=isVisible;
            changed();
        }
    }
    //inline float getLayerAlpha()const{
    //	return m_layerAlpha;
    //}
    //void setLayerAlpha(float layerAlpha){
    //	if (layerAlpha<0)
    //		layerAlpha=0;
    //	else if (layerAlpha>1.0f)
    //		layerAlpha=1.0f;
    //	if (m_layerAlpha!=layerAlpha){
    //		m_layerAlpha=layerAlpha;
    //		allDrawChanged(this);
    //	}
    //}
    
    inline long getLeft()const{
        return m_left;
    }
    virtual  void setLeft(long left){
        if (m_left!=left){
            m_left=left;
            changed();
        }
    }
    inline long getTop()const{
        return m_top;
    }
    virtual  void setTop(long top){
        if (m_top!=top){
            m_top=top;
            changed();
        }
    }
    inline long getRight()const{
        return getLeft()+getWidth();
    }
    inline long getBottom()const{
        return getTop()+getHeight();
    }
    inline long getWidth()const {
        return m_width;
    }
    void setWidth(long width) {
        if (width<0) width=0;
        if (width!=m_width){
            m_width=width;
            changed();
        }
    }
    inline long getHeight() const {
        return m_height;
    }
    void setHeight(long height) {
        if (height<0) height=0;
        if (height!=m_height){
            m_height=height;
            changed();
        }
    }
    inline long getCenterX()const{
        return getLeft()+getWidth()/2;
    }
    inline long getCenterY()const{
        return getTop()+getHeight()/2;
    }
    inline void setCenterX(long centerX){
        setLeft(centerX-getWidth()/2);
    }	
    inline void setCenterY(long centerY){
        setTop(centerY-getHeight()/2);
    }
    
    //Z值决定列表中精灵的绘制顺序 Z值小的可能被Z值大的盖住
    inline long getZ()const{
        return m_z;
    }
    void setZ(long z){
        if (m_z!=z){
            m_z=z;
            changed();
        }
    }
    inline bool getIsEnableSelf()const {
        return m_isEnableSelf;
    }
    void setIsEnableSelf(bool isEnableSelf){
        if (m_isEnableSelf!=isEnableSelf){
            m_isEnableSelf=isEnableSelf;
            changed();
        }
    }
    inline bool getIsEnableChild()const {
        return m_isEnableChild;
    }
    void setIsEnableChild(bool isEnableChild){
        if (m_isEnableChild!=isEnableChild){
            m_isEnableChild=isEnableChild;
            changed();
        }
    }
    
    
    inline void kill(){
        if (!m_isKilled){
            for (long i=0;i<m_spriteList.getSpriteCount();++i)
                getSubSprite(i)->kill();
            doKill();
            setIsVisible(false);
            m_isKilled=true;
            changed();
        }
    }
    inline bool getIsKilled()const{
        return m_isKilled;
    }
    
    inline void changed(){
        m_isChanged=true;
        drawChanged();
    }
    inline bool getIsChanged()const{
        return m_isChanged;
    }
    
    virtual void updateChange(){
        if ((m_isChanged)&&(!m_isInChanging)){
            m_isInChanging=true;
            doUpdateChange();
            m_isInChanging=false;
            m_isChanged=false;
        }
    }
    
    //子类改写这个函数来维护自己的正确状态,比如重新生成计算字段
    virtual void doUpdateChange(){}
    
    void update(double stepTime_s){
        if (m_isKilled) return;
        doUpdate(stepTime_s);
        updateChange();
    }
    //子类改写这个函数来更新自己的随时间而变化的状态
    virtual void doUpdate(double stepTime_s){
    }
    //子类改写这个函数来显示自己
    //virtual void doDraw(const VCanvas& dst,long x0,long y0){
    //}
    
    //是否要处理键盘事件
    virtual bool getIsCanDisposeKeyEvent(){
        return false;
    }
    //键盘输入
    virtual void disposeEventKeyDown(const TSpriteKeyEventInfo& keyEventInfo) {		
    }
    virtual void disposeEventKeyUp(const TSpriteKeyEventInfo& keyEventInfo) {		
    }
    virtual void disposeEventKeyChar(const TSpriteKeyEventInfo& keyEventInfo) {		
    }
    //点是否击中精灵  点是相对于精灵本身的坐标点
    virtual bool getIsHit(long clientX0, long clientY0) {
        return (clientX0>=0)&&(clientX0<m_width)&&(clientY0>=0)&&(clientY0<m_height);
    }
    
    virtual void disposeEventMouseEnter() {
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseEnter(this);
    }
    virtual void disposeEventMouseLeave() {
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseLeave(this);
    }
    virtual void disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo) {		
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseDown(this,mouseEventInfo);
    }
    virtual void disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo) {
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseMoved(this,mouseEventInfo);
    }
    virtual void disposeEventMouseUp(const TSpriteMouseEventInfo& mouseEventInfo) {
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseUp(this,mouseEventInfo);
    }
    virtual void doSpriteEventMouseOverlayMoved(VSprite* dragSrc,const TSpriteMouseEventInfo& mouseEventInfo) {
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseOverlayMoved(this,dragSrc,mouseEventInfo);
    }
    
    virtual void disposeEventSetFocus() {
    }
    virtual void disposeEventLostFocus() {
    }
    inline void setIsWantFocus(bool isWantFocus){ //请求焦点或者释放焦点
        setSpriteWantFocus(this,isWantFocus);
    }
    virtual bool getIsAllowSetFocus(const VSprite* newWantFocusSprite){
        return true;
    }
    inline void wantNextFocus(){ //请求焦点移动到下一个输入Sprite
        spriteWantNextFocus(this);
    }
    
    void getInSpriteListTreePos(long& out_left,long& out_top){
        if (m_parent!=0){
            m_parent->getInSpriteListTreePos(out_left,out_top);
            out_left+=this->getLeft();
            out_top+=this->getTop();
        }else{
            out_left=this->getLeft();
            out_top=this->getTop();
        }
    }
private: 
    //Click点击处理
    VEventDisposer m_ClickEventDisposer;
public: 
    //监听点击事件
    inline void setClickEventListener(IEventListener* clickEventListener,void* callbackData){
        m_ClickEventDisposer.setEventListener(this,clickEventListener,callbackData);
    }
    //触发点击事件
    virtual void disposeEventMouseClick(const TSpriteMouseEventInfo& mouseDownEventInfo,const TSpriteMouseEventInfo& mouseUpEventInfo) { 
        m_ClickEventDisposer.doEvent(0,0,0);	
        
        if (m_mouseEventListener!=0)
            m_mouseEventListener->doSpriteEventMouseClick(this);
    }
private:
    ISpriteMouseEventListener* m_mouseEventListener;
public: 
    inline void setMouseEventListener(ISpriteMouseEventListener* mouseEventListener){
        m_mouseEventListener=mouseEventListener;
    }
    
};

#endif //_VSprite_h_
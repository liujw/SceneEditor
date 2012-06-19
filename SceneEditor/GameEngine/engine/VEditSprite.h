//
//  VEditSprite.h
//
//  Created by housisong on 08-4-17.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VEditSprite_h_
#define _VEditSprite_h_

#include "VSprite.h"
#include "VFont.h"

#if defined(_IOS) || defined(_MACOSX)

class VEditSprite:public VSprite{
protected:
    mutable String	m_text;
    String		m_showHit;
    VFont		m_font;
    Color32     m_color;
    void*       m_editImportHandle;
    bool		m_isUsePassWordChar;
    
    void init(){
        m_text="";
        m_color=cl32White;
        m_editImportHandle=0;
        m_isUsePassWordChar=false;
    }
    virtual void updateChange();
    virtual void doUpdateChange();
    
    static void* createImport(void* onwer,long x0,long y0,long width,long height);
    static void deleteImport(void* textImportHandle);
    void setTextToView();
    void getTextFromView()const;
public:
    virtual void onBeginEdit();
    virtual void onEndEdit();
    //virtual void onInput(long selPos0,long selLength,const char* inputStr,bool& out_isCanInput);
public:
    VEditSprite(long x0,long y0,long width,long height);
    virtual ~VEditSprite();
    const String& getText()const;
    void setText(const String&  text);
    
    const String& getShowHit()const{
        return m_showHit;
    }
    void setShowHit(const String& showHit){
        if (m_showHit!=showHit){
            m_showHit=showHit;
            changed();
        }
    }
    const Color32& getColor()const {
        return m_color;
    }
    void setColor(const Color32& color){
        if (m_color!=color){
            m_color=color;
            changed();
        }
    }
    VFont& getFont(){
        return m_font;
    }
    bool getIsUsePassWordChar()const{
        return m_isUsePassWordChar;
    }
    void setIsUsePassWordChar(bool isUsePassWordChar){
        if (m_isUsePassWordChar!=isUsePassWordChar){
            m_isUsePassWordChar=isUsePassWordChar;
            changed();
        }
    }
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);	
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    
    void setInInputing(bool isEditing=true);
    void setKeyBordIsTransparence(bool keyBordIsTransparence);
};

#else

#include "VLabelSprite.h"
#include "UndoRedo.h"

class VEditSprite:public VSprite,TAutoUndoRedo{
public:
    typedef VLabelSprite::VPosCharCanvas  VPosCharCanvas;
    typedef VLabelSprite::VCharCanvasEffect  VCharCanvasEffect;
    typedef VLabelSprite::TTextCanvasList  TTextCanvasList;
protected:
    W4String    m_text;
    W4String	m_showHit;
    TTextCanvasList m_TextCanvasList;
    bool		m_isUsePassWordChar;
    VFont		m_font;
    VCharCanvasEffect m_charCanvasEffect;
    VCharCanvasEffect m_charCanvasEffect_showHit;
    VCharCanvasEffect m_charCanvasEffect_selected;
    char        m_passWordChar;
    long        m_spaceWidth;
    bool        m_isInInputing;
    long        m_selStartPos;
    long        m_selLength;
    long        m_inputPos;
    long        m_viewBeginPos;
    long        m_viewEndPos;
    bool        m_isShowInputTag;
    long		m_inputTagHeight;
    bool        m_isMouseDown;
    bool        m_isNextFoucsByKeyEnter;
    void init(){
        m_isEnableSelf=true;
        m_text.clear();
        m_showHit.clear();
        m_charCanvasEffect.fontColor=cl32White;
        m_isUsePassWordChar=false;
        m_passWordChar='*';
        m_spaceWidth=0;
        m_TextCanvasList.clear();
        setIsEnableSelf(true);
        
        m_isInInputing=false;
        m_inputPos=0;
        m_selStartPos=0;
        m_selLength=0;
        m_viewBeginPos=0;
        m_viewEndPos=0;
        m_isShowInputTag=false;
        m_inputTagHeight=0;
        m_isMouseDown=false;
        m_isNextFoucsByKeyEnter=false;
    }
    virtual void doUpdate(double stepTime_s);
    virtual void updateChange();
    virtual void doUpdateChange();
    virtual void resetPos(TTextCanvasList& textCanvasList);
    void getViewText(W4String& text)const;
    inline bool getIsViewShowHit()const{
        return 	(m_text.size()==0)&&(!m_isInInputing);
    }
    inline bool getIsViewPassWordChar()const{
        return 	(!getIsViewShowHit()) && m_isUsePassWordChar;
    }
    must_inline long getSelBeginPos()const{
        if (m_selLength<=0)
            return m_selStartPos+m_selLength;
        else
            return m_selStartPos;
    }
    must_inline long getSelEndPos()const{
        if (m_selLength<=0)
            return m_selStartPos;
        else
            return m_selStartPos+m_selLength;
    }
    inline bool getIsInSel(long index){
        return (index>=getSelBeginPos())&&(index<getSelEndPos());
    }
    void insertAChar(UInt32 wChar);
    void insertText(const String& text);
    void delSel();
    void delText(long delIndex,long delLength);
    void keyBack();
    void keyTab();
    void keyDel();
    void keyLeft(bool isShiftDown);
    void keyRight(bool isShiftDown);
    void keyEnter();
    void keyCancel();
    void keyHome();
    void keyEnd();
    void setNewInputPos(long newInputPos,bool isShiftDown);
    long clientXToPosIndex(long clientX);
    void copyToClipborad();
    void cutToClipborad();
    void pasteFromClipBoard();
protected:
    class TEditStateData:public IStateData{
    private:
        W4String    m_text;
        long		m_inputPos;
        long		m_selStartPos;
        long		m_selLength;
        long		m_viewBeginPos;
        long		m_viewEndPos;
    public:
        virtual bool isEqual(const IStateData* stateData){
            return m_text==((const TEditStateData*)stateData)->m_text;
        }
        TEditStateData(VEditSprite* src){
            m_text			=src->m_text		;
            m_inputPos		=src->m_inputPos	;
            m_selStartPos	=src->m_selStartPos	;
            m_selLength		=src->m_selLength	;
            m_viewBeginPos	=src->m_viewBeginPos;
            m_viewEndPos	=src->m_viewEndPos	;
        }
        void saveTo(VEditSprite* dst)const{
            dst->m_text			=m_text			;
            dst->m_inputPos		=m_inputPos		;
            dst->m_selStartPos	=m_selStartPos	;
            dst->m_selLength	=m_selLength	;
            dst->m_viewBeginPos	=m_viewBeginPos	;
            dst->m_viewEndPos	=m_viewEndPos	;
        }
    };
    //friend TEditStateData;
    virtual IStateData* doCreateNowStateData();
    virtual void doSetStateData(const IStateData* stateData);
public:
    virtual void onBeginEdit();
    virtual void onEndEdit();
    virtual bool getIsCanDisposeKeyEvent(){
        return true;
    }
    virtual void disposeEventSetFocus();
    virtual void disposeEventLostFocus();
    virtual void disposeEventKeyDown(const TSpriteKeyEventInfo& keyEventInfo);
    virtual void disposeEventKeyUp(const TSpriteKeyEventInfo& keyEventInfo);
    virtual void disposeEventKeyChar(const TSpriteKeyEventInfo& keyEventInfo);
    virtual void disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void disposeEventMouseUp(const TSpriteMouseEventInfo& mouseEventInfo);
    const String getSelText()const;
    bool getIsNextFoucsByKeyEnter() const { return m_isNextFoucsByKeyEnter; }
    void setIsNextFoucsByKeyEnter(bool isNextFoucsByKeyEnter) { m_isNextFoucsByKeyEnter=isNextFoucsByKeyEnter; }
public:
    VEditSprite(long x0,long y0,long width,long height){
        init();
        setLeft(x0);
        setTop(y0);
        setWidth(width);
        setHeight(height);
    }
    virtual ~VEditSprite();
    const String getText()const;
    void setText(const String&  text);
    const String getShowHit()const;
    void setShowHit(const String& showHit);
    
    const Color32& getColor()const {
        return m_charCanvasEffect.fontColor;
    }
    void setColor(const Color32& color){
        if (m_charCanvasEffect.fontColor!=color){
            m_charCanvasEffect.fontColor=color;
            changed();
        }
    }
    VFont& getFont(){
        return m_font;
    }
    bool getIsUsePassWordChar()const{
        return m_isUsePassWordChar;
    }
    void setIsUsePassWordChar(bool isUsePassWordChar){
        if (m_isUsePassWordChar!=isUsePassWordChar){
            m_isUsePassWordChar=isUsePassWordChar;
            changed();
        }
    }
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);	
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    
    void setInInputing(bool isEditing=true);
    void setKeyBordIsTransparence(bool keyBordIsTransparence);
    
private: 
    //回车事件处理
    VEventDisposer m_EnterEventDisposer;
public: 
    //监听回车事件
    void setEnterEventListener(IEventListener* enterEventListener,void* callbackData){
        m_EnterEventDisposer.setEventListener(this,enterEventListener,callbackData);
    }
    //触发回车事件
    virtual void disposeEventKeyEnter() { 
        m_EnterEventDisposer.doEvent(0,0,0);	
    }
    
};

#endif

#endif //_VEditSprite_h_
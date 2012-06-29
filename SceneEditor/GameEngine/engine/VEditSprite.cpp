//VEditSprite.cpp

#include "VEditSprite.h"
#include "VGame.h"

#if defined(_IOS) || defined(_MACOSX)

#include "../../import/importInclude/EditImport.h"

static void EditOnBeginEditEvent(void* callBackData){
    VEditSprite* editSprite=(VEditSprite*)callBackData;
    editSprite->onBeginEdit();
}
static void EditOnEndEditEvent(void* callBackData){
    VEditSprite* editSprite=(VEditSprite*)callBackData;
    editSprite->onEndEdit();
}
static void EditOnInputEvent(void* callBackData,int selPos0,int selLength,const char* inputStr,int* isCanInput){
    /*VEditSprite* editSprite=(VEditSprite*)callBackData;
     bool b_isCanInput=(*isCanInput)!=0;
     editSprite->onInput(selPos0, selLength,inputStr,b_isCanInput);
     if (b_isCanInput)
     *isCanInput=1;
     else
     *isCanInput=0;*/
    *isCanInput=1;
}

//////

VEditSprite::VEditSprite(long x0,long y0,long width,long height){
    init();
    setLeft(x0);
    setTop(y0);
    setWidth(width);
    setHeight(height);
    m_editImportHandle=createImport(this,x0,y0,width,height);
}

VEditSprite::~VEditSprite(){ 
    deleteImport(m_editImportHandle); 
}
const String& VEditSprite::getText()const{		
    getTextFromView();
    return m_text;
}

void VEditSprite::setText(const String&  text){
    getTextFromView();
    if (m_text!=text){
        m_text=text;
        setTextToView();
        changed();
    }
}

void* VEditSprite::createImport(void* onwer,long x0,long y0,long width,long height){
    return edit_create(x0,y0,width,height,onwer,EditOnBeginEditEvent,EditOnEndEditEvent,EditOnInputEvent);
}
void VEditSprite::deleteImport(void* editImportHandle){
    edit_delete(editImportHandle);
}

void VEditSprite::onBeginEdit(){
}
void VEditSprite::onEndEdit(){
}

//void VEditSprite::onInput(long selPos0,long selLength,const char* inputStr,bool& out_isCanInput){
//	out_isCanInput=true;
//}

void VEditSprite::setTextToView(){
    edit_setText(m_editImportHandle,m_text.c_str());
}
void VEditSprite::getTextFromView()const{
    long oldSize=(long)m_text.size();
    long textSize=edit_getText(m_editImportHandle,&m_text[0],oldSize);
    if (textSize<oldSize){
        String newText(m_text.c_str());
        m_text=newText;
    }else if (textSize>oldSize){
        m_text.resize(textSize);
        edit_getText(m_editImportHandle,&m_text[0],textSize);
    }
}

void VEditSprite::updateChange(){
    
    if (m_font.getIsChanged()){
        changed();
        m_font.updateChange();
    }
    this->setIsEnableSelf(true); //因为 m_editImportHandle 不支持 false
    VSprite::updateChange();
}
void VEditSprite::doUpdateChange(){
    long left; long top;
    this->getInSpriteListTreePos(left,top);
    edit_setPosAndSize(m_editImportHandle,left,top,getWidth(),getHeight());
    edit_setFont(m_editImportHandle,m_font.getName().c_str(),m_font.getSize(),(long)m_font.getIsBold());
    edit_setShowHit(m_editImportHandle, m_showHit.c_str());
    edit_setVisible(m_editImportHandle, (long)getIsVisible());
    edit_setColor(m_editImportHandle,m_color.r,m_color.g,m_color.b,m_color.a);
    edit_setIsUsePassWordChar(m_editImportHandle,(long)m_isUsePassWordChar);
} 
void VEditSprite::doDraw(const VCanvas& dst,long x0,long y0){
    //do nothing
}

void VEditSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    //do nothing
    out_rect.setEmpty();
}


void VEditSprite::setInInputing(bool isEditing){
    edit_setInInputing(m_editImportHandle,(long)isEditing);
}

void VEditSprite::setKeyBordIsTransparence(bool keyBordIsTransparence){
    edit_setKeyBordIsTransparence(m_editImportHandle,(long)keyBordIsTransparence);
}

#else

#ifdef WINCE
extern void openKeyboard();
extern void closeKeyboard();
#else
static void openKeyboard(){}
static void closeKeyboard(){}
#endif

#include "VClipborad.h"

const char csKey_back  =  8;
const char csKey_tab   =  9;
const char csKey_enter = 13;
const char csKey_esc   = 27;
const char csKey_del   = 46;
const char csKey_left  = 37;
const char csKey_right = 39;
const char csKey_home  = 36;
const char csKey_end   = 35;

static void wideStrToStr(const UInt32* pwstr,long length,String& out_str){
    //todo:
    assert(false);
}

static void strToWideStr(const char* pstr,long length,W4String& out_wstr){
    //todo:
    assert(false);
}

must_inline static void wideStrToStr(const W4String& wstr,String& out_str){
    long size=(long)wstr.size();
    if (size>0)
        wideStrToStr(&wstr[0],size,out_str);
    else
        out_str.clear();
}
must_inline static void strToWideStr(const String& str,W4String& out_wstr){
    long size=(long)str.size();
    if (size>0)
        strToWideStr(str.c_str(),size,out_wstr);
    else
        out_wstr.clear();
}

must_inline static bool isCanViewWideChar(const UInt32 wChar){
    if (wChar>asciiCharMaxValue) return true;
    if (wChar < ' ') return false;  //小于空格符
    return (!isspace(wChar))||(wChar==' ');
}

static void delSpaceWideChar(W4String& wstr){
    long insertIndex=0;
    long size=(long)wstr.size();
    for (long i=0;i<size;++i){
        if (isCanViewWideChar(wstr[i])){
            wstr[insertIndex]=wstr[i];
            ++insertIndex;
        }
    }
    wstr.resize(insertIndex);
}



VEditSprite::~VEditSprite(){
    if (m_isInInputing)
        closeKeyboard();
}
void VEditSprite::onBeginEdit(){
}
void VEditSprite::onEndEdit(){
}

void VEditSprite::setText(const String&  text){
    W4String wtext;
    strToWideStr(text,wtext);
    delSpaceWideChar(wtext);
    
    if (m_text!=wtext){
        m_text.swap(wtext);
        
        m_selLength=0;
        m_selStartPos=0;
        m_viewBeginPos=0;
        m_inputPos=(long)m_text.size();
        changed();
    }
}

const String VEditSprite::getText()const{
    String result;
    wideStrToStr(m_text,result);
    return result;
}

void VEditSprite::setShowHit(const String&  showHit){
    W4String wshowHit;
    strToWideStr(showHit,wshowHit);
    delSpaceWideChar(wshowHit);
    
    if (m_showHit!=wshowHit){
        m_showHit.swap(wshowHit);
        changed();
    }
}

const String VEditSprite::getShowHit()const{
    String result;
    wideStrToStr(m_showHit,result);
    return result;
}

void VEditSprite::updateChange(){
    if (m_font.getIsChanged()){
        changed();
        m_font.updateChange();
    }
    VSprite::updateChange();
}

void VEditSprite::getViewText(W4String& text)const{
    if (getIsViewShowHit())
        text=m_showHit;
    else if (!m_isUsePassWordChar)
        text=m_text;
    else
        text.resize(m_text.size(),m_passWordChar);
}

void VEditSprite::doUpdateChange(){
    W4String text;
    getViewText(text);
    
    m_charCanvasEffect_showHit.fontColor=m_charCanvasEffect.fontColor;
    m_charCanvasEffect_showHit.fontColor.a>>=1;
    m_charCanvasEffect_selected.fontColor.argb=!m_charCanvasEffect.fontColor.argb;
    m_charCanvasEffect_selected.fontColor.a=m_charCanvasEffect.fontColor.a;
    
    bool isShowHit=getIsViewShowHit();
    
    
    
    TTextCanvasList newTextCanvasList;
    long size=(long)text.size();
    for (long i=0;i<size;++i){
        VCharCanvasEffect* charCanvasEffect;
        if (isShowHit)
            charCanvasEffect=&m_charCanvasEffect_showHit;
        else if (getIsInSel(i))
            charCanvasEffect=&m_charCanvasEffect_selected;
        else
            charCanvasEffect=&m_charCanvasEffect;
        newTextCanvasList.push_back(VPosCharCanvas(m_font,text[i],charCanvasEffect));
    }
    
    resetPos(newTextCanvasList);
    m_TextCanvasList.swap(newTextCanvasList);
    
    if (m_TextCanvasList.size()>0)
        m_inputTagHeight=m_TextCanvasList[0].getHeight();
    else
        m_inputTagHeight=VPosCharCanvas(m_font,'A',&m_charCanvasEffect).getHeight();
    
    m_viewEndPos=size;
    if (m_inputPos>size) m_inputPos=size;
    if (m_inputPos<0) m_inputPos=0;
    if (size<=1){ //保证至少能看见一个字符
        m_viewBeginPos=0;
    }else{
        //保证至少能看见一个字符 并且输入标志可见
        if (m_viewBeginPos>=m_viewEndPos) m_viewBeginPos=m_viewEndPos-1;
        if (m_viewBeginPos>m_inputPos) m_viewBeginPos=m_inputPos;
        
        //处理没有填满 右移
        while (m_viewBeginPos>0){
            //是否可以右移
            if (m_TextCanvasList[m_viewEndPos-1].getPosXRight()-m_TextCanvasList[m_viewBeginPos-1].posX>getWidth()) break;
            --m_viewBeginPos;
        }
        
        //处理左移
        while (m_viewEndPos-m_viewBeginPos>1){
            if (m_TextCanvasList[m_viewEndPos-1].getPosXRight()-m_TextCanvasList[m_viewBeginPos].posX<=getWidth()) break;
            if (m_viewEndPos>m_inputPos){
                --m_viewEndPos;
                continue;
            }else if (m_viewBeginPos<m_inputPos){
                ++m_viewBeginPos;
                continue;
            }else
                break;
        }
    }	
    
    stateChanged();
} 

void VEditSprite::resetPos(TTextCanvasList& textCanvasList){
    long textCount=(long)textCanvasList.size();
    long x0=0;
    for (long i=0;i<textCount;++i){
        textCanvasList[i].posX=x0;
        long y0=(this->getHeight()-textCanvasList[i].getHeight())/2;
        textCanvasList[i].posY=y0;
        x0+=textCanvasList[i].getWidth();
    }
}

void VEditSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if ((getWidth()<=0)||(getHeight()<=0)) 
        return;
    //debug dst.rect(x0,y0,x0+getWidth(),y0+getHeight(),Color32(255,100,145));
    if (m_viewBeginPos<m_viewEndPos)
        x0-=m_TextCanvasList[m_viewBeginPos].posX;
    
    long inputTagTop=(getHeight()-m_inputTagHeight)/2;
    long inputTagBottom=inputTagTop+m_inputTagHeight;
    if (inputTagBottom>getHeight())
        inputTagBottom=getHeight();
    
    long textCount=(long)m_TextCanvasList.size(); 
    
    bool isHaveSel=false;
    long selMinX;
    long selMaxX; 
    for (long i=m_viewBeginPos;i<m_viewEndPos;++i){	
        const VPosCharCanvas& textCanvas=m_TextCanvasList[i];
        if (getIsInSel(i)){
            if (!isHaveSel){
                isHaveSel=true;
                selMinX=textCanvas.posX;
                selMaxX=textCanvas.posX+textCanvas.getWidth();
            }else{
                if (textCanvas.posX<selMinX) 
                    selMinX=textCanvas.posX;
                else if (textCanvas.posX+textCanvas.getWidth()>selMaxX) 
                    selMaxX=textCanvas.posX+textCanvas.getWidth();
            }
        }
    }
    //绘制选择背景
    if (isHaveSel){
        Color32 selBackColor(m_charCanvasEffect.fontColor);
        selBackColor.a>>=2;
        dst.fillBlend(x0+selMinX,y0+inputTagTop,x0+selMaxX,y0+inputTagBottom,selBackColor);
    }
    
    for (long i=m_viewBeginPos;i<m_viewEndPos;++i){	
        const VPosCharCanvas& textCanvas=m_TextCanvasList[i];
        dst.blendLight(x0+textCanvas.posX,y0+textCanvas.posY,textCanvas.getCanvas(),textCanvas.charCanvasEffect->fontColor);
    }
    
    //tag
    if (m_isInInputing && m_isShowInputTag){
        long left=x0;
        if (textCount==0)
            ;
        else if (m_inputPos<textCount)
            left+=m_TextCanvasList[m_inputPos].posX;
        else
            left+=m_TextCanvasList[textCount-1].posX+m_TextCanvasList[textCount-1].getWidth()-1;
        dst.lineV(left,y0+inputTagTop,y0+inputTagBottom,m_charCanvasEffect.fontColor);
    }
}

void VEditSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    out_rect.setEmpty(x0,y0);
    if ((getWidth()<=0)||(getHeight()<=0)) 
        return;
    
    if (m_viewBeginPos<m_viewEndPos)
        x0-=m_TextCanvasList[m_viewBeginPos].posX;
    
    long inputTagTop=(getHeight()-m_inputTagHeight)/2;
    long inputTagBottom=inputTagTop+m_inputTagHeight;
    if (inputTagBottom>getHeight())
        inputTagBottom=getHeight();
    
    
    long textCount=(long)m_TextCanvasList.size(); 
    
    TRect rect;
    bool isAddRect=false;
    for (long i=m_viewBeginPos;i<m_viewEndPos;++i){	
        const VPosCharCanvas& textCanvas=m_TextCanvasList[i];
        
        if (!isAddRect){
            isAddRect=true;
            rect.setRect(x0+textCanvas.posX,y0+textCanvas.posY,
                         x0+textCanvas.posX+textCanvas.getWidth(),y0+textCanvas.posY+textCanvas.getHeight());
        }else{
            rect.max(x0+textCanvas.posX,y0+textCanvas.posY,
                     x0+textCanvas.posX+textCanvas.getWidth(),y0+textCanvas.posY+textCanvas.getHeight());
        }
    }
    
    //tag
    if (m_isInInputing && m_isShowInputTag)
    {
        long left=x0;
        if (textCount==0)
            ;
        else if (m_inputPos<textCount)
            left+=m_TextCanvasList[m_inputPos].posX;
        else
            left+=m_TextCanvasList[textCount-1].posX+m_TextCanvasList[textCount-1].getWidth()-1;
        
        if (!isAddRect){
            isAddRect=true;
            rect.setRect(left,y0+inputTagTop,left+1,y0+inputTagBottom);
        }else{
            rect.max(left,y0+inputTagTop,left+1,y0+inputTagBottom);
        }
    }
    if (isAddRect)
        out_rect=rect;
}


void VEditSprite::doUpdate(double stepTime_s){
    
    //tag
    const double flastInputTagHalfTime=0.4;
    bool isShowImputTag=fmod(getMMTimerCount()*0.001,flastInputTagHalfTime*2)<flastInputTagHalfTime;
    if (isShowImputTag!=m_isShowInputTag){
        m_isShowInputTag=isShowImputTag;
        drawChanged();
    }
    
}

void VEditSprite::setInInputing(bool isEditing){
    setIsWantFocus(isEditing);
}

void VEditSprite::setKeyBordIsTransparence(bool keyBordIsTransparence){
    //do nothing
}


void VEditSprite::disposeEventSetFocus(){
    openKeyboard();
    m_isInInputing=true;
    drawChanged();
    onBeginEdit();
}
void VEditSprite::disposeEventLostFocus(){
    closeKeyboard();
    m_isInInputing=false;
    drawChanged();
    onEndEdit();
}


void VEditSprite::delText(long delIndex,long delLength){
    if (delLength<=0) return;
    assert(delIndex>=0);
    long size=(long)m_text.size();
    long moveCount=size-(delIndex+delLength);
    assert(moveCount>=0);
    for (long i=delIndex;i<delIndex+moveCount;++i)
        m_text[i]=m_text[i+delLength];
    m_text.resize(size-delLength);
    changed();
}

void VEditSprite::delSel(){
    if (m_selLength!=0){
        m_inputPos=getSelBeginPos();
        delText(getSelBeginPos(),MyBase::abs(m_selLength));
        m_selStartPos=0;
        m_selLength=0;
    }
}

void VEditSprite::insertAChar(UInt32 wChar){
    if (!isCanViewWideChar(wChar)) return;
    delSel();
    m_text.insert(m_text.begin()+m_inputPos,wChar);
    ++m_inputPos;
    changed();
}
void VEditSprite::insertText(const String& text){
    W4String wtext;
    strToWideStr(text,wtext);
    delSpaceWideChar(wtext);
    
    long size=(long)wtext.size();
    for (long i=0;i<size;++i)
        insertAChar(wtext[i]);
}


void VEditSprite::keyBack(){
    if (m_selLength!=0)
        delSel();
    else if (m_inputPos>0){
        delText(m_inputPos-1,1);
        --m_inputPos;
        changed();
    }
}

void VEditSprite::keyDel(){
    if (m_selLength!=0)
        delSel();
    else if (m_inputPos<(long)m_text.size()){
        delText(m_inputPos,1);
        changed();
    }
}

void VEditSprite::setNewInputPos(long newInputPos,bool isShiftDown){
    if (newInputPos<0) newInputPos=0;	
    long textSize=(long)m_text.size();
    if (newInputPos>textSize) newInputPos=textSize;	
    
    if (newInputPos==m_inputPos) return;
    
    if (isShiftDown){
        if (m_selLength==0) 
            m_selStartPos=m_inputPos;
        m_selLength=newInputPos-m_selStartPos;
    }else{
        m_selLength=0;
        m_selStartPos=newInputPos;
    }
    m_inputPos=newInputPos;
    changed();
}

void VEditSprite::keyLeft(bool isShiftDown){
    if ((!isShiftDown)&&(m_selLength!=0)){
        m_inputPos=getSelBeginPos();
        m_selLength=0;
        changed();
    }else
        setNewInputPos(m_inputPos-1,isShiftDown);
}

void VEditSprite::keyRight(bool isShiftDown){
    if ((!isShiftDown)&&(m_selLength!=0)){
        m_inputPos=getSelEndPos();
        m_selLength=0;
        changed();
    }else
        setNewInputPos(m_inputPos+1,isShiftDown);
}

void VEditSprite::keyEnter(){
    if (m_isNextFoucsByKeyEnter)
        wantNextFocus();
#ifdef WINCE
    else
        setInInputing(false);
#endif
    
    //Enter event
}

void VEditSprite::keyTab(){
    wantNextFocus();
}

void VEditSprite::keyCancel(){
    setInInputing(false);
}

void VEditSprite::keyHome(){
    m_inputPos=0;
    m_selLength=0;
}

void VEditSprite::keyEnd(){
    m_inputPos=(long)m_text.size();
    m_selLength=0;
}

void VEditSprite::disposeEventKeyChar(const TSpriteKeyEventInfo& keyEventInfo){
    insertAChar(keyEventInfo.aChar);
}

void VEditSprite::disposeEventKeyUp(const TSpriteKeyEventInfo& keyEventInfo){
}
void VEditSprite::disposeEventKeyDown(const TSpriteKeyEventInfo& keyEventInfo){
    switch (keyEventInfo.aChar){
        case csKey_back:	{ keyBack(); } break; 
        case csKey_tab:		{ keyTab(); } break; 
        case csKey_del:		{ keyDel(); } break; 
        case csKey_left:	{ keyLeft(keyEventInfo.keyBoardState.isShiftDown); } break; 
        case csKey_right:	{ keyRight(keyEventInfo.keyBoardState.isShiftDown); } break; 
        case csKey_enter:	{ keyEnter(); } break; 
        case csKey_esc:		{ keyCancel(); } break; 
        case csKey_home:	{ keyHome(); } break; 
        case csKey_end:		{ keyEnd(); } break; 
            
        case 'C':case 'c':  { if (keyEventInfo.keyBoardState.isCtrlDown) copyToClipborad(); } break; 
        case 'X':case 'x':  { if (keyEventInfo.keyBoardState.isCtrlDown) cutToClipborad(); } break; 
        case 'V':case 'v':  { if (keyEventInfo.keyBoardState.isCtrlDown) pasteFromClipBoard(); } break; 
        case 'Z':case 'z':       { 
            if (keyEventInfo.keyBoardState.isCtrlDown){ 
                if (keyEventInfo.keyBoardState.isShiftDown) 
                    stateRedo(); 
                else  
                    stateUndo(); 
            } 
        } break; 
        case 'Y':case 'y':       { if (keyEventInfo.keyBoardState.isCtrlDown) stateRedo(); } break; 
    }
}

const String VEditSprite::getSelText()const {
    if (m_selLength!=0){
        W4String text;
        getViewText(text);
        String result;
        wideStrToStr(&text[getSelBeginPos()],MyBase::abs(m_selLength),result);
        return result;
    }else
        return "";
}


long VEditSprite::clientXToPosIndex(long clientX){
    updateChange();
    long minValueIndex=m_inputPos;
    long textSize=(long)m_text.size();
    if (textSize<=0) return minValueIndex;
    clientX+=m_TextCanvasList[m_viewBeginPos].posX;
    
    //查找最近的一个位置 特殊处理最后一个位置
    long minValue=1<<30;
    for (long i=0;i<textSize;++i){
        long value;
        if (clientX<=m_TextCanvasList[i].posX)
            value=m_TextCanvasList[i].posX-clientX;
        else if (clientX>=m_TextCanvasList[i].getPosXRight())
            value=clientX-m_TextCanvasList[i].getPosXRight();
        else{
            minValueIndex=i;
            break;
        }
        if (value<minValue){
            minValue=value;
            minValueIndex=i;
        }
    }
    if (clientX>=m_TextCanvasList[minValueIndex].posX+m_TextCanvasList[minValueIndex].getWidth()/2)
        ++minValueIndex;
    return minValueIndex;
}

void VEditSprite::disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo){
    m_isMouseDown=true;
    setNewInputPos(clientXToPosIndex(mouseEventInfo.clientX0),mouseEventInfo.keyBoardState.isShiftDown);
}

void VEditSprite::disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo){
    if (!m_isMouseDown) return ;
    //todo: 当拖动到边界外的时候  控制触发时间间隔和调整量
    setNewInputPos(clientXToPosIndex(mouseEventInfo.clientX0),mouseEventInfo.keyBoardState.isShiftDown || m_isMouseDown);
}
void VEditSprite::disposeEventMouseUp(const TSpriteMouseEventInfo& mouseEventInfo){
    setNewInputPos(clientXToPosIndex(mouseEventInfo.clientX0),mouseEventInfo.keyBoardState.isShiftDown);
    m_isMouseDown=false;
}


void VEditSprite::copyToClipborad(){
    if (m_selLength!=0){
        VClipborad::setText(getSelText());
    }
}

void VEditSprite::cutToClipborad(){
    copyToClipborad();
    delSel();
}
void VEditSprite::pasteFromClipBoard(){
    String text=VClipborad::getText();
    if (text.size()>0){
        insertText(text);
    }
}

//////

VEditSprite::IStateData* VEditSprite::doCreateNowStateData(){
    return new TEditStateData(this);
}

void VEditSprite::doSetStateData(const VEditSprite::IStateData* stateData){
    const TEditStateData* editStateData=(const TEditStateData*)stateData;
    editStateData->saveTo(this);
    doUpdateChange();
}

#endif
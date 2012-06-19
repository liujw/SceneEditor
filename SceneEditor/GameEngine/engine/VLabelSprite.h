//
//  VLabelSprite.h
//
//  Created by housisong on 08-4-9.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VLabelSprite_h_
#define _VLabelSprite_h_

#include "VSprite.h"
#include "VCharCanvas.h"
#include "VFont.h"

const int csTag_Line = '\n';

class VLabelSprite : public VSprite{
    //TODO 支持对齐方式\支持自动换行\ 支持行间距和字间距
public:
    
    //字形修饰
    struct VCharCanvasEffect{
        Color32 fontColor;
        virtual ~VCharCanvasEffect(){}
    };
    
    struct VPosCharCanvas:public VCharCanvas{
        long	posX;
        long	posY;
        VCharCanvasEffect* charCanvasEffect;
        must_inline long getPosXRight()const{ return posX+getWidth(); }
        must_inline bool isInRect(long x,long y,long spaceWidth=0,long spaceHeight=0)const{
            return (x>=posX-(spaceWidth>>1))&&(x<getPosXRight()+(spaceWidth>>1))
            &&(y>=posY-(spaceHeight>>1))&&(y<posY+getHeight()+(spaceHeight>>1));
        }
        must_inline  VPosCharCanvas(const VFont& font,int aChar,VCharCanvasEffect* aCharCanvasEffect):VCharCanvas(font,aChar),charCanvasEffect(aCharCanvasEffect) { }
        must_inline  VPosCharCanvas(const VPosCharCanvas& charCanvas):VCharCanvas(charCanvas),posX(charCanvas.posX),posY(charCanvas.posY),charCanvasEffect(charCanvas.charCanvasEffect){ }
    };
    typedef Vector<VPosCharCanvas> TTextCanvasList;//
    static void getCharCanvasList(const String& text,const VFont& font,VCharCanvasEffect* charCanvasEffect,TTextCanvasList& out_TextCanvasList);
protected:
    String		m_text;
    VFont		m_font;
    VCharCanvasEffect m_charCanvasEffect;
    bool        m_isAllowMultiLine;  //允许多行显示  处理换行符
    bool        m_isAutoLine;        //超过宽度自动换行
    bool        m_isAutoLineNotClipEnWord; //自动换行时 尽量不拆开英文单词
    bool        m_isAutoAlignment;//add new
    long        m_spaceHeight;
    long        m_spaceWidth;
    long        m_txtPixelHeight;//add new
    //记录lable文本是否发生改变
    bool        m_isTextViewsChanged;
    //记录文本的绘制区域大小，这样就不用每次都重新计算区域大小
    TRect       m_textRectInfo;
    TAlignType  m_align;
    //文本发生改变
    must_inline void textChanged(){
        m_isTextViewsChanged=true;
        changed();
    }
    TTextCanvasList m_TextCanvasList;
    typedef Vector<long>  TTextlineStartIndexList;
    TTextlineStartIndexList m_textlineStartIndexList;
    
    long setLinePos_returnHeight(VPosCharCanvas* textCanvas,long count,long posY);
    
    void init(){
        m_text="";
        m_charCanvasEffect.fontColor=cl32White;
        m_TextCanvasList.clear();
        m_spaceHeight=0;
        m_spaceWidth=0;
        m_txtPixelHeight=0;
        m_align=align_left;
        m_isAllowMultiLine=true;
        m_isAutoLine=false;
        m_isAutoLineNotClipEnWord=false;//true;
        m_isAutoAlignment = true;
        m_isTextViewsChanged=true;
    }
    virtual void resetPos(TTextCanvasList& textCanvasList,TTextlineStartIndexList& out_textlineStartIndexList);
    virtual void updateChange();
    virtual void doUpdateChange();
    
    //
    virtual void doDrawAChar(const VCanvas& dst,long x0,long y0,const VCharCanvas& charCanvas,VCharCanvasEffect* charCanvasEffect);
    const VPosCharCanvas* testHitCharCanvas(long x,long y);
private://add new
    void adjustOffetPos(TTextCanvasList& textCanvasList,long lineBeginWordIndex, long lineEndWordIndex);
public:
    explicit VLabelSprite(){
        init();
    }
    explicit VLabelSprite(const String& text){
        init();
        setText(text);
    }    
    const String& getText()const{
        return m_text;
    }
    void setText(const String& text, long showWidthFirstLine = 0){
        if (showWidthFirstLine == 0) {
            if (m_text!=text){
                clear();
                addLine(text);
                textChanged();
            }
        }
        else {
            //只取cutFixedWidthFirstLine宽度的字符进行显示,只有一行
            String str = text;
            cutFixedLenFirstLine(str, showWidthFirstLine);
            clear();
            addLine(str);
            textChanged();
        }
    }
    virtual void clear(){
        if (!m_text.empty()){
            m_text.clear();
            textChanged();
        }
    }
    
    virtual void setAlpha(long alpha)
    {
        if (alpha<0) alpha=0;
        else if (alpha>255) alpha=255;
        if (m_charCanvasEffect.fontColor.a != alpha){
            m_charCanvasEffect.fontColor.a = (UInt8)alpha;
            textChanged();
        }		
    }
    
    virtual void addLine(const String& text){
        if (!m_text.empty())
            m_text+=csTag_Line;
        m_text+=text;
        textChanged();
    }
    const Color32& getColor()const {
        return m_charCanvasEffect.fontColor;
    }
    void setColor(const Color32& color){
        if (m_charCanvasEffect.fontColor!=color){
            m_charCanvasEffect.fontColor=color;
            textChanged();
        }
    }
    VFont& getFont(){
        return m_font;
    }
    const VFont& getFont()const{
        return m_font;
    }
    void setFont(const VFont& font){
        m_font.assign(font);
    }
    
    TAlignType getAlign()const {
        return m_align;
    }
    void setAlign(TAlignType align){
        if (m_align!=align){
            m_align=align;
            textChanged();
        }
    }	
    bool getIsAllowMultiLine()const{
        return m_isAllowMultiLine;
    }
    virtual void setIsAllowMultiLine(bool isAllowMultiLine){
        if (m_isAllowMultiLine!=isAllowMultiLine){
            m_isAllowMultiLine=isAllowMultiLine;
            textChanged();
        }
    }
    bool getIsAutoLine()const{
        return m_isAutoLine;
    }
    virtual void setIsAutoLine(bool isAutoLine){
        if (m_isAutoLine!=isAutoLine){
            m_isAutoLine=isAutoLine;
            textChanged();
        }
    }
    bool getIsAutoLineNotClipEnWord()const{
        return m_isAutoLineNotClipEnWord;
    }
    virtual void setIsAutoLineNotClipEnWord(bool isAutoLineNotClipEnWord){
        if (m_isAutoLineNotClipEnWord!=isAutoLineNotClipEnWord){
            m_isAutoLineNotClipEnWord=isAutoLineNotClipEnWord;
            textChanged();
        }
    }
    bool getIsAutoAlignment(){
        return m_isAutoAlignment;
    }
    virtual void setIsAutoAlignment(bool isAutoAlignment){
        if (m_isAutoAlignment!=isAutoAlignment){
            m_isAutoAlignment=isAutoAlignment;
            textChanged();
        }
    }
    
    long getSpaceHeight()const{
        return m_spaceHeight;
    }
    void setSpaceHeight(long spaceHeight){
        if (m_spaceHeight!=spaceHeight){
            m_spaceHeight=spaceHeight;
            textChanged();
        }
    }
    long getSpaceWidth()const{
        return m_spaceWidth;
    }
    void setSpaceWidth(long spaceWidth){
        if (m_spaceWidth!=spaceWidth){
            m_spaceWidth=spaceWidth;
            textChanged();
        }
    }
    long getLineCount(){
        updateChange();
        return (long)m_textlineStartIndexList.size();
    }
    long getAllTextLinePixelsHeight(){//add new 
        updateChange();
        return m_txtPixelHeight;
    }
    long getMaxLinePixelsWidth(){
        TRect drawRect;
        getWantDrawRect(0, 0, drawRect);
        return drawRect.getWidth();
    }
    virtual void doDraw(const VCanvas& dst,long x0,long y0);	
    virtual bool getIsHit(long clientX0, long clientY0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    
    //取第一行可显示的指定范围宽度的字符
    void cutFixedLenFirstLine(String& text, const long cutWidth);
};

#endif //_VLabelSprite_h_
/*
 *  VMemoSprite.h
 *
 *  Created by housisong on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VMemoSprite_h_
#define _VMemoSprite_h_

#include "VLabelSprite.h"
#include "VScrollBarSprite.h"

class IHotspotListener{
public:
    virtual void doHotspotLink(VSprite* sender,const String& hotspotName,const String& hotspotText)=0;
    virtual ~IHotspotListener(){}
};


class VMemoSprite:public VLabelSprite,IScrollBarListener{
public:
    struct THotspotInfo{
        String		hotspotName;
        String		hotspotText;
        inline bool empty()const{
            return (hotspotName.empty())&&(hotspotText.empty());
        }
    };
    
    //字形修饰
    struct VExCharCanvasEffect:public VCharCanvasEffect{
        long			usedCount;
        bool			isUnderline;
        THotspotInfo	hotspotInfo;
    };
    
    typedef Vector<VExCharCanvasEffect*> TExCharCanvasEffectList;
protected:
    long					m_topLine;
    long					m_viewLineCount;
    IHotspotListener*		m_hotspotListener;
    TTextCanvasList			m_allTextCanvasList;
    long                    m_maxLineCount;
    //标识文本是否发生改变
    bool                    m_isTextViewsChanged;
    must_inline void textChanged(){
        VLabelSprite::textChanged();
        m_isTextViewsChanged=true;
        changed();
    }
    struct TLineText {
        long		textCanvasIndexBegin;
        long        textCanvasIndexEnd;		
    };
    
    Vector<TLineText>	m_lineList;
    
    VScrollBarSpriteBase*	m_scrollBar;
    void updateScrollBar(){
        if (m_scrollBar==0) return;
        double new_posision=0;
        if (getLineCount()-m_viewLineCount>0)
            new_posision=m_topLine/(getLineCount()-m_viewLineCount);
        m_scrollBar->setPosision(new_posision);
    }
    
    void delAllTextCanvasListByIndexs(long delBeginIndex,long delEndIndex);
    
    virtual void doUpdateChange();	
    virtual void doDrawAChar(const VCanvas& dst,long x0,long y0,const VCharCanvas& charCanvas,VCharCanvasEffect* charCanvasEffect);
public:
    //IScrollBarListener
    virtual void onScroll(VScrollBarSpriteBase* sender,double posision){
        updateChange();
        long newTopLine=(long)((getLineCount()-m_viewLineCount)*posision+0.5);
        setTopLine(newTopLine);
        updateChange();
    }
public:
    VMemoSprite(long width,long viewLineCount){ 
        this->setIsEnableSelf(true);
        setWidth(width); 
        setIsAutoLine(true);
        setIsAllowMultiLine(true);
        m_topLine=0;	
        m_hotspotListener=0;
        m_maxLineCount=200;
        setViewLineCount(viewLineCount);
        m_isTextViewsChanged=true;
    }
    virtual ~VMemoSprite(){	clear(); }
    
    long getTopLine()const{
        return m_topLine;
    }
    
    void setTopLine(long topLine){		
        if (topLine>getLineCount()-1) topLine=getLineCount()-1;
        if (topLine<0) topLine=0;
        
        if (m_topLine!=topLine){
            m_topLine=topLine;
            textChanged();
        }
    }
    
    long getLineCount()const{
        return (long)m_lineList.size();
    }
    
    long getViewLineCount()const{
        return m_viewLineCount;
    }
    void setViewLineCount(long viewCount){
        if (viewCount<0) viewCount=0;
        if (m_viewLineCount!=viewCount){
            m_viewLineCount=viewCount;
            textChanged();
        }
    }
    
    long getMaxLineCount()const{
        return m_maxLineCount;
    }
    void setMaxLineCount(long maxLineCount){
        if (m_maxLineCount!=maxLineCount){
            m_maxLineCount=maxLineCount;
            textChanged();
        }			
    }
    
    void setHotspotListener(IHotspotListener* hotspotListener){
        m_hotspotListener=hotspotListener;
    }
    
    virtual void clear();
    
    virtual void addLine(const String& text);
    
    virtual void setIsAllowMultiLine(bool isAllowMultiLine){
        VLabelSprite::setIsAllowMultiLine(true);
    }
    virtual void setIsAutoLine(bool isAutoLine){
        VLabelSprite::setIsAutoLine(true);
    }
    
    void setScrollBar(VScrollBarSpriteBase* scrollBar){
        m_scrollBar=scrollBar;
        this->addASprite(scrollBar);
        scrollBar->setScrollListener(this);
    }
    
    void toFirstLine(){
        //updateChange();
        setTopLine(0);
    }
    void toLastLine(){
        updateChange();
        setTopLine(getLineCount()-getViewLineCount());
    }
    
    virtual void disposeEventMouseClick(const TSpriteMouseEventInfo& mouseDownEventInfo,const TSpriteMouseEventInfo& mouseUpEventInfo);
};




#endif //_VMemoSprite_h_

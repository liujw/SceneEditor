//
//  VBitmapReflectionSprite.h
//
#ifndef _VBitmapReflectionSprite_h_
#define _VBitmapReflectionSprite_h_

#include "VBitmapBaseSprite.h"

class VBitmapReflectionSprite:public VBitmapBaseSprite{
protected:
    long			m_alphaBegin;
    long			m_alphaEnd;
    bool			m_isReversalY;
    Color32			m_coloring;	
    
    Vector<UInt8>	m_alphas;
    Vector<Color32>	m_colorings;
    long            m_ymin;
    long			m_ymax;
    void init(){
        m_alphaBegin=255;
        m_alphaEnd=0;
        m_isReversalY=true;
        m_coloring=cl32ColoringMullEmpty;
        this->setIsEnableSelf(false);
        changed();
    }
    void drawTo(const VCanvas& dst,long x0,long y0,const VCanvas& src);
public:
    explicit VBitmapReflectionSprite(){ init(); }
    explicit VBitmapReflectionSprite(const VCanvas&  bitmap):VBitmapBaseSprite(bitmap){
        init();
    }
    
    //渐变透明度
    inline long getAlphaBegin()const{
        return m_alphaBegin;
    }
    inline void setAlphaBegin(long alpha){
        if (m_alphaBegin!=alpha){
            m_alphaBegin=alpha;
            changed();
        }
    }
    inline long getAlphaEnd()const{
        return m_alphaEnd;
    }
    inline void setAlphaEnd(long alpha){
        if (m_alphaEnd!=alpha){
            m_alphaEnd=alpha;
            changed();
        }
    }
    
    //是否颠倒Y轴
    inline bool getIsReversalY()const{
        return m_isReversalY;
    }
    inline void setIsReversalY(bool isReversalY){
        if (m_isReversalY!=isReversalY){
            m_isReversalY=isReversalY;
            changed();
        }
    }
    
    //着色 
    const Color32& getColoring()const{
        return m_coloring;
    }
    //coloring is ARGB 32bit Color
    void setColoring(const Color32& coloring){
        if (m_coloring!=coloring){
            m_coloring=coloring;
            changed();
        }
    }
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual bool getIsHit(long clientX0, long clientY0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    virtual void doUpdateChange();
};

#endif //_VBitmapReflectionSprite_h_
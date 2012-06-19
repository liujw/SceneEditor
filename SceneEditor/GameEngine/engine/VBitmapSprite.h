//
//  VBitmapSprite.h
//
//  Created by housisong on 08-4-3.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VBitmapSprite_h_
#define _VBitmapSprite_h_

#include "VBitmapBaseSprite.h"

class VBitmapSprite:public VBitmapBaseSprite{
protected:
    long		m_alpha;
    Color32     m_coloring;
    void init(){
        m_alpha=0xFF;
        m_coloring=cl32ColoringMullEmpty;
    }
    
    void drawTo(const VCanvas& dst,long x0,long y0,const VCanvas& src);
public:
    explicit VBitmapSprite(){ init(); }
    explicit VBitmapSprite(const VCanvas&  bitmap):VBitmapBaseSprite(bitmap){
        init();
    }
    
    //透明度 alpha in [0..255]
    long getAlpha()const{
        return m_alpha;
    }
    void setAlpha(long alpha){
        if (m_alpha!=alpha){
            m_alpha=alpha;
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
};

#endif //_VBitmapSprite_h_
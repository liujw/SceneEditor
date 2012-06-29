/*
 *  VColorSprite.h
 *  testEngine
 *
 *  Created by housisong on 08-10-24.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VColorSprite_h_
#define _VColorSprite_h_
#include "VSprite.h"

class VColorSprite:public VSprite{
private:
    Color32 m_color;
public:
    VColorSprite():m_color(cl32Empty){}
    VColorSprite(const Color32& color):m_color(color){}
    const Color32& getColor()const{ return m_color; }
    void setColor(const Color32& color);
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    virtual bool getIsHit(long clientX0, long clientY0);
    virtual void setAlpha(long alpha);
};



#endif //_VColorSprite_h_
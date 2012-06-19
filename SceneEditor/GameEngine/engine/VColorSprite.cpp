/*
 *  VColorSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-10-24.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VColorSprite.h"

void VColorSprite::setColor(const Color32& color){
    if (color!=m_color){
        if ((color.a==0)&&(color.a==m_color.a)){
            m_color=color;
        }else{
            m_color=color;
            changed();
        }
    }
}

void VColorSprite::setAlpha(long alpha)
{
    if (alpha != m_color.a){
        m_color.a = alpha;
        changed();
    }	
}

void VColorSprite::doDraw(const VCanvas& dst,long x0,long y0){
    dst.fillBlend(x0,y0,x0+getWidth(),y0+getHeight(),m_color);
}

void VColorSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    out_rect.setRect(x0,y0,x0+getWidth(),y0+getHeight());
}
bool VColorSprite::getIsHit(long clientX0, long clientY0){
    return (clientX0>=0)&&(clientY0>=0)&&(clientX0<getWidth())&&(clientY0<getHeight());
}
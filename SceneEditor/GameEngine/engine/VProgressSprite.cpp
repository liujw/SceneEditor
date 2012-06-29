/*
 *  VProgressSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VProgressSprite.h"

void VProgressSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (m_canvas.getIsEmpty()) return;
    long sx0=0;
    long sy0=0;
    long sx1=m_canvas.getWidth();
    long sy1=m_canvas.getHeight();
    if (m_directionType==dirt_right){
        sx1=(long)(m_progress*sx1/m_maxProgress);
    }else if (m_directionType==dirt_left){
        sx0=sx1-(long)(m_progress*sx1/m_maxProgress);
    }else if (m_directionType==dirt_top){
        sy0=sy1-(long)(m_progress*sy1/m_maxProgress);
    }else {//if (m_directionType==dirt_bottom){
        sy1=(long)(m_progress*sy1/m_maxProgress);
    }
    
    VCanvas subCanvas;
    m_canvas.getCanvasSub(sx0,sy0,sx1,sy1,subCanvas);
    if (!subCanvas.getIsEmpty())	
        VBitmapSprite::drawTo(dst,x0+sx0,y0+sy0,subCanvas);
}
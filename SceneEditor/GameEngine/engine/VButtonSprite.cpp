/*
 *  VButtonSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-18.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VButtonSprite.h"

void  VButtonSprite::doUpdateChange(){
    VBmpButtonSprite::doUpdateChange();
    VCanvas bmpCanvas; 
    long index=(long)m_state; 
    if (!getIsEnableSelf()) index=3;
    index=m_btnBmpStartIndex + index*m_btnBmpSkipCount;
    m_btnBmpClip.getSubCanvasByClipIndex(index,bmpCanvas);
    VBitmapSprite::setBitmap(bmpCanvas);
}

void VButtonSprite::doDraw(const VCanvas& dst,long x0,long y0){
    VBitmapSprite::doDraw(dst,x0,y0);
}

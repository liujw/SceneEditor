/*
 *  VGrid9BitmapSprite.h
 *  Dzpk
 *
 *  Created by wsh on 08-11-13.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VGrid9BitmapSprite_h_
#define _VGrid9BitmapSprite_h_

#include "VBitmapSprite.h"

class VGrid9BitmapSprite: public VSprite
{
public:
    explicit VGrid9BitmapSprite(const VClipCanvas& bmpClip);
protected:
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    virtual bool getIsHit(long clientX0, long clientY0);
    void getValidRect(long& nWidth, long& nHeight);
    virtual void doUpdateChange();
protected:
    VClipCanvas m_BmpClip;
    VSurface m_Pic;
};

#endif
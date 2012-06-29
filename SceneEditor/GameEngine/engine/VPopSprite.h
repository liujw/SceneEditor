/*
 *  VPopSprite.h
 *  Ddz
 *
 *  Created by wsh on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VPopSprite_h_
#define _VPopSprite_h_

#include "VMoveBitmapSprite.h"
#include "VAction.h"

class VPopSprite: public VMoveBitmapSprite
{
public:
    explicit VPopSprite(const VCanvas&  bitmap);
public:
    void addContent(VSprite* pContent, long x, long y);
    void addContent(const VCanvas& bitmap, long x, long y);
protected:
    VSurface m_Pic;
};

#endif

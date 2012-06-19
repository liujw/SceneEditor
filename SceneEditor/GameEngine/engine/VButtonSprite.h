/*
 *  VButtonSprite.h
 *
 *  Created by housisong on 08-6-18.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _VButtonSprite_h_
#define _VButtonSprite_h_

#include "VBmpButtonSprite.h" 

class VButtonSprite:public VBmpButtonSprite{
protected:
    VClipCanvas		m_btnBmpClip;
    long            m_btnBmpStartIndex;
    long            m_btnBmpSkipCount;
    
    virtual void doUpdateChange();
public:
    //explicit VButtonSprite()(){}
    explicit VButtonSprite(const VClipCanvas& btnBmpClip,long bmpStartIndex=0,long bmpSkipCount=1):m_btnBmpClip(btnBmpClip),m_btnBmpStartIndex(bmpStartIndex),m_btnBmpSkipCount(bmpSkipCount){}
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
};

#endif //_VButtonSprite_h_


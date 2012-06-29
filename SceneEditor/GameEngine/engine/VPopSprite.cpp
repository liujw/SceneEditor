/*
 *  VPopSprite.cpp
 *  Ddz
 *
 *  Created by wsh on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VPopSprite.h"

VPopSprite::VPopSprite(const VCanvas&  bitmap)
{ 
    m_Pic.resizeFast(bitmap.getWidth(), bitmap.getHeight()); 
    m_Pic.getCanvas().copy(bitmap); 
    this->setBitmap(m_Pic.getCanvas());
}

void VPopSprite::addContent(VSprite* pContent, long x, long y)
{
    pContent->updateChange();
    pContent->doDraw(m_Pic.getCanvas(), x, y);
    this->addASprite(pContent);
    pContent->kill();
    this->setBitmap(m_Pic.getCanvas());	
}

void VPopSprite::addContent(const VCanvas& bitmap, long x, long y)
{
    m_Pic.getCanvas().blend(x, y, bitmap);
    this->setBitmap(m_Pic.getCanvas());
}

/*
 VAction* VPopSprite::getNewPopAction(const double& dShow_ZoomTime_s, const double& dShowSleepTime_s, const double& dHide_ZoomTime_s, 
 const double& dSrcZoomScale, const double& dDestZoomScale, 
 const double& srcX0, const double& srcY0, const double& destX0,  const double& destY0){
 
 VAction* pPopAction =new VZoomMoveAction(this, dShow_ZoomTime_s, dSrcZoomScale, dDestZoomScale, srcX0, srcY0, destX0, destY0);
 pPopAction->addASubActionAsLastAction(new VSleepAction(dShowSleepTime_s));
 pPopAction->addASubActionAsLastAction(new VZoomMoveAction(this, dHide_ZoomTime_s, dDestZoomScale, dSrcZoomScale, destX0, destY0, srcX0, srcY0));
 return pPopAction;
 }*/
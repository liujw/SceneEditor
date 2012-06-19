/*
 *  VRectLightFlashEffectSprite.h
 *
 *  Created by housisong on 08-6-27.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VRectLightFlashEffectSprite_h_
#define _VRectLightFlashEffectSprite_h_
#include "VLightFlashEffect.h"

const double csStarLightMaxLife_s=1.2;
const double csStarcreateLightCountV_s=100;
const double csMoveLightMaxLife_s=0.9;

//矩形运动的光
class VRectLightFlashEffectSprite:public VLightFlashEffectRandom{
protected:
    VClipCanvasGray8	m_largeStarClipCanvas;
    VLightFlashSprite*	m_moveLight;
    virtual void doUpdate(double stepTime_s); 
    virtual void setMoveLightPos();
    virtual void setNewLightPos(VLightFlashSprite*	newLight);
    virtual void getCurMovePos(double curSumTime,double AllMoveTime,double& out_x,double& out_y);
public:
    VRectLightFlashEffectSprite(const VClipCanvasGray8& smallStarClipCanvas,const VClipCanvasGray8& largeStarClipCanvas,
                                const VClipCanvasGray8& lightClipCanvas,double flashTime_s,long flashWidth,long flashHeight);
    virtual VLightFlashSprite* getANewLight(double life_s);
};



#endif //_VRectLightFlashEffectSprite_h_
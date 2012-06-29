/*
 *  VRectLightFlashEffectSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-27.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VRectLightFlashEffectSprite.h"

VRectLightFlashEffectSprite::VRectLightFlashEffectSprite(const VClipCanvasGray8& smallStarClipCanvas,const VClipCanvasGray8& largeStarClipCanvas,
                                                         const VClipCanvasGray8& lightClipCanvas,double flashTime_s,long flashWidth,long flashHeight)
:VLightFlashEffectRandom(smallStarClipCanvas,csStarLightMaxLife_s,flashTime_s,csStarcreateLightCountV_s,flashWidth,flashHeight),
m_largeStarClipCanvas(largeStarClipCanvas){
    m_moveLight=new VLightFlashSprite(lightClipCanvas,0,csMoveLightMaxLife_s);
    m_moveLight->setIsVisible(true);
    m_moveLight->setZ(1); //最上面
    this->addASprite(m_moveLight);
}

VLightFlashSprite* VRectLightFlashEffectSprite::getANewLight(double life_s){
    VLightFlashSprite* result=VLightFlashEffectRandom::getANewLight(life_s);
    if (m_rand.next()<0.15*VRandLong::getRandMax())
        result->setClipCanvas(m_largeStarClipCanvas);
    else
        result->setClipCanvas(m_clipCanvas);
    setNewLightPos(result);
    return result;
}


void VRectLightFlashEffectSprite::setMoveLightPos(){
    double x;double y;
    getCurMovePos(m_sumTime,m_allFlashTime,x,y);
    m_moveLight->setLeft((long)(x-m_moveLight->getWidth()*0.5+0.5));
    m_moveLight->setTop ((long)(y-m_moveLight->getHeight()*0.5+0.5));
}

void VRectLightFlashEffectSprite::setNewLightPos(VLightFlashSprite*	newLight){
    double x; double y;
    getCurMovePos(m_sumTime-newLight->getLife(),m_allFlashTime,x,y);
    double dx=pow(m_rand.next()*(1.0/VRandLong::getRandMax()),1.5);
    if (m_rand.next()<0.5*VRandLong::getRandMax())
        dx=-dx;
    x+=m_moveLight->getWidth()*dx*0.25;
    double dy=pow(m_rand.next()*(1.0/VRandLong::getRandMax()),1.5);
    if (m_rand.next()<0.5*VRandLong::getRandMax())
        dy=-dy;
    y+=m_moveLight->getHeight()*dy*0.25;
    
    newLight->setLeft((long)(x-newLight->getWidth()*0.5+0.5));
    newLight->setTop ((long)(y-newLight->getHeight()*0.5+0.5));
}


void VRectLightFlashEffectSprite::getCurMovePos(double curSumTime,double AllMoveTime,double& out_x,double& out_y){
    long width=getWidth();
    long height=getHeight();
    long oneLength=(width+height)*2;
    double allMoveLength=oneLength*(1+0.05);
    double curMoveLength=allMoveLength*(curSumTime/AllMoveTime);
    while (curMoveLength>=oneLength) curMoveLength-=oneLength;
    if (curMoveLength<=height){
        out_x=0;
        out_y=height-curMoveLength;
    }else if(curMoveLength<=(height+width)){
        out_x=curMoveLength-height;
        out_y=0;
    }else if(curMoveLength<=(height*2+width)){
        out_x=width;
        out_y=curMoveLength-(height+width);
    }else{		
        out_x=width-(curMoveLength-(height*2+width));
        out_y=height;
    }
}

void VRectLightFlashEffectSprite::doUpdate(double stepTime_s){
    VLightFlashEffectRandom::doUpdate(stepTime_s);
    
    if (m_moveLight==0) return;
    if (m_sumTime<=m_allFlashTime)
        setMoveLightPos();
    else{
        m_moveLight->kill();
        m_moveLight=0;
    }
}

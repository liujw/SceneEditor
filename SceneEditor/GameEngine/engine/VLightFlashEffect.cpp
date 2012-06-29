/*
 *  VLightFlashEffect.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-26.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VLightFlashEffect.h"

VLightFlashSprite::VLightFlashSprite(const VClipCanvasGray8& clipCanvas,ILightFlashListener* listener,double maxLife_s)
:m_clipCanvas(clipCanvas),m_listener(listener),m_picIndex(-1),m_color(cl32White),m_curLife_s(0),m_maxLife_s(maxLife_s),m_isReverse(false),m_isDrawAsAdd(false){
    this->setWidth(clipCanvas.getClipWidth()); 
    this->setHeight(clipCanvas.getClipHeight());
}

void VLightFlashSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (!getIsNeedDraw()) return;
    
    long picIndex=m_picIndex;
    if (m_isReverse)
        picIndex=m_clipCanvas.getClipCount()-1-picIndex;
    
    VCanvasGray8 srcCanvas;
    m_clipCanvas.getSubCanvasByClipIndex(picIndex,srcCanvas);
    if (m_isDrawAsAdd)
        dst.addLight(x0,y0,srcCanvas,m_color);
    else
        dst.blendLight(x0,y0,srcCanvas,m_color);		
}

void VLightFlashSprite::doUpdate(double stepTime_s){
    m_curLife_s+=stepTime_s;
    
    if (!getIsHaveLife()){
        if (m_listener!=0)
            m_listener->doLightFlashNoLife(this);	
        else
            m_curLife_s-=m_maxLife_s;
    }
    long picIndex=(long)(m_curLife_s*m_clipCanvas.getClipCount()/m_maxLife_s);
    setPicIndex(picIndex);
}

void VLightFlashSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    if (getIsNeedDraw()){
        out_rect.x0=x0;
        out_rect.y0=y0;
        out_rect.x1=x0+m_clipCanvas.getClipWidth();
        out_rect.y1=y0+m_clipCanvas.getClipHeight();
    }else{
        out_rect.setEmpty(x0,y0);
    }
}

/////////////////////

VLightFlashSprite* VLightFlashEffect::getANewLight(double life_s){
    long size=(long)m_backList.size();
    VLightFlashSprite* result=0;
    if (size>0){
        result=m_backList[size-1];
        m_backList.pop_back();
    }else{
        result=createNewLight();
        result->setIsEnableSelf(false);
        result->setIsEnableChild(false);
        result->setIsVisible(false);
        this->addASprite(result);
    }
    result->setLife(life_s);
    result->setColor(m_color);
    return result;
}


/////////////////////////

VLightFlashSprite* VLightFlashEffectRandom::getANewLight(double life_s){
    VLightFlashSprite* result=VLightFlashEffect::getANewLight(life_s);
    if (m_isRandColor)
        result->setColor(Color32(m_rand.next()%256,m_rand.next()%256,m_rand.next()%256,255));
    result->setLeft(m_rand.next()%(this->getWidth()+m_clipCanvas.getClipWidth())-m_clipCanvas.getClipWidth());
    result->setTop(m_rand.next()%(this->getHeight()+m_clipCanvas.getClipHeight())-m_clipCanvas.getClipHeight());
    result->setIsReverse(0!=(m_rand.next()%2));
    result->setIsDrawAsAdd(m_isDrawAsAdd);
    
    return result;
}


void VLightFlashEffectRandom::doUpdate(double stepTime_s){
    if (m_sumTime>=m_allFlashTime) return;
    m_sumTime+=stepTime_s;
    if (m_sumTime>m_allFlashTime)
        stepTime_s-=(m_sumTime-m_allFlashTime);
    double createCountValue=m_lostCreateValue+stepTime_s*m_createLightCountV_s;
    
    long createCount=(long)(createCountValue+m_rand.next()*(1.0/VRandLong::getRandMax()));
    m_lostCreateValue=createCountValue-createCount;
    if (createCount<=0) return;
    
    for (long i=0;i<createCount;++i){
        double createTime=stepTime_s*m_rand.next()*(1.0/VRandLong::getRandMax());
        VLightFlashSprite* light=addALight(createTime);
        light->setMaxLife(m_maxLife_s*(0.7+0.8/VRandLong::getRandMax()));
    }
}


//





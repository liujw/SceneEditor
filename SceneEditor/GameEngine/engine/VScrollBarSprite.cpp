/*
 *  VScrollBarSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VScrollBarSprite.h"

void VScrollBarSpriteBase::scrollEvent(){
    if ((!m_isInCallBack)&&(m_listener!=0)){
        m_isInCallBack=true;
        m_listener->onScroll(this,getPosision());
        m_isInCallBack=false;
    }
}

void VScrollBarSpriteBase::setPageScrollSize(double pageScrollSize){
    if (pageScrollSize<0) pageScrollSize=0;
    else if (pageScrollSize>1) pageScrollSize=1;
    if (m_pageScrollSize!=pageScrollSize){
        m_pageScrollSize=pageScrollSize;
        changed();
    }
}

void VScrollBarSpriteBase::setSmallScrollSize(double smallScrollSize){
    if (smallScrollSize<0) smallScrollSize=0;
    else if (smallScrollSize>1) smallScrollSize=1;
    if (m_smallScrollSize!=smallScrollSize){
        m_smallScrollSize=smallScrollSize;
        changed();
    }
}


void VScrollBarSpriteBase::setPosision(double posision){
    if (m_isInCallBack) return;
    
    if (posision<0) posision=0;
    else if (posision>1) posision=1;
    if (m_posision!=posision){
        m_posision=posision;
        scrollEvent();
        changed();
    }
}

void VScrollBarSpriteBase::pageUp(){
    setPosision(getPosision()-getPageScrollSize());
}
void VScrollBarSpriteBase::pageDown(){
    setPosision(getPosision()+getPageScrollSize());
}
void VScrollBarSpriteBase::smallUp(){
    setPosision(getPosision()-getSmallScrollSize());
}
void VScrollBarSpriteBase::smallDown(){
    setPosision(getPosision()+getSmallScrollSize());
}
/*
 *  VTimerSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-11.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VTimerSprite.h"

void VTimerSprite::doUpdate(double stepTime_s){
    if (!m_enableTimer) {
        m_sumTime_s=0;
        return;
    }
    
    m_sumTime_s+=stepTime_s;
    assert(m_interval_s>0);
    while (m_sumTime_s>=m_interval_s){
        doTimerEvent();
        m_sumTime_s-=m_interval_s;
    }
}

void VTimerSprite::doTimerEvent(){
    if(m_eventListener!=0){
        bool isContinueListen=true;
        m_eventListener->event(this,m_callBackData,0,0,0,isContinueListen);
        if (!isContinueListen)
            setOnTimer(0);
    }
}

void VTimerSprite::setEnableTimer(bool enableTimer){
    if (m_enableTimer!=enableTimer){
        m_enableTimer=enableTimer;
        if (!enableTimer)
            m_sumTime_s=0;
    }
}

void VTimerSprite::setInterval(double interval_s){
    if (m_interval_s!=interval_s){
        m_interval_s=interval_s;
        if (m_sumTime_s>m_interval_s)
            m_sumTime_s=m_interval_s;
    }
}
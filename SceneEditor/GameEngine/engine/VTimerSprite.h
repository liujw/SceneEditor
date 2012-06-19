/*
 *  VTimerSprite.h
 *  testEngine
 *
 *  Created by housisong on 08-6-11.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VTimerSprite_h_
#define _VTimerSprite_h_

#include "VSprite.h"
#include "IEventListener.h"

class VTimerSprite:public VSprite{
protected:
    double          m_sumTime_s;
    double			m_interval_s;
    IEventListener* m_eventListener;
    void*           m_callBackData;
    bool			m_enableTimer;
    void init(){
        m_eventListener=0;
        m_enableTimer=false;
        m_interval_s=0;
        m_sumTime_s=0;
    }	
protected:	
    virtual void doUpdate(double stepTime_s);	
    void doTimerEvent();
public:
    explicit VTimerSprite() { init(); }
    virtual ~VTimerSprite(){}
    
    bool getEnableTimer()const{
        return m_enableTimer;
    }
    void setEnableTimer(bool enableTimer);
    double getInterval() const { return m_interval_s; }
    void setInterval(double interval_s);
    
    void setOnTimer(IEventListener* eventListener,void* callBackData=0){
        m_eventListener=eventListener;
        m_callBackData=callBackData;
    }
    IEventListener* getOnTimer()const{
        return m_eventListener;
    }
};


#endif //_VTimerSprite_h_
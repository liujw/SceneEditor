/*
 *  VCountDownSprite.h
 *  Ddz
 *
 *  Created by wsh on 08-7-3.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VCountDownSprite_h_
#define _VCountDownSprite_h_

#include "VSprite.h"
#include "VMotiveFrame.h"
#include "VBmpNumberSprite.h"
#include "VProgressSprite.h"

class IClockWarnEvent
{
public:
    virtual void doWarnning(const double dRemainTime, void* pCallBackData) = 0;
    virtual ~IClockWarnEvent() {}
};

class VCountDownSprite: public VSprite
{
public:
    VCountDownSprite(const VClipCanvas& pCountDownClip, const VClipCanvas& pNumClip);
    void setWarnListener(IClockWarnEvent* pEvent, void* pCallBackData)
    {
        m_pListener = pEvent;
        m_pCallBackData = pCallBackData;
    }
    void doCountDown(const long& nSecond);
    virtual void doUpdate(double stepTime_s);
    long getFrequency() const {return m_Frequency;}
    void setFrequency(const long& nSecond) {m_Frequency = nSecond;}
    long getWarnningTime() const {return m_WarnningTime;}
    void setWarnningTime(const long& nSecond) {m_WarnningTime = nSecond;}
protected:
    void initSprites(const VClipCanvas& pCountDownClip, const VClipCanvas& pNumClip);
    virtual void doWarnning();
protected:
    void* m_pCallBackData;
    IClockWarnEvent* m_pListener;
    bool m_IsWarn;
    long m_IsAutoHide;
    long m_CountDownTime;
    long m_Frequency;
    double m_RemainTime;
    double m_TotalTime;
    VMotiveFrame* m_pCountDownFrame;
    VBmpNumberSprite* m_pCountDownNum;
    long m_WarnningTime;
};

#endif

/*
 *  VCountDownSprite.cpp
 *  Ddz
 *
 *  Created by wsh on 08-7-3.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VCountDownSprite.h"

VCountDownSprite::VCountDownSprite(const VClipCanvas& pCountDownClip, const VClipCanvas& pNumClip)
: m_CountDownTime(0), m_Frequency(1), m_TotalTime(0), m_RemainTime(0), m_WarnningTime(5), m_IsAutoHide(true)
{
    m_IsWarn = false;
    m_pListener = 0;
    m_pCallBackData = 0;
    initSprites(pCountDownClip, pNumClip);
}


void VCountDownSprite::initSprites(const VClipCanvas& pCountDownClip, const VClipCanvas& pNumClip)
{
    m_pCountDownFrame = new VMotiveFrame(pCountDownClip);
    m_pCountDownFrame->setIsEnableSelf(false);
    m_pCountDownFrame->setPlayInterval(0.2);
    m_pCountDownFrame->setZ(0);
    m_pCountDownFrame->setWidth(pCountDownClip.getClipWidth());
    m_pCountDownFrame->setHeight(pCountDownClip.getClipHeight());
    this->setIsEnableSelf(false);
    this->setWidth(pCountDownClip.getClipWidth());
    this->setHeight(pCountDownClip.getClipHeight());
    this->addASprite(m_pCountDownFrame);
    
    m_pCountDownNum = new VBmpNumberSprite(pNumClip);
    m_pCountDownNum->setIsEnableSelf(false);
    m_pCountDownNum->setIsEnableChild(false);
    m_pCountDownNum->setWidth(this->getWidth());
    m_pCountDownNum->setHeight(pNumClip.getClipHeight());
    m_pCountDownNum->setLeft(mapGamePos(-2));
    m_pCountDownNum->setTop((this->getHeight() - pNumClip.getClipHeight()) / 2 + mapGamePos(2));
    m_pCountDownNum->setAlign(align_center);
    m_pCountDownNum->setZ(2);
    this->addASprite(m_pCountDownNum);
}


void VCountDownSprite::doCountDown(const long& nSecond)
{
    m_TotalTime = nSecond;
    m_RemainTime = nSecond;
    m_pCountDownFrame->stop();
    this->setIsVisible(nSecond > 0);
    if (nSecond < 0) return;
    m_pCountDownFrame->setCurrentFrameIndex(0);
    m_pCountDownNum->setNumValue(nSecond);
    m_RemainTime += 1;
    m_IsWarn = false;
}

void VCountDownSprite::doUpdate(double stepTime_s)
{
    if (m_TotalTime > 0 && m_RemainTime > 0)
    {
        m_RemainTime = m_RemainTime - stepTime_s;
        if (m_pCountDownNum->getNumValue() - (long)m_RemainTime >= m_Frequency)
        {
            m_pCountDownNum->setNumValue((long)m_RemainTime);
        }
        doWarnning();
    }
    else
    {
        if (m_IsAutoHide) this->setIsVisible(false);
    }
}

void VCountDownSprite::doWarnning()
{
    if ((m_RemainTime <= (double)(m_WarnningTime + 1)) && (m_RemainTime > 0))
    {
        if (!m_IsWarn)
        {
            if (m_pListener) m_pListener->doWarnning(m_RemainTime, m_pCallBackData);
            m_IsWarn = true;
        }			
        if (!m_pCountDownFrame->getIsPlaying()) m_pCountDownFrame->play(0);
    }else m_pCountDownFrame->stop();
}
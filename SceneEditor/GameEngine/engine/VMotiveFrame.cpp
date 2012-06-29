/*
 *  VMotiveFrame.cpp
 *  Ddz
 *
 *  Created by wsh on 08-6-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VMotiveFrame.h"
#include "VMoveBitmapSprite.h"

void VMotiveFrame::setCurrentFrameIndex(long idx)
{
    if ((idx >= 0) && (idx < getFrameCount()))
    {
        this->setIsVisible(true);
        if ((m_PlayType == ptNone) && (m_CurrentFrameIdx != idx))
        {
            m_CurrentFrameIdx = idx;
            doDrawFrame(m_CurrentFrameIdx);
        }		
    }
}

void VMotiveFrame::setStopIdx(long nIdx)
{
    if ((nIdx >= 0) && (nIdx < getFrameCount()))
    {
        m_StopIdx = nIdx;
    }
}

void VMotiveFrame::setLastFrameAsStopIdx()
{
    setStopIdx(m_ClipCanvas.getClipCount() - 1);
}

void VMotiveFrame::init()
{
    m_PlayBack = false;
    m_bDoneStop = true;
    m_PlayType = ptNone;
    m_bAutoHide = false;
    m_PlayCount = 0;
    m_PassCount = 0;
    m_CurrentFrameIdx = -1;
    m_pBmpFrame = 0;
    m_PlayTime_S = 0.0;
    m_PlayInterval_S = 0.7;
    m_PassTime = 0;
    m_TimeFlag = 0;	
    m_StopIdx = 0;
    m_LastFrameIdx = -1;
    this->setWidth(m_ClipCanvas.getClipWidth());
    this->setHeight(m_ClipCanvas.getClipHeight());
    setCurrentFrameIndex(0);
}

void VMotiveFrame::setPlayInterval(double dTime_s)
{
    if (dTime_s <= 0.0) m_PlayType = ptNone;
    else m_PlayInterval_S = dTime_s;
}

void VMotiveFrame::increaseFrameIndex()
{
    if (m_PlayBack)
    {
        if (m_CurrentFrameIdx > 0) --m_CurrentFrameIdx;
        else m_CurrentFrameIdx = m_CurrentFrameIdx < getFrameCount() - 1;		
    }
    else {
        if (m_CurrentFrameIdx < getFrameCount() - 1) ++m_CurrentFrameIdx;
        else m_CurrentFrameIdx = 0;			
    }
}

void VMotiveFrame::doUpdateChange()
{
    VCanvas tmpCanvas;
    m_ClipCanvas.getSubCanvasByClipIndex(m_LastFrameIdx, tmpCanvas);
    if (m_pBmpFrame == 0)
    {
        m_pBmpFrame = new VBitmapSprite(tmpCanvas);
        this->addASprite(m_pBmpFrame);
    }
    else m_pBmpFrame->setBitmap(tmpCanvas);	
}

void VMotiveFrame::setAlpha(long alpha){
    if (m_pBmpFrame != 0){
        m_pBmpFrame->setAlpha(alpha);
    }
}

void VMotiveFrame::doDrawFrame(long frameIndex)
{
    if (m_LastFrameIdx != frameIndex)
    {
        m_LastFrameIdx = frameIndex;
        this->changed();	
    }
}

void VMotiveFrame::play(double dPlayTime)
{
    stop();
    m_bDoneStop = false;
    this->setIsVisible(true);
    m_PlayType = ptTime;
    if (dPlayTime > 0.0) m_PlayTime_S = dPlayTime;
    else m_PlayTime_S = 0;
}

void VMotiveFrame::playCount(long nCount, long nStartIdx)
{
    if (nCount <= 0) return;	
    if (nStartIdx < 0) nStartIdx = 0;
    if (nStartIdx >= getFrameCount()) nStartIdx = getFrameCount() - 1;
    stop();
    m_bDoneStop = false;
    this->setIsVisible(true);
    m_PlayType = ptCount;
    m_PlayCount = nCount;
    m_CurrentFrameIdx = nStartIdx;
    doDrawFrame(m_CurrentFrameIdx);
}

void VMotiveFrame::playCount(long nCount)
{
    if (nCount <= 0) return;	
    stop();
    m_bDoneStop = false;
    this->setIsVisible(true);
    m_PlayType = ptCount;
    m_PlayCount = nCount;
    m_CurrentFrameIdx = 0;
    doDrawFrame(m_CurrentFrameIdx);
}

void VMotiveFrame::stop()
{
    m_PlayType = ptNone;
    m_PlayCount = 0;
    m_TimeFlag = 0;
    m_PlayTime_S = 0;
    m_PassCount = 0;
    m_PassTime = 0;
}

void VMotiveFrame::doUpdate(double stepTime_s)
{
    if (m_PlayType == ptTime)
    {
        m_TimeFlag += stepTime_s;
        m_PassTime += stepTime_s;
        if ((m_PlayTime_S > 0.0) && (m_PassTime >= m_PlayTime_S))
        {
            m_PlayType = ptNone;
        }
        else 
        {
            if (m_TimeFlag >= m_PlayInterval_S)
            {
                increaseFrameIndex();
                doDrawFrame(m_CurrentFrameIdx);
                m_TimeFlag = 0;
            }			
        }
        
    }
    else if (m_PlayType == ptCount)
    {
        m_TimeFlag += stepTime_s;
        if (m_TimeFlag >= m_PlayInterval_S)
        {
            long nTmp = m_CurrentFrameIdx;
            increaseFrameIndex();
            m_TimeFlag = 0;
            if (m_PlayBack)
            {
                if (nTmp <= m_CurrentFrameIdx)
                {
                    ++m_PassCount;
                }					
            }
            else {
                if (nTmp >= m_CurrentFrameIdx)
                {
                    ++m_PassCount;
                }				
            }
            
            if (m_PassCount >= m_PlayCount)
            {
                m_PlayType = ptNone;	
            }
            else 
            {
                doDrawFrame(m_CurrentFrameIdx);	
            }
            
        }
    }
    else 
    {
        if (!m_bDoneStop)
        {
            m_bDoneStop = true;
            m_CurrentFrameIdx = m_StopIdx;
            doDrawFrame(m_StopIdx);
            if (m_bAutoHide) this->setIsVisible(false);
        }
    }
    
}

//////////////////////////////////////////////////////////////////////////

VMotiveFrameEx::VMotiveFrameEx(const VClipCanvas& clipCanvas)
: m_zoomX(1),m_zoomY(1),m_rotaryAngle(0),VMotiveFrame(clipCanvas), m_MoveX0(0.0), m_MoveY0(0.0),m_isAutoCacheDraw(false), m_alpha(255){
}

void VMotiveFrameEx::setZoomX(double zoomX){
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp!=0){
        bmp->setZoomX(zoomX);
        m_zoomX=bmp->getZoomX();
    }else
        m_zoomX=zoomX;
}
void VMotiveFrameEx::setZoomY(double zoomY){
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp!=0){
        bmp->setZoomY(zoomY);
        m_zoomY=bmp->getZoomY();
    }else
        m_zoomY=zoomY;
}

void VMotiveFrameEx::setAlpha(long alpha){
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp!=0){
        bmp->setAlpha(alpha);
    }
    m_alpha=alpha;
}

void VMotiveFrameEx::setRotaryAngle(double angle){
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp!=0){
        bmp->setRotaryAngle(angle);
        m_rotaryAngle=bmp->getRotaryAngle();
    }else
        m_rotaryAngle=angle;
}

void VMotiveFrameEx::setMoveX0(double x0)
{
    m_MoveX0 = x0;
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp) bmp->setMoveX0(m_MoveX0);
}

void VMotiveFrameEx::setMoveY0(double y0)
{
    m_MoveY0 = y0;
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp) bmp->setMoveY0(m_MoveY0);
}

void VMotiveFrameEx::doUpdateChange()
{
    VCanvas tmpCanvas;
    m_ClipCanvas.getSubCanvasByClipIndex(m_LastFrameIdx, tmpCanvas);
    if (!m_pBmpFrame)
    {
        m_pBmpFrame = new VMoveBitmapSprite(tmpCanvas); 
        this->addASprite(m_pBmpFrame);
        ((VMoveBitmapSprite*)m_pBmpFrame)->setMoveX0(tmpCanvas.getWidth()/2);
        ((VMoveBitmapSprite*)m_pBmpFrame)->setMoveY0(tmpCanvas.getHeight()/2);	
    }
    else {
        m_pBmpFrame->setBitmap(tmpCanvas);
        ((VMoveBitmapSprite*)m_pBmpFrame)->setMoveX0(m_MoveX0);
        ((VMoveBitmapSprite*)m_pBmpFrame)->setMoveY0(m_MoveY0);	
    }
    
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    bmp->setZoomX(m_zoomX);
    bmp->setZoomY(m_zoomY);
    bmp->setRotaryAngle(m_rotaryAngle);
    //bmp->setMoveX0(tmpCanvas.getWidth()/2);
    //bmp->setMoveY0(tmpCanvas.getHeight()/2);	
    bmp->setIsAutoCacheDraw(m_isAutoCacheDraw);
    bmp->setAlpha(m_alpha);
}


void VMotiveFrameEx::setIsAutoCacheDraw(bool isAutoCacheDraw){
    m_isAutoCacheDraw=isAutoCacheDraw;
    VMoveBitmapSprite* bmp=(VMoveBitmapSprite*)m_pBmpFrame;
    if (bmp) bmp->setMoveY0(m_MoveY0);
}

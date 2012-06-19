/*
 *  VMotiveFrame.h
 *  Ddz
 *
 *  Created by wsh on 08-6-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VMotiveFrame_h_
#define _VMotiveFrame_h_

#include "VBitmapSprite.h"

class VMotiveFrame: public VSprite
{
public:
    explicit VMotiveFrame(const VClipCanvas& clipCanvas): m_ClipCanvas(clipCanvas)
    {
        init();
    }
    void setIsPlayBack(bool playBack){
        m_PlayBack = playBack;
    }
    long getIsPlayBack() const{
        return m_PlayBack;
    }
    void setLastFrameAsStopIdx();
    void setStopIdx(long nIdx);
    void setIsAutoHide(bool bAutoHide) {m_bAutoHide = bAutoHide;}
    double getPlayInterval() const {return m_PlayInterval_S;}
    long getCurrentFrameIndex() const {return m_CurrentFrameIdx;}
    long getFrameCount() const {return m_ClipCanvas.getClipCount();}
    double getAnimationLength() const {return m_ClipCanvas.getClipCount() * m_PlayInterval_S;}
    double getIsPlaying() const {return (m_PlayType != ptNone);}
    void setPlayInterval(double dTime_s);
    void setCurrentFrameIndex(long idx);
    void play(double dPlayTime);
    void playCount(long nCount, long nStartIdx);
    void playCount(long nCount);
    void stop();
    virtual void doUpdate(double stepTime_s);
    virtual void setAlpha(long alpha);
protected:
    virtual void doUpdateChange();
    enum PlayType {ptNone, ptTime, ptCount};
    void init();
    void doDrawFrame(long frameIndex);
    void increaseFrameIndex();
protected:
    bool m_PlayBack;
    bool m_bDoneStop;
    long m_LastFrameIdx;
    bool m_bAutoHide;
    PlayType m_PlayType;
    long m_PlayCount;
    long m_PassCount;
    long m_StopIdx;
    VBitmapSprite* m_pBmpFrame;
    double m_TimeFlag;
    double m_PassTime;
    double m_PlayInterval_S;
    double m_PlayTime_S;
    VClipCanvas m_ClipCanvas;
    long m_CurrentFrameIdx;
};
//增加一个缩放的功能IEnableZoom
class VMotiveFrameEx:public VMotiveFrame, public IEnableZoom{
protected:
    double m_zoomX;
    double m_zoomY;
    double m_rotaryAngle;
    double m_MoveX0;
    double m_MoveY0;
    bool   m_isAutoCacheDraw;
    long   m_alpha;
    virtual void doUpdateChange();
public:
    //IEnableZoom,设置的是子精灵
    virtual void setMoveX0(double x0);
    virtual void setMoveY0(double x0);
    virtual void setZoomX(double zoomX);
    virtual void setZoomY(double zoomY);
public:
    //IEnableAlpha
    virtual void setAlpha(long alpha);
public:
    explicit VMotiveFrameEx(const VClipCanvas& clipCanvas);
    double getZoomX()const{
        return m_zoomX;
    }
    double getZoomY()const{
        return m_zoomY;
    }
    void setZoom(double zoom){
        setZoomX(zoom);
        setZoomY(zoom);
    }
    double getRotaryAngle()const{
        return m_rotaryAngle;
    }
    virtual void setRotaryAngle(double angle);
    void setIsAutoCacheDraw(bool isAutoCacheDraw);
};

#endif
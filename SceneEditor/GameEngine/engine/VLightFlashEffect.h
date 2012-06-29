/*
 *  VLightFlashEffect.h
 *
 *  Created by housisong on 08-6-26.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VLightFlashEffect_h_
#define _VLightFlashEffect_h_
#include "VSprite.h"
#include "../base/Rand.h"

class VLightFlashSprite;

struct ILightFlashListener{
    virtual void doLightFlashNoLife(VLightFlashSprite* sender)=0;
    virtual ~ILightFlashListener(){}
};

class VLightFlashSprite:public VSprite{
protected:
    VClipCanvasGray8		m_clipCanvas;
    ILightFlashListener*	m_listener;
    long				m_picIndex;
    Color32				m_color;
    double				m_curLife_s;
    double				m_maxLife_s;
    bool                m_isReverse;
    bool				m_isDrawAsAdd;
    
    inline bool getIsNeedDraw()const{
        return (m_color.a>0)&&(m_picIndex>=0)&&(m_picIndex<m_clipCanvas.getClipCount());
    }
    void setPicIndex(long picIndex){
        if (picIndex<-1) picIndex=-1;
        else if (picIndex>=m_clipCanvas.getClipCount()) picIndex=-1;
        
        if (m_picIndex!=picIndex){
            m_picIndex=picIndex;
            changed();
        }
    }
public:
    VLightFlashSprite(const VClipCanvasGray8& clipCanvas,ILightFlashListener* listener,double maxLife_s);
    void setMaxLife(double maxLife_s){
        if (m_maxLife_s!=maxLife_s){
            m_maxLife_s=maxLife_s;
        }
    }
    void setLife(double curLife_s){
        if (m_curLife_s!=curLife_s){
            m_curLife_s=curLife_s;
        }
    }
    void setClipCanvas(const VClipCanvasGray8& clipCanvas){
        m_clipCanvas.clip(clipCanvas);	
        this->setWidth(clipCanvas.getClipWidth()); 
        this->setHeight(clipCanvas.getClipHeight());
        changed();
    }
    double getLife()const{
        return m_curLife_s;
    }
    bool getIsHaveLife()const{
        return m_curLife_s<m_maxLife_s;
    }
    void setIsReverse(bool isReverse){
        if (m_isReverse!=isReverse){
            m_isReverse=isReverse; 
            changed();
        }
    }
    void setColor(const Color32& color){
        if (m_color!=color){
            m_color=color;
            changed();
        }
    }
    bool getIsDrawAsAdd()const{
        return m_isDrawAsAdd;
    }
    void setIsDrawAsAdd(bool isDrawAsAdd){
        if (m_isDrawAsAdd!=isDrawAsAdd){
            m_isDrawAsAdd=isDrawAsAdd;
            changed();
        }
    }
    virtual void doUpdate(double stepTime_s);
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
};

class VLightFlashEffect:public VSprite,ILightFlashListener{
protected:
    VClipCanvasGray8	m_clipCanvas;
    Color32				m_color;
    double				m_maxLife_s;
    Vector<VLightFlashSprite*>  m_backList;
    virtual void doLightFlashNoLife(VLightFlashSprite* sender){
        sender->setIsVisible(false);
        m_backList.push_back(sender);
    }
    virtual VLightFlashSprite* createNewLight(){
        return new VLightFlashSprite(m_clipCanvas,this,m_maxLife_s);
    }
    virtual VLightFlashSprite* getANewLight(double life_s);
public:
    VLightFlashEffect(const VClipCanvasGray8& clipCanvas,double lightMaxLife_s,const Color32& lightColor=cl32White)
    :m_clipCanvas(clipCanvas),m_maxLife_s(lightMaxLife_s),m_color(lightColor){
        this->setIsEnableSelf(false);
        this->setIsEnableChild(false);
    }
public:
    VLightFlashSprite* addALight(double life_s){
        VLightFlashSprite* result=getANewLight(life_s);
        result->setIsVisible(true);
        return result;
    }
};

//在一块区域内随机闪烁
class VLightFlashEffectRandom:public VLightFlashEffect{
protected:
    VRandLong   m_rand;	
    double		m_createLightCountV_s;
    double      m_allFlashTime;
    double      m_sumTime;
    double      m_lostCreateValue;
    bool		m_isRandColor;
    bool		m_isDrawAsAdd;
    
    virtual void doUpdate(double stepTime_s);
public:
    VLightFlashEffectRandom(const VClipCanvasGray8& clipCanvas,double lightMaxLife_s,double flashTime_s,double createLightCountV_s,long flashWidth,long flashHeight)
    :VLightFlashEffect(clipCanvas,lightMaxLife_s),m_createLightCountV_s(createLightCountV_s),m_sumTime(0),m_allFlashTime(flashTime_s),m_lostCreateValue(0),m_isRandColor(true),m_isDrawAsAdd(true){
        this->setWidth(flashWidth);
        this->setHeight(flashHeight);
        m_rand.setSeedByNowTime();
    }
public:
    virtual VLightFlashSprite* getANewLight(double life_s);
};



#endif //_VLightFlashEffect_h_





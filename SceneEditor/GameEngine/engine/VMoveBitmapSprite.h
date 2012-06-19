//
//  VMoveBitmapSprite.h
//
//  Created by housisong on 08-4-3.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VMoveBitmapSprite_h_
#define _VMoveBitmapSprite_h_

#include "VBitmapSprite.h"
#include  "math.h"
#include "VAction.h"

enum TFilterState{filter_defult=0,filter_fast=1,filter_quality=2};

class VMoveBitmapSprite:public VBitmapSprite, public IEnableZoom, public IEnableRotation{
protected:
    double  m_rotaryAngle;
    bool    m_isRotaryAngleChaged;
    double  m_rotaryAngle_sin;
    double  m_rotaryAngle_cos;
    double  m_zoomX;
    double  m_zoomY;
    double	m_moveX0_sub_left;
    double	m_moveY0_sub_top;
    bool		m_isAutoCacheDraw;
    VSurface*	m_autoCacheDrawSurface;
    TFilterState    m_filter;
    bool    m_isCanFastDraw;
    void init(){
        m_autoCacheDrawSurface=0;
        m_rotaryAngle=0;
        SinCos(m_rotaryAngle,&m_rotaryAngle_sin,&m_rotaryAngle_cos);
        m_isRotaryAngleChaged=false;
        
        m_zoomX=1;
        m_zoomY=1;
        m_moveX0_sub_left=0;
        m_moveY0_sub_top=0;
        m_filter=filter_defult;
        m_isAutoCacheDraw=true;
        m_isCanFastDraw=true;
    }
protected:
    virtual void doUpdateChange();	
    void drawTo(const VCanvas& dst,double moveX0,double moveY0,bool isCopy,TFilterState filter);
protected:
    virtual void createCacheSurface(){
        if (m_autoCacheDrawSurface==0)
            m_autoCacheDrawSurface=new VSurface();
        getBitmap(*m_autoCacheDrawSurface,m_filter);
    }
    virtual void clearCacheSurface(){
        if (m_autoCacheDrawSurface!=0){
            delete m_autoCacheDrawSurface;
            m_autoCacheDrawSurface=0;
        }
    }
public:
    explicit VMoveBitmapSprite(){ init(); }
    explicit VMoveBitmapSprite(const VCanvas&  bitmap):VBitmapSprite(bitmap){
        init();
    }
    virtual ~VMoveBitmapSprite(){ clearCacheSurface(); }
    bool getIsAutoCacheDraw()const{
        return m_isAutoCacheDraw;
    }
    void setIsAutoCacheDraw(bool isAutoCacheDraw){
        if (isAutoCacheDraw!=m_isAutoCacheDraw){
            m_isAutoCacheDraw=isAutoCacheDraw;
            if (isAutoCacheDraw)
                changed();
        }
    }
    
    double getRotaryAngle()const{
        return m_rotaryAngle;
    }
    virtual void setRotaryAngle(double angle){
        angle=fmod(angle,(2*MyBase::PI));
        if (angle<0) angle+=(2*MyBase::PI);
        if (m_rotaryAngle!=angle){
            m_rotaryAngle=angle;
            m_isRotaryAngleChaged=true;
            changed();
        }
    }
    
    double getZoomX()const{
        return m_zoomX;
    }
    void setZoomX(double zoomX){
        if (m_zoomX!=zoomX){
            m_zoomX=zoomX;
            changed();
        }
    }
    double getZoomY()const{
        return m_zoomY;
    }
    void setZoomY(double zoomY){
        if (m_zoomY!=zoomY){
            m_zoomY=zoomY;
            changed();
        }
    }
    void setZoom(double zoom){
        setZoomX(zoom);
        setZoomY(zoom);
    }
    
    double getMoveX0()const{
        return m_moveX0_sub_left+getLeft();
    }
    void setMoveX0(double moveX0){
        if (getMoveX0()!=moveX0){
            VBitmapSprite::setLeft(0);
            m_moveX0_sub_left=moveX0;
            changed();
        }
    }
    
    double getMoveY0()const {
        return m_moveY0_sub_top+getTop();
    }
    void setMoveY0(double moveY0){
        if (getMoveY0()!=moveY0){
            VBitmapSprite::setTop(0);
            m_moveY0_sub_top=moveY0;
            changed();
        }
    }
    virtual void setLeft(long left){
        setMoveX0(left);
    }
    virtual void setTop(long top){
        setMoveY0(top);
    }
    
    TFilterState getFilter(){
        return m_filter;
    }
    void setFilter(TFilterState filter){
        if (filter!=m_filter){
            m_filter=filter;
            changed();
        }
    }
    //复制一幅高质量的贴图
    void getBitmap(VSurface& out_surface,TFilterState state);
    
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    
    virtual bool getIsHit(long clientX0, long clientY0);
    
};

#endif //_VBitmapSprite_h_
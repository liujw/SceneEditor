//VMoveBitmapSprite.cpp

#include "VMoveBitmapSprite.h"
#include "../draw2d/hRotary.h"

void VMoveBitmapSprite::doUpdateChange(){
    VBitmapSprite::doUpdateChange();
    
    if (m_canvas.getIsEmpty())  {
        return ;
    }	
    if (m_isRotaryAngleChaged){
        SinCos(m_rotaryAngle,&m_rotaryAngle_sin,&m_rotaryAngle_cos);
        m_isRotaryAngleChaged=false;
    }
    
    double sx0=m_canvas.getWidth()*m_zoomX*0.5;
    double sy0=m_canvas.getHeight()*m_zoomY*0.5;
    double dx0,dy0;
    rotaryMap(m_rotaryAngle_sin,m_rotaryAngle_cos,sx0,sy0,dx0,dy0);
    double sx1=-sx0;
    double dx1,dy1;
    rotaryMap(m_rotaryAngle_sin,m_rotaryAngle_cos,sx1,sy0,dx1,dy1);
    double newWidth_f=STD::max(MyBase::abs(dx0),MyBase::abs(dx1))*2;
    double newHeight_f=STD::max(MyBase::abs(dy0),MyBase::abs(dy1))*2;
    long newWidth=((long)(newWidth_f+2));
    long newHeight=((long)(newHeight_f+2));
    
    //更新位置
    double moveX0=m_moveX0_sub_left+getLeft();
    double moveY0=m_moveY0_sub_top+getTop();
    long newLeft=(long)(moveX0-newWidth*0.5);
    m_moveX0_sub_left=moveX0-newLeft;
    long newTop=(long)(moveY0-newHeight*0.5);
    m_moveY0_sub_top=moveY0-newTop;
    VBitmapSprite::setWidth(newWidth);
    VBitmapSprite::setHeight(newHeight);
    VBitmapSprite::setLeft(newLeft);
    VBitmapSprite::setTop(newTop);	
    
    //缓存
    m_isCanFastDraw=m_isAutoCacheDraw 
    &&  ( isMaybeEqual(m_zoomX,1)||isMaybeEqual(m_zoomX,-1) )
    &&  ( isMaybeEqual(m_zoomY,1)||isMaybeEqual(m_zoomY,-1) )
    &&  ( isMaybeEqual(m_rotaryAngle,0)||isMaybeEqual(m_rotaryAngle,MyBase::PI*0.5)||isMaybeEqual(m_rotaryAngle,MyBase::PI)
         ||isMaybeEqual(m_rotaryAngle,MyBase::PI*1.5)||isMaybeEqual(m_rotaryAngle,MyBase::PI*2));			
    
    if ((!m_isCanFastDraw)&&(m_isAutoCacheDraw))
        createCacheSurface();
    else
        clearCacheSurface();
}

void VMoveBitmapSprite::drawTo(const VCanvas& dst,double moveX0,double moveY0,bool isCopy,TFilterState filter){
    if (m_canvas.getIsEmpty()) return;
    
    if (m_isCanFastDraw){
        if (isCopy)
            dst.rotaryCopyFast(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
        else
            dst.rotaryBlendFast(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
        return;
    }
    
    if (isCopy){
        if (filter==filter_fast)
            dst.rotaryCopyFast(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
        else if (filter==filter_quality)
            dst.rotaryCopyQuality(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
        else
            dst.rotaryCopy(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
    }
    else{
        if (filter==filter_fast)
            dst.rotaryBlendFast(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
        else if (filter==filter_quality)
            dst.rotaryBlendQuality(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
        else
            dst.rotaryBlend(m_canvas, m_rotaryAngle_sin,m_rotaryAngle_cos, m_zoomX, m_zoomY, moveX0, moveY0,m_coloring,m_alpha);
    }
    
    
}
void VMoveBitmapSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if ( (!m_isCanFastDraw) && (m_isAutoCacheDraw) )
        dst.blend(x0, y0, m_autoCacheDrawSurface->getCanvas());
    else
        drawTo(dst,m_moveX0_sub_left+x0, m_moveY0_sub_top+y0,m_isCopy,m_filter);
}

void VMoveBitmapSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    if (m_canvas.getIsEmpty()){
        out_rect.setEmpty();
        return;
    }
    updateChange();
    
    out_rect.x0=x0 - 2;
    out_rect.y0=y0 - 2;
    out_rect.x1=x0+getWidth() + 4;
    out_rect.y1=y0+getHeight() + 4;
}

void VMoveBitmapSprite::getBitmap(VSurface& out_surface,TFilterState filter){	
    updateChange();
    out_surface.clear();
    if (m_canvas.getIsEmpty()) return;
    
    out_surface.resize(getWidth(),getHeight());
    drawTo(out_surface.getCanvas(),m_moveX0_sub_left, m_moveY0_sub_top,true,filter);
}

bool VMoveBitmapSprite::getIsHit(long clientX0, long clientY0) {
    if (m_canvas.getIsEmpty()) return false;
    if (clientX0<0) return false;
    if (clientY0<0) return false;
    updateChange();
    
    long testAlpha=(ALPHA_TEST_MINVALUE*m_alpha)>>8;
    if ( (!m_isCanFastDraw) && (m_isAutoCacheDraw) )
        return m_autoCacheDrawSurface->getCanvas().getIsHit(clientX0,clientY0,testAlpha);
    else{
        if (clientX0>=getWidth()) return false;
        if (clientY0>=getHeight()) return false;
        
        double RZoom=1/(m_zoomX*m_zoomY);
        double dx0=(clientX0-m_moveX0_sub_left)*RZoom*m_zoomY;
        double dy0=(clientY0-m_moveY0_sub_top)*RZoom*m_zoomX;
        double sx0,sy0;
        rotaryRMap(m_rotaryAngle_sin,m_rotaryAngle_cos,dx0,dy0,sx0,sy0);
        long srcWidth=m_canvas.getWidth();
        long srcHeight=m_canvas.getHeight();
        sx0+=srcWidth*0.5;
        sy0+=srcHeight*0.5;
        return m_canvas.getIsHit((long)sx0,(long)sy0,testAlpha);
    }
}


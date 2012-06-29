//VBitmapSprite.cpp

#include "VBitmapSprite.h"


void VBitmapSprite::drawTo(const VCanvas& dst,long x0,long y0,const VCanvas& src){
    if (getIsDrawAsGray()){
        if (getIsDrawAsCopy()) 
            dst.copyAsGray(x0,y0,src,m_alpha);
        else 
            dst.blendAsGray(x0,y0,src,m_alpha);		
    }else{
        if (getIsDrawAsCopy()) 
            dst.copyColoring(x0,y0,src,m_coloring,m_alpha);
        else 
            dst.blendColoring(x0,y0,src,m_coloring,m_alpha);
    }
}

void VBitmapSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (!m_canvas.getIsEmpty()) 
        drawTo(dst,x0,y0,m_canvas);
}


bool VBitmapSprite::getIsHit(long clientX0, long clientY0){
    if (m_canvas.getIsEmpty()) return false;
    
    if (m_alpha<=0) return false;
    return m_canvas.getIsHit(clientX0,clientY0,(getLeastAlpha()<<8)/m_alpha);
}
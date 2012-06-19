//VBitmapBaseSprite.cpp
#include "VBitmapBaseSprite.h"


void VBitmapBaseSprite::setAutoFree_owner_surface(VSurface* owner_surface){
    if (owner_surface!=m_owner_surface){
        m_canvas.setEmptyRef();
        if (m_owner_surface!=0){
            delete m_owner_surface;
            m_owner_surface=0;
        }
        if (owner_surface!=0){
            m_canvas.setRef(owner_surface->getCanvas());
            m_owner_surface=owner_surface;
        }
        setFitSize();
        changed();
    }
}

void VBitmapBaseSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (m_canvas.getIsEmpty()) return;
    
    if (m_isCopy)
        dst.copy(x0,y0,m_canvas);
    else
        dst.blend(x0,y0,m_canvas);
}

void VBitmapBaseSprite::loadFrom(const char* fileName)
{
    VSurface* surface=new VSurface();
    surface->loadFrom(fileName);
    setAutoFree_owner_surface(surface);
}

bool VBitmapBaseSprite::getIsHit(long clientX0, long clientY0) {
    if (m_canvas.getIsEmpty())  return false;
    if (m_isCopy)
        return m_canvas.getIsInRect(clientX0, clientY0);
    else{
        return m_canvas.getIsHit(clientX0, clientY0,m_LeastAlpha);
    }
}

void VBitmapBaseSprite::setBitmap(const VCanvas& bitmap){
    if (!m_canvas.getIsEqual(bitmap)){
        setAutoFree_owner_surface(0);
        m_canvas.setRef(bitmap);
        setFitSize();
        changed(); 
    }
}

void VBitmapBaseSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    if (m_canvas.getIsEmpty()) {
        out_rect.setEmpty(x0,y0);
    }else{
        out_rect.x0=x0;
        out_rect.y0=y0;
        out_rect.x1=x0+m_canvas.getWidth();
        out_rect.y1=y0+m_canvas.getHeight();
    }
}

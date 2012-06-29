//VLightBitmapSprite.cpp

#include "VLightBitmapSprite.h"

void VLightBitmapSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (m_canvas.getIsEmpty()) return;
    
    if (getIsDrawAsCopy())
        dst.copyColoring(x0,y0,m_canvas,m_coloring);
    else
        dst.addColoring(x0,y0,m_canvas,m_coloring);
}
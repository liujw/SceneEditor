//VBmpButtonSprite.cpp

#include "VBmpButtonSprite.h"
#include "../draw2d/hBlendAPixel.h"


void VBmpButtonSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (m_canvas.getIsEmpty()) return;
    
    Color32 coloring;
    switch (m_state) {   
        case btnEnter:  
            coloring=Color32(230, 230, 230, 255);
            break;   
        case btnDown:   
            coloring=Color32(180, 180, 180, 255);//变暗
            break;   
        default:   
            coloring=cl32ColoringMullEmpty;
            break;   
    } 
    //coloring.argb=mullColor(coloring,m_coloring);
    
    if (m_isCopy){
        if (m_isEnableSelf)
            dst.copyColoring(x0,y0,m_canvas,coloring,m_alpha);
        else
            dst.copyAsGray(x0, y0, m_canvas,m_alpha);
    } else{
        if (m_isEnableSelf)
            dst.blendColoring(x0,y0,m_canvas,coloring,m_alpha);
        else
            dst.blendAsGray(x0, y0, m_canvas,m_alpha);
    }
}
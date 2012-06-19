//
//  VLightBitmapSprite.h
//
//  Created by housisong on 08-4-3.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VLightBitmapSprite_h_
#define _VLightBitmapSprite_h_

#include "VBitmapBaseSprite.h"

class VLightBitmapSprite:public VBitmapBaseSprite{
protected:
    Color32     m_coloring;
    void init(){
        m_coloring=cl32ColoringMullEmpty;
    }
    
public:
    explicit VLightBitmapSprite(){ init(); }
    explicit VLightBitmapSprite(const VCanvas&  bitmap):VBitmapBaseSprite(bitmap){
        init();
    }
    
    //着色 
    const Color32& getColoring()const{
        return m_coloring;
    }
    void setColoring(const Color32& coloring){
        if (m_coloring!=coloring){
            m_coloring=coloring;
            changed();
        }
    }
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
};

#endif //_VLightBitmapSprite_h_
/*
 *  VProgressSprite.h
 *
 *  Created by housisong on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VProgressSprite_h_
#define _VProgressSprite_h_

#include "VBitmapSprite.h"

enum TDirectionType{ dirt_right=0,dirt_left,dirt_top,dirt_bottom };

class VProgressSprite:public VBitmapSprite, public IProgressSprite{
protected:
    TDirectionType m_directionType;
    double         m_progress; 
    double         m_maxProgress;
    void init(){
        m_directionType=dirt_right;
        m_maxProgress=1;
        m_progress=0;
    }
public:
    explicit VProgressSprite():VBitmapSprite(){ init(); }
    explicit VProgressSprite(const VCanvas&  bitmap):VBitmapSprite(bitmap){	init();	}
    
    double getProgress()const {
        return m_progress;
    }
    virtual void setProgress(double progress){
        if (progress>m_maxProgress) progress=m_maxProgress;
        else if (progress<0) progress=0;
        
        if (m_progress!=progress){
            m_progress=progress;
            changed();
        }
    }
    
    double getMaxProgress()const {
        return m_maxProgress;
    }
    virtual void setMaxProgress(double maxProgress){
        if (maxProgress<=0) return;
        
        if (m_maxProgress!=maxProgress){
            m_maxProgress=maxProgress;
            changed();
        }
    }
    
    //方向
    TDirectionType getDirectionType()const{
        return m_directionType;
    }
    void setDirectionType(const TDirectionType directionType){
        if (m_directionType!=directionType){
            m_directionType=directionType;
            changed();
        }
    }
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
};


#endif //_VProgressSprite_h_
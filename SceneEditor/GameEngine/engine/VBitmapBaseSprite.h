//
//  VBitmapBaseSprite.h
//
//  Created by housisong on 08-4-3.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VBitmapBaseSprite_h_
#define _VBitmapBaseSprite_h_

#include "VSprite.h"
#include "VSurface.h"

class VBitmapBaseSprite:public VSprite{
private:
    VSurface* m_owner_surface; 
    long     m_LeastAlpha;
protected: 
    VCanvas  m_canvas;
    bool m_isGray;
    bool  m_isCopy; 
    
    void init(){
        m_isGray = false;
        m_owner_surface=0;
        m_isCopy=false; //default is draw as blend
        m_LeastAlpha=ALPHA_TEST_MINVALUE;
    }
    
public:
    explicit VBitmapBaseSprite(){
        init();
    }
    explicit VBitmapBaseSprite(const VCanvas&  bitmap){
        init();
        setBitmap(bitmap);
    }
    virtual ~VBitmapBaseSprite(){
        setAutoFree_owner_surface(0);
    }
    const VCanvas& getBitmap()const {
        return m_canvas;
    }
    void setBitmap(const VCanvas& bitmap);
    void setAutoFree_owner_surface(VSurface* owner_surface);
    bool getIsDrawAsCopy()const{
        return m_isCopy;
    }
    void setIsDrawAsCopy(bool isCopy){
        if (m_isCopy!=isCopy){
            m_isCopy=isCopy;
            changed();
        }
    }
    bool getIsDrawAsGray() const{
        return m_isGray;
    }
    void setIsDrawAsGray(bool isGray){
        if (m_isGray != isGray)
        {
            m_isGray = isGray;
            changed();
        }
    }
    long getLeastAlpha()const{
        return m_LeastAlpha;
    }
    void setLeastAlpha(long leastAlpha){
        m_LeastAlpha=leastAlpha;
    }
    
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    void loadFrom(const char* fileName);
    virtual bool getIsHit(long clientX0, long clientY0);
    void setFitSize(){
        VSprite::setWidth(m_canvas.getWidth());
        VSprite::setHeight(m_canvas.getHeight());
    }
};

#endif //_VBitmapBaseSprite_h_
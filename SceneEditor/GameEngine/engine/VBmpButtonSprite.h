//
//  VBmpButtonSprite.h
//
//  Created by housisong on 08-4-8.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VBmpButtonSprite_h_
#define _VBmpButtonSprite_h_

#include "VBitmapSprite.h"

enum TbtnStateType { btnDefault=0,btnEnter=1,btnDown=2 };

class VBmpButtonSprite:public VBitmapSprite{
protected:
    TbtnStateType m_state;
    
    void init(){
        m_state=btnDefault;
        this->setIsEnableSelf(true);
    }
    void setState(TbtnStateType state){
        if (state!=m_state){
            m_state=state;
            changed();
        }
    }
public:
    explicit VBmpButtonSprite(){ init(); }
    explicit VBmpButtonSprite(const VCanvas&  bitmap):VBitmapSprite(bitmap){
        init();
    }
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    
    virtual void disposeEventMouseEnter() {
        VBitmapSprite::disposeEventMouseEnter();
        setState(btnEnter);
    }
    virtual void disposeEventMouseLeave() {
        VBitmapSprite::disposeEventMouseLeave();
        setState(btnDefault);
    }
    virtual void disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo) {
        VBitmapSprite::disposeEventMouseDown(mouseEventInfo);
        setState(btnDown);		
    }
    virtual void disposeEventMouseUp(const TSpriteMouseEventInfo& mouseEventInfo) {
        VBitmapSprite::disposeEventMouseUp(mouseEventInfo);
        setState(btnDefault);
    }
};

#endif //_VBmpButtonSprite_h_
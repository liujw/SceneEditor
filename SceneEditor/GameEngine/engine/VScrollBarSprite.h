/*
 *  VScrollBarSprite.h
 *
 *  Created by housisong on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VScrollBarSprite_h_
#define _VScrollBarSprite_h_

#include "VSprite.h"

class VScrollBarSpriteBase;

class IScrollBarListener{
public:
    virtual void onScroll(VScrollBarSpriteBase* sender,double posision)=0;   //posision is [0..1] 
    virtual ~IScrollBarListener(){}
};

class VScrollBarSpriteBase:public VSprite{
protected:
    double m_posision;
    double m_pageScrollSize;
    double m_smallScrollSize;
    IScrollBarListener* m_listener;
    void init(){
        m_posision=0;
        m_pageScrollSize=0.5;
        m_smallScrollSize=0.1;
        m_listener=0;
        m_isInCallBack=false;
    }
    bool m_isInCallBack;
    void scrollEvent();
public:
    VScrollBarSpriteBase(){ init(); } 
    void setScrollListener(IScrollBarListener* listener) { m_listener=listener; }
    virtual void setScroll(double posision,double pageSize)=0;   //posision is [0..1] 
    double  getPageScrollSize()const{
        return m_pageScrollSize;
    }
    void setPageScrollSize(double pageScrollSize);
    double  getSmallScrollSize()const{
        return m_smallScrollSize;
    }
    void setSmallScrollSize(double smallScrollSize);
    
    //当前滚动条位置
    double getPosision()const{
        return m_posision;
    }
    void setPosision(double posision);
    
    void pageUp();
    void pageDown();
    void smallUp();
    void smallDown();
};

//水平 \ 垂直
enum TScrollBarKind {sbHorizontal, sbVertical};

//TODO: 具体实现一个滚动条 一个背景条按钮 + 两个上\下按钮 + 一个拖动条按钮
class VScrollBarSprite:public VScrollBarSpriteBase{
protected:
public:
};

#endif //_VScrollBarSprite_h_
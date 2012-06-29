/*
 *  VBmpNumberSprite.h
 *
 *  Created by housisong on 08-6-18.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VBmpNumberSprite_h_
#define _VBmpNumberSprite_h_

#include "VSprite.h"
#include "../base/Rect.h"

class VBmpNumberSprite:public VSprite, public IEnableValueChange{
protected:
    Int64		m_value;
    long		m_distanceX;
    long		m_distanceY;
    bool		m_isShowPositiveSign;
    TAlignType  m_align;
    UInt8       m_alpha; 
    
    TRect       m_hitTestRect;
    VClipCanvas m_numClip;
protected:
    struct TNumPos{
        long        clipIndex;
        long		left;
        long		top;
    };
    typedef Vector<TNumPos>  TNumPosList;
    TNumPosList m_posList;
    
    void addClipIndex(TNumPosList& dst,long index);
    virtual void getNumClipIndex(Int64 value,bool isShowPositiveSign,TNumPosList& out_clipIndexList);
    virtual void getNumPos(TAlignType  align,TNumPosList& posList);
    virtual void doUpdateChange();
public:
    void init(){
        m_distanceX=m_numClip.getClipWidth();
        m_distanceY=0;
        m_alpha=0xFF;
        m_value=0;
        m_isShowPositiveSign=false;
        m_align=align_right;
    }
    
public:
    explicit VBmpNumberSprite(const VClipCanvas&  numClip):m_numClip(numClip){
        init();
    }
    inline void setFitSizeHeight(){
        setHeight(m_numClip.getClipHeight());
    }
    
    Int64 getNumValue()const{
        return m_value;
    }
    void setNumValue(const Int64 value){
        if (m_value!=value){
            m_value=value;
            changed();
        }
    }
    
    //透明度 alpha in [0..255]
    long getAlpha()const{
        return m_alpha;
    }
    void setAlpha(long alpha){
        if (m_alpha!=alpha){
            m_alpha=(UInt8)alpha;
            changed();
        }
    }
    
    //数字之间的显示间距
    long getDistanceX()const{
        return m_distanceX;
    }	
    void setDistanceX(long distanceX){
        if (m_distanceX!=distanceX){
            m_distanceX=distanceX;
            changed();
        }
    }	
    long getDistanceY()const{
        return m_distanceY;
    }
    void setDistanceY(long distanceY){
        if (m_distanceY!=distanceY){
            m_distanceY=distanceY;
            changed();
        }
    }
    
    //是否显示加号 +
    bool getIsShowPositiveSign()const{
        return m_isShowPositiveSign;
    }
    void setIsShowPositiveSign(bool isShowPositiveSign){
        if (m_isShowPositiveSign!=isShowPositiveSign){
            m_isShowPositiveSign=isShowPositiveSign;
            changed();
        }
    }
    TAlignType getAlign()const {
        return m_align;
    }
    void setAlign(TAlignType align){
        if (m_align!=align){
            m_align=align;
            changed();
        }
    }	
    
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual bool getIsHit(long clientX0, long clientY0);	
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
};



#endif //_VBmpNumberSprite_h_


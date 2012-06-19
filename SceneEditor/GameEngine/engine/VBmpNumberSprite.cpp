/*
 *  VBmpNumberSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-18.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */ 

#include "VBmpNumberSprite.h"

void VBmpNumberSprite::addClipIndex(TNumPosList& dst,long index){
    TNumPos pos;
    pos.clipIndex=index;
    //pos.width=m_numClip.getClipWidth();
    //pos.height=m_numClip.getClipHeight();
    pos.left=0;
    pos.top=0;
    dst.push_back(pos);
}

void VBmpNumberSprite::getNumClipIndex(Int64 value,bool	isShowPositiveSign,TNumPosList& out_clipIndexList){
    //TODO:
    const long csPositiveSignIndex = 10;
    const long csNegativeSignIndex = 11;
    out_clipIndexList.clear();
    
    if (value ==0)
        addClipIndex(out_clipIndexList,0);
    else{
        Int64 t_num = MyBase::abs(value);
        while (t_num > 0) {
            long BmpIndex =(long)( t_num % 10);
            t_num /=10;
            addClipIndex(out_clipIndexList,BmpIndex);
        }
        if ( (csNegativeSignIndex<m_numClip.getClipCount()) && (value < 0) ) //-
            addClipIndex(out_clipIndexList,csNegativeSignIndex);
        if ( (csPositiveSignIndex<m_numClip.getClipCount()) && (value > 0) && m_isShowPositiveSign) //-
            addClipIndex(out_clipIndexList,csPositiveSignIndex);
    }	
}

void VBmpNumberSprite::getNumPos(TAlignType  align,TNumPosList& posList){
    long size=(long)posList.size();
    if (size<=0) return;
    
    TRect rect;
    rect.setEmpty();
    long x0=0;
    long y0=0;
    for (long i=size-1;i>=0;--i){
        rect.max(x0,y0,x0+m_numClip.getClipWidth(),y0+m_numClip.getClipHeight());
        x0+=m_distanceX;
        y0+=m_distanceY;
    }
    long sumWidth=rect.getWidth();
    long sumHeight=rect.getHeight();
    
    if (align==align_left) {
        x0=-rect.x0;  
        y0=-rect.y0; 
    }else if (align==align_center){ 
        x0=-rect.x0+(getWidth()-sumWidth)/2;  
        y0=-rect.y0+(getHeight()-sumHeight)/2; 
    }else{ 
        x0=-rect.x0+(getWidth()-sumWidth);  
        y0=-rect.y0+(getHeight()-sumHeight); 
    }
    
    for (long i=size-1;i>=0;--i){
        posList[i].left=x0;
        posList[i].top=y0;
        x0+=m_distanceX;
        y0+=m_distanceY;
    }
}

void VBmpNumberSprite::doUpdateChange(){
    getNumClipIndex(m_value,m_isShowPositiveSign,m_posList);
    getNumPos(m_align,m_posList);
    
    //get m_hitTestRect
    long size=(long)m_posList.size();
    if (size<=0) {
        m_hitTestRect.setEmpty();
    }else{
        const TNumPos& gpos=m_posList[0];
        m_hitTestRect.setRect(gpos.left,gpos.top,gpos.left+m_numClip.getClipWidth(),gpos.top+m_numClip.getClipHeight());
        for (long i=1;i<size;++i){
            const TNumPos& pos=m_posList[i];
            m_hitTestRect.max(pos.left,pos.top,pos.left+m_numClip.getClipWidth(),pos.top+m_numClip.getClipHeight());
        }
    }
}


void VBmpNumberSprite::doDraw(const VCanvas& dst,long x0,long y0){
    //updateChange();
    long size=(long)m_posList.size();
    VCanvas numPic;
    for (long i=size-1;i>=0;--i){
        const TNumPos& pos=m_posList[i];
        m_numClip.getSubCanvasByClipIndex(pos.clipIndex,numPic);
        dst.blend(x0+pos.left,y0+pos.top,numPic,m_alpha);
    }
}

bool VBmpNumberSprite::getIsHit(long clientX0, long clientY0){
    updateChange();
    return m_hitTestRect.getIsHit(clientX0, clientY0);
}


void VBmpNumberSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    updateChange();
    out_rect.x0=m_hitTestRect.x0+x0;
    out_rect.y0=m_hitTestRect.y0+y0;
    out_rect.x1=m_hitTestRect.x1+x0;
    out_rect.y1=m_hitTestRect.y1+y0;
}

//
//  VDrawPipe.h
//
//  Created by housisong on 08-4-2.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VDrawPipe_h_
#define _VDrawPipe_h_

#include "VSprite.h"
#include "../base/Rect.h"

class VDrawPipe{
public:
    typedef Vector<ISprite*> TSpriteList;
    typedef HashMap<ISprite*,TRect> TSpriteMap; 
private:
    TSpriteList					m_spriteList;
    TSpriteMap                  m_OldSpriteMap;
    TSpriteMap                  m_NewSpriteMap;
    bool                        m_isHaveNewSpriteMap;
    TRectArray                  m_drawRectList;
    bool						m_isInDrawing;
    TRect						m_clipRect;
    void getNewSpriteMap();
    inline void addSpriteToPipe(ISprite* aSprite){
        m_spriteList.push_back(aSprite);
        m_isHaveNewSpriteMap=false;
    }
    void resetPipeForNewFrame();
    
    void rasterize(const VCanvas& dst,int x0,int y0,bool isFullScreenDraw,TRect** ppRect,long* pRectCount) ;//绘制
public:
    VDrawPipe(): m_isHaveNewSpriteMap(false),m_isInDrawing(false){}
    virtual ~VDrawPipe(){}
    void resetPipe();
    
    void drawBegin();
    void drawSprite(ISprite* aSprite) {  addSpriteToPipe(aSprite); }
    void drawOut(const VCanvas& dst,int x0,int y0,bool isFullScreenDraw,TRect** ppRect=0,long* pRectCount=0) {  rasterize(dst,x0,y0,isFullScreenDraw,ppRect,pRectCount); }
    void drawOutFull(const VCanvas& dst,int x0,int y0) {  rasterize(dst,x0,y0,true,0,0); }
    void drawEnd();
};



#endif //_VDrawPipe_h_
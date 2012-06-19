//VDrawPipe.cpp

#include "VDrawPipe.h"
#include "../base/Rand.h"
#include "assert.h"

void VDrawPipe::resetPipeForNewFrame(){
    m_spriteList.clear();
    m_NewSpriteMap.clear();
    m_isHaveNewSpriteMap=false;
    m_isInDrawing=false;
    m_clipRect.setEmpty();
}


void VDrawPipe::resetPipe(){
    resetPipeForNewFrame();
    m_OldSpriteMap.clear();
}

void VDrawPipe::drawBegin(){
    assert(!m_isInDrawing);
    resetPipeForNewFrame();
    m_isInDrawing=true;
}

void VDrawPipe::getNewSpriteMap(){
    if (m_isHaveNewSpriteMap) return;
    
    long size=(long)m_spriteList.size();
    long insertIndex=0;
    assert(!m_clipRect.getIsEmpty());
    
    for (long i=0;i<size;++i){
        ISprite* aSprite=m_spriteList[i];
        SpriteDrawRectInfo& rectInfo=aSprite->private_getSpriteDrawRectInfo();
        aSprite->getWantDrawRect(rectInfo.drawX0,rectInfo.drawY0,rectInfo.wantDrawRect);
        if (m_clipRect.getIsHit(rectInfo.wantDrawRect)) { //和屏幕有交集
            m_spriteList[insertIndex]=aSprite;
            ++insertIndex;
        }
    }
    size=insertIndex;
    m_spriteList.resize(size);
    
    m_NewSpriteMap.clear();
    for (long i=0;i<size;++i){
        ISprite* aSprite=m_spriteList[i];
        m_NewSpriteMap[aSprite]=aSprite->private_getSpriteDrawRectInfo().wantDrawRect;
    }	
    m_isHaveNewSpriteMap=true;
}

void VDrawPipe::drawEnd(){
    getNewSpriteMap();
    m_OldSpriteMap.swap(m_NewSpriteMap);
    m_NewSpriteMap.clear();
    m_isHaveNewSpriteMap=false;
    
    assert(m_isInDrawing);
    m_isInDrawing=false;
}


static void rasterizeSpriteList(const VCanvas& dst,int x0,int y0,const TRectArray& drawRectList,const VDrawPipe::TSpriteList& spriteList){
    //static VRandLong rand;
    long rectCount=(long)drawRectList.size();
    if (rectCount==0) return;
    long spriteCount=(long)spriteList.size();
    VCanvas dstSubCanvas;
    for (long s=0;s<spriteCount;++s){
        ISprite* aSprite=spriteList[s];
        SpriteDrawRectInfo& rectInfo=aSprite->private_getSpriteDrawRectInfo();
        for (long i=0;i<rectCount;++i){
            const TRect& drawRect=drawRectList[i];
            if (drawRect.getIsHit(rectInfo.wantDrawRect)){
                int dx0=rectInfo.drawX0-drawRect.x0;
                int dy0=rectInfo.drawY0-drawRect.y0;
                int mx0=x0+drawRect.x0;
                int my0=y0+drawRect.y0;
                dst.getCanvasSub(mx0,my0, x0+drawRect.x1,y0+drawRect.y1,dstSubCanvas);
                if (mx0<0) dx0+=mx0;
                if (my0<0) dy0+=my0;
                aSprite->draw(dstSubCanvas,dx0,dy0);
                //dstSubCanvas.fillColoring(Color32(rand.next()%128+128,rand.next()%128+128,rand.next()%128+128,30));
            }
        }
    }
}


void VDrawPipe::rasterize(const VCanvas& dst,int x0,int y0,bool isFullScreenDraw,TRect** ppRect,long* pRectCount) {//绘制
    //isFullScreenDraw=true;
    if (dst.getIsEmpty()) return;
    //获得精灵当前绘制
    m_clipRect=TRect(0,0,dst.getWidth(),dst.getHeight());
    getNewSpriteMap();
    
    
    //全屏幕绘制
    if (isFullScreenDraw){
        m_drawRectList.clear();
        m_drawRectList.push_back(m_clipRect);
        rasterizeSpriteList(dst,x0,y0,m_drawRectList,m_spriteList);
        if (ppRect!=0){
            m_drawRectList[0].move(x0,y0);
            *ppRect=&m_drawRectList[0];
            *pRectCount=1;
        }
        return;
    }
    
    //获取脏矩形
    VRectCalc rectCalc;
    const long bestMaxDrawCount=16;
    rectCalc.init(m_clipRect,bestMaxDrawCount);
    
    long newSpriteCount=(long)m_spriteList.size();
    for (long i=0;i<newSpriteCount;++i){
        ISprite* aSprite=m_spriteList[i];
        SpriteDrawRectInfo& rectInfo=aSprite->private_getSpriteDrawRectInfo();
        if (m_OldSpriteMap.count(aSprite)>0){
            TSpriteMap::iterator it(m_OldSpriteMap.find(aSprite));
            TRect& oldRect=it->second;
            if (rectInfo.isDrawChanged){ //已有的 并且发生了改变
                rectCalc.addRect(rectInfo.wantDrawRect);
                if (oldRect!=rectInfo.wantDrawRect)
                    rectCalc.addRect(oldRect);					
            }else if (oldRect!=rectInfo.wantDrawRect){ //绘图位置发生改变
                rectCalc.addRect(rectInfo.wantDrawRect);
                rectCalc.addRect(oldRect);		
            }
        }else//没有找到 那么是新出现的精灵
            rectCalc.addRect(rectInfo.wantDrawRect);
        
        rectInfo.isDrawChanged=false;
    }
    
    for (TSpriteMap::iterator itOld(m_OldSpriteMap.begin());itOld!=m_OldSpriteMap.end();++itOld){
        if (m_NewSpriteMap.count(itOld->first)==0) //没有找到 那么是消失的精灵
            rectCalc.addRect(itOld->second);
    }
    
    rectCalc.flush();
    
    
    m_drawRectList.swap(rectCalc.getRectList());
    
    /*//测试找出绘制区域错误的精灵
     static VRandLong rand;
     for (long i=0;i<100;++i){
     const int maxW=30;
     const int minW=10;
     long rw=rand.next()%(maxW-minW)+minW;
     long rh=rand.next()%(maxW-minW)+minW;
     long rx0=rand.next()%(dst.getWidth()-rw);
     long ry0=rand.next()%(dst.getHeight()-rh);
     TRect drawRect(rx0,ry0,rx0+rw,ry0+rh);
     m_drawRectList.push_back(drawRect);
     }*/
    
    
    rasterizeSpriteList(dst,x0,y0,m_drawRectList,m_spriteList);
    
    if (ppRect!=0){
        long rectCount=(long)m_drawRectList.size();
        for (long i=0;i<rectCount;++i)
            m_drawRectList[i].move(x0,y0);
        if (rectCount==0)
            m_drawRectList.resize(1);
        *ppRect=&m_drawRectList[0];
        *pRectCount=rectCount;
    }
} 

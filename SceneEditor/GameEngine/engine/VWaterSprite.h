/*
 *  VWaterSprite.h
 *
 *  Created by housisong on 08-7-11.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VWaterSprite_h_
#define _VWaterSprite_h_
#include "VSprite.h"
#include "VSurface.h"

#ifdef WINCE
typedef Int16 TWaterData;
#else
typedef Int32 TWaterData;
#endif

typedef Vector<TWaterData> TWaterDataArray;

struct TDrawWaterLineData{
    Color32*	pDstData;
    Color32*    pBankData;
    Color32*    pOffsetSrcData;
    TWaterData* pWaterbuf;
    long        DrawWidth;
    long        y;
    long        WaterWidth;
    long*       PBorderWidths;
    long*       PBorderHeights;
    long        SrcPixelsBufInfoByteWidth;
    must_inline const Color32& getColor(long x,long y)const{
        UInt8* pColor=(UInt8*)pOffsetSrcData;
        pColor+=SrcPixelsBufInfoByteWidth*y;
        return ((Color32*)(pColor))[x];
    }
};
typedef void (*TDrawWaterLine_proc)(const TDrawWaterLineData& lData);

typedef void (*TRippleSpreadLine_proc)(TWaterData* pDstWaterbufBack,TWaterData* pSrcWaterbuf,long Width,long WaterWidth,long rAttenuation,Color32* pBankData);

//*
class VWaterSprite:public VSprite{
private:
    bool m_isDisposeMouseMove; 
    long m_stoneRadial;
    double m_stoneWeight;
    double m_attenuation;
    double m_speed;
    bool m_isDisposeMouseDown;
    bool m_isHaveBank;
    void init();
    void setWater(const VCanvas* aCanvas,VSurface* aSurface,bool isOwnerSurface,long aWaterWidth,long aWaterHeight);
    void clearRes();
protected:
    VSurface*  m_surface;
    bool m_isOwnerSurface;
    VCanvas   m_canvas;
    double    m_oldSumTime;
    double    m_sumTime;
    TWaterDataArray  m_waterbuf;
    TWaterDataArray  m_waterbufBack;
    long             m_waterWidth;
    long             m_waterHeight;
    Vector<long> m_borderWidths;
    Vector<long> m_borderHeights;
    
protected:
    virtual void doUpdate(double stepTime_s);	
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void doUpdateChange();
    
    virtual void disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void rippleSpread();//水波扩散
    virtual TDrawWaterLine_proc getDrawWaterLine_proc();
    virtual TRippleSpreadLine_proc getRippleSpreadLine_proc();
    virtual void getRAttenuation(void* pRAttenuation4Byte);
    virtual void incWaterValue(void* dst,const double& Value);    
    bool getIsHaveAttenuation();
    
public:
    VWaterSprite(const VCanvas& aCanvas); //水池的底
    VWaterSprite(VSurface* aSurface,bool isOwnerSurface=false); //水池的底 ,是否自动释放底图
    VWaterSprite(long aWaterWidth,long aWaterHeight); //水池宽高
    void resetWater(const VCanvas& aCanvas);
    void resetWater(VSurface* aSurface,bool isOwnerSurface=false); 
    void resetWater(long aWaterWidth,long aWaterHeight); 
    virtual ~VWaterSprite();
    bool getIsHaveBank()const{
        return m_isHaveBank;
    }
    void setIsHaveBank(bool isHaveBank);//是否考虑边界(Alpha通道标明)
    void reposeWater();//水面静止
    
    bool getIsDisposeMouseMove()const{//default true;
        return m_isDisposeMouseMove;
    }
    void setIsDisposeMouseMove(bool isDisposeMouseMove); 
    
    bool getIsDisposeMouseDown()const{//default true;
        return m_isDisposeMouseDown;
    }
    void setIsDisposeMouseDown(bool isDisposeMouseDown);
    
    double getStoneRadial()const{//default 3
        return m_stoneRadial;
    }
    void setStoneRadial(long stoneRadial);
    double getStoneWeight()const{//default 70
        return m_stoneWeight;
    }
    void setStoneWeight(double stoneWeight);
    
    void dropStone(long x0,long y0) { dropStone(x0,y0,m_stoneRadial,m_stoneWeight); }
    virtual void dropStone(long x0,long y0,long stoneRadial,double stoneWeight);//震动波源(波源坐标x,y,波源半径,波源能量(一般32~300));
    
    double getAttenuation()const{//default 0.25; 衰减率in[0..1],  0代表无衰减
        return m_attenuation;
    }
    
    void setAttenuation(double attenuation);
    double getSpeed(){//default 24;
        return m_speed;
    }
    void setSpeed(double speed);
};


#endif //_VWaterSprite_h_
/*
 *  VWaterSprite.cpp
 *  Ddz
 *
 *  Created by housisong on 08-7-11.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VWaterSprite.h"
#include "../draw2d/hBlendAPixel.h"
#include "../draw2d/hDraw.h"

const long csWaterBorder =1;

void VWaterSprite::init(){
    m_oldSumTime=0;
    m_sumTime=0;
    m_surface=0;
    m_isOwnerSurface=false;
    m_isDisposeMouseMove=true;
    m_isDisposeMouseDown=true;
    m_isHaveBank=false;
    m_stoneRadial=5;
    m_stoneWeight=70;
    m_attenuation=0.25;
    m_speed=24; 
    setIsEnableSelf(true);
}

VWaterSprite::VWaterSprite(const VCanvas& aCanvas){
    init();
    setWater(&aCanvas,0,false,0,0);
}

VWaterSprite::VWaterSprite(long aWaterWidth, long aWaterHeight){
    init();
    setWater(0,0,false,aWaterWidth,aWaterHeight);
}
VWaterSprite::VWaterSprite(VSurface* aSurface,bool isOwnerSurface){
    init();
    setWater(0,aSurface,isOwnerSurface,0,0);
}

VWaterSprite::~VWaterSprite(){
    clearRes();
}


void VWaterSprite::resetWater(const VCanvas& aCanvas){
    setWater(&aCanvas,0,false,0,0);
}
void VWaterSprite::resetWater(VSurface* aSurface,bool isOwnerSurface){
    setWater(0,aSurface,isOwnerSurface,0,0);
}
void VWaterSprite::resetWater(long aWaterWidth,long aWaterHeight){
    setWater(0,0,false,aWaterWidth,aWaterHeight);
}

void VWaterSprite::clearRes(){
    if (m_isOwnerSurface){
        m_isOwnerSurface=false;
        delete m_surface;
    }
    m_surface=0;
    m_canvas.setEmptyRef();
    m_waterbuf.clear();
    m_waterbufBack.clear();
    m_borderWidths.clear();
    m_borderHeights.clear();
    changed();
}

void VWaterSprite::setWater(const VCanvas* aCanvas,VSurface* aSurface,bool isOwnerSurface,long aWaterWidth,long aWaterHeight){
    clearRes();
    
    if (aSurface!=0){
        m_isOwnerSurface=isOwnerSurface;
        m_surface=aSurface;
    }
    
    if (aCanvas!=0){
        m_canvas=*aCanvas;
        m_waterWidth=m_canvas.getWidth();
        m_waterHeight=m_canvas.getHeight();
    }else if(m_surface!=0){
        m_canvas=m_surface->getCanvas();
        m_waterWidth=m_canvas.getWidth();
        m_waterHeight=m_canvas.getHeight();
    }else{
        m_waterWidth=aWaterWidth;
        m_waterHeight=aWaterHeight;
    }
    
    setWidth(m_waterWidth);
    setHeight(m_waterHeight);
    
    m_waterWidth+=csWaterBorder*2;
    m_waterHeight+=csWaterBorder*2;
    long size=m_waterWidth*m_waterHeight;
    m_waterbuf.resize(size);
    m_waterbufBack.resize(size);
    reposeWater();
    
    changed();
}


void VWaterSprite::reposeWater(){
    long size=(long)m_waterbuf.size();
    for (long i=0;i<size;++i){
        m_waterbuf[i]=0;
        m_waterbufBack[i]=0;
    }
}


void VWaterSprite::doUpdateChange(){
    //const long maxRunTime = 50;		
    long aRunTime=(long)((m_sumTime-m_oldSumTime)*m_speed);
    if (aRunTime<=0) return;
    
    m_oldSumTime=m_sumTime;
    rippleSpread();
    /*
     if (aRunTime>=maxRunTime) //太多次了 
     reposeWater();
     else{
     for (long i=0;i<aRunTime;++i){
     rippleSpread();
     }
     }*/
}


template<class TSetPixelOp>
must_inline void  tm_drawWaterLineCopy(const TDrawWaterLineData& lData,TSetPixelOp& op){
    long lData_DrawWidth=lData.DrawWidth;
    TWaterData* lData_pWaterbuf=lData.pWaterbuf;
    long lData_WaterWidth=lData.WaterWidth;
    Color32* lData_pBankData=lData.pBankData;
    Color32* lData_pDstData=lData.pDstData;
    long* lData_PBorderWidths=lData.PBorderWidths;
    long* lData_PBorderHeights=lData.PBorderHeights;
    long lData_y=lData.y;
    
    for (long x=0;x<lData_DrawWidth;++x){
        long xoff = (lData_pWaterbuf[x]-lData_pWaterbuf[x+1]) >>3;//计算偏移量
        long yoff = (lData_pWaterbuf[x]-lData_pWaterbuf[x+lData_WaterWidth]) >>3;
        op.set_bank(lData_pBankData[x].a);
        op.set_pixel(&lData_pDstData[x],lData.getColor(lData_PBorderWidths[x+xoff],lData_PBorderHeights[lData_y+yoff]));
    }
}

struct TSetPixelCopy_bank:public TSetPixelCopy{
    must_inline void set_bank(long bank){}
};

struct TSetPixelBlendAlpha_bank:public TSetPixelBlendAlpha{
    must_inline TSetPixelBlendAlpha_bank(const long alpha):TSetPixelBlendAlpha(alpha){ }
    must_inline void set_bank(long bank){ m_alpha=bank; }
};

static void  drawWaterLineCopy(const TDrawWaterLineData& lData){
    TSetPixelCopy_bank op;
    tm_drawWaterLineCopy(lData,op);
}

static void  drawWaterLineBlend(const TDrawWaterLineData& lData){
    TSetPixelBlendAlpha_bank op(0);
    tm_drawWaterLineCopy(lData,op);
}

TDrawWaterLine_proc VWaterSprite::getDrawWaterLine_proc(){
    if (m_isHaveBank&&(!m_canvas.getIsEmpty()))
        return drawWaterLineBlend;
    else
        return drawWaterLineCopy;
}


template<bool isHaveBank>
void tm_RippleSpreadLine(TWaterData* pDstWaterbufBack,TWaterData* pSrcWaterbuf,long Width,long WaterWidth,long Attenuation_16,const Color32* pBankData){
    for (long x=0;x<Width;++x){
        long newh =(( (pSrcWaterbuf[x-1])+(pSrcWaterbuf[x+1])
                     +(pSrcWaterbuf[x-WaterWidth])+(pSrcWaterbuf[x+WaterWidth])
                     ) >>1)  - (pDstWaterbufBack[x]);
        if (isHaveBank){
            newh=(newh*Attenuation_16)>>12;
            //#ifdef WINCE
            //	pDstWaterbufBack[x]=(newh*pBankData[x].a) >>12;
            //#else
            pDstWaterbufBack[x]=(TWaterData)((newh*pBankData[x].a) / (1<<12));
            //#endif
        }else{
            //#ifdef WINCE
            //	pDstWaterbufBack[x]=(newh*Attenuation_16)>>16;
            //#else
            pDstWaterbufBack[x]=(TWaterData)((newh*Attenuation_16) / (1<<16));
            //#endif
        }
    }
}


static void RippleSpreadNoBankLine(TWaterData* pDstWaterbufBack,TWaterData* pSrcWaterbuf,long Width,long WaterWidth,long Attenuation_16,Color32* p_nil_BankData){
    tm_RippleSpreadLine<false>(pDstWaterbufBack, pSrcWaterbuf,Width,WaterWidth,Attenuation_16,p_nil_BankData);
}

static void RippleSpreadBankLine(TWaterData* pDstWaterbufBack,TWaterData* pSrcWaterbuf,long Width,long WaterWidth,long Attenuation_16,Color32* pBankData){
    tm_RippleSpreadLine<true>(pDstWaterbufBack, pSrcWaterbuf,Width,WaterWidth,Attenuation_16,pBankData);
}

TRippleSpreadLine_proc VWaterSprite::getRippleSpreadLine_proc(){
    if (m_isHaveBank&&(!m_canvas.getIsEmpty()))
        return RippleSpreadBankLine;
    else
        return RippleSpreadNoBankLine;
}

void VWaterSprite::getRAttenuation(void* pRAttenuation4Byte){
    long Attenuation_16=(long)(0.5+(1 << 16)*(1-MyBase::sqr(m_attenuation)));
    if (Attenuation_16>=(1 << 16) )
        Attenuation_16=(1 << 16)-1;
    *(Int32*)pRAttenuation4Byte=Attenuation_16;
}

void VWaterSprite::rippleSpread(){	
    if (m_attenuation==1){
        reposeWater();
        return;
    }
    
    Pixels32Ref BankPixelsBufInfo;
    Color32*   pBankData=0;
    if ((m_isHaveBank)&&(!m_canvas.getIsEmpty())){
        BankPixelsBufInfo=m_canvas.getRef();
        pBankData=BankPixelsBufInfo.pdata;
    }
    
    Int32 rAttenuation;
    getRAttenuation(&rAttenuation);
    
    TRippleSpreadLine_proc RippleSpreadLine=getRippleSpreadLine_proc();
    long pos=csWaterBorder*m_waterWidth+csWaterBorder;
    for (long y=csWaterBorder ; y<m_waterHeight-csWaterBorder;++y){
        RippleSpreadLine(&m_waterbufBack[pos],&m_waterbuf[pos],m_waterWidth-csWaterBorder*2,m_waterWidth,rAttenuation,pBankData);
        pos+=m_waterWidth;
        (UInt8*&)pBankData+=BankPixelsBufInfo.byte_width;
    }
    
    //交换波能数据缓冲区
    m_waterbuf.swap(m_waterbufBack);
}



void VWaterSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    out_rect.x0=x0;
    out_rect.y0=y0;
    out_rect.x1=x0+m_waterWidth-2*csWaterBorder;
    out_rect.y1=y0+m_waterHeight-2*csWaterBorder;	
}


void VWaterSprite::doDraw(const VCanvas& dst,long x0,long y0){
    const long csMaxWaterOffset =512;
    
    long Dstx0=x0;
    long Dsty0=y0;
    
    long SrcOldWidth=m_waterWidth-2*csWaterBorder;
    long SrcOldHeight=m_waterHeight-2*csWaterBorder;
    long SrcX0=0;
    long SrcY0=0; 
    long SrcX1=SrcOldWidth;  
    long SrcY1=SrcOldHeight;
    
    if ( ! clipDraw(dst.getWidth(),dst.getHeight(),Dstx0,Dsty0,
                    SrcOldWidth,SrcOldHeight,SrcX0,SrcY0,SrcX1,SrcY1) ) 
        return;
    long DrawWidth=SrcX1-SrcX0;
    long DrawHeight=SrcY1-SrcY0;
    Pixels32Ref DstPixelsBufInfo=dst.getRef().getSubRef(Dstx0,Dsty0,Dstx0+DrawWidth,Dsty0+DrawHeight);
    if (DstPixelsBufInfo.pdata==0) return ;
    
    Pixels32Ref SrcPixelsBufInfo;
    VSurface  tmpPixelBuf;
    if (m_canvas.getIsEmpty()){
        tmpPixelBuf.resizeFast(DstPixelsBufInfo.width,DstPixelsBufInfo.height);
        SrcPixelsBufInfo=tmpPixelBuf.getCanvas().getRef();
        SrcPixelsBufInfo.width=DstPixelsBufInfo.width;
        SrcPixelsBufInfo.height=DstPixelsBufInfo.height;
        copy(SrcPixelsBufInfo,0,0,DstPixelsBufInfo);
    } else{
        SrcPixelsBufInfo=m_canvas.getRef().getSubRef(SrcX0,SrcY0,SrcX1,SrcY1);
    }
    
    long tmpSize=DrawWidth+csMaxWaterOffset*2;
    if (tmpSize!=m_borderWidths.size()){
        m_borderWidths.resize(tmpSize);
        for (long x=0;x<csMaxWaterOffset;++x) m_borderWidths[x]=0;
        for (long x=0;x<DrawWidth;++x) m_borderWidths[x+csMaxWaterOffset]=x;
        for (long x=DrawWidth+csMaxWaterOffset;x<DrawWidth+csMaxWaterOffset*2;++x) m_borderWidths[x]=DrawWidth-1;
    }
    tmpSize=DrawHeight+csMaxWaterOffset*2;
    if (tmpSize!=m_borderHeights.size()){
        m_borderHeights.resize(tmpSize);
        for (long y=0;y<csMaxWaterOffset;++y) m_borderHeights[y]=0;
        for (long y=0;y<DrawHeight;++y) m_borderHeights[y+csMaxWaterOffset]=y;
        for (long y=DrawHeight+csMaxWaterOffset;y<DrawHeight+csMaxWaterOffset*2;++y) m_borderHeights[y]=DrawHeight-1;
    }
    
    TDrawWaterLineData DrawWaterLineData;
    DrawWaterLineData.PBorderWidths=&m_borderWidths[csMaxWaterOffset];
    DrawWaterLineData.PBorderHeights=&m_borderHeights[csMaxWaterOffset];
    DrawWaterLineData.pBankData=SrcPixelsBufInfo.pdata;
    DrawWaterLineData.pOffsetSrcData=SrcPixelsBufInfo.pdata;
    DrawWaterLineData.pDstData=DstPixelsBufInfo.pdata;
    DrawWaterLineData.pWaterbuf=&m_waterbuf[(csWaterBorder+SrcY0)*m_waterWidth+(csWaterBorder+SrcX0)];
    DrawWaterLineData.SrcPixelsBufInfoByteWidth=SrcPixelsBufInfo.byte_width;
    DrawWaterLineData.DrawWidth=DrawWidth;
    DrawWaterLineData.WaterWidth=m_waterWidth;
    
    TDrawWaterLine_proc DrawWaterLine=this->getDrawWaterLine_proc();
    for (long y=0;y<DrawHeight;++y){
        DrawWaterLineData.y=y;
        DrawWaterLine(DrawWaterLineData);
        (UInt8*&)DrawWaterLineData.pBankData+=SrcPixelsBufInfo.byte_width;
        (UInt8*&)DrawWaterLineData.pDstData+=DstPixelsBufInfo.byte_width;
        (UInt8*&)DrawWaterLineData.pWaterbuf+=m_waterWidth*sizeof(TWaterData);
    }
}



void VWaterSprite::disposeEventMouseDown(const TSpriteMouseEventInfo& mouseEventInfo){
    VSprite::disposeEventMouseDown(mouseEventInfo);
    
    if (m_isDisposeMouseDown)
        dropStone(mouseEventInfo.clientX0,mouseEventInfo.clientY0);
}
void VWaterSprite::disposeEventMouseMoved(const TSpriteMouseEventInfo& mouseEventInfo){
    VSprite::disposeEventMouseMoved(mouseEventInfo);
    
    if (m_isDisposeMouseMove)
        dropStone(mouseEventInfo.clientX0,mouseEventInfo.clientY0);
}

void VWaterSprite::doUpdate(double stepTime_s){
    m_sumTime+=stepTime_s;
    //rippleSpread();
    changed();
}


void VWaterSprite::dropStone(long x0,long y0,long stoneRadial,double stoneWeight){
    const double R_255 =1.0/255;
    
    x0+=csWaterBorder;
    y0+=csWaterBorder;
    long left=STD::max(csWaterBorder,x0-stoneRadial);
    long right=STD::min(x0+stoneRadial,m_waterWidth-2*csWaterBorder);
    if (left>=right) return;
    long top=STD::max(csWaterBorder,y0-stoneRadial);
    long bottom=STD::min(y0+stoneRadial,m_waterHeight-2*csWaterBorder);
    if (top>=bottom) return;
    stoneWeight=-stoneWeight;
    
    double R_stoneRadial=1.0/stoneRadial;
    if (m_isHaveBank&&(!m_canvas.getIsEmpty())){
        Pixels32Ref BankPixelsBufInfo=m_canvas.getRef();
        Color32* pBankData=&BankPixelsBufInfo.pixels(-csWaterBorder,top-csWaterBorder);
        R_stoneRadial=R_stoneRadial*R_255;
        for (long y=top;y<bottom;++y){
            long pos=y*m_waterWidth+left;
            long maxXXsize=MyBase::sqr(stoneRadial)-MyBase::sqr(y-y0);
            for (long x=left;x<right;++x){
                if (MyBase::sqr(x-x0)<=maxXXsize) {
                    double tmp= stoneWeight*(stoneRadial-MyBase::abs(x-x0))*R_stoneRadial*pBankData[x].a;
                    incWaterValue(&m_waterbuf[pos],tmp);
                }
                ++pos;
            }
            (UInt8*&)pBankData+=BankPixelsBufInfo.byte_width;
        }
    } else {
        for (long y=top;y<bottom;++y){
            long pos=y*m_waterWidth+left;
            long maxXXsize=MyBase::sqr(stoneRadial)-MyBase::sqr(y-y0);
            for (long x=left;x<right;++x){
                if (MyBase::sqr(x-x0)<=maxXXsize) {
                    double tmp= stoneWeight*(stoneRadial-MyBase::abs(x-x0))*R_stoneRadial;
                    incWaterValue(&m_waterbuf[pos],tmp);
                }
                ++pos;
            }
        }
    } 
}


void VWaterSprite::setIsHaveBank(bool isHaveBank){
    if (m_isHaveBank!=isHaveBank){
        m_isHaveBank=isHaveBank;
        changed();
    }		
}

void VWaterSprite::setIsDisposeMouseMove(bool isDisposeMouseMove){
    if (m_isDisposeMouseMove!=isDisposeMouseMove){
        m_isDisposeMouseMove=isDisposeMouseMove;
    }
}

void VWaterSprite::setIsDisposeMouseDown(bool isDisposeMouseDown){
    if (m_isDisposeMouseDown!=isDisposeMouseDown){
        m_isDisposeMouseDown=isDisposeMouseDown;
    }
}

void VWaterSprite::setStoneRadial(long stoneRadial){
    if (m_stoneRadial!=stoneRadial){
        m_stoneRadial=stoneRadial;
    }
}

void VWaterSprite::setStoneWeight(double stoneWeight){
    if (m_stoneWeight!=stoneWeight){
        m_stoneWeight=stoneWeight;
    }
}

void VWaterSprite::setAttenuation(double attenuation){
    if (attenuation<0) attenuation=0;
    if (attenuation>1) attenuation=1;
    if (m_attenuation!=attenuation){
        m_attenuation=attenuation;
        //
    }
}

void VWaterSprite::setSpeed(double speed){
    if (speed<0) speed=0;
    if (m_speed!=speed){
        m_speed=speed;
    }
}



void VWaterSprite::incWaterValue(void* dst,const double& Value){
    (*(TWaterData*)dst)+=(TWaterData)Value;
}







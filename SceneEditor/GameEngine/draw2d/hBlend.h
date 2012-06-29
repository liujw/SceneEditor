//hBlend.h


#ifndef _hBlend_h_
#define _hBlend_h_
#include "hColor32.h"
#include "../base/SysImportType.h"
#include "hBlendAPixel.h"

template<class ISetPixelFill,class TColor>
void tm_fillLine(TColor* dst,ISetPixelFill& op,const long width){
    if (TSetPixelTraits<ISetPixelFill>::isOptimizeFillLine()){
        TSetPixelTraits<ISetPixelFill>::optimizeFillLine(dst,width,op);
    }else{
        TColor* dstEnd =&dst[width];
        if (TSetPixelTraits<ISetPixelFill>::isExpandCodeForLoop()){
            TColor* dstEndFast =&dst[(width & (~3))];
            for (; dst != dstEndFast; dst+=4){
                op.set_pixel(&dst[0]);
                op.set_pixel(&dst[1]);
                op.set_pixel(&dst[2]);
                op.set_pixel(&dst[3]);
            }
        }
        for (; dst != dstEnd; ++dst)
            op.set_pixel(dst);
    }
    do_emms();
}
template<class ISetPixelFill,class TColor>
void tm_fillLineV(TColor* dst,long byte_width,ISetPixelFill& src,const long height){
    for (long y=0; y < height; ++y){
        src.set_pixel(dst);
        (UInt8*&)dst+=byte_width;
    }
}
template<class ISetPixelFill,class TPixelsRef>
void tm_fill(const TPixelsRef& dst,ISetPixelFill& src){
    long width=dst.width;
    if (width<=0) return;
    typename TPixelsRef::TColor* dstLine=dst.pdata;
    for (long y=0;y<dst.height;++y) {
        tm_fillLine(dstLine,src,width);
        dst.nextLine(dstLine);
    }
}

template<class ISetPixelCopy,class TDstColor,class TSrcColor>
void tm_copyLine(TDstColor* dst,const TSrcColor* src,ISetPixelCopy& op,const long width){
    if (TSetPixelTraits<ISetPixelCopy>::isOptimizeCopyLine()){
        TSetPixelTraits<ISetPixelCopy>::optimizeCopyLine(dst,src,width,op);
    }else{
        TDstColor* dstEnd =&dst[width];
        if (TSetPixelTraits<ISetPixelCopy>::isExpandCodeForLoop()){
            TDstColor* dstEndFast =&dst[(width & (~3))];
            for (; dst != dstEndFast; dst+=4,src+=4){
                op.set_pixel(&dst[0],src[0]);
                op.set_pixel(&dst[1],src[1]);
                op.set_pixel(&dst[2],src[2]);
                op.set_pixel(&dst[3],src[3]);
            }
        }
        for (; dst != dstEnd; ++dst,++src){
            op.set_pixel(dst,*src);
        }
    }
    do_emms();
}

template<class ISetPixelCopy,class TDstPixelsRef,class TSrcPixelsRef>
void tm_copy(const TDstPixelsRef& dst,long dstX0, long dstY0,const TSrcPixelsRef& src,ISetPixelCopy& op){
    typedef typename TDstPixelsRef::TColor TDstColor;
    typedef typename TSrcPixelsRef::TColor TSrcColor;
    long srcX0=0; long srcY0=0;
    if (dstX0<0) { srcX0+=-dstX0; dstX0=0; }
    if (dstY0<0) { srcY0+=-dstY0; dstY0=0; }
    long width = STD::min(dst.width-dstX0, src.width-srcX0);
    if (width <= 0) return;
    long height =  STD::min(dst.height-dstY0, src.height-srcY0);
    TDstColor* dstLine=&dst.pixels(dstX0,dstY0);
    TSrcColor* srcLine=&src.pixels(srcX0,srcY0);
    for (long y=0;y<height;++y) {
        TDrawNewLineBeginAttribute<ISetPixelCopy>::setDrawNewLineBegin(op,srcX0,srcY0+y);
        tm_copyLine<ISetPixelCopy,TDstColor,TSrcColor>(dstLine,srcLine,op,width);
        dst.nextLine(dstLine);
        src.nextLine(srcLine);
    }
}

template<int testBorderSize,class TColor,class TPixelsRef>
must_inline void getColorBuf(TColor* scBuf,long x0,long y0,const TPixelsRef& srcRef){
    for (long y=0;y<testBorderSize;++y){
        for (long x=0;x<testBorderSize;++x){
            scBuf[x]=srcRef.getPixelsBorder(x0+x,y0+y);
        }
        scBuf+=testBorderSize;
    }
}


template<bool isFast,class TPixelsRef,class TFastZoomKernel,int srcBorderSize,int dstBorderSize,long MaskNoEmpty>
void tm_fastZoom(const TPixelsRef& dstRef,const TPixelsRef& srcRef,TFastZoomKernel& op){
    typedef typename TPixelsRef::TColor TColor;
    const TColor* sline=(const TColor*)srcRef.pdata;
    TColor* dline=(TColor*)dstRef.pdata;
    long dstLineInc=dstRef.byte_width;
    long srcLineInc=srcRef.byte_width;
    if ((srcBorderSize==1)&&(dstBorderSize==2)){ //!!! 1To2 特殊的边界处理
        long xCount=STD::min(dstRef.width/dstBorderSize,(srcRef.width-1)/srcBorderSize);
        long yCount=STD::min(dstRef.height/dstBorderSize,(srcRef.height-1)/srcBorderSize);
        TColor scBuf[(srcBorderSize+1)*(srcBorderSize+1)];
        for (long y=0;y<yCount;++y){
            for (long x=0;x<xCount;++x){
                op.template tm_zoomKernel<isFast,MaskNoEmpty>(&dline[x*dstBorderSize],dstLineInc,&sline[x*srcBorderSize],srcLineInc);
            }
            for (long x=xCount;x*dstBorderSize+dstBorderSize<=dstRef.width;++x){
                getColorBuf<srcBorderSize+1>(&scBuf[0],x*srcBorderSize,y*srcBorderSize,srcRef);
                op.template tm_zoomKernel<isFast,MaskNoEmpty>(&dline[x*dstBorderSize],dstLineInc,&scBuf[0],(srcBorderSize+1)*sizeof(TColor));
            }
            (UInt8*&)sline+=srcLineInc*srcBorderSize;
            (UInt8*&)dline+=dstLineInc*dstBorderSize;
        } 
        for (long y=yCount;y*dstBorderSize+dstBorderSize<=dstRef.height;++y){
            for (long x=0;x*dstBorderSize+dstBorderSize<=dstRef.width;++x){
                getColorBuf<srcBorderSize+1>(&scBuf[0],x*(srcBorderSize+1),y*(srcBorderSize+1),srcRef);
                op.template tm_zoomKernel<isFast,MaskNoEmpty>(&dline[x*dstBorderSize],dstLineInc,&scBuf[0],(srcBorderSize+1)*sizeof(TColor));
            }
            (UInt8*&)sline+=srcLineInc*srcBorderSize;
            (UInt8*&)dline+=dstLineInc*dstBorderSize;
        }
    }else{
        long xCount=STD::min(dstRef.width/dstBorderSize,srcRef.width/srcBorderSize);
        long yCount=STD::min(dstRef.height/dstBorderSize,srcRef.height/srcBorderSize);
        TColor scBuf[srcBorderSize*srcBorderSize];
        for (long y=0;y<yCount;++y){
            for (long x=0;x<xCount;++x){
                op.template tm_zoomKernel<isFast,MaskNoEmpty>(&dline[x*dstBorderSize],dstLineInc,&sline[x*srcBorderSize],srcLineInc);
            }
            for (long x=xCount;x*dstBorderSize+dstBorderSize<=dstRef.width;++x){
                getColorBuf<srcBorderSize>(&scBuf[0],x*srcBorderSize,y*srcBorderSize,srcRef);
                op.template tm_zoomKernel<isFast,MaskNoEmpty>(&dline[x*dstBorderSize],dstLineInc,&scBuf[0],srcBorderSize*sizeof(TColor));
            }
            (UInt8*&)sline+=srcLineInc*srcBorderSize;
            (UInt8*&)dline+=dstLineInc*dstBorderSize;
        } 
        for (long y=yCount;y*dstBorderSize+dstBorderSize<=dstRef.height;++y){
            for (long x=0;x*dstBorderSize+dstBorderSize<=dstRef.width;++x){
                getColorBuf<srcBorderSize>(&scBuf[0],x*srcBorderSize,y*srcBorderSize,srcRef);
                op.template tm_zoomKernel<isFast,MaskNoEmpty>(&dline[x*dstBorderSize],dstLineInc,&scBuf[0],srcBorderSize*sizeof(TColor));
            }
            (UInt8*&)sline+=srcLineInc*srcBorderSize;
            (UInt8*&)dline+=dstLineInc*dstBorderSize;
        }
    }
}

#endif //_hBlend_h_
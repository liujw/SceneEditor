//hDraw.cpp
#include "hDraw.h"
#include "hBlend.h"
#include "hBlendAPixel.h"
#include "hZoom.h"
#include "hRotary.h"

#ifndef _DEBUG
#ifdef WINCE
#define IS_USE_ARM_ASM
#endif
#endif

void fill(const Pixels32Ref& dst,const Color32& color){
    TSetPixelFill src(color);
    tm_fill(dst,src);
}
void fill(const PixelsGray8Ref& dst,const Gray8& color){
    TSetPixelGray8Fill src(color);
    tm_fill(dst,src);
}

void fillColoring(const Pixels32Ref& dst,const Color32& color) {
    if (color.argb==cl32ColoringMullEmpty.argb)
        return;
    else {
        TSetPixelFillColoring src(color);
        tm_fill(dst,src);
    }
}

void fillBlend(const Pixels32Ref& dst,const Color32& color) {
    if (color.a==0)
        return;
    else if (color.a==255)
        fill(dst,color);
    else {
        TSetPixelFillBlend src(color);
        tm_fill(dst,src);
    }
}



void fillAdd(const Pixels32Ref& dst,const Color32& color){
    if (color.argb == 0){
        return;
    } else {
        TSetPixelFillAdd src(color);
        tm_fill(dst,src);
    }
}

#ifdef IS_USE_ARM_ASM
#ifdef __cplusplus
extern "C" {
#endif
    
    void _colorCopy_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width);
    /*void __colorCopy_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width){
     dst_byte_width-=width*sizeof(Color32);
     src_byte_width-=width*sizeof(Color32);
     TSetPixelBlend op; 
     for (long y=0;y<height;++y) {
     Color32* dstEnd =&dst[width];
     Color32* dstEndFast =&dst[(width & (~3))];
     for (; dst != dstEndFast; dst+=4,src+=4){
     op.set_pixel(&dst[0],src[0]);
     op.set_pixel(&dst[1],src[1]);
     op.set_pixel(&dst[2],src[2]);
     op.set_pixel(&dst[3],src[3]);
     }
     for (; dst != dstEnd; ++dst,++src)
     op.set_pixel(dst,*src);
     
     ((UInt8*&)dst)+=dst_byte_width;
     ((UInt8*&)src)+=src_byte_width;
     }
     }*/
    
#define DrawClipFor_ARM  \
long srcX0=0; long srcY0=0;\
if (dstX0<0) { srcX0+=-dstX0; dstX0=0; }\
if (dstY0<0) { srcY0+=-dstY0; dstY0=0; }\
long width = STD::min(dst.width-dstX0, src.width-srcX0);\
if (width <= 0) return;\
long height =  STD::min(dst.height-dstY0, src.height-srcY0);
    
    
    must_inline void colorCopy_arm(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src){
        DrawClipFor_ARM
        _colorCopy_arm(&dst.pixels(dstX0,dstY0),&src.pixels(srcX0,srcY0),width,height,dst.byte_width,src.byte_width);
    }
#ifdef __cplusplus
}
#endif
#endif

void copyColoring(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src,const Color32&  color,long alpha){
    if (alpha>=255){
        if (color.argb==cl32ColoringMullEmpty.argb){
#ifdef IS_USE_ARM_ASM
            colorCopy_arm(dst,dstX0,dstY0,src);
#else
            TSetPixelCopy op;
            tm_copy(dst,dstX0,dstY0,src,op);
#endif
        } else{
            TSetPixelCopyColoring op(color);
            tm_copy(dst,dstX0,dstY0,src,op);
        }
    } else {
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelCopyAlpha op(alpha);
            tm_copy(dst,dstX0,dstY0,src,op);
        } else{
            TSetPixelCopyColoringAlpha op(color,alpha);
            tm_copy(dst,dstX0,dstY0,src,op);
        }
    }
}



void copy(const PixelsGray8Ref& dst,long dstX0, long dstY0, const PixelsGray8Ref& src){
    TSetPixelGray8Copy op;
    tm_copy(dst,dstX0,dstY0,src,op);	
}


void copyAsGray(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src,long alpha){
    if (alpha>=255){
        TSetPixelCopyAsGray op;
        tm_copy(dst,dstX0,dstY0,src,op);	
    }else if (alpha<=0){
        TSetPixelCopyAsGrayNoAlpha op;
        tm_copy(dst,dstX0,dstY0,src,op);	
    }else{
        TSetPixelCopyAsGrayAlpha op(alpha);
        tm_copy(dst,dstX0,dstY0,src,op);
    }	
}

void blendAsGray(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src,long alpha){
    if (alpha>=255){
        TSetPixelBlendAsGray op;
        tm_copy(dst,dstX0,dstY0,src,op);	
    }else if (alpha<=0){
        return;	
    }else{
        TSetPixelBlendAsGrayAlpha op(alpha);
        tm_copy(dst,dstX0,dstY0,src,op);
    }	
}

#ifdef IS_USE_ARM_ASM
#ifdef __cplusplus
extern "C" {
#endif
    
    void _colorBlend_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width);
    
    must_inline void colorBlend_arm(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src){
        DrawClipFor_ARM
        _colorBlend_arm(&dst.pixels(dstX0,dstY0),&src.pixels(srcX0,srcY0),width,height,dst.byte_width,src.byte_width);
    }
    
    void _colorBlendWithAlpha_arm(Color32 * dst,const Color32 * src,long width,long height,long dst_byte_width,long src_byte_width,long alpha);
    
    must_inline void colorBlendWithAlpha_arm(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src,long alpha){
        DrawClipFor_ARM
        _colorBlendWithAlpha_arm(&dst.pixels(dstX0,dstY0),&src.pixels(srcX0,srcY0),width,height,dst.byte_width,src.byte_width,alpha);
    }
    
#ifdef __cplusplus
}
#endif
#endif


void blendForBuffer(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src){
    TSetPixelBlendBuffer  op;
    tm_copy(dst,dstX0,dstY0,src,op);	
}

void blendColoring(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& src,const Color32&  color,long alpha){
    if (alpha<=0)
        return;
    else if (alpha>=255){
        if (color.argb==cl32ColoringMullEmpty.argb){
#ifdef IS_USE_ARM_ASM
            colorBlend_arm(dst,dstX0,dstY0,src);
#else
            TSetPixelBlend  op;
            tm_copy(dst,dstX0,dstY0,src,op);	
#endif
        } else{
            TSetPixelBlendColoring  op(color);
            tm_copy(dst,dstX0,dstY0,src,op);	
        }
    }
    else{
        if (color.argb==cl32ColoringMullEmpty.argb){
#ifdef IS_USE_ARM_ASM
            colorBlendWithAlpha_arm(dst,dstX0,dstY0,src,alpha);
#else
            TSetPixelBlendAlpha  op(alpha);
            tm_copy(dst,dstX0,dstY0,src,op);
#endif
        } else{
            TSetPixelBlendAlphaColoring  op(color,alpha);
            tm_copy(dst,dstX0,dstY0,src,op);	
        }
    }
}

void addColoring(const Pixels32Ref& dst,long dstX0, long dstY0,const Pixels32Ref& src,const Color32& color){
    if (color.argb==cl32ColoringMullEmpty.argb){
        TSetPixelAdd op;
        tm_copy(dst,dstX0,dstY0,src,op);
    } else {
        TSetPixelAddColoring op(color);
        tm_copy(dst,dstX0,dstY0,src,op);
    }
}

void blendForAdd(const Pixels32Ref& dst){
    TSetPixelBlendForAdd op;
    tm_fill(dst,op);
}

//////////////////////////////////////////////////////////////////////////////

template<class IFilter>
void tm_zoomAsCopy(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    if ((dst.width==src.height)&&(dst.height==src.height)){
        copyColoring(dst,0,0,src,color,alpha);
        return;
    }
    
    if (alpha>=255){
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelCopy op;
            zoom<IFilter>(dst,src,op);
        } else{
            TSetPixelCopyColoring op(color);
            zoom<IFilter>(dst,src,op);
        }
    } else {
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelCopyAlpha op(alpha);
            zoom<IFilter>(dst,src,op);
        } else{
            TSetPixelCopyColoringAlpha op(color,alpha);
            zoom<IFilter>(dst,src,op);
        }
    }
}

template<class IFilter>
void tm_zoomAsBlend(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    if ((dst.width==src.height)&&(dst.height==src.height)){
        blendColoring(dst,0,0,src,color,alpha);
        return;
    }
    
    if (alpha<=0)
        return;
    else if (alpha>=255){
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelBlend  op;
            zoom<IFilter>(dst,src,op);
        } else{
            TSetPixelBlendColoring  op(color);
            zoom<IFilter>(dst,src,op);
        }
    }
    else{
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelBlendAlpha  op(alpha);
            zoom<IFilter>(dst,src,op);	
        } else{
            TSetPixelBlendAlphaColoring  op(color,alpha);
            zoom<IFilter>(dst,src,op);
        }
    }	
}

template<class IFilter>
void tm_zoomAsAdd(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color){
    if ((dst.width==src.height)&&(dst.height==src.height)){
        addColoring(dst,0,0,src,color);
        return;
    }
    
    if (color.argb==cl32ColoringMullEmpty.argb){
        TSetPixelAdd op;
        zoom<IFilter>(dst,src,op);	
    } else {
        TSetPixelAddColoring op(color);
        zoom<IFilter>(dst,src,op);	
    }
}



void zoomNearestNeighborAsCopy(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    tm_zoomAsCopy<TNearestNeighborFilter>(dst,src,color,alpha);
}
void zoomNearestNeighborAsBlend(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    tm_zoomAsBlend<TNearestNeighborFilter>(dst,src,color,alpha);
}
void zoomNearestNeighborAsAdd(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color){
    tm_zoomAsAdd<TNearestNeighborFilter>(dst,src,color);
}


void zoomBilinearAsCopy(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    tm_zoomAsCopy<TBilinearFilter>(dst,src,color,alpha);
}
void zoomBilinearAsBlend(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    tm_zoomAsBlend<TBilinearFilter>(dst,src,color,alpha);
}
void zoomBilinearAsAdd(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color){
    tm_zoomAsAdd<TBilinearFilter>(dst,src,color);
}


void zoomBicubicAsCopy(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    tm_zoomAsCopy<TBicubicFilter>(dst,src,color,alpha);
}
void zoomBicubicAsBlend(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color,long alpha){
    tm_zoomAsBlend<TBicubicFilter>(dst,src,color,alpha);
}
void zoomBicubicAsAdd(const Pixels32Ref& dst,const Pixels32Ref& src,const Color32&  color){
    tm_zoomAsAdd<TBicubicFilter>(dst,src,color);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class IFilter>
void tm_rotaryAsCopy(const Pixels32Ref& dst,const Pixels32Ref& src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    if (alpha>=255){
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelCopy op;
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, 0.0, 0.0, true);
        } else{
            TSetPixelCopyColoring op(color);
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, 0.0, 0.0, true);
        }
    } else {
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelCopyAlpha op(alpha);
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, 0.0, 0.0, true);
        } else{
            TSetPixelCopyColoringAlpha op(color,alpha);
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, 0.0, 0.0, true);
        }
    }
}

template<class IFilter>
void tm_rotaryAsBlend(const Pixels32Ref& dst,const Pixels32Ref& src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha, 
                      double rx0, double ry0, bool isDefRotaryCenter = true){
    if (alpha<=0)
        return;
    else if (alpha>=255){
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelBlend  op;
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, rx0, ry0, isDefRotaryCenter);
        } else{
            TSetPixelBlendColoring  op(color);
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, rx0, ry0, isDefRotaryCenter);
        }
    }
    else{
        if (color.argb==cl32ColoringMullEmpty.argb){
            TSetPixelBlendAlpha  op(alpha);
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, rx0, ry0, isDefRotaryCenter);	
        } else{
            TSetPixelBlendAlphaColoring  op(color,alpha);
            rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, rx0, ry0, isDefRotaryCenter);
        }
    }	
}

template<class IFilter>
void tm_rotaryAsAdd(const Pixels32Ref& dst,const Pixels32Ref& src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color){
    if (color.argb==cl32ColoringMullEmpty.argb){
        TSetPixelAdd op;
        rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, 0.0, 0.0, true);	
    } else {
        TSetPixelAddColoring op(color);
        rotary<IFilter>(dst,src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,op, 0.0, 0.0, true);	
    }
}


void rotaryNearestNeighborAsCopy(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    tm_rotaryAsCopy<TNearestNeighborFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
}
void rotaryNearestNeighborAsBlend(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    tm_rotaryAsBlend<TNearestNeighborFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha, 0.0, 0.0, true);
}
void rotaryNearestNeighborAsAdd(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color){
    tm_rotaryAsAdd<TNearestNeighborFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color);
}
void rotaryBilinearAsCopy(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    tm_rotaryAsCopy<TBilinearFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
}
void rotaryBilinearAsBlend(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    tm_rotaryAsBlend<TBilinearFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha, 0.0, 0.0, true);
}
void rotaryBilinearAsBlendEx(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y, double rx0, double ry0, bool isDefRotaryCenter, const Color32& color, long alpha){
    tm_rotaryAsBlend<TBilinearFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha, rx0, ry0, isDefRotaryCenter);
}
void rotaryBilinearAsAdd(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color){
    tm_rotaryAsAdd<TBilinearFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color);
}
void rotaryBicubicAsCopy(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    tm_rotaryAsCopy<TBicubicFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
}
void rotaryBicubicAsBlend(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color,long alpha){
    tm_rotaryAsBlend<TBicubicFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha, 0.0, 0.0, true);
}
void rotaryBicubicAsAdd(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color){
    tm_rotaryAsAdd<TBicubicFilter>(Dst,Src,RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

void copyToGray(const PixelsGray8Ref& dst,long x0, long y0,const Pixels32Ref& src){
    TSetPixelCopyToGray op;
    tm_copy(dst,x0,y0,src,op);
}
void copyFromGray(const Pixels32Ref& dst,long x0, long y0,const PixelsGray8Ref& src){
    TSetPixelCopyFromGray op;
    tm_copy(dst,x0,y0,src,op);
}
void copyAsTextPixels(const PixelsGray8Ref& dst,long x0, long y0,const Pixels32Ref& src){
    TSetPixelCopyToText op;
    tm_copy(dst,x0,y0,src,op);
}

void blendLight(const Pixels32Ref& dst,long dstX0, long dstY0,const PixelsGray8Ref& src,const Color32& color) {
    long alpha=color.a;
    if (alpha==0) 
        return;
    else if (alpha==255){
        TSetPixelLightBlend op(color);
        tm_copy(dst,dstX0,dstY0,src,op);
    } else{
        TSetPixelLightBlendAlpha op(color);
        tm_copy(dst,dstX0,dstY0,src,op);
    }
}

void addLight(const Pixels32Ref& dst,long dstX0, long dstY0,const PixelsGray8Ref& src,const Color32& color) {
    long alpha=color.a;
    if (alpha==0) 
        return;
    else if (alpha==255){
        TSetPixelLightAdd op(color);
        tm_copy(dst,dstX0,dstY0,src,op);
    } else{
        TSetPixelLightAddAlpha op(color);
        tm_copy(dst,dstX0,dstY0,src,op);
    }
}


void lineH(const Pixels32Ref& dst,long y, long x0, long x1, const Color32& color) {
    long a=color.a;
    if (a==0) return;
    if ((y<0)||(y>=dst.height)) return;
    if (x0>x1){
        long t=x0; x0=x1+1; x1=t+1;
    }
    if ((x1<=0)||(x0>=dst.width)) return;
    if (x0<0) x0=0;
    if (x1>dst.width) x1=dst.width;
    if (x0>=x1) return;
    
    Color32* pDstLine=&dst.pixels(x0,y);
    if (a==255){
        TSetPixelFill src(color);
        tm_fillLine(pDstLine,src, x1-x0);
    }else{
        TSetPixelFillColoring src(color);
        tm_fillLine(pDstLine,src, x1-x0);
    }
}

void lineV(const  Pixels32Ref& dst,long x, long y0, long y1,  const Color32& color) {
    long a=color.a;
    if (a==0) return;
    if ((x<0)||(x>=dst.width)) return;
    if (y0>y1){
        long t=y0; y0=y1+1; y1=t+1;
    }
    if ((y1<=0)||(y0>=dst.height)) return;
    if (y0<0) y0=0;
    if (y1>dst.height) y1=dst.height; 
    if (y0>=y1) return;
    
    Color32* pDstLine=&dst.pixels(x,y0);
    if (a==255){
        TSetPixelFill src(color);
        tm_fillLineV(pDstLine,dst.byte_width,src, y1-y0);
    }else{
        TSetPixelFillColoring src(color);
        tm_fillLineV(pDstLine,dst.byte_width,src, y1-y0);
    }
}

void rect(const Pixels32Ref& dst,long x0, long y0, long x1, long y1,  const Color32& color) {
    if ((x0==x1)||(y0==y1)) return;
    if (x0>x1) {
        long t=x0;  x0=x1;  x1=t;
    }
    if (y0>y1){
        long t=y0;  y0=y1;  y1=t;
    }
    lineH(dst,y0,x0,x1,color);
    lineH(dst,y1-1,x0,x1,color);
    lineV(dst,x0,y0,y1,color);
    lineV(dst,x1-1,y0,y1,color);
}	


static void unionLayer(const Pixels32Ref& dst_backPic,long dstX0, long dstY0, const Pixels32Ref& frontPic){
    TSetPixelUnionLayer op;
    tm_copy(dst_backPic,dstX0,dstY0,frontPic,op);	
}


void darkle(const Pixels32Ref& dst,long delLight){
    if (delLight==0) 
        return;
    else{
        TSetPixelDarkleFill src(delLight);
        tm_fill(dst,src);
    }
}


void blendWithYAlphas(const Pixels32Ref& dst,long x0,long y0,const Pixels32Ref& src,const UInt8* alphas){
    TSetPixelBlendWithYAlphas op(alphas);
    tm_copy(dst,x0,y0,src,op);	
}
void blendWithYColorings(const Pixels32Ref& dst,long x0,long y0,const Pixels32Ref& src,const Color32* colorings){
    TSetPixelBlendWithYColorings op(colorings);
    tm_copy(dst,x0,y0,src,op);
}


const long node_width=8;
must_inline static void rotary90FastAsCopy_forDevice_copyLine_Node(Color32* dst,const Color32* src,long byte_width){
    dst[ 0]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 1]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 2]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 3]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 4]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 5]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 6]=*src; ((UInt8*&)src)+=byte_width;
    dst[ 7]=*src; ((UInt8*&)src)+=byte_width;
}

static void rotary90FastAsCopy_forDevice_Node(Color32* dst,long dst_byte_width,const Color32* src,long src_byte_width){
    for (long y=0;y<node_width;++y)
    {
        rotary90FastAsCopy_forDevice_copyLine_Node(dst,src,src_byte_width);
        ++src;
        (UInt8*&)dst+=dst_byte_width;
    }
}


static void rotary90FastAsCopy_forDevice(const Pixels32Ref& dst,const Pixels32Ref& src){
    Color32* dstLine=dst.pdata;
    Color32* srcLine=(Color32*)src.pdata;
    long width=dst.width;
    for (long y=0;y<dst.height;y+=node_width) {
        Color32* node_srcLine=srcLine;
        for (long x=0;x<width;x+=node_width){
            rotary90FastAsCopy_forDevice_Node(&dstLine[x],dst.byte_width,node_srcLine,src.byte_width);
            src.nextLine(node_srcLine,node_width);
        }
        dst.nextLine(dstLine,node_width);
        srcLine+=node_width;
    }
}




bool clipDraw(const long dstWidth,const long dstHeight,long& dstX0,long& dstY0,
              const long srcWidth,const long srcHeight,long& srcX0,long& srcY0,long& srcX1,long& srcY1){
    if ((srcX0<0)||(srcY0<0)||(srcX1>srcWidth)||(srcY1>srcHeight)) return false; //ERROR!
    
    if (dstX0<0){
        srcX0-=dstX0;
        dstX0=0;
    }
    if (dstY0<0){
        srcY0-=dstY0;
        dstY0=0;
    }
    if (dstX0+(srcX1-srcX0)>=dstWidth)
        srcX1=dstWidth-dstX0+srcX0;
    if (srcX0>=srcX1) return false;
    
    if (dstY0+(srcY1-srcY0)>=dstHeight)
        srcY1=dstHeight-dstY0+srcY0;	
    if (srcY0>=srcY1) return false;
    
    return true;
}



////////////////////////////

#ifdef MMX_ACTIVE
#pragma warning(disable:4799)

must_inline void zoomTo1_op_begin(){
    asm{
        PXOR      MM7,MM7
    }
}
must_inline void zoomTo1_op_end(){
    do_emms();
}

must_inline UInt32 zoom2to1_op(const UInt32* srcLine0,const UInt32* srcLine1){
    UInt32 result;
    asm{
        mov       eax,srcLine0
        mov       edx,srcLine1
        movq      MM0,[eax]
        movq      MM1,[edx]
        movq      mm2,mm0
        movq      mm3,mm1
        PUNPCKLBW MM0,MM7
        PUNPCKLBW MM1,MM7
        PUNPCKhBW MM2,MM7
        PUNPCKhBW MM3,MM7
        
        PADDUSW   MM0,MM1
        PADDUSW   MM2,MM3
        PADDUSW   MM0,MM2
        
        PSRAW     MM0,2   //div 4
        PACKUSWB  MM0,MM7
        MOVD      result,MM0
    }
    return result;
}

must_inline UInt32 zoom2Yto1_op(const UInt32* srcLine0,const UInt32* srcLine1){
    UInt32 result;
    asm{
        mov       eax,srcLine0
        mov       edx,srcLine1
        movd      MM0,[eax]
        movd      MM1,[edx]
        PUNPCKLBW MM0,MM7
        PUNPCKLBW MM1,MM7
        
        PADDUSW   MM0,MM1
        
        PSRAW     MM0,1   //div 2
        PACKUSWB  MM0,MM7
        MOVD      result,MM0
    }
    return result;
}

must_inline UInt32 zoom2Xto1_op(const UInt32* srcLine){
    UInt32 result;
    asm{
        mov       eax,srcLine               
        movq      MM0,[eax]
        movq      mm2,mm0
        PUNPCKLBW MM0,MM7
        PUNPCKhBW MM2,MM7
        
        PADDUSW   MM0,MM2
        
        PSRAW     MM0,1   //div 2
        PACKUSWB  MM0,MM7
        MOVD      result,MM0
    }
    return result;
}

#pragma warning(default:4799)
#else

must_inline static void zoomTo1_op_begin(){
}
must_inline static void zoomTo1_op_end(){
}

must_inline static UInt32 zoom2to1_op(const UInt32* srcLine0,const UInt32* srcLine1){
    UInt32 color=srcLine0[0];
    UInt32 AG =((color & 0xFF00FF00)>>2);
    UInt32 RB =(color & 0x00FF00FF);
    color=srcLine0[1];
    AG+=((color & 0xFF00FF00)>>2);
    RB+=(color & 0x00FF00FF);           
    color=srcLine1[0];
    AG+=((color & 0xFF00FF00)>>2);
    RB+=(color & 0x00FF00FF);           
    color=srcLine1[1];
    AG+=((color & 0xFF00FF00)>>2);
    RB+=(color & 0x00FF00FF);
    return ((AG & 0xFF00FF00)) | ((RB>>2) & 0x00FF00FF);
}
must_inline static UInt32 zoom2to1_op(UInt32 color0,UInt32 color1){
    UInt32 AG =((color0 & 0xFF00FF00)>>1);
    UInt32 RB =(color0 & 0x00FF00FF);
    AG+=((color1 & 0xFF00FF00)>>1);
    RB+=(color1 & 0x00FF00FF);
    return ((AG & 0xFF00FF00)) | ((RB>>1) & 0x00FF00FF);
}

must_inline static UInt32 zoom2Yto1_op(const UInt32* srcLine0,const UInt32* srcLine1){
    return zoom2to1_op(srcLine0[0],srcLine1[0]);
}

must_inline static UInt32 zoom2Xto1_op(const UInt32* srcLine){
    return zoom2to1_op(srcLine[0],srcLine[1]);
}
#endif

must_inline static UInt8 zoom2to1_op(const UInt8* srcLine0,const UInt8* srcLine1){
    return (srcLine0[0]+srcLine0[1]+srcLine1[0]+srcLine1[1])>>2;
}

must_inline static UInt8 zoom2Yto1_op(const UInt8* srcLine0,const UInt8* srcLine1){
    return (srcLine0[0]+srcLine1[0])>>1;
}

must_inline static UInt8 zoom2Xto1_op(const UInt8* srcLine){
    return (srcLine[0]+srcLine[1])>>1;
}

template<class TColorData>
inline void zoom2to1_line(TColorData* dstLine,const TColorData* srcLine0,const TColorData* srcLine1,long width){
    for (long x=0;x<width;++x){
        dstLine[x]=zoom2to1_op(&(srcLine0[x*2]),&(srcLine1[x*2]));
    }
}

template<class TColorData>
inline void zoom2Yto1_line(TColorData* dstLine,const TColorData* srcLine0,const TColorData* srcLine1,long width){
    for (long x=0;x<width;++x){
        dstLine[x]=zoom2Yto1_op(&srcLine0[x],&srcLine1[x]);
    }
}

template<class TColorData>
inline void zoom2Xto1_line(TColorData* dstLine,const TColorData* srcLine,long width){
    for (long x=0;x<width;++x){
        dstLine[x]=zoom2Xto1_op(&srcLine[x*2]);
    }
}

must_inline static void halfAlpha(UInt32& c){
    Color32& cl=(Color32&)c;
    cl.a=(cl.a>>1);
}
must_inline static void halfAlpha(Gray8& c){
}

template<class TPixelsRef,class TColorData>
void tm_zoom2to1(const TPixelsRef& dst,const TPixelsRef& src){
    assert(dst.width==(src.width+1)/2);
    assert(dst.height==(src.height+1)/2);
    typedef typename TPixelsRef::TColor TColor;
    
    zoomTo1_op_begin();
    TColorData* dstLine=(TColorData*)dst.pdata;
    const TColorData* srcLine0=(const TColorData*)src.pdata;
    const TColorData* srcLine1=(const TColorData*)srcLine0; 
    src.nextLine((TColor*&)srcLine1);
    long fast_y=src.height>>1;
    long fast_x=src.width>>1;
    long y=0;
    for (;(y<fast_y);++y){
        zoom2to1_line(dstLine,srcLine0,srcLine1,fast_x);
        if (fast_x*2!=src.width){ 
            dstLine[fast_x]=zoom2Yto1_op(&srcLine0[fast_x*2],&srcLine1[fast_x*2]);
            halfAlpha(dstLine[fast_x]);
        }
        dst.nextLine((TColor*&)dstLine);
        src.nextLine((TColor*&)srcLine0,2);
        src.nextLine((TColor*&)srcLine1,2);
    }
    if (fast_y*2!=src.height){
        zoom2Xto1_line(dstLine,srcLine0,fast_x);
        for (long x=0;x<fast_x;++x)
            halfAlpha(dstLine[x]);
        if (fast_x*2!=src.width){ 
            dstLine[fast_x]=srcLine0[fast_x*2];
            halfAlpha(dstLine[fast_x]);
            halfAlpha(dstLine[fast_x]);
        }
    }
    zoomTo1_op_end();
}

template<class TPixelsRef,class TColorData>
void tm_zoom2Yto1(const TPixelsRef& dst,const TPixelsRef& src){
    assert(dst.width==src.width);
    assert(dst.height*2==src.height);
    typedef typename TPixelsRef::TColor TColor;
    
    zoomTo1_op_begin();
    TColorData* dstLine=(TColorData*)dst.pdata;
    const TColorData* srcLine0=(const TColorData*)src.pdata;
    const TColorData* srcLine1=(const TColorData*)srcLine0; 
    src.nextLine((TColor*&)srcLine1);
    for (long y=0;y<dst.height;++y){
        zoom2Yto1_line(dstLine,srcLine0,srcLine1,dst.width);
        dst.nextLine((TColor*&)dstLine);
        src.nextLine((TColor*&)srcLine0,2);
        src.nextLine((TColor*&)srcLine1,2);
    }
    zoomTo1_op_end();
}

template<class TPixelsRef,class TColorData>
void tm_zoom2Xto1(const TPixelsRef& dst,const TPixelsRef& src){
    assert(dst.width*2==src.width);
    assert(dst.height==src.height);
    typedef typename TPixelsRef::TColor TColor;
    
    zoomTo1_op_begin();
    TColorData* dstLine=(TColorData*)dst.pdata;
    const TColorData* srcLine=(const TColorData*)src.pdata;
    for (long y=0;y<dst.height;++y){
        zoom2Xto1_line(dstLine,srcLine,dst.width);
        dst.nextLine((TColor*&)dstLine);
        src.nextLine((TColor*&)srcLine);
    }
    zoomTo1_op_end();
}


template<class TPixelsRef,class TColorData,class TPixelsBuf>
void tm_mipZoomBilinear(const TPixelsRef& dst,const TPixelsRef& src){
    if ((src.width>=dst.width*2)||(src.height>=dst.height*2)){
        long mwidth=(src.width+1)>>1;
        long mheight=(src.height+1)>>1;
        if (mwidth<dst.width) mwidth=dst.width;
        if (mheight<dst.height) mheight=dst.height;
        bool isUseMipBuf=(mwidth!=dst.width)||(mheight!=dst.height);
        
        TPixelsRef mRef(dst);
        TPixelsBuf mBuf;
        if (isUseMipBuf){
            mBuf.resizeFast(mwidth,mheight);
            mRef=mBuf.getRef();
        }
        
        if ((mwidth*2==src.width)&&(mheight*2==src.height))
            _zoom2To1(mRef,src);
        else if ((mwidth*2==src.width)&&(mheight==src.height))
            _zoom2XTo1(mRef,src);
        else if ((mwidth==src.width)&&(mheight*2==src.height))
            _zoom2YTo1(mRef,src);
        else
            zoomBilinearAsCopy(mRef,src);
        
        if (isUseMipBuf)
            _mipZoomBilinear(dst,mRef);
    }else{
        zoomBilinearAsCopy(dst,src);
    }
}

#include "hPixels32.h"

static void _zoom2To1(const Pixels32Ref& dst,const Pixels32Ref& src){
    tm_zoom2to1<Pixels32Ref,UInt32>(dst,src);
}

static void _zoom2YTo1(const Pixels32Ref& dst,const Pixels32Ref& src){
    tm_zoom2Yto1<Pixels32Ref,UInt32>(dst,src);
}

static void _zoom2XTo1(const Pixels32Ref& dst,const Pixels32Ref& src){
    tm_zoom2Xto1<Pixels32Ref,UInt32>(dst,src);
}

static void _mipZoomBilinear(const Pixels32Ref& dst,const Pixels32Ref& src){
    tm_mipZoomBilinear<Pixels32Ref,UInt32,Pixels32>(dst,src);
}

static void _zoom2To1(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    tm_zoom2to1<PixelsGray8Ref,UInt8>(dst,src);
}

static void _zoom2YTo1(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    tm_zoom2Yto1<PixelsGray8Ref,UInt8>(dst,src);
}

static void _zoom2XTo1(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    tm_zoom2Xto1<PixelsGray8Ref,UInt8>(dst,src);
}

static void _mipZoomBilinear(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    tm_mipZoomBilinear<PixelsGray8Ref,UInt8,PixelsGray8>(dst,src);
}

void zoom2To1(const Pixels32Ref& dst,const Pixels32Ref& src){
    _zoom2To1(dst,src);
}

void zoom2YTo1(const Pixels32Ref& dst,const Pixels32Ref& src){
    _zoom2YTo1(dst,src);
}

void zoom2XTo1(const Pixels32Ref& dst,const Pixels32Ref& src){
    _zoom2XTo1(dst,src);
}

void mipZoomBilinear(const Pixels32Ref& dst,const Pixels32Ref& src){
    _mipZoomBilinear(dst,src);
}

void zoom2To1Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    _zoom2To1(dst,src);
}

void zoom2YTo1Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    _zoom2YTo1(dst,src);
}

void zoom2XTo1Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    _zoom2XTo1(dst,src);
}

void mipZoomBilinearGray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    _mipZoomBilinear(dst,src);
}

void zoom1To1(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    copy(dstRef,0,0,srcRef);
}
void zoom1To1Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef){
    copy(dstRef,0,0,srcRef);
}

must_inline static void Bilinear_Fast_Common(Gray8* PColor0,Gray8* PColor1,unsigned long u_8,unsigned long v_8,Gray8* result)
{
    unsigned long pm3_8=(u_8*v_8)>>8;
    unsigned long pm2_8=u_8-pm3_8;
    unsigned long pm1_8=v_8-pm3_8;
    unsigned long pm0_8=256-pm1_8-pm2_8-pm3_8;
    *result=(Gray8)( ( PColor0[0]*pm0_8 + PColor0[1]*pm2_8 + PColor1[0]*pm1_8 + PColor1[1]*pm3_8 ) >>8 );
}
void zoomBilinearAsCopy(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    //警告 一个有误差的实现 
    if (  (0==dst.width)||(0==dst.height)
        ||(2>src.width)||(2>src.height)) return;
    
    long xrIntFloat_16=((src.width-1)<<16)/dst.width; 
    long yrIntFloat_16=((src.height-1)<<16)/dst.height;
    
    unsigned long dst_width=dst.width;
    long Src_byte_width=src.byte_width;
    Gray8* pDstLine=dst.pdata;
    long srcy_16=0;
    for (unsigned long y=0;(long)y<dst.height;++y)
    {
        unsigned long v_8=(srcy_16 & 0xFFFF)>>8;
        Gray8* PSrcLineColor= (Gray8*)((UInt8*)(src.pdata)+Src_byte_width*(srcy_16>>16)) ;
        long srcx_16=0;
        for (unsigned long x=0;x<dst_width;++x)
        {
            Gray8* PColor0=&PSrcLineColor[srcx_16>>16];
            Bilinear_Fast_Common(PColor0,(Gray8*)((UInt8*)(PColor0)+Src_byte_width),(srcx_16 & 0xFFFF)>>8,v_8,&pDstLine[x]);
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((UInt8*&)pDstLine)+=dst.byte_width;
    }
}


//+-90度旋转的实现
//优化写方向
template<long MaskNoEmpty>
void tm_copy_rotary90or270_opWrite(UInt32* dstLine,long width,long height,long dstLineInc,
                                   const UInt32* srcLine,long srcLineInc){
    long fastWidth=width>>2<<2;
    for (long y=0;y<height;++y){
        const UInt32* src=srcLine;
        for (long x=0;x<fastWidth;x+=4){ //循环展开4次
            dstLine[x  ]=(*src) | MaskNoEmpty; ((UInt8*&)src)+=srcLineInc;
            dstLine[x+1]=(*src) | MaskNoEmpty; ((UInt8*&)src)+=srcLineInc;
            dstLine[x+2]=(*src) | MaskNoEmpty; ((UInt8*&)src)+=srcLineInc;
            dstLine[x+3]=(*src) | MaskNoEmpty; ((UInt8*&)src)+=srcLineInc;
        }
        for (long x=fastWidth;x<width;++x){
            dstLine[x  ]=(*src) | MaskNoEmpty; ((UInt8*&)src)+=srcLineInc;
        }
        ++srcLine;
        ((UInt8*&)dstLine)+=dstLineInc;
    }
}

inline void copy_rotary90or270_opWrite(UInt32* dstLine,long width,long height,long dstLineInc,
                                       const UInt32* srcLine,long srcLineInc,bool isMaskNoEmpty){
    if (isMaskNoEmpty)
        tm_copy_rotary90or270_opWrite<1>(dstLine,width,height,dstLineInc,srcLine,srcLineInc);
    else
        tm_copy_rotary90or270_opWrite<0>(dstLine,width,height,dstLineInc,srcLine,srcLineInc);
}

//+-90度旋转的实现
//优化读方向
template<long MaskNoEmpty>
void tm_copy_rotary90or270_opRead(UInt32* dstLine,long width,long height,long dstLineInc,
                                  const UInt32* srcLine,long srcLineInc){
    long fastHeight=height>>2<<2;
    for (long x=0;x<width;++x){
        UInt32* dst=dstLine;
        for (long y=0;y<fastHeight;y+=4){ //循环展开4次
            *dst=srcLine[y  ] | MaskNoEmpty; ((UInt8*&)dst)+=dstLineInc;
            *dst=srcLine[y+1] | MaskNoEmpty; ((UInt8*&)dst)+=dstLineInc;
            *dst=srcLine[y+2] | MaskNoEmpty; ((UInt8*&)dst)+=dstLineInc;
            *dst=srcLine[y+3] | MaskNoEmpty; ((UInt8*&)dst)+=dstLineInc;
        }
        for (long y=fastHeight;y<height;++y){
            *dst=srcLine[y  ] | MaskNoEmpty; ((UInt8*&)dst)+=dstLineInc;
        }
        ((UInt8*&)srcLine)+=srcLineInc;
        ++dstLine;
    }
}

inline void copy_rotary90or270_opRead(UInt32* dstLine,long width,long height,long dstLineInc,
                                      const UInt32* srcLine,long srcLineInc,bool isMaskNoEmpty){
    if (isMaskNoEmpty)
        tm_copy_rotary90or270_opRead<1>(dstLine,width,height,dstLineInc,srcLine,srcLineInc);
    else
        tm_copy_rotary90or270_opRead<0>(dstLine,width,height,dstLineInc,srcLine,srcLineInc);
}

//copy_rotary90
void copy_rotary90(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    long width=STD::min(dstRef.width,srcRef.height);
    long height=STD::min(dstRef.height,srcRef.width);
    const UInt32* srcLine=(const UInt32*)(((UInt8*)srcRef.pdata)+(srcRef.height-1)*srcRef.byte_width);
    copy_rotary90or270_opWrite((UInt32*)dstRef.pdata,width,height,dstRef.byte_width,srcLine,-srcRef.byte_width,isMaskNoEmpty);
    //copy_rotary90or270_opRead((UInt32*)dstRef.pdata,width,height,dstRef.byte_width,srcLine,-srcRef.byte_width,isMaskNoEmpty);
}


//copy_rotary270
void copy_rotary270(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    long width=STD::min(dstRef.width,srcRef.height);
    long height=STD::min(dstRef.height,srcRef.width);
    UInt32* dstLine=(UInt32*)( ((UInt8*)(dstRef.pdata)) + (height-1)*dstRef.byte_width);
    copy_rotary90or270_opWrite(dstLine,width,height,-dstRef.byte_width,(const UInt32*)srcRef.pdata,srcRef.byte_width,isMaskNoEmpty);
    //copy_rotary90or270_opRead(dstLine,width,height,-dstRef.byte_width,(const UInt32*)srcRef.pdata,srcRef.byte_width,isMaskNoEmpty);
}


///////////////////////////////////////////////////////////////////////////////////////

template<long MaskNoEmpty>
void tm_zoom1To1_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    long width=dstRef.width; 
    if (width>srcRef.width) width=srcRef.width;
    long height=dstRef.height; 
    if (height>srcRef.height) height=srcRef.height;
    const UInt32* sline=(const UInt32*)srcRef.pdata;
    UInt32* dline=(UInt32*)dstRef.pdata;
    long fastWidth=width>>2<<2;
    
    for (long y=0;y<height;++y){
        for (long x=0;x<fastWidth;x+=4){
            dline[x  ]=sline[x  ] | MaskNoEmpty;
            dline[x+1]=sline[x+1] | MaskNoEmpty;
            dline[x+2]=sline[x+2] | MaskNoEmpty;
            dline[x+3]=sline[x+3] | MaskNoEmpty;
        }
        for (long x=fastWidth;x<width;++x){
            dline[x  ]=sline[x  ] | MaskNoEmpty;
        }
        (UInt8*&)sline+=srcRef.byte_width;
        (UInt8*&)dline+=dstRef.byte_width;
    }
}
void zoom1To1_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    if (isMaskNoEmpty)
        tm_zoom1To1_fast<1>(dstRef,srcRef);
    else
        tm_zoom1To1_fast<0>(dstRef,srcRef);
}

void zoom2To1_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    TFastZoomKernel_2To1 op;
    tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_2To1,2,1,0>(dstRef,srcRef,op);
}

void zoom1To2_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    TFastZoomKernel_1To2 op;
    tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_1To2,1,2,0>(dstRef,srcRef,op);
}

void zoom1To2(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    TFastZoomKernel_1To2 op;
    tm_fastZoom<false,Pixels32Ref,TFastZoomKernel_1To2,1,2,0>(dstRef,srcRef,op);
}

void zoom3To2Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef){
    assert(false);
}
void zoom4To3Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef){
    assert(false);
}
void zoom3To4Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef){
}
void zoom2To3Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef){
    assert(false);
}

void zoom1To2Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src){
    assert(false);
}


void zoom3To2_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    TFastZoomKernel_3To2 op;
    tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_3To2,3,2,0>(dstRef,srcRef,op);
}

void zoom3To2(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    TFastZoomKernel_3To2 op;
    tm_fastZoom<false,Pixels32Ref,TFastZoomKernel_3To2,3,2,0>(dstRef,srcRef,op);
}


void zoom2To3_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    if (isMaskNoEmpty){
        TFastZoomKernel_2To3 op;
        tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_2To3,2,3,1>(dstRef,srcRef,op);
    }else{
        TFastZoomKernel_2To3 op;
        tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_2To3,2,3,0>(dstRef,srcRef,op);
    }
}

void zoom2To3(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    TFastZoomKernel_2To3 op;
    tm_fastZoom<false,Pixels32Ref,TFastZoomKernel_2To3,2,3,0>(dstRef,srcRef,op);
}

void zoom4To3_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    TFastZoomKernel_4To3 op;
    tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_4To3,4,3,0>(dstRef,srcRef,op);
}

void zoom4To3(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    TFastZoomKernel_4To3 op;
    tm_fastZoom<false,Pixels32Ref,TFastZoomKernel_4To3,4,3,0>(dstRef,srcRef,op);
}

void zoom3To4_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty){
    TFastZoomKernel_3To4 op;
    tm_fastZoom<true,Pixels32Ref,TFastZoomKernel_3To4,3,4,0>(dstRef,srcRef,op);
}

void zoom3To4(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    TFastZoomKernel_3To4 op;
    tm_fastZoom<false,Pixels32Ref,TFastZoomKernel_3To4,3,4,0>(dstRef,srcRef,op);
}

//-------------------------------------------------------------------------------------
template<bool isPack,int rbit,int gbit,int bbit>
must_inline UInt32 color32bit_to_16bit_fast(UInt32 c){
    const long bMask=((1<<bbit)-1)<<( 8-bbit);
    const long gMask=((1<<gbit)-1)<<(16-gbit);
    const long rMask=((1<<rbit)-1)<<(24-rbit);
    const long argbMask=bMask|gMask|rMask| (0xFF<<24);
    if (isPack)
        return ( ((c&bMask)>>(8-bbit))|((c&gMask)>>(16-bbit-gbit))|((c&rMask)>>(24-bbit-gbit-rbit)) );
    else
        return ( c&argbMask );
}

template<bool isPack,int rbit,int gbit,int bbit>
must_inline UInt32 color32bit_to_16bit_fast(long r,long g,long b,long a){
    const long bMask=((1<<bbit)-1)<<( 8-bbit);
    const long gMask=((1<<gbit)-1)<<(16-gbit);
    const long rMask=((1<<rbit)-1)<<(24-rbit);
    const long argbMask=bMask|gMask|rMask| (0xFF<<24);
    if (isPack)
        return (b>>(8-bbit)) | ((g>>(8-gbit))<<bbit) | ((r>>(8-rbit))<<(bbit+gbit)) ;
    else
        return ( b | (g<<8) | (r<<16) | (a<<24) ) & argbMask;
}

struct TErrorColor{
    long dR;
    long dG;
    long dB;
};

template<int CBit>
must_inline long getBestRGB16_Color(const long wantColor){
    const long rMax=((1<<CBit)-1);
    if (wantColor<=0) 
        return 0;
    else if (wantColor>=(rMax<<(8-CBit)))
        return rMax;
    else
        return wantColor>>(8-CBit);
}
template<int CBit>
must_inline long getC8ErrorColor(const long rColor){
    const long rMax=((1<<CBit)-1);
    const long rcMul=(((1<<16)*255+rMax-1)/rMax);
    return ((rColor*rcMul)>>16);
}

static UInt8 _BestRGB16_Color_Table5[256*5]={
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 
    0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 
    0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 
    0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F 
};
static UInt8 _BestRGB16_Color_Table6[256*5]={
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 
    0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 
    0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 
    0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19, 0x19, 0x1A, 0x1A, 0x1A, 0x1A, 0x1B, 0x1B, 0x1B, 0x1B, 
    0x1C, 0x1C, 0x1C, 0x1C, 0x1D, 0x1D, 0x1D, 0x1D, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 
    0x20, 0x20, 0x20, 0x20, 0x21, 0x21, 0x21, 0x21, 0x22, 0x22, 0x22, 0x22, 0x23, 0x23, 0x23, 0x23, 
    0x24, 0x24, 0x24, 0x24, 0x25, 0x25, 0x25, 0x25, 0x26, 0x26, 0x26, 0x26, 0x27, 0x27, 0x27, 0x27, 
    0x28, 0x28, 0x28, 0x28, 0x29, 0x29, 0x29, 0x29, 0x2A, 0x2A, 0x2A, 0x2A, 0x2B, 0x2B, 0x2B, 0x2B, 
    0x2C, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x2F, 
    0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 
    0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 
    0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 
    0x3C, 0x3C, 0x3C, 0x3C, 0x3D, 0x3D, 0x3D, 0x3D, 0x3E, 0x3E, 0x3E, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
};
const  UInt8* BestRGB16_Color_Table5=&_BestRGB16_Color_Table5[256*2];
const  UInt8* BestRGB16_Color_Table6=&_BestRGB16_Color_Table6[256*2];
/*struct _TAutoInit_BestRGB16_Color_Table{
 _TAutoInit_BestRGB16_Color_Table(){
 for (long i=0;i<256*5;++i){
 _BestRGB16_Color_Table5[i]=(UInt8)getBestRGB16_Color<5>(i-256*2);
 _BestRGB16_Color_Table6[i]=(UInt8)getBestRGB16_Color<6>(i-256*2);
 }
 }
 };
 static _TAutoInit_BestRGB16_Color_Table _AutoInit_BestRGB16_Color_Table;*/

template<int cbit>
must_inline UInt8 BestRGB16_Color_Table(long c){
    return getBestRGB16_Color<cbit>(c);
}
template<>
must_inline UInt8 BestRGB16_Color_Table<5>(long c){
    return BestRGB16_Color_Table5[c];
}
template<>
must_inline UInt8 BestRGB16_Color_Table<6>(long c){
    return BestRGB16_Color_Table6[c];
}


/*
 template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
 void tm_copy_32bit_to_16bit_ErrorDiffuse_line(TDstColor16* pDst,const UInt8* pSrc,long width,TErrorColor* PHLineErr){
 TErrorColor HErr;
 HErr.dR=0; HErr.dG=0; HErr.dB=0;
 PHLineErr[-1].dB=0; PHLineErr[-1].dG=0; PHLineErr[-1].dR=0; 
 for (long x=0;x<width;++x) {
 long cB=pSrc[x*4+0]+HErr.dB+(PHLineErr[x].dB>>1);
 long cG=pSrc[x*4+1]+HErr.dG+(PHLineErr[x].dG>>1);
 long cR=pSrc[x*4+2]+HErr.dR+(PHLineErr[x].dR>>1);
 long rB=BestRGB16_Color_Table<bbit>(cB);//getBestRGB16_Color<bbit>(cB);
 long rG=BestRGB16_Color_Table<gbit>(cG);//getBestRGB16_Color<gbit>(cG);
 long rR=BestRGB16_Color_Table<rbit>(cR);//getBestRGB16_Color<rbit>(cR);
 if (isPack)
 pDst[x]= (TDstColor16)( rB|(rG<<bbit)|(rR<<(bbit+gbit)) );
 else
 pDst[x]= (TDstColor16)( (rB<<(8-bbit))|(rG<<(16-gbit))|(rR<<(24-rbit))|(pSrc[x*4+3]<<24) );
 
 HErr.dB=(cB-getC8ErrorColor<bbit>(rB))>>1;
 HErr.dG=(cG-getC8ErrorColor<gbit>(rG))>>1;
 HErr.dR=(cR-getC8ErrorColor<rbit>(rR))>>1;
 PHLineErr[x-1].dB+=HErr.dB;
 PHLineErr[x-1].dG+=HErr.dG;
 PHLineErr[x-1].dR+=HErr.dR;
 PHLineErr[x]=HErr;
 }
 }
 
 
 template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
 void tm_copy_32bit_to_16bit_ErrorDiffuse(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
 long width=dstRef.width; if (width>srcRef.width) width=srcRef.width;
 long height=dstRef.height; if (height>srcRef.height) height=srcRef.height;
 const UInt8* sline=(const UInt8*)srcRef.pdata;
 TDstColor16* dline=(TDstColor16*)dstRef.pdata;
 
 TErrorColor* _HLineErr=new TErrorColor[width+2]; 
 for (long x=0;x<width+2;++x){
 _HLineErr[x].dR=0;
 _HLineErr[x].dG=0;
 _HLineErr[x].dB=0;
 }
 TErrorColor* HLineErr=&_HLineErr[1];
 
 for (long y=0;y<height;++y){
 tm_copy_32bit_to_16bit_ErrorDiffuse_line<TDstColor16,isPack,rbit,gbit,bbit>(dline,sline,width,HLineErr);
 (UInt8*&)sline+=srcRef.byte_width;
 (UInt8*&)dline+=dstRef.byte_width;
 }
 delete[]_HLineErr;
 }*/

template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
void tm_copy_32bit_to_16bit_ErrorDiffuse_line(TDstColor16* dst,const UInt8* src,long width){
    long erR=0;
    long erG=0;
    long erB=0;
    for (long x=0;x<width;++x) {
        erB+=src[x*4+0];
        erG+=src[x*4+1];
        erR+=src[x*4+2];
        long rB=BestRGB16_Color_Table<bbit>(erB);
        long rG=BestRGB16_Color_Table<gbit>(erG);
        long rR=BestRGB16_Color_Table<rbit>(erR);
        if (isPack)
            dst[x]= (TDstColor16)( rB|(rG<<bbit)|(rR<<(bbit+gbit)) );
        else
            dst[x]= (TDstColor16)( (rB<<(8-bbit))|(rG<<(16-gbit))|(rR<<(24-rbit))|(src[x*4+3]<<24) );
        erB-=getC8ErrorColor<bbit>(rB);
        erG-=getC8ErrorColor<gbit>(rG);
        erR-=getC8ErrorColor<rbit>(rR);
    }
}


template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
void tm_copy_32bit_to_16bit_ErrorDiffuse(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    long width=dstRef.width; if (width>srcRef.width) width=srcRef.width;
    if (width<=0) return;
    long height=dstRef.height; if (height>srcRef.height) height=srcRef.height;
    const UInt8* sline=(const UInt8*)srcRef.pdata;
    TDstColor16* dline=(TDstColor16*)dstRef.pdata;
    
    for (long y=0;y<height;++y){
        tm_copy_32bit_to_16bit_ErrorDiffuse_line<TDstColor16,isPack,rbit,gbit,bbit>(dline,sline,width);
        (UInt8*&)sline+=srcRef.byte_width;
        (UInt8*&)dline+=dstRef.byte_width;
    }
}

void copy_32bit_to_16bit555_ErrorDiffuse(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32){
    if (dstBit==16){
        assert(isPack);
        tm_copy_32bit_to_16bit_ErrorDiffuse<UInt16,true,5,5,5>(dstRef,srcRef32);
    } else if (dstBit==32){
        if (isPack)
            tm_copy_32bit_to_16bit_ErrorDiffuse<unsigned long,true,5,5,5>(dstRef,srcRef32);
        else
            tm_copy_32bit_to_16bit_ErrorDiffuse<unsigned long,false,5,5,5>(dstRef,srcRef32);
    }else
        assert(false);
}

void copy_32bit_to_16bit565_ErrorDiffuse(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32){
    if (dstBit==16){
        assert(isPack);
        tm_copy_32bit_to_16bit_ErrorDiffuse<UInt16,true,5,6,5>(dstRef,srcRef32);
    } else if (dstBit==32){
        if (isPack)
            tm_copy_32bit_to_16bit_ErrorDiffuse<unsigned long,true,5,6,5>(dstRef,srcRef32);
        else
            tm_copy_32bit_to_16bit_ErrorDiffuse<unsigned long,false,5,6,5>(dstRef,srcRef32);
    }else
        assert(false);
}


//////////

template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
void tm_copy_32bit_to_16bit_fast_line(TDstColor16* dst,const UInt32* src,long width){
    TDstColor16* dstEnd =&dst[width];
    TDstColor16* dstEndFast =&dst[(width & (~3))];
    for (; dst != dstEndFast; dst+=4,src+=4){
        dst[0]=(TDstColor16)color32bit_to_16bit_fast<isPack,rbit,gbit,bbit>(src[0]);
        dst[1]=(TDstColor16)color32bit_to_16bit_fast<isPack,rbit,gbit,bbit>(src[1]);
        dst[2]=(TDstColor16)color32bit_to_16bit_fast<isPack,rbit,gbit,bbit>(src[2]);
        dst[3]=(TDstColor16)color32bit_to_16bit_fast<isPack,rbit,gbit,bbit>(src[3]);
    }
    for (; dst != dstEnd; ++dst,++src)
        *dst=(TDstColor16)color32bit_to_16bit_fast<isPack,rbit,gbit,bbit>(*src);
}

template<class TDstColor16,bool isPack,int rbit,int gbit,int bbit>
void tm_copy_32bit_to_16bit_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef){
    long width=dstRef.width; if (width>srcRef.width) width=srcRef.width;
    long height=dstRef.height; if (height>srcRef.height) height=srcRef.height;
    const UInt32* sline=(const UInt32*)srcRef.pdata;
    TDstColor16* dline=(TDstColor16*)dstRef.pdata;
    
    for (long y=0;y<height;++y){
        tm_copy_32bit_to_16bit_fast_line<TDstColor16,isPack,rbit,gbit,bbit>(dline,sline,width);
        (UInt8*&)sline+=srcRef.byte_width;
        (UInt8*&)dline+=dstRef.byte_width;
    }
}

void copy_32bit_to_16bit555_fast(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32){
    if (dstBit==16){
        assert(isPack);
        tm_copy_32bit_to_16bit_fast<UInt16,true,5,5,5>(dstRef,srcRef32);
    } else if (dstBit==32){
        if (isPack)
            tm_copy_32bit_to_16bit_fast<unsigned long,true,5,5,5>(dstRef,srcRef32);
        else
            tm_copy_32bit_to_16bit_fast<unsigned long,false,5,5,5>(dstRef,srcRef32);
    }else
        assert(false);
}

void copy_32bit_to_16bit565_fast(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32){
    if (dstBit==16){
        assert(isPack);
        tm_copy_32bit_to_16bit_fast<UInt16,true,5,6,5>(dstRef,srcRef32);
    } else if (dstBit==32){
        if (isPack)
            tm_copy_32bit_to_16bit_fast<unsigned long,true,5,6,5>(dstRef,srcRef32);
        else
            tm_copy_32bit_to_16bit_fast<unsigned long,false,5,6,5>(dstRef,srcRef32);
    }else
        assert(false);
}

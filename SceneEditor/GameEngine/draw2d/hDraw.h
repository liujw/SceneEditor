//hDraw.h

#ifndef _hDraw_h_
#define _hDraw_h_

#include "hColor32.h"
#include "hBlendAPixel.h"

// 填充颜色
void fill(const Pixels32Ref& dst,const Color32& color);
void fill(const PixelsGray8Ref& dst,const Gray8& color);
// Alpha混合填充
void fillBlend(const Pixels32Ref& dst,const Color32& color);
//着色
void fillColoring(const Pixels32Ref& dst,const Color32& color);
// 颜色加 
void fillAdd(const Pixels32Ref& dst,const Color32& color);

// 拷贝
void copyColoring(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic,const Color32&  color,long alpha);
inline void copy(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic) { copyColoring(dst,dstX0,dstY0,srcPic,cl32ColoringMullEmpty,255); }
inline void copy(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic,long alpha) { copyColoring(dst,dstX0,dstY0,srcPic,cl32ColoringMullEmpty,alpha);	}
inline void copyColoring(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic, const Color32&  color) { copyColoring(dst,dstX0,dstY0,srcPic,color,255); }
void copy(const PixelsGray8Ref& dst,long dstX0, long dstY0, const PixelsGray8Ref& srcPic);

// 灰度
void copyAsGray(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic,long alpha);
inline void copyAsGray(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic) { copyAsGray(dst,dstX0,dstY0,srcPic,255); }
void blendAsGray(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic,long alpha);
inline void blendAsGray(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic) { blendAsGray(dst,dstX0,dstY0,srcPic,255); }

// Alpha颜色混合
void blendColoring(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic,const Color32&  color,long alpha);
inline void blend(const Pixels32Ref& dst,long dstX0, long dstY0,const Pixels32Ref& srcPic) { blendColoring(dst,dstX0,dstY0,srcPic,cl32ColoringMullEmpty,255); }
inline void blend(const Pixels32Ref& dst,long dstX0, long dstY0,const Pixels32Ref& srcPic, long alpha) { blendColoring(dst,dstX0,dstY0,srcPic,cl32ColoringMullEmpty,alpha); }
inline void blendColoring(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic,const Color32&  color) { blendColoring(dst,dstX0,dstY0,srcPic,color,255); }

// 颜色加 用于光照等
void addColoring(const Pixels32Ref& dst,long dstX0, long dstY0,const Pixels32Ref& srcPic,const Color32& color);
inline void add(const Pixels32Ref& dst,long dstX0, long dstY0,const Pixels32Ref& srcPic) { addColoring(dst,dstX0,dstY0,srcPic,cl32ColoringMullEmpty);	}
void blendForAdd(const Pixels32Ref& dst);	


void blendForBuffer(const Pixels32Ref& dst,long dstX0, long dstY0, const Pixels32Ref& srcPic);

// 缩放 近邻取样插值
void zoomNearestNeighborAsCopy(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void zoomNearestNeighborAsBlend(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void zoomNearestNeighborAsAdd(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty);
// 缩放 二次线性插值 
void zoomBilinearAsCopy(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void zoomBilinearAsBlend(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void zoomBilinearAsAdd(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty);
void zoomBilinearAsCopy(const PixelsGray8Ref& dst,const PixelsGray8Ref& srcPic); //警告 一个有误差的实现
// 缩放 三次卷积插值 
void zoomBicubicAsCopy(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void zoomBicubicAsBlend(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void zoomBicubicAsAdd(const Pixels32Ref& dst,const Pixels32Ref& srcPic,const Color32&  color=cl32ColoringMullEmpty);

// 旋转 近邻取样插值
void rotaryNearestNeighborAsCopy(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void rotaryNearestNeighborAsBlend(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void rotaryNearestNeighborAsAdd(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty);
// 旋转 二次线性插值 
void rotaryBilinearAsCopy(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void rotaryBilinearAsBlend(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void rotaryBilinearAsBlendEx(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y, double rx0, double ry0, bool isDefRotaryCenter = true, const Color32& color=cl32ColoringMullEmpty, long alpha=255);
void rotaryBilinearAsAdd(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty);
// 旋转 三次卷积插值 
void rotaryBicubicAsCopy(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void rotaryBicubicAsBlend(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty,long alpha=255);
void rotaryBicubicAsAdd(const Pixels32Ref& Dst,const Pixels32Ref& Src,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double  move_y,const Color32&  color=cl32ColoringMullEmpty);

void copyToGray(const PixelsGray8Ref& dst,long x0, long y0,const Pixels32Ref& Src);
void copyFromGray(const Pixels32Ref& dst,long x0, long y0,const PixelsGray8Ref& Src);

void lineH(const Pixels32Ref& dst,long y, long x0, long x1, const Color32& color);
void lineV(const Pixels32Ref& dst,long x, long y0, long y1,  const Color32& color);
void rect(const Pixels32Ref& dst,long x0, long y0, long x1, long y1,  const Color32& color);

void copyAsTextPixels(const PixelsGray8Ref& dst,long x0, long y0,const Pixels32Ref& src);
void blendLight(const Pixels32Ref& dst,long dstX0, long dstY0,const PixelsGray8Ref& src,const Color32& color);
void addLight(const Pixels32Ref& dst,long dstX0, long dstY0,const PixelsGray8Ref& src,const Color32& color);

//void unionLayer(const Pixels32Ref& dst_backPic,long dstX0, long dstY0, const Pixels32Ref& frontPic);
//inline void unionLayer(const Pixels32Ref& dst_backPic, const Pixels32Ref& frontPic){ unionLayer(dst_backPic,0,0,frontPic); }

void darkle(const Pixels32Ref& dst,long delLight);

void blendWithYAlphas(const Pixels32Ref& dst,long x0,long y0,const Pixels32Ref& src,const UInt8* alphas);
void blendWithYColorings(const Pixels32Ref& dst,long x0,long y0,const Pixels32Ref& src,const Color32* colorings);



bool clipDraw(const long dstWidth,const long dstHeight,long& dstX0,long& dstY0,
              const long srcWidth,const long srcHeight,long& srcX0,long& srcY0,long& srcX1,long& srcY1);

// 缩放 二次线性插值 支持很大图片缩放到很小图片
typedef void (*TZoomCopyProc)(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef);
typedef void (*TZoomCopyGray8Proc)(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef);
void mipZoomBilinear(const Pixels32Ref& dst,const Pixels32Ref& src);
void zoom2To1(const Pixels32Ref& dst,const Pixels32Ref& src);
void zoom2YTo1(const Pixels32Ref& dst,const Pixels32Ref& src);
void zoom2XTo1(const Pixels32Ref& dst,const Pixels32Ref& src);
void mipZoomBilinearGray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src);
void zoom2To1Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src);
void zoom2YTo1Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src);
void zoom2XTo1Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src);

void zoom1To2(const Pixels32Ref& dst,const Pixels32Ref& src);
void zoom3To2(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef); 
void zoom4To3(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef); 
void zoom1To1(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef);
void zoom3To4(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef); 
void zoom2To3(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef);
void zoom1To2Gray8(const PixelsGray8Ref& dst,const PixelsGray8Ref& src);
void zoom3To2Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef); 
void zoom4To3Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef); 
void zoom1To1Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef);
void zoom3To4Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef); 
void zoom2To3Gray8(const PixelsGray8Ref& dstRef,const PixelsGray8Ref& srcRef);


//用于设备匹配分辨率显示
typedef void (*TZoomMapProc)(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty);
void zoom2To1_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false); //为了速度质量有降低 不保证alpha通道
void zoom3To2_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false); //为了速度质量有降低 不保证alpha通道
void zoom4To3_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false); //为了速度质量有降低 不保证alpha通道
void zoom1To1_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false);
void zoom3To4_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false); //为了速度质量有降低 不保证alpha通道
void zoom2To3_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false); //为了速度质量有降低 不保证alpha通道
void zoom1To2_fast(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false); //为了速度质量有降低 不保证alpha通道  警告：srcRef需要预留边框(右、下),否则内存访问越界 
//
void copy_32bit_to_16bit565_fast(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32);
void copy_32bit_to_16bit555_fast(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32);
//误差扩散
void copy_32bit_to_16bit565_ErrorDiffuse(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32);
void copy_32bit_to_16bit555_ErrorDiffuse(const Pixels32Ref& dstRef,long dstBit,bool isPack,const Pixels32Ref& srcRef32);

void copy_rotary90(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false);
void copy_rotary270(const Pixels32Ref& dstRef,const Pixels32Ref& srcRef,bool isMaskNoEmpty=false);

#endif //_hDraw_h_

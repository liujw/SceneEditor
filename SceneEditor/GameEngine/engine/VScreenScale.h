#ifndef _VScreenScale_h_
#define _VScreenScale_h_

#include "../../import/importInclude/SysImport.h"

enum TScreenScaleType{sl_auto,sl_2to1,sl_3to2,sl_4to3,sl_1to1,sl_3to4,sl_2to3,sl_1to2 };

enum TScreenColorType{ sc_Color32_BGRA,sc_Color16_565,sc_Color16_555 };

class VScreenScale;

//屏幕\游戏自动缩放的一些参数
extern VScreenScale* g_screenScale;

class VScreenScale{
public:
    TScreenScaleType	screenScale;	  //屏幕坐标缩放
    TScreenColorType	screenColorType;  //屏幕颜色数
    bool				isUseErrorDiffuse;//不支持真彩色时是否使用误差扩散
    long				screenMoveX0;     //屏幕映射x轴坐标偏移量
    long				screenMoveY0;     //屏幕映射y轴坐标偏移量
    bool				isUsePicHD;		  //是否使用高清图片
    double              picHDScale;       //HD增大大小  默认2
    TScreenScaleType	picScale;		  //图片载入缩放
    TScreenScaleType	posScale;		  //坐标位置缩放
    bool                isZoomGray8Pic;   //是否缩放gray8文件,默认false
    double* slOldSizes;
    double* slNewSizes;
    long*	llOldSizes;
    long*	llNewSizes;
    String				tool_saveScaledPicPath;//
private:
    inline bool isCanSetScale(TScreenScaleType sl,long gameWidth,long gameHeight,long screenWidth,long screenHeight){
        return ( (gameWidth*slNewSizes[sl] <= screenWidth) 
                &&   (gameHeight*slNewSizes[sl] <= screenHeight) );
    }
    TScreenScaleType getBestScale(long gameWidth,long gameHeight,long screenWidth,long screenHeight);
    inline double _mapFromScPos(double screen_p)const{ assert(screen_p>=0); return (screen_p*slOldSizes[screenScale]+0.000001);}
    inline double _mapToScPos(double p)const{  return (p*slNewSizes[screenScale]+0.000001);}
public:
    inline bool isDrawScreenNeedBuffer()const{ return (screenScale!=sl_1to1)||(screenColorType!=sc_Color32_BGRA); }
    void setBestScreenScale(long gameWidth,long gameHeight,long screenWidth,long screenHeight);
    void setAutoPicScale(long gameWidth,long gameHeight,long screenWidth,long screenHeight);
    inline long mapFromScX(double screen_x)const { return (long)_mapFromScPos(screen_x-screenMoveX0); }
    inline long mapFromScY(double screen_y)const { return (long)_mapFromScPos(screen_y-screenMoveY0); }
    inline long mapToScXMin(double game_x)const{ return (long)_mapToScPos(game_x)+screenMoveX0; }
    inline long mapToScXMax(double game_x)const{ return (long)(0.99999+_mapToScPos(game_x))+screenMoveX0; }
    inline long mapToScYMin(double game_y)const{ return (long)_mapToScPos(game_y)+screenMoveY0; }
    inline long mapToScYMax(double game_y)const{ return (long)(0.99999+_mapToScPos(game_y))+screenMoveY0; }
    inline long mapPicSize(long p)const{ 
        if (picScale!=sl_auto) 
            return ((p+llOldSizes[picScale]-1)/llOldSizes[picScale])*llNewSizes[picScale];
        else
            return (long)(0.5+p*slNewSizes[picScale]);
    }
    template<class T> inline T mapPos(T p) const{ double hdScale=1; if (isUsePicHD) hdScale=picHDScale; return (T)( 0.49999-((T)0.49999)+(p*slNewSizes[posScale]*hdScale) );}
    template<class T> inline T rmapPos(T p)const{ double hdScale=1; if (isUsePicHD) hdScale=picHDScale; return (T)( 0.49999-((T)0.49999)+(p*slOldSizes[posScale])/hdScale );}
    
    void doPicScale(const void* dstRef,const void* srcRef,TScreenScaleType picScale);
    void doPicGray8Scale(const void* dstRef,const void* srcRef,TScreenScaleType picScale);
    void doDrawToScreen(const void* dstRef,const void* tempRef32,const void* srcGameBufRef32,long* pDstRect,long rectCount);
    bool isHavePicHDPath()const;
    String transPicFileName(const String& fileName)const;
    void tool_saveScaledPic(const void* ref,const char* srcFileName)const;//自动保存缩放好的文件
};


#define mapGamePos(p) g_screenScale->mapPos(p)
#define mapFromGamePos(p) g_screenScale->rmapPos(p)
#define mapToScXMin(p) g_screenScale->mapToScXMin(p)
#define mapToScXMax(p) g_screenScale->mapToScXMax(p)
#define mapToScYMin(p) g_screenScale->mapToScYMin(p)
#define mapToScYMax(p) g_screenScale->mapToScYMax(p)
#define mapFromScX(p) g_screenScale->mapFromScX(p)
#define mapFromScY(p) g_screenScale->mapFromScY(p)
#define transformPicFileName(fn) g_screenScale->transPicFileName(fn)

//TOOL_SAVE_SCALED_PNG==1 用来启用预先缩放png大小到合适尺寸的功能
#define TOOL_SAVE_SCALED_PNG 0

//自动缩放加载的图片
template<class Pixels>
static inline void screenScale_doPicScale(Pixels& o_pic,long row=1,long col=1,const char* srcFileName=0){
    if (g_screenScale->picScale==sl_1to1) return;
    const bool isGray8=(sizeof(typename Pixels::TColor)==8);
    if (isGray8&&(!g_screenScale->isZoomGray8Pic)) return;
    
    double sclipWidth=1.0*o_pic.getWidth()/col;
    long dclipWidth=g_screenScale->mapPicSize((long)(sclipWidth+0.5));
    double sclipHeight=1.0*o_pic.getHeight()/row;
    long dclipHeight=g_screenScale->mapPicSize((long)(sclipHeight+0.5));
    Pixels  pic; pic.resizeFast(dclipWidth*col,dclipHeight*row);
    
    /*typedef typename Pixels::TPixelsRefType TPixelsRef;
     TPixelsRef dstRef,srcRef;
     for (long cy=0;cy<row;++cy){
     for (long cx=0;cx<col;++cx){
     pic.getRef().getSubRef_fast(cx*dclipWidth,cy*dclipHeight,(cx+1)*dclipWidth,(cy+1)*dclipHeight,dstRef);
     o_pic.getRef().getSubRef_fast((long)(cx*sclipWidth),(long)(cy*sclipHeight),(long)((cx+1)*sclipWidth),(long)((cy+1)*sclipHeight),srcRef);
     if (isGray8)
     g_screenScale->doPicGray8Scale(&dstRef,&srcRef,g_screenScale->picScale);
     else
     g_screenScale->doPicScale(&dstRef,&srcRef,g_screenScale->picScale);
     }
     }*/
    if (isGray8)
        g_screenScale->doPicGray8Scale(&pic.getRef(),&o_pic.getRef(),g_screenScale->picScale);
    else
        g_screenScale->doPicScale(&pic.getRef(),&o_pic.getRef(),g_screenScale->picScale);
    o_pic.swap(pic);
    if (TOOL_SAVE_SCALED_PNG&&(srcFileName!=0))
        g_screenScale->tool_saveScaledPic(&o_pic.getRef(),srcFileName);
}


#endif
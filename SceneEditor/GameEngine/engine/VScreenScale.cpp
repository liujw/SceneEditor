#include "VScreenScale.h"
#include "../draw2d/hDraw.h"
#include "../base/Rect.h"

#ifndef _IOS
#ifndef WINCE
#ifndef _ANDROID	
#define _High_Performance_Device
#endif
#endif
#endif


long   _llOldSizes[]={ 0,   2,     3,    4, 1,    3,    2,   1 };
long   _llNewSizes[]={ 0,   1,     2,    3, 1,    4,    3,   2 };
double _slOldSizes[]={ 0, 2.0,   1.5,4.0/3, 1, 0.75,2.0/3, 0.5 };
double _slNewSizes[]={ 0, 0.5, 2.0/3, 0.75, 1,4.0/3,  1.5, 2.0 };

const TZoomMapProc slZoomVProcs[]={
    0,
    zoom2To1_fast,
    zoom3To2_fast,
    zoom4To3_fast,
    zoom1To1_fast,
    zoom3To4_fast,
    zoom2To3_fast,
    zoom1To2_fast
};

const TZoomCopyProc slZoomPicProcs[]={
    0,
    zoom2To1,
    zoom3To2,
    zoom4To3,
    zoom1To1,
    zoom3To4,
    zoom2To3,
    zoom1To2
};

const TZoomCopyGray8Proc slZoomPicGray8Procs[]={
    0,
    zoom2To1Gray8,
    zoom3To2Gray8,
    zoom4To3Gray8,
    zoom1To1Gray8,
    zoom3To4Gray8,
    zoom2To3Gray8,
    zoom1To2Gray8
};


TScreenScaleType VScreenScale::getBestScale(long gameWidth,long gameHeight,long screenWidth,long screenHeight){
    for (TScreenScaleType i=sl_1to2;i>=sl_2to1;i=(TScreenScaleType)(i-1)){
        if (isCanSetScale(i,gameWidth,gameHeight,screenWidth,screenHeight)){
            return i;
        }
    }
    return sl_auto;
}

inline bool isFitSize(long width,long height,long screenWidth,long screenHeight){
    return  ((width==screenWidth)&&(height<=screenHeight)) || ((width<=screenWidth)&&(height==screenHeight));
}

void VScreenScale::setBestScreenScale(long gameWidth,long gameHeight,long screenWidth,long screenHeight){
    screenScale=sl_1to1;
    isUsePicHD=false;
    TScreenScaleType screenScaleNew=getBestScale(gameWidth,gameHeight,screenWidth,screenHeight);
    long hdGameWidth=(long)(gameWidth*picHDScale+0.4999);
    long hdGameHeight=(long)(gameHeight*picHDScale+0.4999);
    if ((screenScaleNew>=sl_1to1)&&(isHavePicHDPath())){
        TScreenScaleType screenScaleHD=getBestScale(hdGameWidth,hdGameHeight,screenWidth,screenHeight);
        if (screenScaleHD!=sl_auto){
            //判断标准:1.面积大 2.HD好
            double hdW=(double)hdGameWidth*slNewSizes[screenScaleHD];
            double newW=(double)gameWidth*slNewSizes[screenScaleNew];
            double hdV=MyBase::sqr(hdW);
            double newV=MyBase::sqr(newW);
            double isHD=(hdV/newV)*(hdW/gameWidth);
            if (isHD>1){
                isUsePicHD=true;
                screenScaleNew=screenScaleHD;
            }
        }
    }
    screenScale=screenScaleNew;
    
    long gtWidth=gameWidth;
    long gtHeight=gameHeight;
    if (isUsePicHD){
        gtWidth=hdGameWidth;
        gtHeight=hdGameHeight;
    }
#ifdef _High_Performance_Device	
    //非手持设备
    screenMoveX0=0;
    screenMoveY0=0;
    if ((screenScale==sl_auto)||(!isFitSize(mapToScXMax(gtWidth),mapToScYMax(gtHeight),screenWidth,screenHeight))){
        screenScale=sl_auto;
        isUsePicHD=(gameWidth<screenWidth) && (gameHeight<screenHeight);
        gtWidth=gameWidth;
        gtHeight=gameHeight;
        if (isUsePicHD){
            gtWidth=hdGameWidth;
            gtHeight=hdGameHeight;
        }
    }
#endif
    
    if (screenScale==sl_auto){
        double g=1;
        if (gtWidth*screenHeight>=gtHeight*screenWidth){
            g=((double)gtWidth)/screenWidth;
            slOldSizes[sl_auto]=g;
            slNewSizes[sl_auto]=1.0/g;
            long gcd=MyBase::gcd(gtWidth,screenWidth);
            llOldSizes[sl_auto]=gtWidth/gcd;
            llNewSizes[sl_auto]=screenWidth/gcd;
        }else{
            g=((double)gtHeight)/screenHeight;
            slOldSizes[sl_auto]=g;
            slNewSizes[sl_auto]=1.0/g;
            long gcd=MyBase::gcd(gtHeight,screenHeight);
            llOldSizes[sl_auto]=gtHeight/gcd;
            llNewSizes[sl_auto]=screenHeight/gcd;
        }
    }
    screenMoveX0=0;
    screenMoveY0=0;
    screenMoveX0=(screenWidth-mapToScXMax(gtWidth))/2;//居中
    screenMoveY0=(screenHeight-mapToScYMax(gtHeight))/2;
}

void VScreenScale::setAutoPicScale(long gameWidth,long gameHeight,long screenWidth,long screenHeight){
    setBestScreenScale(gameWidth,gameHeight,screenWidth,screenHeight);
    picScale=screenScale;
    screenScale=sl_1to1;
}

void VScreenScale::doPicScale(const void* dstRef,const void* srcRef,TScreenScaleType picScale){
#if (defined(_High_Performance_Device))||(TOOL_SAVE_SCALED_PNG!=0)
    zoomBicubicAsCopy(*(Pixels32Ref*)dstRef,*(Pixels32Ref*)srcRef);
#else
    if (picScale!=sl_auto)
        slZoomPicProcs[picScale](*(Pixels32Ref*)dstRef,*(Pixels32Ref*)srcRef);
    else
        mipZoomBilinear(*(Pixels32Ref*)dstRef,*(Pixels32Ref*)srcRef);
#endif
}

void VScreenScale::doPicGray8Scale(const void* dstRef,const void* srcRef,TScreenScaleType picScale){
    if (picScale!=sl_auto)
        slZoomPicGray8Procs[picScale](*(PixelsGray8Ref*)dstRef,*(PixelsGray8Ref*)srcRef);
    else
        mipZoomBilinearGray8(*(PixelsGray8Ref*)dstRef,*(PixelsGray8Ref*)srcRef);
}

static void screenScale_doDrawToScreen(const void* dstRef,TScreenColorType screenColorType,const void* tempRef32,
                                const void* srcGameBufRef32,TScreenScaleType screenScale,bool isErrorDiffuse){
    /*
     //Scale
     Pixels32Ref* dstRef32=(Pixels32Ref*)dstRef;
     if (screenColorType!=sc_Color32_BGRA){
     dstRef32=(Pixels32Ref*)tempRef32;
     }
     //
     
     //
     if (isErrorDiffuse){
     switch (screenColorType){
     case sc_Color32_BGRA:{
     copy_32bit_to_16bit555_ErrorDiffuse(*(Pixels32Ref*)dstRef,32,false,*dstRef32);
     } break;
     case sc_Color16_565:{
     copy_32bit_to_16bit565_ErrorDiffuse(*(Pixels32Ref*)dstRef,16,true,*dstRef32);
     } break;
     case sc_Color16_555:{
     copy_32bit_to_16bit555_ErrorDiffuse(*(Pixels32Ref*)dstRef,16,false,*dstRef32);
     } break;
     }
     }else if (dstRef32!=dstRef){
     switch (screenColorType){
     case sc_Color32_BGRA:{
     copy(*(Pixels32Ref*)dstRef,0,0,*dstRef32);
     } break;
     case sc_Color16_565:{
     copy_32bit_to_16bit565_fast(*(Pixels32Ref*)dstRef,16,true,*dstRef32);
     } break;
     case sc_Color16_555:{
     copy_32bit_to_16bit555_fast(*(Pixels32Ref*)dstRef,16,false,*dstRef32);
     }break;
     }
     }*/
}

void VScreenScale::doDrawToScreen(const void* dstRef,const void* tempRef32,const void* srcGameBufRef32,long* pDstRect,long rectCount){
    /*
     switch (screenColorType){
     case sc_Color32_BGRA:{
     copy_32bit_to_16bit555_ErrorDiffuse(*(Pixels32Ref*)dstRef,32,false,*dstRef32);
     } break;
     case sc_Color16_565:{
     copy_32bit_to_16bit565_ErrorDiffuse(*(Pixels32Ref*)dstRef,16,true,*dstRef32);
     } break;
     case sc_Color16_555:{
     copy_32bit_to_16bit555_ErrorDiffuse(*(Pixels32Ref*)dstRef,16,false,*dstRef32);
     } break;
     }
     
     if (pSrcRect==0){
     screenScale_doDrawToScreen(dstRef,screenColorType,tempRef32,srcGameBufRef32,screenScale,isUseErrorDiffuse&&());
     }else{
     for (long i=0;i<rectCount;++i){
     TRect& rt=((TRect*)pDstRect)[i];
     
     Pixels32Ref sref=srcRef.getSubRef(mapPos(pRect[0]-m_mapPosMvX0),mapPos(pRect[1]-m_mapPosMvY0),mapPos(pRect[2]-m_mapPosMvX0),mapPos(pRect[3]-m_mapPosMvY0));
     Pixels32Ref dref=dstRef.getSubRef(rt.x0,rt.y0,rt.x1,rt.y1);
     screenScale_doDrawToScreen();
     }
     }*/
}



#if (TOOL_SAVE_SCALED_PNG!=0)
#include "../draw2d/imageFile/hPngFile.h"
#endif

void VScreenScale::tool_saveScaledPic(const void* ref,const char* srcFileName) const{
#if (TOOL_SAVE_SCALED_PNG!=0)
    String fname;
    if (replaceStr(srcFileName,fname,"/PicHD/",7,("/"+tool_saveScaledPicPath+"/").c_str(),tool_saveScaledPicPath.size()+2)){
        TPngColor32File png;
        png.saveTo(*(Pixels32Ref*)ref,fname.c_str());
    }else{
        fname=srcFileName;
        if (fname=="Default@2x.png"){
            fname="Default@"+tool_saveScaledPicPath+".png";
            TPngColor32File png;
            png.saveTo(*(Pixels32Ref*)ref,fname.c_str());
        }//else
        //assert(false);
    }
#endif
}

static bool getCanUseFHFileName(const String& filename,String& fhFileName){
    fhFileName=filename;
    long dot=posFromEnd(fhFileName.c_str(),(long)fhFileName.size(),'.');
    if (dot<0) return false;
    fhFileName[dot]='_';
    fhFileName+=".fh";
    return file_exists(fhFileName.c_str());
}

String VScreenScale::transPicFileName(const String& fileName)const{
    String fn=fileName;
    if(isUsePicHD){
        if (!replaceStr(fn,fn,"/Pic/",5,"/PicHD/",7)){
            if (fn=="Default.png")
                fn="Default@2x.png";
        }
    }
#if (TOOL_SAVE_SCALED_PNG!=0)
    return fn;
#else
    /*if ((fn=="Default.png")||(fn=="Default@2x.png")){
     if (file_exists(fn.c_str()))
     return fn;
     }*/
    
    String fhFileName;
    if (getCanUseFHFileName(fn,fhFileName))
        return fhFileName;
    else 
        return fn;
#endif
}

bool VScreenScale::isHavePicHDPath()const{
    return path_exists("Res/PicHD");
    //return file_exists("Default@2x.png");
}

VScreenScale _g_screenScale={ 
    sl_1to1 ,
    sc_Color32_BGRA , 
    true,
    0,
    0, 
    false,
    2,
    sl_1to1,
    sl_1to1,
    false,
    &_slOldSizes[0],
    &_slNewSizes[0],
    &_llOldSizes[0],
    &_llNewSizes[0]
};

VScreenScale* g_screenScale=&_g_screenScale;


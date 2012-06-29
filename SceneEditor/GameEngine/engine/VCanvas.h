//
//  VCanvas.h
//
//  Created by housisong on 08-3-25.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VCanvas_h_
#define _VCanvas_h_

#include "../draw2d/hDraw.h"
#include "../draw2d/hPixels32.h"

///////////////////
class ISurfaceAutoCache{
public:
    virtual void cacheLock()=0;
    virtual void cacheUnLock()=0;
    virtual ~ISurfaceAutoCache(){}
};

class VCanvasGray8{
public:
    typedef PixelsGray8Ref	TPixelsRef;
    typedef TPixelsRef::TColor TColor;
    typedef VCanvasGray8 TCanvasType;
protected:
    TPixelsRef	m_PicRef;
    mutable ISurfaceAutoCache*  m_cache;
    
    inline void cacheLock()const{
        if (m_cache) m_cache->cacheLock();
    }
    inline void cacheUnLock()const{
        if (m_cache){ 
            m_cache->cacheUnLock();
            m_cache=0;
        }
    }
public:
    inline explicit VCanvasGray8():m_cache(0){
    }
    inline VCanvasGray8(const TCanvasType& value)
    :m_PicRef(value.m_PicRef),m_cache(value.m_cache){
        cacheLock();
    }
    inline explicit VCanvasGray8(const TPixelsRef& lockPicRef):m_PicRef(lockPicRef),m_cache(0){
    }
    inline explicit VCanvasGray8(const TPixelsRef& lockPicRef,ISurfaceAutoCache* cache):m_PicRef(lockPicRef),m_cache(cache){
        cacheLock();
    }
    inline TCanvasType& operator =(const TCanvasType& c){
        setRef(c);
        return *this;
    }
    inline ~VCanvasGray8(){
        cacheUnLock();
    }
    inline void setRef(const TPixelsRef& lockPicRef){
        m_PicRef=lockPicRef;
        cacheUnLock();
    }
    inline void setRef(const TCanvasType& value){
        setRef(value.m_PicRef,value.m_cache);
    }
    inline void setRef(const TPixelsRef& lockPicRef,ISurfaceAutoCache* cache){
        if (cache!=0) cache->cacheLock();
        cacheUnLock();
        m_PicRef=lockPicRef;
        m_cache=cache;
    }
    inline void setEmptyRef(){
        cacheUnLock();
        m_PicRef.setNullRef();
    }
    
    inline void getCanvasSub(long x0,long y0,long x1,long y1,TCanvasType& out_canvasSub) const {
        out_canvasSub.setRef(m_PicRef.getSubRef(x0, y0, x1, y1),m_cache);
    }      
    
    inline bool getIsEmpty()const{
        return m_PicRef.getIsEmpty();
    }
    inline bool getIsEqual(const TCanvasType& value)const{
        return (m_cache==value.m_cache)&&(m_PicRef.getIsEqual(value.m_PicRef));
    }
    
    inline const TPixelsRef& getRef()const{ 
        return m_PicRef; 
    }
    inline long getWidth() const {
        return m_PicRef.width;
    }
    inline long getHeight()const {
        return m_PicRef.height;
    }
    
    inline bool getIsInRect(long x,long y)const {
        return m_PicRef.getIsInRect(x,y) ;
    }
    inline TColor getPixel(long x,long y)const {
        return m_PicRef.getPixel(x, y);
    }
    inline TColor getPixelsBorder(long x,long y)const {
        return m_PicRef.getPixelsBorder(x, y);
    }
    inline void setPixel(long x,long y,const TColor& color)const{
        if (m_PicRef.getIsInRect(x,y))
            m_PicRef.setPixel(x, y, color);
    }
    inline void fill(const TColor& color)const {
        ::fill(m_PicRef,color);
    }
    inline void copy(const TCanvasType& srcCanvas)const{
        copy(0,0,srcCanvas);
    }
    inline void copy(long x0,long y0,const TCanvasType& srcCanvas)const{
        ::copy(m_PicRef,x0,y0,srcCanvas.m_PicRef);
    }
    
    inline void copyAsTextPixels(long x0, long y0,const Pixels32Ref& text_src)const{
        ::copyAsTextPixels(m_PicRef,x0,y0,text_src);
    }
};

class VCanvas;
const long ALPHA_TEST_MINVALUE=32; 

class VCanvas{
protected:
    //unsigned long m_sumSrcLayerAlpha;
public:
    //inline void setSumSrcLayerAlpha(long sumSrcLayerAlpha){
    //	m_sumSrcLayerAlpha=sumSrcLayerAlpha;
    //}
public:
    typedef Pixels32Ref	TPixelsRef;
    typedef TPixelsRef::TColor TColor;
    typedef VCanvas TCanvasType;
protected:
    TPixelsRef	m_PicRef;
    mutable ISurfaceAutoCache*  m_cache;
    
    inline void cacheLock()const{
        if (m_cache) m_cache->cacheLock();
    }
    inline void cacheUnLock()const{
        if (m_cache){ 
            m_cache->cacheUnLock();
            m_cache=0;
        }
    }
    must_inline void copyTo(const VCanvas& dst,long x0,long y0)const{
        ::copy(dst.getRef(),x0,y0,m_PicRef);//,m_sumSrcLayerAlpha);
    }
    must_inline void blendTo(const VCanvas& dst,long x0,long y0)const{
        ::blend(dst.getRef(),x0,y0,m_PicRef);//,m_sumSrcLayerAlpha);
    }
public:
    inline explicit VCanvas():m_cache(0){//,m_sumSrcLayerAlpha(255){
    }
    inline VCanvas(const TCanvasType& value)
    :m_PicRef(value.m_PicRef),m_cache(value.m_cache){
        //this->m_sumSrcLayerAlpha=value.m_sumSrcLayerAlpha;
        cacheLock();
    }
    inline explicit VCanvas(const TPixelsRef& lockPicRef):m_PicRef(lockPicRef),m_cache(0){//,m_sumSrcLayerAlpha(255){
    }
    inline explicit VCanvas(const TPixelsRef& lockPicRef,ISurfaceAutoCache* cache):m_PicRef(lockPicRef),m_cache(cache){//,m_sumSrcLayerAlpha(255){
        cacheLock();
    }
    inline TCanvasType& operator =(const TCanvasType& c){
        setRef(c);
        //this->m_sumSrcLayerAlpha=c.m_sumSrcLayerAlpha;
        return *this;
    }
    inline ~VCanvas(){
        cacheUnLock();
    }
    inline void setRef(const TPixelsRef& lockPicRef){
        m_PicRef=lockPicRef;
        cacheUnLock();
    }
    inline void setRef(const TCanvasType& value){
        setRef(value.m_PicRef,value.m_cache);
        //this->m_sumSrcLayerAlpha=value.m_sumSrcLayerAlpha;
    }
    inline void setRef(const TPixelsRef& lockPicRef,ISurfaceAutoCache* cache){
        if (cache!=0) cache->cacheLock();
        cacheUnLock();
        m_PicRef=lockPicRef;
        m_cache=cache;
    }
    inline void setEmptyRef(){
        cacheUnLock();
        m_PicRef.setNullRef();
    }
    
    inline void getCanvasSub(long x0,long y0,long x1,long y1,TCanvasType& out_canvasSub) const {
        out_canvasSub.setRef(m_PicRef.getSubRef(x0, y0, x1, y1),m_cache);
        //out_canvasSub.m_sumSrcLayerAlpha=m_sumSrcLayerAlpha;
    }      
    
    inline bool getIsEmpty()const{
        return m_PicRef.getIsEmpty();
    }
    inline bool getIsEqual(const TCanvasType& value)const{
        return (m_cache==value.m_cache)&&(m_PicRef.getIsEqual(value.m_PicRef));
    }
    must_inline const TPixelsRef& getRef()const{ 
        return m_PicRef; 
    }
    inline long getWidth() const {
        return m_PicRef.width;
    }
    inline long getHeight()const {
        return m_PicRef.height;
    }
    
    inline bool getIsInRect(long x,long y)const {
        return m_PicRef.getIsInRect(x,y) ;
    }
    inline TColor getPixel(long x,long y)const {
        return m_PicRef.getPixel(x, y);
    }
    TColor getPixelsBorder(long x,long y)const {
        return m_PicRef.getPixelsBorder(x, y);
    }
    inline void setPixel(long x,long y,const TColor& color)const{			
        if (getIsInRect(x,y)){
            getRef().setPixel(x, y, color);
        }
    }
    inline void fill(const TColor& color)const {
        ::fill(getRef(),color);
    }
    inline void copy(const TCanvasType& srcCanvas)const{
        copy(0,0,srcCanvas);
    }
    inline void copy(long x0,long y0,const TCanvasType& srcCanvas)const{
        srcCanvas.copyTo(*this,x0,y0);
    }
    
    ////
    
    //判断一个点是否在图像中；x,y为图象点坐标；透明Alpha值小于LeastAlpha的像素点不认为是图像的一部分
    inline bool getIsHit(long x,long y,long LeastAlpha=ALPHA_TEST_MINVALUE) const {
        if (!getIsInRect(x,y))
            return false;
        else
            return getPixel(x, y).a>=LeastAlpha;
    }
    
    inline void fillColoring(Color32 color)const {
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::fillColoring(getRef(),color);
    }
    inline void fillColoring(long x0, long y0, long x1,long y1,Color32 color)const {
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::fillColoring(getRef().getSubRef(x0, y0, x1, y1),color);
    }
    inline void fillBlend(Color32 color)const {
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::fillBlend(getRef(),color);
    }
    inline void fillBlend(long x0, long y0, long x1,long y1,Color32 color)const {
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::fillBlend(getRef().getSubRef(x0, y0, x1, y1),color);
    }
    
    inline void copy(long x0, long y0, const VCanvas& srcCanvas, long alpha)const {
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        if (alpha>=255)
            copy(x0,y0,srcCanvas);
        else{
            ::copy(getRef(),x0,y0,srcCanvas.getRef(),alpha);
        }
    }
    inline void copyColoring(long x0, long y0, const VCanvas& srcCanvas, Color32 color)const {
        if (color==cl32ColoringMullEmpty)
            copy(x0,y0,srcCanvas);
        else{
            //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
            ::copyColoring(getRef(),x0,y0,srcCanvas.getRef(),color);
        }
    }
    inline void copyColoring(long x0, long y0, const VCanvas& srcCanvas, Color32 color,long alpha)const {
        if ((alpha>=255)&&(color==cl32ColoringMullEmpty))
            copy(x0,y0,srcCanvas);
        else{
            //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
            ::copyColoring(getRef(),x0,y0,srcCanvas.getRef(),color,alpha);
        }
    }
    inline void copyAsGray(long x0, long y0, const VCanvas& srcCanvas) const{
        ::copyAsGray(getRef(),x0,y0,srcCanvas.getRef());//,m_sumSrcLayerAlpha);
    }
    inline void copyAsGray(long x0, long y0, const VCanvas& srcCanvas, long alpha) const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::copyAsGray(getRef(),x0,y0,srcCanvas.getRef(),alpha);
    }
    inline void blendAsGray(long x0, long y0, const VCanvas& srcCanvas) const{
        ::blendAsGray(getRef(),x0,y0,srcCanvas.getRef());//,m_sumSrcLayerAlpha);
    }
    inline void blendAsGray(long x0, long y0, const VCanvas& srcCanvas, long alpha)const {
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::blendAsGray(getRef(),x0,y0,srcCanvas.getRef(),alpha);
    }
    inline void blend(const VCanvas& srcCanvas)const {
        blend(0,0,srcCanvas);
    }
    inline void blend(long x0,long y0,const VCanvas& srcCanvas)const{
        srcCanvas.blendTo(*this,x0,y0);
    }
    inline void blend(long x0,long y0,const VCanvas& srcCanvas,long alpha)const{
        if (alpha>=255)
            blend(x0,y0,srcCanvas);
        else{
            //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
            ::blend(getRef(),x0,y0,srcCanvas.getRef(),alpha);
        }
    }		
    inline void blendForBuffer(const VCanvas& srcCanvas)const {
        blendForBuffer(0,0,srcCanvas);
    }
    inline void blendForBuffer(long x0,long y0,const VCanvas& srcCanvas)const{
        ::blendForBuffer(getRef(),x0,y0,srcCanvas.getRef());
    }
    inline void blendColoring(long x0,long y0,const VCanvas& srcCanvas, Color32 color)const{
        if (color==cl32ColoringMullEmpty)
            copy(x0,y0,srcCanvas);
        else{
            //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
            ::blendColoring(getRef(),x0,y0,srcCanvas.getRef(),color);
        }
    }
    inline void blendColoring(long x0,long y0,const VCanvas& srcCanvas,Color32 color,long alpha)const{
        if ((alpha>=255)&&(color==cl32ColoringMullEmpty))
            blend(x0,y0,srcCanvas);
        else{
            //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
            ::blendColoring(getRef(),x0,y0,srcCanvas.getRef(),color,alpha);
        }
    }
    inline void add(const VCanvas& srcCanvas)const{
        add(0,0,srcCanvas);
    }
    inline void add(long x0,long y0,const VCanvas& canvas)const{
        //if (m_sumSrcLayerAlpha>=255)
        ::add(getRef(),x0,y0,canvas.getRef());
        //else
        //	::addColoring(getRef(),x0,y0,canvas.getRef(),Color32(255,255,255,m_sumSrcLayerAlpha));
    }
    inline void addColoring(long x0,long y0,const VCanvas& canvas,Color32 color)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::addColoring(getRef(),x0,y0,canvas.getRef(),color);
    }
    inline void blendForAdd()const{
        ::blendForAdd(getRef());
    }
    
    inline void zoomCopy(const VCanvas& canvas,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::zoomBilinearAsCopy(getRef(),canvas.getRef(),color,alpha);
    }
    inline void zoomBlend(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::zoomBilinearAsBlend(getRef(),canvas.getRef(),color,alpha);
    }
    inline void zoomAdd(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::zoomBilinearAsAdd(getRef(),canvas.getRef(),color);
    }
    inline void zoomCopyFast(const VCanvas& canvas,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::zoomNearestNeighborAsCopy(getRef(),canvas.getRef(),color,alpha);
    }
    inline void zoomBlendFast(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::zoomNearestNeighborAsBlend(getRef(),canvas.getRef(),color,alpha);
    }
    inline void zoomAddFast(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::zoomNearestNeighborAsAdd(getRef(),canvas.getRef(),color);
    }
    inline void zoomCopyQuality(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::zoomBicubicAsCopy(getRef(),canvas.getRef(),color,alpha);
    }
    inline void zoomBlendQuality(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::zoomBicubicAsBlend(getRef(),canvas.getRef(),color,alpha);
    }
    inline void zoomAddQuality(const  VCanvas& canvas,Color32  color=cl32ColoringMullEmpty)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::zoomBicubicAsAdd(getRef(),canvas.getRef(),color);
    }
    
    inline void rotaryCopy(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryBilinearAsCopy(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
    }
    inline void rotaryBlend(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryBilinearAsBlend(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
    }
    inline void rotaryBlendEx(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y, double rx0, double ry0, bool isDefRotaryCenter = true, Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryBilinearAsBlendEx(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,rx0, ry0, isDefRotaryCenter, color,alpha);
    }
    inline void rotaryAdd(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::rotaryBilinearAsAdd(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color);
    }
    inline void rotaryCopyFast(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryNearestNeighborAsCopy(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
    }
    inline void rotaryBlendFast(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryNearestNeighborAsBlend(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
    }
    inline void rotaryAddFast(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::rotaryNearestNeighborAsAdd(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color);
    }
    inline void rotaryCopyQuality(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryBicubicAsCopy(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
    }
    inline void rotaryBlendQuality(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty,long alpha=255)const{
        //alpha=(UInt8)(alpha*m_sumSrcLayerAlpha/255);
        ::rotaryBicubicAsBlend(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color,alpha);
    }
    inline void rotaryAddQuality(const VCanvas& canvas,double RotaryAngle_sin,double RotaryAngle_cos,double ZoomX,double ZoomY,double move_x,double move_y,Color32  color=cl32ColoringMullEmpty)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::rotaryBicubicAsAdd(getRef(),canvas.getRef(),RotaryAngle_sin,RotaryAngle_cos,ZoomX,ZoomY,move_x,move_y,color);
    }
    
    inline void blendLight(long x0, long y0, const VCanvasGray8& lightCanvas, Color32  color) const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::blendLight(getRef(),x0,y0,lightCanvas.getRef(),color);
    }
    inline void addLight(long x0, long y0, const VCanvasGray8& lightCanvas, Color32  color) const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::addLight(getRef(),x0,y0,lightCanvas.getRef(),color);
    }
    ////////
    //绘制水平线条
    inline void lineH(long y,long x0,long x1,Color32 color)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::lineH(getRef(),y,x0,x1,color);
    }
    //绘制垂直线条
    inline void lineV(long x,long y0,long y1,Color32 color)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::lineV(getRef(),x,y0,y1,color);
    }
    //绘制矩形
    inline void rect(long x0,long y0,long x1,long y1,Color32 color)const{
        //color.a=(UInt8)(color.a*m_sumSrcLayerAlpha/255);
        ::rect(getRef(),x0,y0,x1,y1,color);
    }
    
    //inline void unionLayer(long x0,long y0,const VCanvas& frontCanvas)const{
    //	::unionLayer(getRef(),x0,y0,frontCanvas.getRef());
    //}
    
    inline void blendWithYAlphas(long x0,long y0,const VCanvas& srcCanvas,bool isReversalSrcY,const UInt8* alphas)const{
        //alphas=(UInt8)(alphas*m_sumSrcLayerAlpha/255);
        if (isReversalSrcY)
            ::blendWithYAlphas(getRef(),x0,y0,srcCanvas.getRef().getReversalRef(),alphas);
        else
            ::blendWithYAlphas(getRef(),x0,y0,srcCanvas.getRef(),alphas);
    }
    inline void blendWithYColorings(long x0,long y0,const VCanvas& srcCanvas,bool isReversalSrcY,const Color32* colorings)const{
        //alphas=(UInt8)(alphas*m_sumSrcLayerAlpha/255);
        if (isReversalSrcY)
            ::blendWithYColorings(getRef(),x0,y0,srcCanvas.getRef().getReversalRef(),colorings);
        else
            ::blendWithYColorings(getRef(),x0,y0,srcCanvas.getRef(),colorings);
    }
};

template<class TCanvas>
class TClipCanvasBase{
protected:
    TCanvas		m_canvas;
    double m_fClipWidth;
    double m_fClipHeight;
    long		m_clipRowCount;
    long		m_clipColCount;
    long		m_clipWidth;
    long		m_clipHeight;
public:
    TClipCanvasBase(const TCanvas& canvas,long row,long col){ clip(canvas,row,col); }
    TClipCanvasBase(const TClipCanvasBase& clipCanvas){ clip(clipCanvas); }
    TClipCanvasBase(){ clip(1,1); }
    void clip(const TCanvas& canvas,long row,long col){
        if (row<=0) row=1;
        if (col<=0) col=1;
        m_canvas.setRef(canvas);
        m_clipRowCount=row;
        m_clipColCount=col;
        m_fClipWidth = ((double)m_canvas.getWidth()) / ((double)col);
        m_fClipHeight = ((double)m_canvas.getHeight()) / ((double)row);
        m_clipWidth = (long)m_fClipWidth;
        m_clipHeight = (long)m_fClipHeight;
        //m_clipWidth=m_canvas.getWidth()/col;
        //m_clipHeight=m_canvas.getHeight()/row;
    }
    inline void clip(long row,long col){		
        clip(m_canvas,row,col);
    }
    inline void clip(const TClipCanvasBase& clipCanvas){
        clip(clipCanvas.m_canvas,clipCanvas.m_clipRowCount,clipCanvas.m_clipColCount);
    }
    
    void  getSubCanvasByClipIndex(long index,TCanvas& out_canvasSub)const{
        long r=index/m_clipColCount;
        /*long x0=m_clipWidth*(index-r*m_clipColCount);
         long y0=r*m_clipHeight;
         */
        long c = index - r * m_clipColCount;
        long x0 = (long)(m_fClipWidth * (double)c);
        long y0 = (long)(m_fClipHeight * (double)r);
        m_canvas.getCanvasSub(x0, y0, x0 + m_clipWidth, y0 + m_clipHeight,out_canvasSub);
    }  
    
    must_inline TCanvas getSubCanvasByClipIndex(long index)const{
        TCanvas result;
        getSubCanvasByClipIndex(index,result);
        return result;
    }  
    
    inline const TCanvas&  getCanvas()const{
        return m_canvas;
    }  
    
    inline long getClipWidth()const{
        return m_clipWidth;
    }
    inline long getClipHeight()const{
        return m_clipHeight;
    }
    inline long getClipCount()const{
        return m_clipRowCount*m_clipColCount;
    }
};

typedef TClipCanvasBase<VCanvasGray8> VClipCanvasGray8;
typedef TClipCanvasBase<VCanvas> VClipCanvas;


#endif //_VCanvas_h_
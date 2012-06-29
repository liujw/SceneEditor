//
//  VSurface.h
//
//  Created by housisong on 08-3-25.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VSurface_h_
#define _VSurface_h_

#include "../draw2d/hDraw.h"
#include "../draw2d/hPixels32.h"
#include "VCanvas.h"

template<class TPixels,class TCanvasBase>
class TSurfaceBase:protected ISurfaceAutoCache{
protected:
    typedef TSurfaceBase<TPixels,TCanvasBase> TSurfaceType;
    TPixels      m_Pic;
    bool         m_isDoAutoPicScaleWhenLoadPic;
    
    String  m_cachePicFileName;
    long	m_cacheLockCount;
    long	m_cacheRow;
    long	m_cacheCol;
    
    //ISurfaceAutoCache
    virtual void cacheLock(){
        ++m_cacheLockCount;
        assert(m_cacheLockCount>0);
    }
    virtual void cacheUnLock(){
        --m_cacheLockCount;
        assert(m_cacheLockCount>=0);
        if ((m_cacheLockCount==0)&&(!m_cachePicFileName.empty())){//unload
            m_Pic.resizeFast(0,0);
        }
    }
    void doLoad(const char* fileName,IInputStream* stream,long row,long col){
        if (fileName!=0)
            m_Pic.loadFrom(fileName);
        else
            m_Pic.loadFrom(*stream);
        if (m_isDoAutoPicScaleWhenLoadPic)
            screenScale_doPicScale(m_Pic,row,col,fileName);
    }
public:
    void loadFrom(const char* fileName,long row=1,long col=1,bool isAutoCache=false){
        assert(m_cacheLockCount==0);
        if (isAutoCache){
            clear();
            m_cachePicFileName=fileName;
            m_cacheRow=row;
            m_cacheCol=col;
        }else{
            m_cachePicFileName.clear();
            doLoad(fileName,0,row,col);
        }
    }
    inline void loadFromStream(IInputStream& stream,long row=1,long col=1){
        assert(m_cacheLockCount==0);
        m_cachePicFileName.clear();
        doLoad(0,&stream,row,col);
    }
    inline const String& getCachePicFileName()const{
        return m_cachePicFileName;
    }
    inline long getCachePicRow()const{
        return m_cacheRow;
    }
    inline long getCachePicCol()const{
        return m_cacheCol;
    }
    inline void setCachePicClip(long row,long col){
        m_cacheRow=row;
        m_cacheCol=col;
    }
    inline bool getIsDoAutoPicScaleWhenLoadPic()const{
        return m_isDoAutoPicScaleWhenLoadPic;
    }
    inline void setIsDoAutoPicScaleWhenLoadPic(bool isDoAutoPicScaleWhenLoadPic){
        m_isDoAutoPicScaleWhenLoadPic=m_isDoAutoPicScaleWhenLoadPic;
    }
    template<class TClipCanvasType>
    inline const TClipCanvasType getCacheClipCanvas(){
        return TClipCanvasType(getCanvas(),m_cacheRow,m_cacheCol);
    }
    inline void notCache(){
        if ((m_cacheLockCount==0)&&(!m_cachePicFileName.empty()))
            doLoad(m_cachePicFileName.c_str(),0,m_cacheRow,m_cacheCol);
        m_cachePicFileName.clear();
    }
public:
    inline TSurfaceBase():m_cacheLockCount(0),m_isDoAutoPicScaleWhenLoadPic(true){
    }
    
    //复制大小和图像
    inline void assign(const TSurfaceType& srcSurface) {
        assign(srcSurface.getCanvas());
    }
    inline void assign(const TCanvasBase& srcCanvas){
        resizeFast(srcCanvas.getWidth(), srcCanvas.getHeight());
        getCanvas().copy(srcCanvas);
    }
    inline void swap(TSurfaceBase& surface){
        m_Pic.swap(surface.m_Pic);
    }
    
    inline void clear(){
        resizeFast(0, 0);
    }
    inline long getWidth()const {
        return m_Pic.getWidth();
    }
    inline long getHeight()const {
        return m_Pic.getHeight();
    }
    //设置新的图像大小,并保留以前的图像
    inline void resize(long width,long height){
        m_Pic.resize(width, height);
    }
    //设置新的图像大小,图像数据可能为任何值 
    inline void resizeFast(long width,long height){
        m_Pic.resizeFast(width, height);
    }
    inline bool getIsEmpty()const {
        return (m_Pic.getWidth()<=0)||(m_Pic.getHeight()<=0);
    }
    inline TCanvasBase getCanvas()const {
        TCanvasBase out_canvas;
        getCanvas(out_canvas);
        return out_canvas;
    } 
    inline void getCanvas(TCanvasBase& out_canvas)const {
        TSurfaceType* self=(TSurfaceType*)this; //缓存语义上不改变
        if ((m_cacheLockCount==0)&&(!m_cachePicFileName.empty()))
            self->doLoad(m_cachePicFileName.c_str(),0,m_cacheRow,m_cacheCol);
        if (!m_cachePicFileName.empty())
            out_canvas.setRef(m_Pic.getRef(),self);
        else
            out_canvas.setRef(m_Pic.getRef());
    } 
    
    //getCanvasSub: 返回一个子区域的绘图VCanvas   
    inline void getCanvasSub(long x0,long y0,long x1,long y1,TCanvasBase& out_canvasSub)const {
        TCanvasBase all_canvas;
        getCanvas(all_canvas);
        all_canvas.getCanvasSub(x0, y0, x1, y1, out_canvasSub);
    }   
    inline void getCanvasSub(long rows,long cols,long nodeIndex,TCanvasBase& out_canvasSub) const {
        TCanvasBase all_canvas;
        getCanvas(all_canvas);
        all_canvas.getCanvasSub(rows,cols,nodeIndex,out_canvasSub);
    }
};


typedef TSurfaceBase<Pixels32,VCanvas>			VSurface;
typedef TSurfaceBase<PixelsGray8,VCanvasGray8>	VSurfaceGray8;


#endif //_VSurface_h_
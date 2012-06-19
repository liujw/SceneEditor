//
//  VCharCanvas.h
//
//  Created by housisong on 08-4-7.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VCharCanvas_h_
#define _VCharCanvas_h_

#include "VSurface.h"
#include "VFont.h"


//引擎内部使用
struct TCharCanvasDataKey{
public:
    int  		  m_char;
    VFont	      m_font;
public:
    inline TCharCanvasDataKey(const VFont& font,int aChar):m_char(aChar),m_font(font){  }
    virtual ~TCharCanvasDataKey(){}
};

struct TCharCanvasDataBase:public TCharCanvasDataKey{
protected: 
    mutable VCanvasGray8	m_canvas; 
    inline TCharCanvasDataBase(const VFont& font,int aChar):TCharCanvasDataKey(font,aChar){}
public:
    must_inline const VCanvasGray8& getCanvas() const {
        return m_canvas;
    }
    must_inline long getWidth() const { return m_canvas.getWidth(); }
    must_inline long getHeight()const { return m_canvas.getHeight(); }
    must_inline int  getChar()const { return m_char; }
    
    static const TCharCanvasDataBase* getCharCanvasData(const VFont& font,int aChar);
    static const TCharCanvasDataBase* copyCharCanvasData(const TCharCanvasDataBase* src);
    inline static void setCharCanvasData(const TCharCanvasDataBase*& dst,const TCharCanvasDataBase* src){
        if (dst!=src){
            releaseCharCanvasData(dst);
            dst=copyCharCanvasData(src);
        }
    }		
    static void releaseCharCanvasData(const TCharCanvasDataBase* dst);
    virtual ~TCharCanvasDataBase(){}
};

//对外的类
struct VCharCanvas{
private:
    const TCharCanvasDataBase*	m_charCanvasData;
public:
    must_inline  VCharCanvas(const VFont& font,int aChar):m_charCanvasData(TCharCanvasDataBase::getCharCanvasData(font,aChar)) { }
    must_inline  VCharCanvas(const VCharCanvas& charCanvas):m_charCanvasData(TCharCanvasDataBase::copyCharCanvasData(charCanvas.m_charCanvasData)) { }
    must_inline  VCharCanvas& operator=(const VCharCanvas& charCanvas)      { TCharCanvasDataBase::setCharCanvasData(m_charCanvasData,charCanvas.m_charCanvasData); return *this; }
    must_inline virtual ~VCharCanvas(){ TCharCanvasDataBase::releaseCharCanvasData(m_charCanvasData); }
    
    must_inline const VCanvasGray8& getCanvas() const { return m_charCanvasData->getCanvas(); }
    must_inline long getWidth() const { return m_charCanvasData->getWidth(); }
    must_inline long getHeight()const { return m_charCanvasData->getHeight(); }
    must_inline int  getChar()const { return m_charCanvasData->getChar(); }
};



#endif //_VCharCanvas_h_
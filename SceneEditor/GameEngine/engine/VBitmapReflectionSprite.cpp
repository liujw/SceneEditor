//VBitmapReflectionSprite.cpp

#include "VBitmapReflectionSprite.h"


void VBitmapReflectionSprite::doUpdateChange(){
    long height=m_canvas.getHeight();
    m_alphas.resize(height);
    m_colorings.resize(height);
    m_ymin=0;
    m_ymax=height;
    if (height==0) return;
    if (height==1){
        long alpha=(m_alphaBegin+m_alphaEnd)>>1;
        alpha=MyBase::min(alpha,(long)255);
        alpha=MyBase::max(alpha,(long)0);
        m_alphas[0]=(UInt8)alpha;
    }else if ((m_alphaBegin<=0)&&(m_alphaEnd<=0)){
        for (long y=0;y<height;++y) 
            m_alphas[y]=0;
    }else if (m_alphaBegin==m_alphaEnd){
        const long alpha=MyBase::min(m_alphaBegin,(long)255);
        for (long y=0;y<height;++y)
            m_alphas[y]=(UInt8)alpha;
    }else{
        //height>1
        //const IntFloat_16 divM_16=((m_alphaBegin-m_alphaEnd)<<16)/(height-1);
        for (long y=0;y<height;++y){
            //long alpha=m_alphaBegin-(y*divM_16>>16);
            long alpha=m_alphaBegin-y*(m_alphaBegin-m_alphaEnd)/(height-1);
            alpha=MyBase::min(alpha,(long)255);
            alpha=MyBase::max(alpha,(long)0);
            m_alphas[y]=(UInt8)alpha;
        }
    }
    while ((m_ymax!=m_ymin)&&(m_alphas[m_ymax-1]==0)) --m_ymax;
    while ((m_ymax!=m_ymin)&&(m_alphas[m_ymin]==0)) ++m_ymin;
    
    for (long y=0;y<height;++y){
        Color32 c(m_coloring);
        c.a=(UInt8)(((c.a+1)*m_alphas[y])>>8);
        m_colorings[y]=c;
    }
}

void VBitmapReflectionSprite::drawTo(const VCanvas& dst,long x0,long y0,const VCanvas& src){
    if (m_ymax!=m_ymin){
        if (m_coloring.argb==cl32ColoringMullEmpty.argb)
            dst.blendWithYAlphas(x0,y0,src,m_isReversalY,&m_alphas[0]);
        else
            dst.blendWithYColorings(x0,y0,src,m_isReversalY,&m_colorings[0]);	
    }
}

void VBitmapReflectionSprite::doDraw(const VCanvas& dst,long x0,long y0){
    if (!m_canvas.getIsEmpty()) 
        drawTo(dst,x0,y0,m_canvas);
}


bool VBitmapReflectionSprite::getIsHit(long clientX0, long clientY0){
    if (m_canvas.getIsEmpty()) return false;
    if (!m_canvas.getIsHit(clientX0,clientY0,(getLeastAlpha()))) return false;
    long alpha=m_alphas[clientY0];
    if (alpha<=0) return false;
    return m_canvas.getIsHit(clientX0,clientY0,(getLeastAlpha()<<8)/alpha);
}

void VBitmapReflectionSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    out_rect.x0=x0;
    out_rect.y0=y0+m_ymin;
    out_rect.x1=x0+m_canvas.getWidth();
    out_rect.y1=y0+m_ymax;
}

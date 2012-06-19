//hZoom.h
//缩放算法


#ifndef _hZoom_h_
#define _hZoom_h_
#include "hColor32.h"
#include "hFilter.h"


template<class IFilter,class ISetPixelOp>
void zoom(const Pixels32Ref& dstPic,const Pixels32Ref& srcPic,ISetPixelOp& op){
    if (  (0>=dstPic.width)||(0>=dstPic.height)
        ||(0>=srcPic.width)||(0>=srcPic.height)) return;
    
    int xrIntFloat_16=((srcPic.width)<<16)/dstPic.width+1; 
    int yrIntFloat_16=((srcPic.height)<<16)/dstPic.height+1;
    const long borderSize=IFilter::getBorderSize();
    
    if (borderSize<=0){
        //近邻取样插值
        Color32* dstLine = dstPic.pdata;
        int srcy_16 = 0;
        for (int y = 0; y < dstPic.height; ++y) {
            IFilter::fastLine(dstLine,dstPic.width,srcPic,0,srcy_16,xrIntFloat_16,0,op);
            srcy_16 += yrIntFloat_16;
            dstPic.nextLine(dstLine);
        }
        return;
    }
    
    const int csDErrorX=-(1<<15)+(xrIntFloat_16>>1);
    const int csDErrorY=-(1<<15)+(yrIntFloat_16>>1);
    
    //计算出需要特殊处理的边界
    int border_y0=(((borderSize-1)<<16)-csDErrorY)/yrIntFloat_16+1;//y0+y*yr>=(borderSize-1); y0=csDErrorY => y>=(borderSize-1-csDErrorY)/yr
    if (border_y0>=dstPic.height) border_y0=dstPic.height;
    int border_x0=(((borderSize-1)<<16)-csDErrorX)/xrIntFloat_16+1;   
    if (border_x0>=dstPic.width ) border_x0=dstPic.width; 
    int border_y1=(((srcPic.height-1-borderSize)<<16)-csDErrorY)/yrIntFloat_16+1; //y0+y*yr<=(height-1-borderSize) => y<=(height-1-borderSize-csDErrorY)/yr
    if (border_y1<border_y0) border_y1=border_y0;
    int border_x1=(((srcPic.width-1-borderSize)<<16)-csDErrorX)/xrIntFloat_16+1; 
    if (border_x1<border_x0) border_x1=border_x0;
    
    Color32* dstLine=dstPic.pdata;
    int srcy_16=csDErrorY;
    int y;
    for (y=0;y<border_y0;++y)
    {
        IFilter::borderLine(dstLine,dstPic.width,srcPic,csDErrorX,srcy_16,xrIntFloat_16,0,op);//border
        srcy_16+=yrIntFloat_16;
        dstPic.nextLine(dstLine);
    }
    for (y=border_y0;y<border_y1;++y)
    {
        filterLine<IFilter,ISetPixelOp>(dstLine,0,border_x0,border_x1,dstPic.width,srcPic,csDErrorX,srcy_16,xrIntFloat_16,0,op);
        srcy_16+=yrIntFloat_16;
        dstPic.nextLine(dstLine);
    }
    for (y=border_y1;y<dstPic.height;++y)
    {
        IFilter::borderLine(dstLine,dstPic.width,srcPic,csDErrorX,srcy_16,xrIntFloat_16,0,op);//border
        srcy_16+=yrIntFloat_16;
        dstPic.nextLine(dstLine);
    }
}


#endif //_hZoom_h_

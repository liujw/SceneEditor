//hFilter.h
//插值算法


#ifndef _hFilter_h_
#define _hFilter_h_
#include "hColor32.h"
//#include "hBlendAPixel.h"

//近邻取样插值

//近邻取样插值内部实现 快速 不检查边界
must_inline static Color32 nearestNeighborFast(const Pixels32Ref& srcPic,long srcx_16,long srcy_16){
    return srcPic.pixels(srcx_16>>16,srcy_16>>16);
}

//近邻取样插值 检查边界(进行边界饱和处理)
must_inline static Color32 nearestNeighborBorder(const Pixels32Ref& srcPic,long srcx_16,long srcy_16){
    return srcPic.getPixelsBorder(srcx_16>>16,srcy_16>>16);
}


//二次线性插值

//二次线性插值内部实现 快速 不检查边界
#ifdef MMX_ACTIVE
#pragma warning(disable:4799)
must_inline static UInt32 bilinearFastBase(const Color32* PColor,long byte_width,UInt32 u_8,UInt32 v_8)
{        
    UInt32 result;
    asm
    {    
        MOVD      MM5,u_8
        MOVD      MM6,v_8
        mov       eax,byte_width
        mov       edx,PColor
        PXOR      mm7,mm7
        add       eax,edx
        
        MOVD         MM2,dword ptr [eax]  //MM2=Color1
        MOVD         MM0,dword ptr [eax+4]//MM0=Color3
        PUNPCKLWD    MM5,MM5
        PUNPCKLWD    MM6,MM6
        MOVD         MM3,dword ptr [edx]  //MM3=Color0
        MOVD         MM1,dword ptr [edx+4]//MM1=Color2
        PUNPCKLDQ    MM5,MM5 //mm5=u_8
        PUNPCKLBW    MM0,MM7
        PUNPCKLBW    MM1,MM7
        PUNPCKLBW    MM2,MM7
        PUNPCKLBW    MM3,MM7
        PSUBw        MM0,MM2
        PSUBw        MM1,MM3
        PSLLw        MM2,8
        PSLLw        MM3,8
        PMULlw       MM0,MM5
        PMULlw       MM1,MM5
        PUNPCKLDQ    MM6,MM6 //mm6=v_8
        PADDw        MM0,MM2
        PADDw        MM1,MM3
        
        PSRLw        MM0,8
        PSRLw        MM1,8
        PSUBw        MM0,MM1
        PSLLw        MM1,8
        PMULlw       MM0,MM6
        PADDw        MM0,MM1
        
        PSRLw        MM0,8
        PACKUSwb     MM0,MM7
        movd      result,MM0 
        //emms
    }
    return result;
}	
#pragma warning(default:4799)

#else
//使用了同时运算两路颜色分量的优化
must_inline static UInt32 bilinearFastBase(const Color32* PColor,long byte_width,UInt32 u_8,UInt32 v_8)
{  
    const UInt32 MASK=0x00FF00FF;
    UInt32 pm3_8=(u_8*v_8)>>8;
    UInt32 pm2_8=u_8-pm3_8;
    UInt32 pm1_8=v_8-pm3_8;
    UInt32 pm0_8=256-pm1_8-pm2_8-pm3_8;
    
    UInt32 Color=PColor[0].argb;
    UInt32 BR=(Color & MASK)*pm0_8;
    UInt32 GA=((Color>>8) & MASK)*pm0_8;
    Color=PColor[1].argb;
    BR+=(Color & MASK)*pm2_8;
    GA+=((Color>>8) & MASK)*pm2_8;
    (UInt8*&)PColor+=byte_width;
    Color=PColor[0].argb;
    BR+=(Color & MASK)*pm1_8;
    GA+=((Color>>8) & MASK)*pm1_8;
    Color=PColor[1].argb;
    BR+=(Color & MASK)*pm3_8;
    GA+=((Color>>8) & MASK)*pm3_8;
    
    return (GA & (MASK<<8))|((BR>>8) & MASK);
}
#endif


must_inline static Color32 bilinearFast(const Pixels32Ref& srcPic,long srcx_16,long srcy_16){
    const Color32* PColor=&srcPic.pixels(srcx_16 >> 16,srcy_16 >> 16);
    long u_8=(srcx_16>>8) & 0xFF;
    long v_8=(srcy_16>>8) & 0xFF;
    return Color32(bilinearFastBase(PColor,srcPic.byte_width,u_8,v_8));
}

//二次线性插值 检查边界(进行边界饱和处理)
inline static Color32 bilinearBorder(const Pixels32Ref& srcPic,long srcx_16,long srcy_16){
    long x=(srcx_16>>16);
    long y=(srcy_16>>16);
    Color32 data[2*2];
    data[0]=srcPic.getPixelsBorder(x,y);
    data[1]=srcPic.getPixelsBorder(x+1,y);
    data[2]=srcPic.getPixelsBorder(x,y+1);
    data[3]=srcPic.getPixelsBorder(x+1,y+1);
    long u_8=(srcx_16>>8) & 0xFF;
    long v_8=(srcy_16>>8) & 0xFF;
    return Color32(bilinearFastBase(&data[0],2*sizeof(Color32),u_8,v_8));
}

//三次卷积插值

extern const long* SinXDivX_Table_8;

Color32 bicubicFast(const Pixels32Ref& pic,const long srcx_16,const long srcy_16);
Color32 bicubicBorder(const Pixels32Ref& pic,const long srcx_16,const long srcy_16);

template<class ISetPixelOp>	
void nearestNeighborLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op) {
    if (yrIntFloat_16==0) {
        Color32* pSrcLine = &srcPic.pixels(0,srcy_16 >> 16);
        for (long x = 0; x < width; ++x) {
            op.set_pixel(&dstLine[x],pSrcLine[srcx_16 >> 16]);
            srcx_16 += xrIntFloat_16;
        }
    } else {
        for (int x=0;x<width;++x){
            op.set_pixel(&dstLine[x],srcPic.getPixel(srcx_16>>16,srcy_16>>16));
            srcx_16+=xrIntFloat_16;
            srcy_16+=yrIntFloat_16;
        }
    }
    do_emms();
}



//////////////////////////////////////////////////////

template<class ISetPixelOp>	
static void bilinearFastLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op) {
    if (yrIntFloat_16==0){
        const Color32* pSrcLine = &srcPic.pixels(0,srcy_16 >> 16);
        long srcbyte_with=srcPic.byte_width;
        long v_8=(srcy_16>>8) & 0xFF;
        for (long x = 0; x < width; ++x) {			
            long u_8=(srcx_16>>8) & 0xFF;
            op.set_pixel(&dstLine[x],Color32(bilinearFastBase(&pSrcLine[srcx_16 >> 16],srcbyte_with,u_8,v_8)));
            srcx_16+= xrIntFloat_16;	
        }
    } else {
        for (int x=0;x<width;++x){
            op.set_pixel(&dstLine[x],bilinearFast(srcPic,srcx_16,srcy_16));
            srcx_16+=xrIntFloat_16;
            srcy_16+=yrIntFloat_16;
        }
    }
    do_emms();
}
template<class ISetPixelOp>	
static void bilinearBorderLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op) {
    for (long x = 0; x < width; ++x) {			
        op.set_pixel(&dstLine[x],bilinearBorder(srcPic,srcx_16,srcy_16));
        srcx_16+=xrIntFloat_16;	
        srcy_16+=yrIntFloat_16;
    }
    do_emms();
}	

/*must_inline static long isCheck4Alpha(const Color32* c0,const Color32* c1){
 return (c0[0].argb | c0[1].argb | c1[0].argb | c1[1].argb )>>24;
 }
 template<> 	
 static void bilinearFastLine<TSetPixelBlend>(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,TSetPixelBlend& op) {
 if (yrIntFloat_16==0){
 const Color32* pSrcLine = &srcPic.pixels(0,srcy_16 >> 16);
 long srcbyte_with=srcPic.byte_width;
 long v_8=(srcy_16>>8) & 0xFF;
 for (long x = 0; x < width; ++x) {
 const Color32* PColor=&pSrcLine[srcx_16 >> 16];
 if (isCheck4Alpha(PColor,(Color32*)((UInt8*)PColor+srcbyte_with))){
 long u_8=(srcx_16>>8) & 0xFF;
 op.set_pixel(&dstLine[x],Color32(bilinearFastBase(PColor,srcbyte_with,u_8,v_8)));
 }
 srcx_16+= xrIntFloat_16;	
 }
 } else {
 long srcbyte_with=srcPic.byte_width;
 for (int x=0;x<width;++x){
 const Color32* PColor=&srcPic.pixels(srcx_16 >> 16,srcy_16 >> 16);
 if (isCheck4Alpha(PColor,(Color32*)((UInt8*)PColor+srcbyte_with))){
 long u_8=(srcx_16>>8) & 0xFF;
 long v_8=(srcy_16>>8) & 0xFF;
 op.set_pixel(&dstLine[x], Color32(bilinearFastBase(PColor,srcbyte_with,u_8,v_8)));
 }
 srcx_16+=xrIntFloat_16;
 srcy_16+=yrIntFloat_16;
 }
 }
 do_emms();
 }
 template<> 	
 static void bilinearBorderLine<TSetPixelBlend>(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,TSetPixelBlend& op) {
 for (long x = 0; x < width; ++x) {	
 {
 long x=(srcx_16>>16);
 long y=(srcy_16>>16);
 Color32 data[2*2];
 data[0]=srcPic.getPixelsBorder(x,y);
 data[1]=srcPic.getPixelsBorder(x+1,y);
 data[2]=srcPic.getPixelsBorder(x,y+1);
 data[3]=srcPic.getPixelsBorder(x+1,y+1);
 if (isCheck4Alpha(&data[0],&data[2])){
 long u_8=(srcx_16>>8) & 0xFF;
 long v_8=(srcy_16>>8) & 0xFF;
 op.set_pixel(&dstLine[x], Color32(bilinearFastBase(&data[0],2*sizeof(Color32),u_8,v_8)));
 }
 
 }
 srcx_16+=xrIntFloat_16;	
 srcy_16+=yrIntFloat_16;
 }
 do_emms();
 }	
 */




template<class ISetPixelOp>	
static void bicubicFastLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op) {
    for (long x = 0; x < width; ++x) {			
        op.set_pixel(&dstLine[x],bicubicFast(srcPic,srcx_16,srcy_16));
        srcx_16+=xrIntFloat_16;	
        srcy_16+=yrIntFloat_16;
    }
    do_emms();
}	

template<class ISetPixelOp>	
static void bicubicBorderLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op) {
    for (long x = 0; x < width; ++x) {			
        op.set_pixel(&dstLine[x],bicubicBorder(srcPic,srcx_16,srcy_16));
        srcx_16+=xrIntFloat_16;	
        srcy_16+=yrIntFloat_16;
    }
    do_emms();
}	

//tempalte struct IFilter{
//  enum { BorderSize=? };
//  static long    getBorderSize();
//	static Color32 filterFast(const Pixels32Ref& srcPic,long srcx_16,long srcy_16);
//	static Color32 filterBorder(const Pixels32Ref& srcPic,long srcx_16,long srcy_16) ;
//  template<class ISetPixelOp>	
//  static void fastLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op);
//  template<class ISetPixelOp>	
//  static void borderLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op);
//};


struct TNearestNeighborFilter{
    enum { BorderSize=0 };
    must_inline static long    getBorderSize() { return BorderSize; }
    must_inline static Color32 filterFast(const Pixels32Ref& srcPic,long srcx_16,long srcy_16) { return nearestNeighborFast(srcPic,srcx_16,srcy_16);  }
    must_inline static Color32 filterBorder(const Pixels32Ref& srcPic,long srcx_16,long srcy_16)  { return nearestNeighborBorder(srcPic,srcx_16,srcy_16);  }
    template<class ISetPixelOp>	
    must_inline static void fastLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){
        nearestNeighborLine(dstLine,width,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op); }
    template<class ISetPixelOp>	
    must_inline static void borderLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){ }
};

struct TBilinearFilter{
    enum { BorderSize=1 };
    must_inline static long    getBorderSize() { return BorderSize; }
    must_inline static Color32 filterFast(const Pixels32Ref& srcPic,long srcx_16,long srcy_16) { return bilinearFast(srcPic,srcx_16,srcy_16);  }
    must_inline static Color32 filterBorder(const Pixels32Ref& srcPic,long srcx_16,long srcy_16)  { return bilinearBorder(srcPic,srcx_16,srcy_16);  }
    template<class ISetPixelOp>	
    must_inline static void fastLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){
        bilinearFastLine(dstLine,width,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op); }
    template<class ISetPixelOp>	
    must_inline static void borderLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){
        bilinearBorderLine(dstLine,width,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op); }
};

struct TBicubicFilter{
    enum { BorderSize=2 };
    must_inline static long    getBorderSize() { return BorderSize; }
    must_inline static Color32 filterFast(const Pixels32Ref& srcPic,long srcx_16,long srcy_16) { return bicubicFast(srcPic,srcx_16,srcy_16);  }
    must_inline static Color32 filterBorder(const Pixels32Ref& srcPic,long srcx_16,long srcy_16)  { return bicubicBorder(srcPic,srcx_16,srcy_16);  }
    template<class ISetPixelOp>	
    must_inline static void fastLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){
        bicubicFastLine(dstLine,width,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op); }
    template<class ISetPixelOp>	
    must_inline static void borderLine(Color32* dstLine, long width, const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){
        bicubicBorderLine(dstLine,width,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op); }
};

template<class IFilter,class ISetPixelOp>
void filterLine(Color32* dstLine,long dst_x0_boder,long dst_x0_in, long dst_x1_in,long dst_x1_boder, 
                const Pixels32Ref& srcPic,long srcx_16,long srcy_16,long xrIntFloat_16,long yrIntFloat_16,ISetPixelOp& op){
    long borderWidth0=dst_x0_in-dst_x0_boder;
    if (borderWidth0>0){
        IFilter::borderLine(&dstLine[dst_x0_boder],borderWidth0,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op);
        srcx_16+=borderWidth0*xrIntFloat_16;
        srcy_16+=borderWidth0*yrIntFloat_16;
    }
    long fastWidth=dst_x1_in-dst_x0_in;
    if (fastWidth>0)
    {
        IFilter::fastLine(&dstLine[dst_x0_in],fastWidth,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op);
        srcx_16+=fastWidth*xrIntFloat_16;
        srcy_16+=fastWidth*yrIntFloat_16;
    }
    long borderWidth1=dst_x1_boder-dst_x1_in;
    if (borderWidth1>0){
        IFilter::borderLine(&dstLine[dst_x1_in],borderWidth1,srcPic,srcx_16,srcy_16,xrIntFloat_16,yrIntFloat_16,op);
        //srcx_16+=borderWidth1*xrIntFloat_16;
        //srcy_16+=borderWidth1*yrIntFloat_16;
    }
    
}


#endif //_hFilter_h_

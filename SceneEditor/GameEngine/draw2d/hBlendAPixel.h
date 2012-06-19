//hBlendAPixel.h


#ifndef _hBlendAPixel_h_
#define _hBlendAPixel_h_

#include "hColor32.h"

const UInt32 BLEND_MASK=0x00FF00FF;
const UInt32 BLEND_G_MASK=0x0000FF00;

//alpha颜色混合  
//这里使用了两路颜色分量同时运算的优化策略 
must_inline static UInt32 _alphaBlendV(UInt32 dColor, UInt32 ra,UInt32 RB_Mul_Alpha,UInt32 AG_Mul_Alpha) {
    //result=(dColor*(255-sAlpha)+sColor*sAlpha)>>>8;
    UInt32 result_RB = (dColor & BLEND_MASK) * ra +RB_Mul_Alpha;
    UInt32 result_AG = ((dColor>> 8) & BLEND_MASK)  * ra + AG_Mul_Alpha;
    return ((result_RB>> 8) & BLEND_MASK)  | (result_AG & (BLEND_MASK<<8));
}

#ifdef MMX_ACTIVE
#pragma warning(disable:4799)
must_inline UInt32 alphaBlendV(UInt32 dColor, UInt32 sColor,UInt32 sAlpha)
{
    UInt32 result;
    asm
    {
        PXOR      MM7,MM7
        MOVD      MM0,sColor
        MOVD      MM2,dColor
        MOVD      MM1,sAlpha
        PUNPCKLBW MM0,MM7
        PUNPCKLBW MM2,MM7
        PUNPCKLWD MM1,MM1
        PSUBW     MM0,MM2
        PUNPCKLDQ MM1,MM1
        PSLLW     MM2,8
        PMULLW    MM0,MM1
        PADDW     MM2,MM0
        PSRLW     MM2,8
        PACKUSWB  MM2,MM7
        MOVD      result,MM2
    }
    return result;
}
#pragma warning(default:4799)

#else
must_inline static UInt32 alphaBlendV(UInt32 dColor, UInt32 sColor,UInt32 sAlpha) {
    //result=(dColor*(255-sAlpha)+sColor*sAlpha)>>>8;
    return _alphaBlendV(dColor,255 - sAlpha,(sColor & BLEND_MASK) * sAlpha, ((sColor>> 8) & BLEND_MASK)  * sAlpha);
}
#endif

//除以255的快速算法;  x 必须属于 [0..255*255]
must_inline static UInt32 fastDiv255(UInt32 x) {
    //x in [0..66298]  //66258>256*258
    const UInt32 kMulDiv255_23=((1<<23)+127)/255;
    return (x*kMulDiv255_23)>>23;
    //另一个实现 return (x+(x>>8)+1)>>8; //x in [0..255*255]
}

must_inline static UInt32 fastMixAlpha(UInt32 a0,UInt32 a1){
    return a0+a1-fastDiv255(a0*a1);   
}
must_inline static UInt32 alphaBlendBufferV(UInt32 dColor, UInt32 sColor,UInt32 sAlpha) {
    //result=(dColor*(255-sAlpha)+sColor*sAlpha)>>>8;
    //result.a=d.a+s.a-(d.a*s.a>>7);
    UInt32 ra=255-sAlpha;
    UInt32 result_RB = (dColor & BLEND_MASK) * ra +(sColor & BLEND_MASK) * sAlpha;
    UInt32 result_G = (dColor & BLEND_G_MASK)  * ra + (sColor & BLEND_G_MASK)  * sAlpha;
    UInt32 result_A = fastMixAlpha(dColor>>24,sAlpha);
    return    ((result_RB>> 8) & BLEND_MASK)  
    | ((result_G>>8) & BLEND_G_MASK)
    | (result_A<<24);
}

must_inline static UInt32 alphaBlend(UInt32 dColor, UInt32 sColor,UInt32 sAlpha) {
    /*if (sAlpha >= 255)
     return sColor;
     else if (sAlpha == 0)
     return dColor;
     else
     return alphaBlendV(dColor,sColor,sAlpha);*/
    switch(sAlpha){
        case 255:
            return sColor;
        case 0:
            return dColor;
        default:
            return alphaBlendV(dColor,sColor,sAlpha);
    }
}
must_inline static UInt32 alphaBlend(UInt32 dColor, UInt32 sColor) {
    return alphaBlend(dColor,sColor,sColor >> 24);
}
must_inline static UInt32 alphaBlendWithAlpha(UInt32 dColor, UInt32 sColor,UInt32 aAlpha) {
    UInt32 sAlpha=(sColor>> 24); 
    if (sAlpha == 0)
        return dColor;
    else
        return alphaBlendV(dColor,sColor,(sAlpha*aAlpha)>>8);
}
must_inline static void alphaBlendTo(UInt32* dColor, UInt32 sColor,UInt32 sAlpha) {
    /*if (sAlpha >= 255)
     *dColor=sColor;
     else if (sAlpha == 0)
     return;
     else
     *dColor=alphaBlendV(*dColor,sColor,sAlpha);*/
    switch(sAlpha){
        case 255:
            *dColor=sColor;
            break;
        case 0:
            break;
        default:
            *dColor=alphaBlendV(*dColor,sColor,sAlpha);
            break;
    }
}
must_inline static void alphaBlendBufferTo(UInt32* dColor, UInt32 sColor,UInt32 sAlpha) {
    switch(sAlpha){
        case 255:
            *dColor=sColor;
            break;
        case 0:
            break;
        default:
            *dColor=alphaBlendBufferV(*dColor,sColor,sAlpha);
            break;
    }
}
must_inline static void alphaBlendTo(UInt32* dColor, UInt32 sColor) {
    alphaBlendTo(dColor,sColor,sColor >> 24);
}
must_inline static void alphaBlendBufferTo(UInt32* dColor, UInt32 sColor) {
    alphaBlendBufferTo(dColor,sColor,sColor >> 24);
}
must_inline static void alphaBlendToWithAlpha(UInt32* dColor, UInt32 sColor,UInt32 aAlpha) {
    UInt32 sAlpha=(sColor>> 24); 
    if (sAlpha == 0)
        return;
    else
        *dColor=alphaBlendV(*dColor,sColor,(sAlpha*aAlpha)>>8);
}

must_inline static  UInt32 fastToGray8(const UInt32 sColor){
    return ((sColor&0x000000FF) + ((sColor>>7)&(0x000000FF<<1)) + ((sColor>>16)&0x000000FF) )>>2;
}
must_inline static  UInt32 fastToColor32Gray(const UInt32 sColor){
    UInt32 gray=fastToGray8(sColor);
    return gray|(gray<<8)|(gray<<16);
}

must_inline static UInt32 mullColor(const Color32& s0,const Color32& s1){
    return ( ((s0.b)*(s1.b))>>8 )  | ( (((s0.g)*(s1.g))>>8)<<8 )   
    | ( (((s0.r)*(s1.r))>>8)<<16 ) | ( (((s0.a)*(s1.a))>>8)<<24 );
}

must_inline static void addTo(Color32* dst,const Color32& src) {
    dst->r=color_table[dst->r + src.r];
    dst->g=color_table[dst->g + src.g];
    dst->b=color_table[dst->b + src.b];
}
must_inline static void addTo(Color32* dst,const Color32& src,UInt32 alpha) {
    dst->r=color_table[dst->r + ((src.r*alpha)>>8)];
    dst->g=color_table[dst->g + ((src.g*alpha)>>8)];
    dst->b=color_table[dst->b + ((src.b*alpha)>>8)];
}
must_inline static void addToWithSrcAlpha(Color32* dst,const Color32& src) {
    addTo(dst,src,src.a);
}
must_inline static void addToWithSrcAlpha(Color32* dst,const Color32& src,UInt32 alpha) {
    alpha*=src.a;
    dst->r=color_table[dst->r + ((src.r*alpha)>>16)];
    dst->g=color_table[dst->g + ((src.g*alpha)>>16)];
    dst->b=color_table[dst->b + ((src.b*alpha)>>16)];
}


//template struct  ISetPixelFill
//{
//	void set_pixel(ColorType* pDstColor);
//};

//template struct  ISetPixelOp
//{
//	void set_pixel(ColorType* dst,const ColorType& src);
//};
template<class T>
struct TSetPixelTraitsBase{
    must_inline static bool isExpandCodeForLoop() { return false; } //是否可以使用代码展开优化
    must_inline static bool isOptimizeFillLine() { return false; }  //是否自己提供填充行函数
    must_inline static void optimizeFillLine(void* dst,Int32 width,T& op) { }
    must_inline static bool isOptimizeCopyLine() { return false; }  //是否自己提供拷贝行函数
    must_inline static void optimizeCopyLine(void* dst,const void* src,Int32 width,T& op) { }
};


//pixel的处理时特性,用于优化或者特殊处理
template<class T>
struct TSetPixelTraits:public TSetPixelTraitsBase<T>{
};


struct TSetPixelFill
{
    UInt32 m_fillColor;
    must_inline TSetPixelFill(const Color32& fillColor):m_fillColor(fillColor.argb){}
    must_inline void set_pixel(Color32* dst) { dst->argb=m_fillColor; }
    
    must_inline TSetPixelFill(const Gray8 fillColor):m_fillColor(fillColor){}
    must_inline void set_pixel(Gray8* dst) { *dst=m_fillColor; }
};
template<>
struct TSetPixelTraits<TSetPixelFill>{
    must_inline static bool isExpandCodeForLoop() { return true; }
    must_inline static bool isOptimizeFillLine() { return true; }
    must_inline static void optimizeFillLine(Gray8* dst,Int32 width,TSetPixelFill& op) {
        Gray8 fillData; op.set_pixel(&fillData);
        memset(dst,fillData,width);
    }
    must_inline static void optimizeFillLine(Color32* dst,Int32 width,TSetPixelFill& op) {
        Color32 fillData; op.set_pixel(&fillData);
        Color32* dstEnd =&dst[width];
        Color32* dstEndFast =&dst[(width & (~7))];
        for (; dst != dstEndFast; dst+=8){
            dst[0]=fillData; dst[1]=fillData; 
            dst[2]=fillData; dst[3]=fillData;
            dst[4]=fillData; dst[5]=fillData; 
            dst[6]=fillData; dst[7]=fillData;
        }
        for (; dst != dstEnd; ++dst)
            *dst=fillData;
    }
};

struct TSetPixelCopy
{
    static must_inline void set_pixel(Color32* dst,const Color32& src) { *dst=src; }
    static must_inline void set_pixel(Gray8* dst,const Gray8 src) { *dst=src; }
};
template<>
struct TSetPixelTraits<TSetPixelCopy>:public TSetPixelTraitsBase<TSetPixelCopy>{
    must_inline static bool isExpandCodeForLoop() { return true; }
    must_inline static bool isOptimizeCopyLine() { return true; }
    must_inline static void optimizeCopyLine(Gray8* dst,const Gray8* src,Int32 width,TSetPixelCopy& op) {
        memcpy(dst,src,width*sizeof(Gray8)); 
    }
    must_inline static void optimizeCopyLine(Color32* dst,const Color32* src,Int32 width,TSetPixelCopy& op) {
        memcpy(dst,src,width*sizeof(Color32)); 
    }
};


#define TSetPixelGray8Fill TSetPixelFill
#define TSetPixelGray8Copy TSetPixelCopy


struct TSetPixelFillColoring
{
    Color32 m_color;
    must_inline TSetPixelFillColoring(const Color32& color):m_color(color){  }
    must_inline void set_pixel(Color32* dst) {
        dst->argb=mullColor(*dst,m_color);
    }
};

struct TSetPixelFillAdd
{
    Color32 m_color;
    must_inline TSetPixelFillAdd(const Color32& color):m_color(color){ }
    must_inline void set_pixel(Color32* dst) {
        addTo(dst,m_color);
    }
};


struct TSetPixelCopyNoAlpha
{
    must_inline static void set_pixel(Color32* dst,const Color32& src) {
        dst->argb= (src.argb & 0x00FFFFFF);
    }
};
template<>
struct TSetPixelTraits<TSetPixelCopyNoAlpha>:public TSetPixelTraitsBase<TSetPixelCopyNoAlpha>{
    must_inline static bool isExpandCodeForLoop() { return true; }
};

struct TSetPixelCopyAlpha
{
    UInt32 m_alpha;
    must_inline TSetPixelCopyAlpha(const long alpha):m_alpha(alpha+1){  }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        UInt32 sColor = src.argb;
        UInt32 a = ((sColor>> 24)*m_alpha)>>8;
        dst->argb=(sColor & 0x00FFFFFF) | (a<<24);
    }
};


struct TSetPixelCopyColoring
{
    Color32 m_color;
    must_inline TSetPixelCopyColoring(const Color32& color):m_color(color){ }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        dst->argb=mullColor(src,m_color);
    }
};
struct TSetPixelCopyColoringAlpha
{
    Color32 m_color;
    unsigned long m_alpha;
    must_inline TSetPixelCopyColoringAlpha(const Color32& color,long alpha):m_color(color),m_alpha(alpha){  init(); }
    must_inline void init(){
        if (m_alpha<=0)
            m_color.a=0;
        else if (m_alpha<255)
            m_color.a=(UInt8)((m_color.a*m_alpha)/255);
    }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        dst->argb=mullColor(src,m_color);
    }
};

struct TSetPixelCopyAsGray
{
    must_inline static void set_pixel(Color32* dst,const Color32& src) {
        UInt32 sColor=src.argb;
        dst->argb=fastToColor32Gray(sColor)|(sColor&0xFF000000);
    }
};

struct TSetPixelCopyAsGrayNoAlpha
{
    must_inline static void set_pixel(Color32* dst,const Color32& src) {
        dst->argb=fastToColor32Gray(src.argb);
    }
};

struct TSetPixelCopyAsGrayAlpha
{
    long m_alpha;
    must_inline TSetPixelCopyAsGrayAlpha(long alpha):m_alpha(alpha){}
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        UInt32 alpha=(m_alpha*src.a)>>8;
        UInt32 sColor=src.argb;
        dst->argb=fastToColor32Gray(sColor)|(alpha<<24);
    }
};

struct TSetPixelBlendAsGray
{
    must_inline static void set_pixel(Color32* dst,const Color32& src) {
        UInt32 sColor=src.argb;
        UInt32 a=sColor>>24;
        if (a==255)
            dst->argb=fastToColor32Gray(sColor)|(0xFF000000);
        else if (a!=0){
            dst->argb=alphaBlendV(dst->argb,fastToColor32Gray(sColor)|(a<<24),a);
        }
    }
};

struct TSetPixelBlendAsGrayAlpha
{
    long m_alpha;
    must_inline TSetPixelBlendAsGrayAlpha(long alpha):m_alpha(alpha){}
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        UInt32 sColor=src.argb;
        UInt32 a=sColor>>24;
        if (a!=0){
            a=(m_alpha*a)>>8;
            dst->argb=alphaBlendV(dst->argb,fastToColor32Gray(sColor)|(a<<24),a);
        }
    }
};


struct TSetPixelBlend
{
    static must_inline void set_pixel(Color32* dst,const Color32& src) {  
        alphaBlendTo(&(dst->argb),src.argb);
    }
};

struct TSetPixelBlendBuffer
{
    static must_inline void set_pixel(Color32* dst,const Color32& src) {  
        alphaBlendBufferTo(&(dst->argb),src.argb,src.a);
    }
};

struct TSetPixelBlendColoring
{
    Color32 m_color;
    must_inline TSetPixelBlendColoring(const Color32& color):m_color(color){  }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        UInt32 sColor=src.argb;
        UInt32 sa=sColor>>24;
        if (sa!=0){
            sColor=mullColor(src,m_color);
            dst->argb=alphaBlendV(dst->argb,sColor,sa);
        }
    }
};

struct TSetPixelBlendAlpha
{
    long m_alpha;
    must_inline TSetPixelBlendAlpha(const long alpha):m_alpha(alpha){ }
    must_inline void set_pixel(Color32* dst,const Color32& src) {  
        alphaBlendToWithAlpha(&(dst->argb),src.argb,m_alpha); 
    }
};

struct TSetPixelBlendAlphaColoring
{
    Color32 m_color;
    long m_alpha;
    must_inline TSetPixelBlendAlphaColoring(const Color32& color,const long alpha):m_color(color),m_alpha(alpha){ }
    must_inline void set_pixel(Color32* dst,const Color32& src) {  
        UInt32 sColor=mullColor(src,m_color);
        alphaBlendToWithAlpha(&dst->argb,sColor,m_alpha);
    }
};

struct TSetPixelAdd
{
    must_inline static void set_pixel(Color32* dst,const Color32& src) {
        addTo(dst,src);
    }
};

struct TSetPixelAddColoring
{
    Color32 m_color;
    must_inline TSetPixelAddColoring(const Color32& color):m_color(color){ }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        Color32 tmp;
        tmp.argb=mullColor(src,m_color);
        addTo(dst,tmp);		
    }
};

struct TSetPixelBlendForAdd
{
    must_inline static void set_pixel(Color32* dst) {
        UInt32 dColor=dst->argb;
        UInt32 a = dColor >> 24;
        if (a != 0xFF) 
        {
            if (a == 0)
                dst->argb = 0xFF000000;
            else{
                UInt32 result_RB = (dColor & BLEND_MASK) * a;
                UInt32 result_G =  (dColor & 0x0000FF00) * a;
                dst->argb = (((result_RB & (BLEND_MASK<<8)) | (result_G & 0x00FF0000))>>8) | 0xFF000000;
            }
        }
    }
};

struct TSetPixelFillBlend
{
    UInt32 m_RB_MUL_Alpha;
    UInt32 m_AG_MUL_Alpha;
    UInt32 m_ra;
    must_inline void init(const Color32& color){
        UInt32 sColor=color.argb;
        UInt32 sAlpha=sColor>>24;
        m_RB_MUL_Alpha=(sColor & BLEND_MASK) * sAlpha;
        m_AG_MUL_Alpha= ((sColor>> 8) & BLEND_MASK)  * sAlpha;
        m_ra=255-sAlpha;
    }
    must_inline TSetPixelFillBlend(const Color32& color){ init(color); }
    must_inline void set_pixel(Color32* dst) {
        dst->argb=_alphaBlendV(dst->argb,m_ra,m_RB_MUL_Alpha,m_AG_MUL_Alpha);
    }
};


//////////////////////////////////////

struct TSetPixelCopyFromGray{
    must_inline static void set_pixel(Color32* dst,const Gray8 src) {
        dst->argb=Color32::packColor32Data(src,src,src);
        /*dst->b=src;
         dst->g=src;
         dst->r=src;
         dst->a=255;*/
    }		
};

struct TSetPixelCopyToGray{
    must_inline static void set_pixel(Gray8* dst,const Color32& src) {
        // 0.114*B + 0.587*G + 0.299*R => ( 7471*B + 38470*G + 19595*R )>>16
        //*dst=(src.b*7471+src.g*38470+src.r*19595)>>16;
        *dst=(src.b+src.g*2+src.r)>>2; 
    }		
};

//文本绘制相关
struct TSetPixelCopyToText{
    must_inline static void set_pixel(Gray8* dst,const Color32& src) {
        *dst=src.b;
    }
};
template<>
struct TSetPixelTraits<TSetPixelCopyToText>:public TSetPixelTraitsBase<TSetPixelCopyToText>{
    must_inline static bool isExpandCodeForLoop() { return true; }
};

struct TSetPixelLightBlend{
    UInt32 m_color;
    must_inline void init(const Color32& color){
        m_color=color.argb;
    }
    must_inline TSetPixelLightBlend(const Color32& color){ init(color); }
    must_inline void set_pixel(Color32* dst,const Gray8 src) {
        alphaBlendTo(&dst->argb,m_color,src);
    }
};
struct TSetPixelLightBlendAlpha{
    UInt32 m_color;
    UInt32 m_sAlpha;
    must_inline void init(const Color32& color){
        m_color=color.argb;
        m_sAlpha=color.a;
    }
    must_inline TSetPixelLightBlendAlpha(const Color32& color){ init(color); }
    must_inline void set_pixel(Color32* dst,const Gray8 src) {
        alphaBlendTo(&dst->argb,m_color,(src*m_sAlpha)>>8);
    }
};

struct TSetPixelLightAdd{
    Color32 m_color;
    must_inline void init(const Color32& color){
        m_color=color;
    }
    must_inline TSetPixelLightAdd(const Color32& color){ init(color); }
    must_inline void set_pixel(Color32* dst,const Gray8 src) {
        addTo(dst,m_color,src);
    }
};
struct TSetPixelLightAddAlpha{
    Color32 m_color;
    must_inline void init(const Color32& color){
        m_color=color;	
    }
    must_inline TSetPixelLightAddAlpha(const Color32& color){ init(color); }
    must_inline void set_pixel(Color32* dst,const Gray8 src) {
        addToWithSrcAlpha(dst,m_color,src);
    }
};

struct TSetPixelDarkleFill{
    long m_delLight;
    must_inline TSetPixelDarkleFill(const long delLight):m_delLight(delLight){}
    must_inline void set_pixel(Color32* dst) { 
        dst->r=color_table[dst->r-m_delLight]; 
        dst->g=color_table[dst->g-m_delLight]; 
        dst->b=color_table[dst->b-m_delLight]; 
    }
};
template<>
struct TSetPixelTraits<TSetPixelDarkleFill>:public TSetPixelTraitsBase<TSetPixelDarkleFill>{
    must_inline static bool isExpandCodeForLoop() { return true; }
};

struct TSetPixelBlendWithYAlphas{
    const UInt8* m_alphas;
    UInt8		 m_yAlpha;
    inline void setDrawNewLineBegin(long srcX0,long srcY){
        m_yAlpha=m_alphas[srcY];
    }
    
    must_inline TSetPixelBlendWithYAlphas(const UInt8* alphas):m_alphas(alphas){ }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        alphaBlendToWithAlpha(&dst->argb,src.argb,m_yAlpha);
    }
};

struct TSetPixelBlendWithYColorings{
    const Color32* m_colorings;
    Color32		 m_yColoring;
    inline void setDrawNewLineBegin(long srcX0,long srcY){
        m_yColoring=m_colorings[srcY];
    }
    
    must_inline TSetPixelBlendWithYColorings(const Color32* colorings):m_colorings(colorings){ }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        UInt32 sColor=mullColor(src,m_yColoring);
        alphaBlendTo(&(dst->argb),sColor);
    }
};

template<class TSetPixel>
struct TDrawNewLineBeginAttribute{
    must_inline static void setDrawNewLineBegin(TSetPixel& op,long srcX0,long srcY){ }	
};

template<>
struct TDrawNewLineBeginAttribute<TSetPixelBlendWithYAlphas>{
    must_inline static void setDrawNewLineBegin(TSetPixelBlendWithYAlphas& op,long srcX0,long srcY){
        op.setDrawNewLineBegin(srcX0,srcY);
    }	
};


struct TSetPixelUnionLayer
{
    must_inline TSetPixelUnionLayer(){ }
    must_inline long getColor(long d,long da,long s,long sa,long mulA_16){
        d*=da;
        long result= ( (d+s*sa-((d*sa)>>8)) * mulA_16 )>>16;
        //assert((result>=0)&&(result<=255));
        return result;
    }
    must_inline void set_pixel(Color32* dst,const Color32& src) {
        long da=dst->a;
        long sa=src.a;
        long a=da+sa-((da*sa+255)>>8);
        //assert((a>=0)&&(a<=255));
        dst->a=(UInt8)a;
        if (a!=0){
            long mulA_16=divAlpha_table_16[a];
            dst->r=(UInt8)getColor(dst->r,da,src.r,sa,mulA_16);
            dst->g=(UInt8)getColor(dst->g,da,src.g,sa,mulA_16);
            dst->b=(UInt8)getColor(dst->b,da,src.b,sa,mulA_16);
        }
    }
};

//////////////////
//颜色平均合成等

// isFast==true,时使用近似算法,并且可能不处理alpha通道

const UInt32 color7BitMask=0xFEFEFE;

template<bool isFast> 
must_inline static UInt32 color32_average_11(UInt32 color0,UInt32 color1){
    //return (a+b)/2;
    if (isFast){
        return ( (color0&color7BitMask)+(color1&color7BitMask) )>>1;
    }else{
        UInt32 AG =((color0 & 0xFF00FF00)>>1);
        UInt32 RB =(color0 & 0x00FF00FF);
        AG+=((color1 & 0xFF00FF00)>>1);
        RB+=(color1 & 0x00FF00FF);
        return ((AG & 0xFF00FF00)) | ((RB>>1) & 0x00FF00FF);
    }
}

template<bool isFast>
must_inline static UInt32 color32_average_1111(UInt32 color0,UInt32 color1,UInt32 color2,UInt32 color3){
    //return (a+b+c+d)/4;
    
    if (isFast){
        //return ( (color0&(0xFCFCFC))+(color1&(0xFCFCFC))+(color2&(0xFCFCFC))+(color3&(0xFCFCFC)) )>>2;
        return ( ( ((color0&color7BitMask)+(color1&color7BitMask))&(color7BitMask<<1) )
                +( ((color2&color7BitMask)+(color3&color7BitMask))&(color7BitMask<<1) ) )>>2;
    }else{
        UInt32 AG =((color0 & 0xFF00FF00)>>2);
        UInt32 RB =(color0 & 0x00FF00FF);
        AG+=((color1 & 0xFF00FF00)>>2);
        RB+=(color1 & 0x00FF00FF);           
        AG+=((color2 & 0xFF00FF00)>>2);
        RB+=(color2 & 0x00FF00FF);           
        AG+=((color3 & 0xFF00FF00)>>2);
        RB+=(color3 & 0x00FF00FF);
        return ((AG & 0xFF00FF00)) | ((RB>>2) & 0x00FF00FF);
    }
}


template<bool isFast>
must_inline UInt32 color32_average_4221(const UInt32 color0,const UInt32 color1,const UInt32 color2,const UInt32 color3){
    //return (a*4+b*2+c*2+d)/9;
    
    const UInt32 s9=14;
    const UInt32 m9=((1<<s9)+5)/9;
    //x/9 => x*m9>>s9
    
    if (isFast){
        UInt32 cs = ( (color1&color7BitMask)+(color2&color7BitMask) )&(color7BitMask<<1);
        cs = ( ((color0&color7BitMask)<<2) + (cs<<1) );
        UInt32 c =((cs & (0xFF<< 3))+(color3 & 0x0000FF))*m9>>s9;
        c |=      ((cs & (0xFF<<11))+(color3 & 0x00FF00))*m9>>(s9+8)<<8;
        c |=     (((cs & (0xFF<<19))+(color3 & 0xFF0000))>>16)*m9>>s9<<16;
        return c;
    }else{
        UInt32 AG_=((color1 & 0xFF00FF00)>>8);
        UInt32 RB_=(color1 & 0x00FF00FF);
        AG_+=((color2 & 0xFF00FF00)>>8);
        RB_+=(color2 & 0x00FF00FF);
        UInt32 AG =((color0 & 0xFF00FF00)>>8);
        UInt32 RB =(color0 & 0x00FF00FF);
        AG =AG*4+((color3 & 0xFF00FF00)>>8);
        RB =RB*4+(color3 & 0x00FF00FF); 
        AG+=AG_*2;
        RB+=RB_*2;
        
        UInt32 c = (RB & 0x0000FFFF)*m9>>s9;
        c |=      ((RB & 0xFFFF0000)>>16)*m9>>s9<<16;
        c |=       (AG & 0x0000FFFF)*m9>>s9<<8;
        c |=      ((AG & 0xFFFF0000)>>16)*m9>>s9<<24;
        return c;
    }
}

template<bool isFast>
must_inline static UInt32 color32_average_9331(const UInt32 color0,const UInt32 color1,const UInt32 color2,const UInt32 color3){
    //return (a*9+b*3+c*3+d)/16;
    
    if (isFast){
        UInt32 c0 = ( (color1&color7BitMask)+(color2&color7BitMask) )&(color7BitMask<<1);
        UInt32 c1 = ( (c0>>1)+(color3&color7BitMask) )&(color7BitMask<<1);
        c0 = (c0+c1)&(color7BitMask<<2);
        c1 = color0&color7BitMask;
        c0= (c0 + (c1<<2))&(color7BitMask<<3);
        return ( (c1<<3)+c0 )>>4;
    }else{
        UInt32 AG_=((color1 & 0xFF00FF00)>>4);
        UInt32 RB_=(color1 & 0x00FF00FF);
        AG_+=((color2 & 0xFF00FF00)>>4);
        RB_+=(color2 & 0x00FF00FF);
        UInt32 AG =((color0 & 0xFF00FF00)>>4);
        UInt32 RB =(color0 & 0x00FF00FF);
        AG =AG*9+((color3 & 0xFF00FF00)>>4);
        RB =RB*9+(color3 & 0x00FF00FF); 
        AG+=AG_*3;
        RB+=RB_*3;
        return ((AG & 0xFF00FF00)) | ((RB>>4) & 0x00FF00FF);
    }
}

template<bool isFast>
must_inline static UInt32 color32_average_3311(const UInt32 color0,const UInt32 color1,const UInt32 color2,const UInt32 color3){
    //return (a*3+b*3+c+d)/8;
    
    if (isFast){
        UInt32 c0 = ( (color0&color7BitMask)+(color1&color7BitMask) )&(color7BitMask<<1);
        UInt32 c1 = ( (color2&color7BitMask)+(color3&color7BitMask) )&(color7BitMask<<1);
        return ( (c0<<1)+((c0+c1)&(color7BitMask<<2)) )>>3;
    }else{
        UInt32 AG_=((color0 & 0xFF00FF00)>>3);
        UInt32 RB_=(color0 & 0x00FF00FF);
        AG_+=((color1 & 0xFF00FF00)>>3);
        RB_+=(color1 & 0x00FF00FF);
        UInt32 AG =((color2 & 0xFF00FF00)>>3);
        UInt32 RB =(color2 & 0x00FF00FF);
        AG +=((color3 & 0xFF00FF00)>>3);
        RB +=(color3 & 0x00FF00FF); 
        AG+=AG_*3;
        RB+=RB_*3;
        return ((AG & 0xFF00FF00)) | ((RB>>3) & 0x00FF00FF);
    }
}

template<bool isFast>
must_inline static UInt32 color32_average_21(const UInt32 color0,const UInt32 color1){
    //return (a*2+b)/3;
    
    const UInt32 s3=11;
    const UInt32 m3=((1<<s3)+2)/3;
    //x/3 => x*m3>>s3
    
    UInt32 cs=color0<<1;
    UInt32 c =((cs & (0xFF<< 1))+(color1 & 0x0000FF))*m3>>s3;
    c |=      ((cs & (0xFF<< 9))+(color1 & 0x00FF00))*m3>>(s3+8)<<8;
    c |=     (((cs & (0xFF<<17))+(color1 & 0xFF0000))>>16)*m3>>s3<<16;
    if (!isFast){
        c |= ( ((color0 & 0xFF000000)>>23)+((color1 & 0xFF000000)>>24) )*m3>>s3<<24;
    }
    return c;
}




struct TFastZoomKernel_2To1
{
    template<bool isFast,long MaskNoEmpty>
    must_inline void tm_zoomKernel(Color32* _dcolor,long dstLineInc,const Color32* _scolor,long srcLineInc){
        UInt32*& dcolor=(UInt32*&)_dcolor;
        const UInt32*& scolor0=(const UInt32*&)_scolor;
        
        const UInt32* scolor1=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        dcolor[0]=color32_average_1111<true>(scolor0[0],scolor0[1],scolor1[0],scolor1[1]);
    }
};


struct TFastZoomKernel_1To2
{
    template<bool isFast,long MaskNoEmpty>
    must_inline void tm_zoomKernel(Color32* _dcolor,long dstLineInc,const Color32* _scolor,long srcLineInc){
        UInt32*& dcolor=(UInt32*&)_dcolor;
        const UInt32*& scolor0=(const UInt32*&)_scolor;
        
        dcolor[0]=scolor0[0];
        dcolor[1]=color32_average_11<isFast>(scolor0[0],scolor0[1]);
        const UInt32* scolor1=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=color32_average_11<isFast>(scolor0[0],scolor1[0]);
        dcolor[1]=color32_average_1111<isFast>(scolor0[0],scolor0[1],scolor1[0],scolor1[1]);
    }
};


struct TFastZoomKernel_3To2
{
    template<bool isFast,long MaskNoEmpty>
    must_inline void tm_zoomKernel(Color32* _dcolor,long dstLineInc,const Color32* _scolor,long srcLineInc){
        UInt32*& dcolor=(UInt32*&)_dcolor;
        const UInt32*& scolor0=(const UInt32*&)_scolor;
        
        const UInt32* scolor1=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        dcolor[0]=color32_average_4221<isFast>(scolor0[0],scolor0[1],scolor1[0],scolor1[1]);
        dcolor[1]=color32_average_4221<isFast>(scolor0[2],scolor0[1],scolor1[2],scolor1[1]);
        scolor0=(const UInt32*)((const UInt8*)scolor1+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=color32_average_4221<isFast>(scolor0[0],scolor0[1],scolor1[0],scolor1[1]);
        dcolor[1]=color32_average_4221<isFast>(scolor0[2],scolor0[1],scolor1[2],scolor1[1]);
    }
};



struct TFastZoomKernel_2To3
{
    template<bool isFast,long MaskNoEmpty>
    must_inline void tm_zoomKernel(Color32* _dcolor,long dstLineInc,const Color32* _scolor,long srcLineInc){
        UInt32*& dcolor=(UInt32*&)_dcolor;
        const UInt32*& scolor0=(const UInt32*&)_scolor;
        
        dcolor[0]=scolor0[0] | MaskNoEmpty;
        dcolor[1]=color32_average_11<isFast>(scolor0[0],scolor0[1]) | MaskNoEmpty;
        dcolor[2]=scolor0[1] | MaskNoEmpty;
        
        const UInt32* scolor1=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        UInt32 d0=color32_average_11<isFast>(scolor0[0],scolor1[0]);
        dcolor[0]=d0 | MaskNoEmpty;
        UInt32 d2=color32_average_11<isFast>(scolor0[1],scolor1[1]);
        dcolor[1]=color32_average_11<isFast>(d0,d2) | MaskNoEmpty;
        dcolor[2]=d2 | MaskNoEmpty;
        
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=scolor1[0] | MaskNoEmpty;
        dcolor[1]=color32_average_11<isFast>(scolor1[0],scolor1[1]) | MaskNoEmpty;
        dcolor[2]=scolor1[1] | MaskNoEmpty;
    }
};



struct TFastZoomKernel_4To3
{
    template<bool isFast,long MaskNoEmpty>
    must_inline void tm_zoomKernel(Color32* _dcolor,long dstLineInc,const Color32* _scolor,long srcLineInc){
        UInt32*& dcolor=(UInt32*&)_dcolor;
        const UInt32*& scolor0=(const UInt32*&)_scolor;
        
        const UInt32* scolor1=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        dcolor[0]=color32_average_9331<isFast>(scolor0[0],scolor0[1],scolor1[0],scolor1[1]);
        dcolor[1]=color32_average_3311<isFast>(scolor0[1],scolor0[2],scolor1[1],scolor1[2]);
        dcolor[2]=color32_average_9331<isFast>(scolor0[3],scolor0[2],scolor1[3],scolor1[2]);
        
        scolor0=(const UInt32*)((const UInt8*)scolor1+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=color32_average_3311<isFast>(scolor0[0],scolor1[0],scolor0[1],scolor1[1]);
        dcolor[1]=color32_average_1111<isFast>(scolor0[1],scolor0[2],scolor1[1],scolor1[2]);
        dcolor[2]=color32_average_3311<isFast>(scolor0[3],scolor1[3],scolor0[2],scolor1[2]);
        
        scolor1=scolor0;
        scolor0=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=color32_average_9331<isFast>(scolor0[0],scolor0[1],scolor1[0],scolor1[1]);
        dcolor[1]=color32_average_3311<isFast>(scolor0[1],scolor0[2],scolor1[1],scolor1[2]);
        dcolor[2]=color32_average_9331<isFast>(scolor0[3],scolor0[2],scolor1[3],scolor1[2]);
    }
};



struct TFastZoomKernel_3To4
{
    template<bool isFast,long MaskNoEmpty>
    must_inline void tm_zoomKernel(Color32* _dcolor,long dstLineInc,const Color32* _scolor,long srcLineInc){
        UInt32*& dcolor=(UInt32*&)_dcolor;
        const UInt32*& scolor0=(const UInt32*&)_scolor;
        
        dcolor[0]=scolor0[0];
        dcolor[1]=color32_average_21<isFast>(scolor0[1],scolor0[0]);
        dcolor[2]=color32_average_21<isFast>(scolor0[1],scolor0[2]);
        dcolor[3]=scolor0[2];
        
        const UInt32* scolor1=(const UInt32*)((const UInt8*)scolor0+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=color32_average_21<isFast>(scolor1[0],scolor0[0]);
        dcolor[1]=color32_average_4221<isFast>(scolor1[1],scolor1[0],scolor0[1],scolor0[0]);
        dcolor[2]=color32_average_4221<isFast>(scolor1[1],scolor1[2],scolor0[1],scolor0[2]);
        dcolor[3]=color32_average_21<isFast>(scolor1[2],scolor0[2]);
        
        scolor0=(const UInt32*)((const UInt8*)scolor1+srcLineInc);
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=color32_average_21<isFast>(scolor1[0],scolor0[0]);
        dcolor[1]=color32_average_4221<isFast>(scolor1[1],scolor1[0],scolor0[1],scolor0[0]);
        dcolor[2]=color32_average_4221<isFast>(scolor1[1],scolor1[2],scolor0[1],scolor0[2]);
        dcolor[3]=color32_average_21<isFast>(scolor1[2],scolor0[2]);
        
        (UInt8*&)dcolor+=dstLineInc;
        dcolor[0]=scolor0[0];
        dcolor[1]=color32_average_21<isFast>(scolor0[1],scolor0[0]);
        dcolor[2]=color32_average_21<isFast>(scolor0[1],scolor0[2]);
        dcolor[3]=scolor0[2];
    }
};




#endif //_hBlendAPixel_h_
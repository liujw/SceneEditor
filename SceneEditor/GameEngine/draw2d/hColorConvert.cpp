//hColorConvert.cpp

#include "hColorConvert.h"


template<class IPixelConvert,class TDstColor,class TSrcColor>
void tm_convertLine(TDstColor* dst,const TSrcColor* src,const IPixelConvert& op,const long width){
    long right4 = (width & (~3));
    long x = 0;
    for (; x < right4; x += 4) {
        op.convert_pixel(dst[x  ],src[x  ]);
        op.convert_pixel(dst[x+1],src[x+1]);
        op.convert_pixel(dst[x+2],src[x+2]);
        op.convert_pixel(dst[x+3],src[x+3]);
    }
    for (; x < width; ++x)
        op.convert_pixel(dst[x],src[x]);
}

struct TPixelConvert_Color32_to_Color32{
    must_inline void convert_pixel(Color32& dst,const Color32& src)const{
        dst=src;
    }
};
struct TPixelConvert_Color32NoAlpha_to_Color32{
    must_inline void convert_pixel(Color32& dst,const Color32& src)const{
        dst.argb=src.argb & 0xFF000000;
    }
};

struct Color24{
    UInt8  b;
    UInt8  g;
    UInt8  r;
};

struct TPixelConvert_Color24_to_Color32{
    must_inline void convert_pixel(Color32& dst,const Color24& src)const{
        dst=Color32(src.r,src.g,src.b);
    }
};

template<int rbit,int gbit,int bbit>
struct TColor16bit{
    UInt16 rgb16;
    must_inline UInt8 getR()const{ return (rgb16&( ((1<<rbit)-1)<<(gbit+bbit) ))>>(rbit+gbit+bbit-8); }
    must_inline UInt8 getG()const{ return (rgb16&( ((1<<gbit)-1)<<(bbit) ))>>(gbit+bbit-8); }
    must_inline UInt8 getB()const{ return (rgb16&( ((1<<bbit)-1) ))<<(8-bbit); }
};
typedef TColor16bit<5,6,5> Color16_565;
typedef TColor16bit<5,5,5> Color16_555;
typedef TColor16bit<6,5,5> Color16_655;

template<class TColor16bit>
struct TPixelConvert_TColor16bit_to_Color32{
    must_inline void convert_pixel(Color32& dst,const TColor16bit& src)const{
        dst=Color32(src.getR(),src.getG(),src.getB());
    }
};

typedef TPixelConvert_TColor16bit_to_Color32<Color16_565> TPixelConvert_Color16_565_to_Color32;
typedef TPixelConvert_TColor16bit_to_Color32<Color16_555> TPixelConvert_Color16_555_to_Color32;
typedef TPixelConvert_TColor16bit_to_Color32<Color16_655> TPixelConvert_Color16_655_to_Color32;

struct TPixelConvert_Gray8_to_Color32{
    must_inline void convert_pixel(Color32& dst,const Gray8 src)const{
        dst=Color32(src,src,src);
    }
};

struct TPixelConvert_Color8_to_Color32{
    const Color32* m_colorTable;
    TPixelConvert_Color8_to_Color32(const Color32* colorTable):m_colorTable(colorTable){}
    
    must_inline void convert_pixel(Color32& dst,const UInt8 src)const{
        dst=m_colorTable[src];
    }
};


template <class TDstColor>
void convertLine_Color4bit(TDstColor* dst,const void* _src,const long width,const TDstColor* colorTable){
    const UInt8* src=(const UInt8*)_src;
    for (long x=0;x<(width>>1);++x){
        unsigned int tmpByte=src[x];
        dst[0] =colorTable[tmpByte >> 4];
        dst[1] =colorTable[tmpByte & 0xF];
        dst=&dst[2];
    }
    if ((width&1)!=0) {
        unsigned int tmpByte=src[width>>1]; // is safe
        dst[0] =colorTable[tmpByte >> 4];
    }
}

template <class TDstColor>
void convertLine_Color1bit(TDstColor* dst,const void* _src,const long width,const TDstColor* colorTable){
    const UInt8* src=(const UInt8*)_src;
    for (long x=0;x<(width>>3);++x){
        unsigned int tmpByte=src[x];
        dst[0] =colorTable[ tmpByte >> 7];
        dst[1] =colorTable[(tmpByte >> 6) & 1];
        dst[2] =colorTable[(tmpByte >> 5) & 1];
        dst[3] =colorTable[(tmpByte >> 4) & 1];
        dst[4] =colorTable[(tmpByte >> 3) & 1];
        dst[5] =colorTable[(tmpByte >> 2) & 1];
        dst[6] =colorTable[(tmpByte >> 1) & 1];
        dst[7] =colorTable[tmpByte & 1];
        
        dst=&dst[8];
    }
    
    long borderWidth=width & 7;
    if (borderWidth>0){
        unsigned int tmpByte=src[width >> 3];// is safe
        for (long x=0;x<borderWidth;++x)
            dst[x]=colorTable[(tmpByte >> (7-x)) & 1];
    }
}


/////
struct TPixelConvert_Color32_to_Gray8{
    must_inline void convert_pixel(Gray8& dst,const Color32& src)const{
        dst=src.toGrayFast();
    }
};

struct TPixelConvert_Color24_to_Gray8{
    must_inline void convert_pixel(Gray8& dst,const Color24& src)const{
        dst=Color32::toGrayFast(src.r,src.g,src.b);
    }
};

template<class TColor16bit>
struct TPixelConvert_TColor16bit_to_Gray8{
    must_inline void convert_pixel(Gray8& dst,const TColor16bit& src)const{
        dst=Color32::toGrayFast(src.getR(),src.getG(),src.getB());
    }
};

typedef TPixelConvert_TColor16bit_to_Gray8<Color16_565> TPixelConvert_Color16_565_to_Gray8;
typedef TPixelConvert_TColor16bit_to_Gray8<Color16_555> TPixelConvert_Color16_555_to_Gray8;
typedef TPixelConvert_TColor16bit_to_Gray8<Color16_655> TPixelConvert_Color16_655_to_Gray8;

struct TPixelConvert_Gray8_to_Gray8{
    must_inline void convert_pixel(Gray8& dst,const Gray8 src)const{
        dst=src;
    }
};

struct TPixelConvert_Color8_to_Gray8{
    const UInt8* m_colorTable;
    TPixelConvert_Color8_to_Gray8(const UInt8* colorTable):m_colorTable(colorTable){}
    
    must_inline void convert_pixel(Gray8& dst,const UInt8 src)const{
        dst=m_colorTable[src];
    }
};



/////////////////////////////////////////////////////////

void convertLine_Color32bit_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Color32*)src,TPixelConvert_Color32_to_Color32(),width);
}
void convertLine_Color32bitNoAlpha_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Color32*)src,TPixelConvert_Color32NoAlpha_to_Color32(),width);
}
void convertLine_Color24bit_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Color24*)src,TPixelConvert_Color24_to_Color32(),width);
}
void convertLine_Color16bit_565_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Color16_565*)src,TPixelConvert_Color16_565_to_Color32(),width);
}
void convertLine_Color16bit_555_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Color16_555*)src,TPixelConvert_Color16_555_to_Color32(),width);
}
void convertLine_Color16bit_655_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Color16_655*)src,TPixelConvert_Color16_655_to_Color32(),width);
}
void convertLine_Gray8_to_Color32(Color32* dst,const void* src,const long width,const Color32*){
    tm_convertLine(dst,(const Gray8*)src,TPixelConvert_Gray8_to_Color32(),width);
}

void convertLine_Color8bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* colorTable){
    tm_convertLine(dst,(const UInt8*)src,TPixelConvert_Color8_to_Color32(colorTable),width);
}

void convertLine_Color4bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* colorTable){
    convertLine_Color4bit<Color32>(dst,src,width,colorTable);
}

void convertLine_Color1bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* colorTable){
    convertLine_Color1bit<Color32>(dst,src,width,colorTable);
}

void convertLine_Color32bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8*){
    tm_convertLine(dst,(const Color32*)src,TPixelConvert_Color32_to_Gray8(),width);
}
void convertLine_Color24bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8*){
    tm_convertLine(dst,(const Color24*)src,TPixelConvert_Color24_to_Gray8(),width);
}
void convertLine_Color16bit_565_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8*){
    tm_convertLine(dst,(const Color16_565*)src,TPixelConvert_Color16_565_to_Gray8(),width);
}
void convertLine_Color16bit_555_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8*){
    tm_convertLine(dst,(const Color16_555*)src,TPixelConvert_Color16_555_to_Gray8(),width);
}
void convertLine_Color16bit_655_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8*){
    tm_convertLine(dst,(const Color16_655*)src,TPixelConvert_Color16_655_to_Gray8(),width);
}
void convertLine_Gray8_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8*){
    tm_convertLine(dst,(const Gray8*)src,TPixelConvert_Gray8_to_Gray8(),width);
}

void convertLine_Color8bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* colorTable){
    tm_convertLine(dst,(const UInt8*)src,TPixelConvert_Color8_to_Gray8(colorTable),width);
}

void convertLine_Color4bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* colorTable){
    convertLine_Color4bit<Gray8>(dst,src,width,colorTable);
}

void convertLine_Color1bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* colorTable){
    convertLine_Color1bit<Gray8>(dst,src,width,colorTable);
}


static TConvertLine_Colorbit_to_Color32_Proc _convertLine_Colorbit_to_Color32_array[33]={
    0,&convertLine_Color1bit_to_Color32,0,0,&convertLine_Color4bit_to_Color32,0,0,0,&convertLine_Color8bit_to_Color32,0,
    0,0,0,0,0,&convertLine_Color16bit_555_to_Color32,&convertLine_Color16bit_565_to_Color32,0,0,0,
    0,0,0,0,&convertLine_Color24bit_to_Color32,0,0,0,0,0,
    0,0,&convertLine_Color32bit_to_Color32
};
static TConvertLine_Colorbit_to_Gray8_Proc _convertLine_Colorbit_to_Gray8_array[33]={
    0,&convertLine_Color1bit_to_Gray8,0,0,&convertLine_Color4bit_to_Gray8,0,0,0,&convertLine_Color8bit_to_Gray8,0,
    0,0,0,0,0,&convertLine_Color16bit_555_to_Gray8,&convertLine_Color16bit_565_to_Gray8,0,0,0,
    0,0,0,0,&convertLine_Color24bit_to_Gray8,0,0,0,0,0,
    0,0,&convertLine_Color32bit_to_Gray8
};

const TConvertLine_Colorbit_to_Color32_Proc* convertLine_Colorbit_to_Color32_array=&_convertLine_Colorbit_to_Color32_array[0];
const  TConvertLine_Colorbit_to_Gray8_Proc*  convertLine_Colorbit_to_Gray8_array=&_convertLine_Colorbit_to_Gray8_array[0];

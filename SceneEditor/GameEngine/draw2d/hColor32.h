//hColor32.h
#ifndef _hColor32_h_
#define _hColor32_h_
#include "../base/SysImportType.h"


//定点数
typedef Int32 IntFloat_16;
typedef Int32 IntFloat_8;

//执行颜色饱和的表
extern const unsigned char* color_table;

typedef UInt8 Gray8;

struct Color32
{
    union
    {
        UInt32   argb;
        struct
        {
            /*#ifdef __BIG_ENDIAN__
             UInt8  a;
             UInt8  r;
             UInt8  g;
             UInt8  b;*/
            //#else    // Little endian
            UInt8  b;
            UInt8  g;
            UInt8  r;
            UInt8  a;
            //#endif 
        };
    };
    must_inline Color32(){}
    must_inline Color32(const Color32& color32):argb(color32.argb){}
    must_inline explicit Color32(const UInt32 color32):argb(color32){}
    must_inline explicit Color32(unsigned long r8,unsigned long g8,unsigned long b8,unsigned long a8=255):argb(packColor32Data(r8,g8,b8,a8)){}
    must_inline static UInt32 packColor32Data(unsigned long r8,unsigned long g8,unsigned long b8,unsigned long a8=255) { return b8|(g8<<8)|(r8<<16)|(a8<<24);  }
    must_inline void setColorData(const Color32& color32)  { *this=color32;  }
    must_inline void setColorData(const UInt32 color32)  { argb=color32;  }
    must_inline void setColorData(unsigned long r8,unsigned long g8,unsigned long b8,unsigned long a8=255)  { argb=packColor32Data(r8,g8,b8,a8);  }
    must_inline bool operator ==(const Color32& color32) const { return argb==color32.argb; }
    must_inline bool operator !=(const Color32& color32) const{ return !((*this)==color32); }
    
    
    void toColorText(String& out_result)const;
    must_inline String toColorText()const{ String result; toColorText(result); return result; }
    
    bool fromColorText_try(const char* colorText,long strLength);
    must_inline bool fromColorText_try(const char* colorText){ return fromColorText_try(colorText,(long)strlen(colorText)); }
    must_inline bool fromColorText_try(const String& colorText){ return fromColorText_try(colorText.c_str(),(long)colorText.size()); }
    must_inline UInt8 toGray()const{  return (19595*r + 38470*g + 7471*b) >> 16; }  //Gray=0.299*R+0.587*G+0.114*B
    must_inline UInt8 toGrayFast()const{ return toGrayFast(r,g,b); }
    must_inline static UInt8 toGrayFast(UInt8 r,UInt8 g,UInt8 b) { return (r+(g+g)+b)>>2; }
};
const  Color32  cl32Empty			(0x00000000);
const  Color32  cl32Black			(0xFF000000);
const  Color32  cl32White			(0xFFFFFFFF);
const  Color32  cl32Red				(0xFFFF0000);
const  Color32  cl32Green			(0xFF00FF00);
const  Color32  cl32Blue			(0xFF0000FF);
const  Color32  cl32BlackHalf		(0x7F000000);
const  Color32  cl32WhiteHalf		(0x7FFFFFFF);
const  Color32  cl32RedHalf			(0x7FFF0000);
const  Color32  cl32GreenHalf		(0x7F00FF00);
const  Color32  cl32BlueHalf		(0x7F0000FF);

const  Color32  cl32ColoringMullEmpty	(0xFFFFFFFF);

////

template<class TColor>
struct TColorAttribute{
};

template<>
struct TColorAttribute<Color32>{
    must_inline	static const Color32& getEmptyColor() { return cl32Empty; }
    must_inline static void setAlpha(Color32& color,UInt8 alpha) { color.a=alpha; }
};

template<>
struct TColorAttribute<Gray8>{
    must_inline	static const Gray8 getEmptyColor() { return 0; }
    must_inline static void setAlpha(Gray8& color,UInt8 alpha) { }
};

//(1<<16)/alpha
extern const  long*  divAlpha_table_16;

extern const  Color32*  gray_paltette256;



template<class _TColor>
struct TPixelsRefBase{
public:
    typedef TPixelsRefBase<_TColor> TPixelsRef;
    typedef _TColor TColor;
    TColor*     pdata;        
    long        byte_width;   
    long        width;        
    long        height; 
    must_inline TPixelsRefBase()   :pdata(0),byte_width(0),width(0),height(0){}
    must_inline TPixelsRefBase(const TPixelsRef& ref) :pdata(ref.pdata),byte_width(ref.byte_width),width(ref.width),height(ref.height){}
    must_inline TPixelsRefBase(TColor* apdata,long awidth,long aheight,long abyte_width)  :pdata(apdata),width(awidth),height(aheight),byte_width(abyte_width){}
    must_inline void setNullRef()   { pdata=0; byte_width=0; width=0; height=0;  }
    must_inline void setDataRef(const TPixelsRef& ref)   { pdata=ref.pdata; byte_width=ref.byte_width; width=ref.width; height=ref.height; }
    
    must_inline TColor& pixels(const long x,const long y) const { return getLinePixels(y)[x];  }
    must_inline bool getIsInRect(int x,int y) const { return (x>=0)&&(x<width)&&(y>=0)&&(y<height); }
    must_inline TColor  getPixel(const long x,const long y) const { return pixels(x,y);}
    must_inline void  setPixel(const long x,const long y,const TColor& color) const { pixels(x,y)=color;}
    must_inline TColor* getLinePixels(const long y) const { return (TColor*) ( ((UInt8*)pdata) + byte_width*y );  }
    must_inline bool clipSubRef(long& x0, long& y0, long& x1, long& y1)const{
        if (x0<0) x0=0; if (x1>width) x1=width;
        if (y0<0) y0=0; if (y1>height) y1=height;
        if ( (x0>=width) || (x1<=0) || (y0>=height) || (y1<=0) || (x0>=x1) || (y0>=y1) ) 
            return false;
        return true;
    }
    must_inline void getSubRef_fast(long x0, long y0, long x1, long y1,TPixelsRef& result)const {
        result.pdata = &pixels(x0,y0);
        result.width = x1 - x0;
        result.height = y1 - y0;
        result.byte_width =byte_width;
    }
    must_inline TPixelsRef getSubRef(long x0, long y0, long x1, long y1)const {
        TPixelsRef result;
        if (getIsEmpty()) return result;
        //剪切
        if (!clipSubRef(x0, y0, x1, y1)) return result;
        getSubRef_fast(x0,y0,x1,y1,result);
        return result;
    }
    
    must_inline void swap(TPixelsRef& ref)   {  STD::swap(*this,ref); }
    must_inline bool getIsEmpty()const { return ((width<=0)||(height<=0)); }
    must_inline bool getIsEqual(const TPixelsRef& ref)const {  
        return (pdata==ref.pdata)&&(byte_width==ref.byte_width)&&(width==ref.width)&&(height==ref.height);
    }
    must_inline void reversal(){ if (this->height<=1) return; this->pdata=&this->pixels(0,this->height-1);  this->byte_width=-this->byte_width;   }//上下翻转
    must_inline TPixelsRef getReversalRef() const {  TPixelsRef result(*this); result.reversal(); return result;}
    /*void reversalColor(){
     TColor* pline0=this.pdata;
     TColor* pline1=getLinePixels(height-1);
     for (long cy=(height>>1);cy>=0;--cy){
     for (long x=0;x<width;++x){
     MyBase::swap(pline0[x],pline1[x]);
     }
     nextLine(pline0);
     prevLine(pline1);
     }		
     }*/
    must_inline void nextLine(TColor*& pline)const {  ((UInt8*&)pline)+=byte_width;    }
    must_inline void prevLine(TColor*& pline)const {  ((UInt8*&)pline)-=byte_width;    }
    must_inline void nextLine(TColor*& pline,const long dy)const {  ((UInt8*&)pline)+=(byte_width*dy);  }
    
    must_inline bool clipBorder(long& x, long& y)const{
        bool isIn = true;
        if (x < 0) { 
            isIn = false; x = 0;
        } else if (x >= width) { 
            isIn = false; x = width - 1;
        }
        if (y < 0) {
            isIn = false; y = 0;
        } else if (y >= height) {
            isIn = false; y = height - 1;
        }
        return isIn;
    }
    
    //获取一个点的颜色  当坐标超出区域的时候返回的颜色为最近的边界上的alpha置零的颜色值  //警告! 图片区域不能为空 速度很慢 
    must_inline TColor getPixelsBorder(long x, long y) const
    {
        bool isIn = clipBorder(x,y);
        TColor result = pixels(x,y);
        if (!isIn)
            TColorAttribute<TColor>::setAlpha(result,0);
        return result;
    }
};


typedef TPixelsRefBase<Color32> Pixels32Ref;

typedef TPixelsRefBase<Gray8> PixelsGray8Ref;




#endif //__hColor32_h_
//SysImportType.h

#ifndef _SysImportType_h_
#define _SysImportType_h_

#include "../../import/importInclude/SysImport.h"
#include "mmTimer.h"
#include "stdlib.h"
#include "ctype.h"

#ifdef max //liujw max min　系统已有定义
#undef max
#endif

#ifdef min
#undef min
#endif

class CheckAssertError:public Exception{
public:
    virtual const char* what() const throw() { return "class CheckAssertError"; }
};

inline void check_assert(bool value,const char* messgae=0){
    if (!value){
        if (messgae!=0)
            debug_out(messgae);
        throw new CheckAssertError();
    }
}

class InfoException:public Exception{
private:
    String m_error;
public:
    inline explicit InfoException():m_error("class InfoException Error."){}
    inline explicit InfoException(const char* error):m_error(error){}
    inline explicit InfoException(const String& error):m_error(error){}
    
    virtual const char* what() const throw() { return m_error.c_str(); }
    virtual ~InfoException() throw(){}
};


inline static size_t hashCode(const char* str,long strLength){
    size_t result = 2166136261U;
    while(strLength>0){
        result =result*131 +(*str);
        ++str;
        --strLength;
    }
    return result;
}

#ifdef _MSC_VER
namespace stdext{
    must_inline static size_t hash_value(const String& str){ return hashCode(str.c_str(),(long)str.size()); }
}
#else
namespace __gnu_cxx {
    template<>
    struct hash<String>
    {
        must_inline size_t operator()(const String& str) const{
            return hashCode(str.c_str(),(long)str.size());
        }
    };
}
#endif

namespace MyBase{
    template<typename _ForwardIterator, typename _StrictWeakOrdering>
    inline bool is_sorted(_ForwardIterator it_begin, _ForwardIterator it_end,  _StrictWeakOrdering comp){
        if (it_begin == it_end)
            return true;
        
        _ForwardIterator it_next(it_begin);
        for (++it_next; it_next != it_end; it_begin = it_next, ++it_next)
            if (comp(*it_next, *it_begin))
                return false;
        return true;
    }
    
    const double PI=3.1415926535897932384626433832795;
    
    template<class T>
    must_inline static const T abs(const T& a){
        if (a>=0)
            return a;
        else
            return -a;
    }
    
    template<class T>
    must_inline static const T sqr(const T& a){
        return a*a;
    }
    
    template<class T>
    must_inline static const T min(const T& a,const T& b){
        if (a<=b)
            return a;
        else
            return b;
    }
    template<class T>
    must_inline static const T max(const T& a,const T& b){
        if (a>=b)
            return a;
        else
            return b;
    }
    
    template<class T>
    must_inline static void swap(T& a,T& b){
        T tmp(a);
        a=b;
        b=tmp;
    }
    
    
    //数的整数次方
    template<class TValue>
    TValue intPower(const TValue x,long power){
        switch (power){
            case 0: return 1;
            case 1: return x;
            case 2: return x*x;
            case 3: return x*x*x;
            case 4: return sqr<TValue>(x*x);
            case 5: return sqr<TValue>(x*x)*x;
            case 6: return sqr<TValue>(x*x*x);
            case 7: return sqr<TValue>(x*x*x)*x;
            case 8: return sqr<TValue>(sqr<TValue>(x*x));
            case 9: return sqr<TValue>(sqr<TValue>(x*x))*x;
        }
        if (power<0) 
            return ((TValue)1)/intPower<TValue>(x,-power);
        long hpower=power>>1;
        if ((hpower<<1)==power)
            return sqr<TValue>(intPower<TValue>(x,hpower));
        else
            return sqr<TValue>(intPower<TValue>(x,hpower))*x;
    }
    
    /*long gcd(long a, long b){
     if (a == 0) return b;
     if (b == 0) return a;
     if (!(a & 1) && !(b & 1)) 
     return gcd(a>>1, b>>1) << 1;
     else if (!(b & 1)) 
     return gcd(a, b>>1);
     else if (!(a & 1)) 
     return gcd(a>>1, b);
     else 
     return gcd(abs(a - b), min(a, b));
     }*/
    
    
    static long gcd(long a,long b){
        if(a==b) 
            return a;
        if((a&1)==0) {
            if((b&1)==0)
                return gcd(a>>1,b>>1)<<1;
            else
                return gcd(a>>1,b);
        } else {
            if((b&1)==0)
                return gcd(a,b>>1);
            else
                return gcd(((a>b)?(a-b):(b-a))>>1,(a+b)>>1);
        }
    }
    
}//end namespace

must_inline static bool isMaybeEqual(const double a,const double b,const double abs_error){
    return (MyBase::sqr(a-b)<=MyBase::sqr(abs_error));
}
must_inline static bool isMaybeEqual(const float a,const float b,const float abs_error){
    return (MyBase::sqr(a-b)<=MyBase::sqr(abs_error));
}
must_inline static bool isMaybeEqual(const double a,const double b){
    return isMaybeEqual(a,b,0.0001);
}
must_inline static bool isMaybeEqual(const float a,const float b){
    return isMaybeEqual(a,b,(float)0.0001);
}

void SinCos(double angle,double* out_sin,double* out_cos);

void dataCopyToString(const unsigned char* data,long it0,long it1,String& out_str);

//find char 
long posEx(const char* str,long strLength,long pos0,const char value);
must_inline static long pos(const char* str,long strLength,const char value)				{ return posEx(str,strLength,0,value); }
must_inline static long pos(const char* str,const char value)						{ return posEx(str,(long)strlen(str),0,value); }
long posFromEnd(const char* str,long strLength,const char value);
must_inline static long posFromEnd(const char* str,const char value)				{ return posFromEnd(str,(long)strlen(str),value); }

long posExIgnoreCase(const char* str,long strLength,long pos0,const char value);
must_inline static long posIgnoreCase(const char* str,long strLength,const char value)		{ return posExIgnoreCase(str,strLength,0,value); }
must_inline static long posIgnoreCase(const char* str,const char value)				{ return posExIgnoreCase(str,(long)strlen(str),0,value); }
long posFromEndIgnoreCase(const char* str,long strLength,const char value);
must_inline static long posFromEndIgnoreCase(const char* str,const char value)		{ return posFromEndIgnoreCase(str,(long)strlen(str),value); }

//将头尾的0x09-0x0D、0x20删除
void trim(const char* strSrc,long strLength,String& out_str);
must_inline static String trim(const char* strSrc,long strLength) { String result; trim(strSrc,strLength,result); return result; }
must_inline static String trim(const String& strSrc) { return trim(strSrc.c_str(),(long)strSrc.size()); }
must_inline static void   trimTo(String& str) { trim(str.c_str(),(long)str.size(),str); }

//是否是空白字符
extern const UInt8* charIsSpaceBuf;// [-255..255]
must_inline static bool charIsSpace(char c){ return 0!=charIsSpaceBuf[(UInt8)c];}
//是否是字母
extern const UInt8* charIsAlphabetBuf;// [-255..255]
must_inline static bool charIsAlphabet(char c){ return 0!=charIsAlphabetBuf[(UInt8)c];}
//是否是数字
must_inline static bool charIsNumber(char c){ return (c>='0')&&(c<='9'); }

//是否是字母\数字\下划线_
extern const UInt8* charIsKeyCharBuf;// [-255..255]
must_inline static bool charIsKeyChar(char c){  return 0!=charIsKeyCharBuf[(UInt8)c]; }

//是否都是ascii字符,不含多字节字符
must_inline static bool isAsciiChars(const char* strSrc,int strLength){
    for (int i=0;i<strLength;++i){
        if (((UInt8)strSrc[i])>asciiCharMaxValue)
            return false;
    }
    return true;
}
must_inline static bool isAsciiChars(const char* strSrc){
    return isAsciiChars(strSrc, strlen(strSrc));
}
must_inline static bool isAsciiChars(const String& strSrc){
    return isAsciiChars(strSrc.c_str(), strSrc.size());
}


//遍历单个字符 考虑多字节
UInt32 getNextChar(const char*& text);

long getStrValueCount(const String& Values,const char spaceChar);
void getNextStrValue(String& Values,const char spaceChar,String& out_result);
must_inline static String getNextStrValue(String& Values,const char spaceChar){
    String result;
    getNextStrValue(Values,spaceChar,result);
    return result;
}

//字符串查找
//查找csSub是否包含在csSrc中，并返回开始位置  失败返回-1
long findStr(const char* csSrc, int srcLen, const char* csSub, int subLen);
long findStrIgnoreCase(const char* csSrc, int srcLen, const char* csSub, int subLen);
must_inline static long findStr(const char* csSrc, const char* csSub){ return findStr(csSrc,(long)strlen(csSrc), csSub,(long)strlen(csSub)); }
must_inline static long findStrIgnoreCase(const char* csSrc, const char* csSub){ return findStrIgnoreCase(csSrc,(long)strlen(csSrc), csSub,(long)strlen(csSub)); }
must_inline static long findStr(const String& csSrc, const String& csSub){ return findStr(&csSrc[0],(long)csSrc.size(), &csSub[0],(long)csSub.size()); }
must_inline static long findStrIgnoreCase(const String& csSrc, const String& csSub){ return findStrIgnoreCase(&csSrc[0],(long)csSrc.size(), &csSub[0],(long)csSub.size()); }

//字符查找 不考虑双字节问题 
must_inline static const char* findChar_fast(const char* csSrc,const char* csSrcEnd, const char sub){
    while (csSrc!=csSrcEnd){
        if (*csSrc!=sub)
            ++csSrc;
        else
            return csSrc;
    }
    return csSrcEnd;
}
must_inline static const char* findChar_fast_unsafe(const char* csSrc, const char sub){ //确信有"哨兵"位
    while (*csSrc!=sub)
        ++csSrc;
    return csSrc;
}

//字符串查找 不考虑双字节问题 
inline static const char* findStr_fast(const char* csSrc,const char* csSrcEnd, const char* csSub, int subLen){
    assert(subLen>0);
    const char sub=csSub[0];
    csSrc=findChar_fast(csSrc,csSrcEnd,sub);
    while (csSrcEnd-csSrc>=subLen){
        bool isEq=true;
        for (long i=1;i<subLen;++i){
            if (csSrc[i]!=csSub[i]){
                isEq=false;
                break;
            }
        }
        if (!isEq)
            csSrc=findChar_fast(csSrc+1,csSrcEnd,sub);
        else
            return csSrc;
    }
    return csSrcEnd;
}

bool strIsEqual(const char* str0, int strLength0, const char* str1, int strLength1);
bool strIsEqualIgnoreCase(const char* str0, int strLength0, const char* str1, int strLength1);
must_inline static bool strIsEqual(const String& str0, const String& str1) { return strIsEqual(&str0[0],(long)str0.size(),&str1[0],(long)str1.size()); }
must_inline static bool strIsEqualIgnoreCase(const String& str0, const String& str1) { return strIsEqualIgnoreCase(&str0[0],(long)str0.size(),&str1[0],(long)str1.size()); }

//替换
bool replaceStr(const String& str,String& out_str,const char* csSub, int subLen,const char* csNewSub, int newSubLen);

const char* getFileSuffix(const char* fileName,long strLength);
inline static const char* getFileSuffix(const char* fileName)  { return getFileSuffix(fileName,(long)strlen(fileName)); }

must_inline static bool isFileSuffixEqual(const char* afileSuffix,long aStrLength,const char* bfileSuffix,long bStrLength){
    return strIsEqualIgnoreCase(afileSuffix,aStrLength,bfileSuffix,bStrLength);
} 

const char* getFileNameNoPath(const char* oldfileNameAndPath,long strLength);

String getFilePath(const char* oldfileNameAndPath,long strLength);
#ifdef WINCE
W2String getFilePath(const UInt16* oldfileNameAndPath,long strLength);
#endif

inline static String getFilePath(const char* oldfileNameAndPath){
    long strLength=(long)strlen(oldfileNameAndPath);
    return getFilePath(oldfileNameAndPath,strLength);
}

inline static const char* getFileNameNoPath(const char* oldfileNameAndPath){
    long strLength=(long)strlen(oldfileNameAndPath); 
    return getFileNameNoPath(oldfileNameAndPath,strLength);
}


extern const char* charUpperCase_table;
must_inline static char upperCaseChar(const char c){
    return charUpperCase_table[(UInt8)c];
}
extern const char* charLowerCase_table;
must_inline static char lowerCaseChar(const char c){
    return charLowerCase_table[(UInt8)c];
}

void upperCase(char* str,long strLength);
void lowerCase(char* str,long strLength);

must_inline static void upperCase(char* str)	{ upperCase(str,(long)strlen(str)); }
must_inline static void upperCase(String& str)	{ upperCase(&str[0],(long)str.size()); }
must_inline static void lowerCase(char* str)	{ lowerCase(str,(long)strlen(str)); }
must_inline static void lowerCase(String& str)	{ lowerCase(&str[0],(long)str.size()); }

//file
#ifndef csNullFileHandle
#define csNullFileHandle 0
#endif
void* file_create(const char* fileName);
void* file_open_read(const char* fileName);
bool  file_exists(const char* fileName);
bool  path_exists(const char* pathName);
long get_file_size(void* fileHandle);
long file_seek(void* fileHandle,long newPos=-1);
long file_read(void* fileHandle,UInt8* dst,unsigned long readSize);
long file_write(void* fileHandle,const UInt8* src,unsigned long writeSize);
void file_close(void* fileHandle);
long file_writeMyData(const char* fileName,const UInt8* src,unsigned long writeSize);
long file_readMyData(const char* fileName,UInt8* dst,unsigned long readSize);



extern const UInt8* hexToInt_table;

//一个16进制字符转换为整数
must_inline static long AHexToInt(char hex){
    return hexToInt_table[(UInt8)hex];
}

//16进制字符串转换为整数
long HexToInt(const char* hex,long length);
long checkHexStrLength(const char* hex,long length);
must_inline static bool checkHexStr(const char* hex,long length){
    return (checkHexStrLength(hex,length)==length);
}

void IntToHex(unsigned long src,char* hex,long hexSize);
void DataToHex(const void* src,char* hex,long hexSize);
void hexStrToData(const String& hexStr, String& str);//十六进制字符串转换为字符串，默认两位为一个字符

long checkIntStrLength(const char* intStr,long length);

//浮点数转换为整数
//dot 小数点位置 
//size 字符串最小大小
void ftoa(double f,long dot,long size,String& out_str);
void itoa(long i,String& out_str);
void itoa64(Int64 i,String& out_str);

bool atof(const char* str,long strLength,double& out_f);
must_inline static bool atof(const char* str,double& out_f) { return atof(str,(long)strlen(str), out_f); }
must_inline static bool atof(const String& str,double& out_f) { return atof(&str[0],(long)str.size(), out_f); }
bool atoi(const char* str,long strLength,long& out_i);
must_inline static bool atoi(const char* str,long& out_i) { return atoi(str,(long)strlen(str), out_i); }
must_inline static bool atoi(const String& str,long& out_i) { return atoi(&str[0],(long)str.size(), out_i); }
bool atoi64(const char* str,long strLength,Int64& out_i);
must_inline static bool atoi64(const char* str,Int64& out_i) { return atoi64(str,(long)strlen(str), out_i); }
must_inline static bool atoi64(const String& str,Int64& out_i) { return atoi64(&str[0],(long)str.size(), out_i); }


must_inline static bool strToBool(const String& str){
    if (strIsEqualIgnoreCase(str, "1") || strIsEqualIgnoreCase(str, "true")) return true;
    return false;
}

must_inline static String boolToStr(bool bVal){
    if (bVal) return "1";
    return "0";
}

must_inline static String intToStr(long i){ String result; itoa(i,result); return result;}
must_inline static String int64ToStr(Int64 i){ String result; itoa64(i,result); return result;}

must_inline static int strToInt(const String& strVal){
    long nVal = 0;
    atoi(strVal, nVal);
    return nVal;
}

must_inline static int strToInt(const char* strVal,long strLength){
    long nVal = 0;
    atoi(strVal,strLength, nVal);
    return nVal;
}

must_inline static String floatToStr(const double& dVal){
    String strVal;
    ftoa(dVal, 4, 6, strVal);
    return strVal;
}

must_inline static double strToFloat(const String& strVal){
    double dVal;
    atof(strVal, dVal);
    return dVal;
}

must_inline static Int64 strToInt64(const String& str)
{
    Int64 result;
    if (atoi64(str.c_str(), result))
        return result;
    else return 0;	
}

String dateToStr(const TDateTime dateTime);
String timeToStr(const TDateTime dateTime);
String dateTimeToStr(const TDateTime dateTime);
bool strToDateTime(const String& str, TDateTime& dt);//格式必须如：2011-1-1 12:30:30.000
#ifdef WINCE
//内存未对齐数据的读写兼容
static must_inline void write4Byte(UInt32& dst,UInt32 src){
    UInt8* d=(UInt8*)&dst;
    d[0]=(UInt8)(src);
    d[1]=(UInt8)(src>>8);
    d[2]=(UInt8)(src>>16);
    d[3]=(UInt8)(src>>24);
}
static must_inline UInt32 read4Byte(const UInt32& src){
    UInt8* s=(UInt8*)&src;
    return s[0]|(s[1]<<8)|(s[2]<<16)|(s[3]<<24);
}
#else
static must_inline void write4Byte(UInt32& dst,UInt32 src){
    dst=src;
}
static must_inline UInt32 read4Byte(const UInt32& src){
    return src;
}
#endif
//截取指定长度的utf8编码的字符串，num为截取的数量长度
must_inline static void cutUtf8SpecifiedLenStr(String& str, const long num){
    utf8ToGbk(str.c_str(), str.c_str()+str.size(), &str);
    String outStr = "";
    long nCount = 0;
    for (long l = 0; l < str.size(); ++l)
    {
        if (nCount > num){
            break;
        }
        if (str[l] >= 0 || l == str.size() - 1){
            outStr += str[l];
            ++nCount;
        }
        else {
            outStr += str[l];
            outStr += str[l+1];
            ++l;
            ++nCount;
            /*
             if ((code >= 0xB0A1 && code<=0xF7FE)||(code>=0x8140&&code<=0xA0FE)||(code>=0xAA40&&code<=0xFEA0))
             {
             gbkToUtf8(str.c_str(), str.c_str()+str.size(), &str);
             std::cout<<"China Font:"<<str<<std::endl;
             }*/
        }
    }
    gbkToUtf8(outStr.c_str(), outStr.c_str()+outStr.size(), &str);
}
#endif //_SysImport_h_

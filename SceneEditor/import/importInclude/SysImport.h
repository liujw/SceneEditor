//SysImport.h

#ifndef _SysImport_h_
#define _SysImport_h_

#pragma warning(disable:4996)

#ifdef __DEPRECATED
#undef __DEPRECATED
#endif

#include "string.h" //memcpy
#include "assert.h" //assert

#ifdef _MSC_VER 
#define must_inline __forceinline 
typedef	__int64				Int64;
typedef	unsigned __int64	UInt64;
#else
#ifdef __GNUC__
#define must_inline __attribute__((always_inline)) 
#else
#define must_inline inline 
#endif
typedef	long long			Int64;
typedef	unsigned long long	UInt64;
#endif 

#ifdef __cplusplus 
extern "C" {
#endif
    
#if (defined(WINCE) && !defined(WINDOWS))
#define WINDOWS
#endif
#if (defined(WIN32) && !defined(WINDOWS))
#define WINDOWS
#endif
#if (defined(_WINDOWS) && !defined(WINDOWS))
#define WINDOWS
#endif
#if (defined(WINDOWS) && !defined(_WINDOWS))
#define _WINDOWS
#endif
#if (defined(WINDOWS) && !defined(WIN32_IPHONE))
#define WIN32_IPHONE
#endif
    
#if defined(WINCE) 
#define _SOUND_PALY_ONE_
#endif
    
    /*#if defined(WIN32) && (!defined(_WIN64)) && (!defined(WINCE))
     #ifndef asm 
     #define asm __asm 
     #endif
     #define MMX_ACTIVE 1
     must_inline static void do_emms() { asm{ emms }  }
     #else
     must_inline static void do_emms() {}
     #endif
     */
    must_inline static void do_emms() {}
    
    //base type
    typedef char   			Int8;
    typedef unsigned char   UInt8;
    typedef short  			Int16;
    typedef unsigned short  UInt16;
    //__LP64__ for macos x64
#if __LP64__
    typedef unsigned int		UInt32;
    typedef signed int          Int32;
#else
    typedef unsigned long       UInt32;
    typedef signed long         Int32;
#endif
    
#define asciiCharMaxValue 127
    
    typedef double TDateTime;
    TDateTime getNowTime_s();
    
    typedef
    struct TDateTimeData{
        Int32	year;
        Int8	month;
        Int8	day;
        Int8	hour;
        Int8	minute;
        double	second;
    } TDateTimeData;
    void dateTimeToDateTimeData(TDateTime dateTime,TDateTimeData* out_dateTimeData);
    bool dateTimeDataToDataTime(const TDateTimeData& dateTimeData, TDateTime& dt);	
    void debug_out(const char* str);
    
    
    extern int g_isVerticalScreenImport;
    extern const char* g_szDeviceToken;
    
    //const char* getMachineUniqueIdentifier();
    const char* getLanguageID();
    const char* getSystemName();
    const char* getSystemVersion();
    const char* getDeviceName();
    const char* getDeviceHardwareVersion();    
    
    const char* getDefaultFontName();
    
    void exitApp();
    
#ifdef WINDOWS
    struct _TAutoInitApp_sysImport{
        _TAutoInitApp_sysImport();
        ~_TAutoInitApp_sysImport();
    };
    static _TAutoInitApp_sysImport _autoInitApp_sysImport;
    
    wchar_t *ansiCharToUnicode(const char *buffer);
    char * unicodeToAnsiChar(wchar_t * wbuffer);
#endif
#ifdef WINCE
#define TEXTOUT_AA 4
#endif
    
#ifdef __cplusplus 
}
#endif


#include <string> //std::string
#include <vector>
#include <deque>
#include <algorithm>
#define Vector	std::vector
#define Deque	std::deque
typedef std::exception  Exception;
typedef std::string	String;                 //utf8
//typedef std::wstring	WString;
typedef std::basic_string<UInt16> W2String; //utf16
typedef std::basic_string<UInt32> W4String; //utf32
#ifdef __GNUC__
#include <hash_set.h> //可能有一个警告 这是由于C++标准重新命名了hash容器系列
#include <hash_map.h>
#define HASH_STD
#else
#include <hash_set>
#include <hash_map>
#ifdef _MSC_VER
#define HASH_STD stdext
#else
#define HASH_STD std
#endif
#endif
#define STD std

#include <map>
#include <set>
#define Map std::map
#define Set std::set
#define MultiMap std::multimap
#define MultiSet std::multiset


typedef Vector<String>		StringList;
#define HashMap	HASH_STD::hash_map
#define HashSet HASH_STD::hash_set
#define HashMultiMap HASH_STD::hash_multimap
#define HashMultiSet HASH_STD::hash_multiset

template<class TMap>
must_inline static const typename TMap::mapped_type* getMapPValue(const TMap& values,const typename TMap::key_type& key){
    typename TMap::const_iterator it(values.find(key));
    if (it!=values.end()){
        return &(it->second);
    }else
        return 0;
}
template<class TMap>
must_inline static typename TMap::mapped_type* getMapPValue(TMap& values,const typename TMap::key_type& key){
    typename TMap::iterator it(values.find(key));
    if (it!=values.end()){
        return &(it->second);
    }else
        return 0;
}

//#define _TestMemAlloc
#ifdef _TestMemAlloc
void* allocPixelsMemery(long memSize);
void  freePixelsMemery(void* pMem);
#else
static inline void* allocPixelsMemery(long memSize){ return new UInt8[memSize]; }
static inline void  freePixelsMemery(void* pMem){ delete[](UInt8*)pMem; }
#endif

void utf8ToGbk(const char* str,const char* strEnd,String* outStr);
void gbkToUtf8(const char* str,const char* strEnd,String* outStr);


#include "../../GameEngine/engine/VScreenScale.h"

#endif //_SysImport_h_
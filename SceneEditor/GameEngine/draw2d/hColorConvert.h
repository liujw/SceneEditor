//hColorConvert.h


#ifndef _hColorConvert_h_
#define _hColorConvert_h_

#include "hColor32.h"

void convertLine_Color32bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);
void convertLine_Color32bitNoAlpha_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);
void convertLine_Color24bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);
void convertLine_Color16bit_565_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);
void convertLine_Color16bit_555_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);
void convertLine_Color16bit_655_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);
void convertLine_Gray8_to_Color32(Color32* dst,const void* src,const long width,const Color32* nilvalue=0);

void convertLine_Color8bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* colorTable);
void convertLine_Color4bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* colorTable);
void convertLine_Color1bit_to_Color32(Color32* dst,const void* src,const long width,const Color32* colorTable);


void convertLine_Color32bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* nilvalue=0);
void convertLine_Color24bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* nilvalue=0);
void convertLine_Color16bit_565_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* nilvalue=0);
void convertLine_Color16bit_555_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* nilvalue=0);
void convertLine_Color16bit_655_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* nilvalue=0);
void convertLine_Gray8_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* nilvalue=0);

void convertLine_Color8bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* colorTable);
void convertLine_Color4bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* colorTable);
void convertLine_Color1bit_to_Gray8(Gray8* dst,const void* src,const long width,const Gray8* colorTable);

typedef void (*TConvertLine_Colorbit_to_Color32_Proc)(Color32* dst,const void* src,const long width,const Color32*);
typedef void (*TConvertLine_Colorbit_to_Gray8_Proc)(Gray8* dst,const void* src,const long width,const Gray8*);

extern const TConvertLine_Colorbit_to_Color32_Proc* convertLine_Colorbit_to_Color32_array;//[33];
extern const TConvertLine_Colorbit_to_Gray8_Proc* convertLine_Colorbit_to_Gray8_array;//[33];

///

#endif //_hColorConvert_h_
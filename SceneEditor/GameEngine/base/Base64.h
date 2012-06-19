#ifndef _Base64_h_
#define _Base64_h_

#include "SysImportType.h"

//获得编码后的输出字符大小(原数据大小)
must_inline static unsigned long base64_code_size(const unsigned long data_size){
    return (data_size+2)/3*4;
}


void base64_encode(const void* pdata,const unsigned long data_size,void* out_pcode);


enum B64ReultType{ 
    b64Result_OK=0,
    b64Result_CODE_SIZE_ERROR,
    b64Result_DATA_SIZE_SMALLNESS,
    b64Result_CODE_ERROR
};

must_inline static unsigned long base64_data_maxsize(const unsigned long code_size){
    return code_size/4*3;
}


B64ReultType base64_decode(const void* pcode,const unsigned long code_size,void* out_pdata,const unsigned long data_size,unsigned long* out_pwrited_data_size);


void base64_encode(const String& data,String& outCode);
void base64_encode(const void* pdata,const unsigned long data_size,String& outCode);

B64ReultType base64_decode(const String& code,String& out_data);

#endif
#include "Base64.h"

const unsigned char BASE64_CODE[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const unsigned char BASE64_PADDING='=';  //输入数据不足3的倍数时 输出字符后面填充'='号

inline void base64_addpaing(const unsigned char* input,const int bord_width,unsigned char* output)
{
    if (bord_width==1)
    {
        unsigned int input0=input[0];
        unsigned int output0=BASE64_CODE[   input0 >> 2 ];
        unsigned int output1=BASE64_CODE[  (input0 << 4) & 0x3F ];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | ((BASE64_PADDING<<16) | (BASE64_PADDING<<24));
    }
    else if (bord_width==2)
    {
        unsigned int input0=input[0];
        unsigned int input1=input[1];
        unsigned int output0=BASE64_CODE[   input0 >> 2 ];
        unsigned int output1=BASE64_CODE[ ((input0 << 4) | (input1 >> 4)) & 0x3F ];
        unsigned int output2=BASE64_CODE[  (input1 << 2) & 0x3F ];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (BASE64_PADDING<<24);
    }
}

//使用64字节的表
void base64_encode(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;
    
    for(;input+2<input_end;input+=3,output+=4)
    {
        unsigned int input0=input[0];
        unsigned int input1=input[1];
        unsigned int input2=input[2];
        unsigned int output0=BASE64_CODE[   input0 >> 2 ];
        unsigned int output1=BASE64_CODE[ ((input0 << 4) | (input1 >> 4)) & 0x3F ];
        unsigned int output2=BASE64_CODE[ ((input1 << 2) | (input2 >> 6)) & 0x3F ];
        unsigned int output3=BASE64_CODE[   input2 & 0x3F ];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (output3<<24);
    }
    
    base64_addpaing(input,(int)(input_end-input),output);
}



//Base64解码函数
B64ReultType base64_decode(const void* pcode,const unsigned long code_size,void* out_pdata,const unsigned long data_size,unsigned long* out_pwrited_data_size)
{
    const unsigned char DECODE_DATA_MAX = 64-1;
    const unsigned char DECODE_PADDING  = DECODE_DATA_MAX+2;
    const unsigned char DECODE_ERROR    = DECODE_DATA_MAX+3;
    
    static unsigned char BASE64_DECODE[256];
    static bool initialized=false;
    if(!initialized)
    {
        unsigned long i;
        for(i=0;i<256;++i) BASE64_DECODE[i]=DECODE_ERROR;
        for(i='A';i<='Z';++i) BASE64_DECODE[i]=(unsigned char)(i-'A');
        for(i='a';i<='z';++i) BASE64_DECODE[i]=(unsigned char)(i-'a'+26);
        for(i='0';i<='9';++i) BASE64_DECODE[i]=(unsigned char)(i-'0'+26*2);
        BASE64_DECODE['+']=26*2+10;
        BASE64_DECODE['/']=26*2+10+1;
        BASE64_DECODE['=']=DECODE_PADDING;
        initialized=true;
    }
    
    *out_pwrited_data_size=0;
    unsigned long code_node=code_size/4;
    if ((code_node*4)!=code_size)
        return b64Result_CODE_SIZE_ERROR;
    else if (code_node==0)
        return b64Result_OK;
    //code_node>0
    
    const unsigned char* input=(const unsigned char*)pcode;
    unsigned char*       output=(unsigned char*)out_pdata;
    unsigned long output_size=code_node*3;
    if (input[code_size-2]==BASE64_PADDING)
    {
        if (input[code_size-1]!=BASE64_PADDING)
            return b64Result_CODE_ERROR;
        output_size-=2;
    }
    else if (input[code_size-1]==BASE64_PADDING)
        --output_size;
    if (output_size>data_size) return b64Result_DATA_SIZE_SMALLNESS;
    
    const unsigned char* input_last_fast_node=&input[output_size/3*4];
    
    for(;input<input_last_fast_node;input+=4,output+=3)
    {
        unsigned int code0=BASE64_DECODE[input[0]];
        unsigned int code1=BASE64_DECODE[input[1]];
        unsigned int code2=BASE64_DECODE[input[2]];
        unsigned int code3=BASE64_DECODE[input[3]];
        if ( ((code0|code1)|(code2|code3)) <= DECODE_DATA_MAX )
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
            output[1]=(unsigned char)((code1<<4) + (code2>>2));
            output[2]=(unsigned char)((code2<<6) + code3);
        }
        else
            return b64Result_CODE_ERROR;
    }
    
    unsigned long bord_width=output_size%3;
    if (bord_width==1)
    {
        unsigned int code0=BASE64_DECODE[input[0]];
        unsigned int code1=BASE64_DECODE[input[1]];
        if ((code0|code1) <= DECODE_DATA_MAX)
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
        }
        else
            return b64Result_CODE_ERROR;
    }
    else if (bord_width==2)
    {
        unsigned int code0=BASE64_DECODE[input[0]];
        unsigned int code1=BASE64_DECODE[input[1]];
        unsigned int code2=BASE64_DECODE[input[2]];
        if ((code0|code1|code2) <= DECODE_DATA_MAX)
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
            output[1]=(unsigned char)((code1<<4) + (code2>>2));
        }
        else
            return b64Result_CODE_ERROR;
    }
    *out_pwrited_data_size=output_size;
    return b64Result_OK;
}

//////////////////////


void base64_encode(const String& data,String& outCode){
    unsigned long data_size=(unsigned long)data.size();
    outCode.resize(base64_code_size(data_size));
    base64_encode(&data[0],data_size,&outCode[0]);
}

void base64_encode(const void* pdata,const unsigned long data_size,String& outCode){
    outCode.resize(base64_code_size(data_size));
    base64_encode(pdata,data_size,&outCode[0]);
}

B64ReultType base64_decode(const String& code,String& out_data){
    unsigned long code_size=(unsigned long)code.size();
    String data;
    data.resize(base64_data_maxsize(code_size));
    unsigned long writed_data_size=0;
    B64ReultType result=base64_decode(&code[0],code_size,&data[0],(unsigned long)data.size(),&writed_data_size);
    if (result==b64Result_OK){
        data.resize(writed_data_size);
        out_data.swap(data);
    }
    return result;
}






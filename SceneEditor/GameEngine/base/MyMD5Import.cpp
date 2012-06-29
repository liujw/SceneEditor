//MyMD5Import.cpp

#include "MyMD5Import.h"
#include "md5.h"
#include "SysImportType.h"

static void MyMD5Init(md5_state_t *pms){
    /*
     pms->count[0] = pms->count[1] = 0;
     pms->abcd[0] = 0x67452301;
     pms->abcd[1] = T_MASK ^ 0x10325476;  //0xefcdab89
     pms->abcd[2] = T_MASK ^ 0x67452301;  //0x98badcfe
     pms->abcd[3] = 0x10325476;
     */
    memset(pms,0,sizeof(md5_state_t));
    pms->abcd[0] = 0x39023A3C;
    pms->abcd[1] = 0xECFDA8B9;  
    pms->abcd[2] = 0x89BAD3FE;  
    pms->abcd[3] = 0x8732765B;
}


const long md5DigestCount=16;
void getMyMD5StrBin8(const void* data,long byte_length,char* out_md5Str){
    md5_byte_t MD5Digest[md5DigestCount];
    for (long i=0;i<md5DigestCount;++i)
        MD5Digest[i]=(unsigned char)( i + 1);
    
    md5_state_t MD5Context;
    MyMD5Init(&MD5Context);
    md5_append(&MD5Context,(const md5_byte_t *)data,byte_length);
    md5_finish(&MD5Context,MD5Digest);
    
    *out_md5Str=MD5Digest[ 0];	++out_md5Str;
    *out_md5Str=MD5Digest[ 3];	++out_md5Str;
    *out_md5Str=MD5Digest[ 4];	++out_md5Str;
    *out_md5Str=MD5Digest[ 7];	++out_md5Str;
    *out_md5Str=MD5Digest[ 9];	++out_md5Str;
    *out_md5Str=MD5Digest[12];	++out_md5Str;
    *out_md5Str=MD5Digest[13];	++out_md5Str;
    *out_md5Str=MD5Digest[14];	++out_md5Str;
}

void getMyMD5StrHex16(const void* data,long byte_length,char* out_md5Str){
    char md5bin[8];
    getMyMD5StrBin8(data,byte_length,md5bin);
    for (long i=0;i<8;++i){
        IntToHex(md5bin[i],out_md5Str,2);
        out_md5Str+=2;
    }
}


void getMD5StdHex32(const void* data,long byte_length,char* out_md5Str){
    md5_byte_t MD5Digest[md5DigestCount];
    for (long i=0;i<md5DigestCount;++i)
        MD5Digest[i]= (unsigned char)(i + 1);
    
    md5_state_t MD5Context;
    md5_init(&MD5Context);
    md5_append(&MD5Context,(const md5_byte_t *)data,byte_length);
    md5_finish(&MD5Context,MD5Digest);
    
    for (long i=0;i<md5DigestCount;++i){
        IntToHex(MD5Digest[i],out_md5Str,2);
        out_md5Str+=2;
    }
}

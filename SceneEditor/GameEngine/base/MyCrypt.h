//MyCrypt.h

#ifndef _MyCrypt_h_
#define _MyCrypt_h_
#include "SysImportType.h"
#include "md5.h"

//TDesKey 密钥
struct TDesKey{
    UInt32 data[4];  //128bit
};


TDesKey getRandomDesKey(); //生成一个随机密钥
TDesKey strToDesKey(const String& strKey); //将一个字符串转成一个对应的密钥

void desEncrypt(const UInt8* pByteData,long dataSize,const TDesKey& key,UInt8* pOutData); //加密
must_inline static void desEncrypt(UInt8* pByteData,long dataSize,const TDesKey& key){
    desEncrypt(pByteData,dataSize,key,pByteData);
}

void desDecrypt(const UInt8* pByteData,long dataSize,const TDesKey& key,UInt8* pOutData); //解密
must_inline static void desDecrypt(UInt8* pByteData,long dataSize,const TDesKey& key){
    desDecrypt(pByteData,dataSize,key,pByteData);
}

#endif
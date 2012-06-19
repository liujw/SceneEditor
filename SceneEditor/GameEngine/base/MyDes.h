//MyDes.h

#ifndef _MyDes_h_
#define _MyDes_h_

#include "SysImportType.h"

struct TMyDesKey {
    UInt32 data[4];
};

void myDesEncrypt(const UInt8* pSrcData, UInt8* pOutdata, UInt32 dataSize, TMyDesKey& key);
void myDesDecrypt(const UInt8* pSrcData, UInt8* pOutdata, UInt32 dataSize, TMyDesKey key);

TMyDesKey desKeyByString(const String& Key);
String getMyEncryptData(const String& srcData, const String& key);
String getMyDecryptData(const String& srcData, const String& key);

String getMyEncryptData(const String& srcData, TMyDesKey key);
String getMyDecryptData(const String& srcData, TMyDesKey key);
String strToHexStr(const String& srcData);
String hexStrToStr(const String& srcData);
#endif
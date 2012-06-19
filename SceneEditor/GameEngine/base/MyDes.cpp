//MyDes.cpp
#include  "MyDes.h"

const UInt32 csMyDesRAND0 = 0x1D84D2EF;

must_inline static void myDesRound1(UInt32& _W, UInt32 X, UInt8 S)
{
    UInt32 W = _W ^ X;
    UInt32 w1 = (W << S);
    UInt32 w2 = W;
    w2 = (w2 >> (32 - S));
    W = w1 | w2;
    _W = W + X;
}

must_inline static void myDesROUND0(UInt32 rdata, TMyDesKey& key){
    myDesRound1(key.data[0], key.data[1] ^ rdata, 5);
    rdata = rdata + 36717559;
    myDesRound1(key.data[1], key.data[2] - rdata, 23);
    rdata = rdata + 39880843;
    myDesRound1(key.data[2], key.data[3] ^ rdata, 7);
    rdata = rdata + 52692169;
    myDesRound1(key.data[3], key.data[0] + rdata, 19);
}

void myDesEncrypt(const UInt8* pSrcData, UInt8* pOutdata, UInt32 dataSize, TMyDesKey& key){
    UInt32 rdata = csMyDesRAND0 ^ key.data[0];
    UInt32 borderSize = dataSize & 3;
    if (borderSize > 0){
        myDesROUND0(rdata, key);
    }
    for (UInt32 i = 0; i < borderSize; ++i){
        pOutdata[i] = ((signed char)(pSrcData[i]) + key.data[i]) ^ key.data[3 - i];
        rdata = ((rdata << 4) + (signed char)(pOutdata[i])) ^ key.data[rdata & 3];
    }
    UInt32 i = borderSize;
    while (i < dataSize){
        myDesROUND0(rdata, key);
        pOutdata[i + 0] = ((signed char)(pSrcData[i + 0]) + rdata + key.data[0]) ^ key.data[1];
        rdata = ((rdata << 4) - (signed char)(pOutdata[i + 0])) ^ key.data[3];
        pOutdata[i + 1] = ((signed char)(pSrcData[i + 1]) - rdata + key.data[1]) ^ key.data[2];
        rdata = ((rdata << 4) + (signed char)(pOutdata[i + 1])) - key.data[2];
        pOutdata[i + 2] = ((signed char)(pSrcData[i + 2]) + rdata - key.data[2]) ^ key.data[3];
        rdata = ((rdata << 4) - (signed char)(pOutdata[i + 2])) ^ key.data[1];
        pOutdata[i + 3] = ((signed char)(pSrcData[i + 3]) - rdata - key.data[3]) ^ key.data[0];
        rdata = ((rdata << 4) + (signed char)(pOutdata[i + 3])) + key.data[0];
        i += 4;
    }
}

TMyDesKey desKeyByString(const String& Key){
    TMyDesKey mDecData;
    const UInt8* mKey = (const UInt8*)&Key[0];
    String mData;
    UInt32 keySize = Key.size();
    mData.resize(keySize + 16);
    UInt8* pDst = (UInt8*)&mData[0];
    for (UInt32 i = 0; i < keySize; ++i){
        pDst[i] = mKey[i];
    }
    pDst[keySize] = (UInt8)keySize;
    mDecData.data[0] = 0xE5C3E49D;
    mDecData.data[1] = 0xC91092DC;
    mDecData.data[2] = 0x94473C52;
    mDecData.data[3] = 0xA63CB779;
    myDesEncrypt((const UInt8*)pDst, pDst, mData.size(), mDecData);
    return mDecData;
}

String getMyEncryptData(const String& srcData, const String& key){
    String destData = "";
    if (key.size() > 0){
        TMyDesKey desKey = desKeyByString(key);
        destData = getMyEncryptData(srcData, desKey);
    }
    return destData;
}

String getMyDecryptData(const String& srcData, const String& key){
    String destData = "";
    if (key.size() > 0){
        TMyDesKey desKey = desKeyByString(key);
        destData = getMyDecryptData(srcData, desKey);
    }
    return destData;
}


String getMyEncryptData(const String& srcData, TMyDesKey key){
    String destData = "";
    UInt32 srcDataLen = srcData.size();
    if (srcDataLen > 0){
        const UInt8* pSrcData = (const UInt8*)&srcData[0];
        destData.resize(srcDataLen);
        UInt8* pDestData = (UInt8*)&destData[0];
        myDesEncrypt(pSrcData, pDestData, srcDataLen, key);
    }
    return destData;
}

String getMyDecryptData(const String& srcData, TMyDesKey key){
    String destData = "";
    UInt32 srcDataLen = srcData.size();
    if (srcDataLen > 0){
        const UInt8* pSrcData = (const UInt8*)&srcData[0];
        destData.resize(srcDataLen);
        UInt8* pDestData = (UInt8*)&destData[0];
        myDesDecrypt(pSrcData, pDestData, srcDataLen, key);
    }
    return destData;
}

void myDesDecrypt(const UInt8* pSrcData, UInt8* pOutdata, UInt32 dataSize, TMyDesKey key)
{
    UInt32 rdata = csMyDesRAND0 ^ key.data[0];
    UInt32 borderSize = dataSize & 3;
    if (borderSize > 0){
        myDesROUND0(rdata, key);
    }
    UInt32 srcData = 0;
    for (UInt32 i = 0; i < borderSize; ++i){
        srcData = (signed char)(pSrcData[i]);
        pOutdata[i] = (srcData ^ key.data[3 - i]) - key.data[i];
        rdata = ((rdata << 4) + srcData) ^ key.data[rdata & 3];
    }
    UInt32 i = borderSize;
    while (i < dataSize) {
        myDesROUND0(rdata, key);
        srcData = (signed char)(pSrcData[i + 0]);
        pOutdata[i + 0] = (srcData ^ key.data[1]) - rdata - key.data[0];
        rdata = ((rdata << 4) - srcData) ^ key.data[3];
        srcData = (signed char)(pSrcData[i + 1]);
        pOutdata[i + 1] = (srcData ^ key.data[2]) + rdata - key.data[1];
        rdata = ((rdata << 4) + srcData) - key.data[2];
        srcData = (signed char)(pSrcData[i + 2]);
        pOutdata[i + 2] = (srcData ^ key.data[3]) - rdata + key.data[2];
        rdata = ((rdata << 4) - srcData) ^ key.data[1];
        srcData = (signed char)(pSrcData[i + 3]);
        pOutdata[i + 3] = (srcData ^ key.data[0]) + rdata + key.data[3];
        rdata = ((rdata << 4) + srcData) + key.data[0];
        i += 4;
    }
}

String strToHexStr(const String& srcData){
    long size = srcData.size();
    if (size > 0){
        String dest;
        dest.resize(size * 2);
        DataToHex(srcData.c_str(), &dest[0], dest.size());
        return dest;
    }
    return "";
}

String hexStrToStr(const String& srcData){
    long size = srcData.size();
    if (size > 0){
        String dest;
        dest.resize(size / 2);
        for (long i = 0; i < dest.size(); ++i){
            dest[i] = (char)HexToInt(&srcData[i * 2], 2);
        }
        return dest;
    }
    return  "";
}

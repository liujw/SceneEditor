//MyCrypt.cpp
#include  "MyCrypt.h"
#include  "Rand.h"

TDesKey getRandomDesKey(){
    static VRandLong rand;
    static bool isInit=false;
    if (!isInit){
        rand.setSeedByNowTime();
        isInit=true;
    }
    
    TDesKey result;
    for (long i=0;i<4;++i)
        result.data[i]=rand.nextUInt32Value();
    return result;
}

TDesKey strToDesKey(const String& strKey){
    md5_state_t ms;
    md5_init(&ms);
    md5_append(&ms,(const md5_byte_t*)&strKey[0],(int)strKey.size());
    md5_byte_t result[16];
    md5_finish(&ms,&result[0]);
    return *(TDesKey*)&result[0];
}


const TDesKey csXorKey  = {0x7C619BEF, 0xB0150038, 0xC129233D, 0x5ABA14E7};
const UInt32  csRAND0=0x1D84D2EF; 
const UInt32  csRAND1=0xAE65B441;


must_inline static void Round1(UInt32& W_,UInt32  Data,UInt8 S){
    UInt32 W=W_+ Data;
    W_ = (W << S) | (W >> (32 - S));
}

must_inline static void ROUND0(UInt32 rdata,TDesKey& XorKey,TDesKey& key){
    Round1(XorKey.data[0],key.data[0]+rdata,13);
    Round1(rdata,0x11B9058F,7);
    XorKey.data[1]^=key.data[1]+rdata;
    Round1(rdata,0x791F2C6C,23);
    XorKey.data[2]^=key.data[2]+rdata;
    Round1(rdata,0x4DAC3B62,19);
    XorKey.data[3]^=key.data[3]+rdata;
    
    key.data[0]^=XorKey.data[3];
    key.data[1]^=XorKey.data[0];
    key.data[2]^=XorKey.data[1];
    key.data[3]^=XorKey.data[2];
}

void desEncrypt(const UInt8* pByteData,long dataSize,const TDesKey& key,UInt8* pOutData){
    TDesKey dKey=key;
    TDesKey XorKey=csXorKey;
    UInt32 rdata=csRAND0;
    long borderSize=dataSize & 15;
    for (long i=0;i<borderSize;++i){
        Round1(rdata,csRAND1^dKey.data[1],11);
        ROUND0(rdata,XorKey,dKey);
        pOutData[i]=(UInt8)( (((pByteData[i] + dKey.data[0]) ^ dKey.data[1])-dKey.data[3]) ^ dKey.data[2] );
        Round1(rdata,dKey.data[3]+pOutData[i],19);
    }
    
    const UInt32* src=(const UInt32*)&pByteData[borderSize];
    UInt32* dst=(UInt32*)&pOutData[borderSize];
    dataSize=(dataSize >>4)<<2;
    for (long i=0;i<dataSize;i+=4){
        Round1(rdata,csRAND1^dKey.data[1],11);
        ROUND0(rdata,XorKey,dKey);
        write4Byte(dst[i+0],(read4Byte(src[i+0]) + dKey.data[0]) ^ dKey.data[1]);
        Round1(rdata,XorKey.data[0]+read4Byte(dst[i+0]),19);
        write4Byte(dst[i+1],(read4Byte(src[i+1]) + dKey.data[2]) ^ dKey.data[3]);
        Round1(rdata,XorKey.data[3]+read4Byte(dst[i+1]),7);
        write4Byte(dst[i+2],(read4Byte(src[i+2]) + dKey.data[3]) ^ dKey.data[0]);
        Round1(rdata,XorKey.data[3]+read4Byte(dst[i+2]),21);
        write4Byte(dst[i+3],(read4Byte(src[i+3]) + dKey.data[1]) ^ dKey.data[2]);
        Round1(rdata,XorKey.data[3]+read4Byte(dst[i+3]),15);
    }
}
void desDecrypt(const UInt8* pByteData,long dataSize,const TDesKey& key,UInt8* pOutData){
    TDesKey dKey=key;
    TDesKey XorKey=csXorKey;
    UInt32 rdata=csRAND0;
    long borderSize=dataSize & 15;
    for (long i=0;i<borderSize;++i){
        Round1(rdata,csRAND1^dKey.data[1],11);
        ROUND0(rdata,XorKey,dKey);
        Round1(rdata,dKey.data[3]+pByteData[i],19);
        pOutData[i]=(UInt8)( (((pByteData[i] ^ dKey.data[2]) + dKey.data[3]) ^ dKey.data[1]) - dKey.data[0] );
    }
    
    const UInt32* src=(const UInt32*)&pByteData[borderSize];
    UInt32* dst=(UInt32*)&pOutData[borderSize];
    dataSize=(dataSize >>4)<<2;
    for (long i=0;i<dataSize;i+=4){
        Round1(rdata,csRAND1^dKey.data[1],11);
        ROUND0(rdata,XorKey,dKey);
        Round1(rdata,XorKey.data[0]+read4Byte(src[i+0]),19);
        write4Byte(dst[i+0],(read4Byte(src[i+0]) ^ dKey.data[1]) - dKey.data[0]);
        Round1(rdata,XorKey.data[3]+read4Byte(src[i+1]),7);
        write4Byte(dst[i+1],(read4Byte(src[i+1]) ^ dKey.data[3]) - dKey.data[2]);
        Round1(rdata,XorKey.data[3]+read4Byte(src[i+2]),21);
        write4Byte(dst[i+2],(read4Byte(src[i+2]) ^ dKey.data[0]) - dKey.data[3]);
        Round1(rdata,XorKey.data[3]+read4Byte(src[i+3]),15);	
        write4Byte(dst[i+3],(read4Byte(src[i+3]) ^ dKey.data[2]) - dKey.data[1]);	
    }	
}
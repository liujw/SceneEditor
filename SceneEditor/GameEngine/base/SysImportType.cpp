//SysImportType.cpp

#include "SysImportType.h"
#include "../../import/importInclude/FileImport.h"
#include "GamePackFile.h"
#include "math.h"

void SinCos(double angle,double* out_sin,double* out_cos){
    if (angle!=0){
#ifdef asm
        asm{
            fld angle
            mov eax,out_sin
            mov edx,out_cos
            fsincos
            fstp qword ptr [edx]
            fstp qword ptr [eax]
        }
#else
        *out_sin=sin(angle);
        *out_cos=cos(angle);
#endif
    }else{
        *out_sin=0;
        *out_cos=1;
    }
}

//find char 
template<bool isIgnoreCase,class TChar>
inline long _posEx(const TChar* str,long strLength,long pos0,TChar value){
    if (isIgnoreCase&&(sizeof(TChar)==1)) value=upperCaseChar((char)value); // !!!
    while (pos0<strLength){
        const TChar aChar=str[pos0];
        if (isIgnoreCase&&(sizeof(TChar)==1)){
            if (upperCaseChar((char)aChar)==value)
                return pos0;
        }else{
            if (aChar==value)
                return pos0;
        }
        ++pos0;
    }
    return -1;
}

template<bool isIgnoreCase,class TChar>
inline long _posFromEnd(const TChar* str,long strLength,TChar value){
    long lastPos=-1;
    while (true){
        long pos=_posEx<isIgnoreCase,TChar>(str,strLength,lastPos+1,value);
        if (pos>=0)
            lastPos=pos;
        else
            return lastPos;
    }
    return -1;
}

long posEx(const char* str,long strLength,long pos0,const char value){
    return _posEx<false,char>(str,strLength,pos0,value);
}
long posExIgnoreCase(const char* str,long strLength,long pos0,const char value){
    return _posEx<true,char>(str,strLength,pos0,value);
}

long posFromEnd(const char* str,long strLength,const char value){
    return _posFromEnd<false,char>(str,strLength,value);
}
long posFromEndIgnoreCase(const char* str,long strLength,const char value){
    return _posFromEnd<true,char>(str,strLength,value);
}


#ifdef WINCE
long posFromEnd(const wchar_t* str,long strLength,const wchar_t value){
    return _posFromEnd<false,wchar_t>(str,strLength,value);
}
#endif

static const UInt8* _4_5_6_byte_utf8ToAWChar4(UInt32 char0,const UInt8* utf8,UInt32* unicode){
    if ((char0 & 0xF8) == 0xF0){ // 四位
        UInt32 char1=*(utf8++);
        assert(char1!=0);
        UInt32 char2=*(utf8++);
        assert(char2!=0);
        UInt32 char3=*(utf8++);
        assert(char3!=0);
        *unicode = ((char0 & 0x07) << 18) | ((char1 & 0x3F) << 12)
        | ((char2 & 0x3F) << 6) | (char3 & 0x3F);
        return utf8;
    }
    if ((char0 & 0xFC) == 0xF8){ // 五位
        UInt32 char1=*(utf8++); 
        assert(char1!=0);
        UInt32 char2=*(utf8++); 
        assert(char2!=0);
        UInt32 char3=*(utf8++); 
        assert(char3!=0);
        UInt32 char4=*(utf8++); 
        assert(char4!=0);
        *unicode =  ((char0 & 0x03) << 24) | ((char1 & 0x3F) << 18)
        | ((char2 & 0x3F) << 12) | ((char3 & 0x3F) << 6)
        |  (char4 & 0x3F);
        return utf8;
    }
    if ((char0 & 0xFE) == 0xFC){ // 六位
        UInt32 char1=*(utf8++); 
        assert(char1!=0);
        UInt32 char2=*(utf8++); 
        assert(char2!=0);
        UInt32 char3=*(utf8++);
        assert(char3!=0);
        UInt32 char4=*(utf8++);
        assert(char4!=0);
        UInt32 char5=*(utf8++); 
        assert(char5!=0);
        *unicode =  ((char0 & 0x01) << 30) | ((char1 & 0x3F) << 24)
        | ((char2 & 0x3F) << 18) | ((char3 & 0x3F) << 12)
        | ((char4 & 0x3F) << 6) |  (char5 & 0x3F);
        return utf8;
    }
    //todo: assert(false);
    *unicode=0;
    return utf8;    
}

must_inline static const UInt8* utf8ToAWChar4(const UInt8* utf8,UInt32* unicode){
    assert(utf8!=0);
    UInt32 char0=*(utf8++);
    if ( (char0 & 0x80) == 0x0)  { //  一位
        *unicode = char0;
        return utf8;
    }
    if ((char0 & 0xF0) == 0xE0){ // 三位
        UInt32 char1=*(utf8++); 
        assert(char1!=0);
        UInt32 char2=*(utf8++);
        assert(char2!=0);
        *unicode = ((char0 & 0x0F) << 12) | ((char1 & 0x3F) << 6) | (char2 & 0x3F);
        return utf8;
    }
    if ((char0 & 0xE0) == 0xC0){ // 两位
        UInt32 char1=*(utf8++);
        assert(char1!=0);
        *unicode = ((char0 & 0x1F) << 6) | (char1 & 0x3F);
        return utf8;
    }
    return _4_5_6_byte_utf8ToAWChar4(char0,utf8,unicode);
}

//遍历单个字符 考虑多字节
UInt32 getNextChar(const char*& text){
    if (text==0) return 0;
    UInt32 result;
    text=(const char*)utf8ToAWChar4((const UInt8*)text,&result);
    return result;
}

//need? UInt8* aWChar4ToUtf8(UInt32 unicode,UInt8* utf8){

//////

const char csSuffixTag='.';
const char csPathTag='/';
const char csPathTagWin='\\';

const char* getFileSuffix(const char* fileName,long strLength){
    long s=posFromEnd(fileName, strLength, csSuffixTag);
    if (s<0) return 0;
    const char* result=&fileName[s];
    long p=pos(result, strLength-s, csPathTag);
    if (p>=0) return 0;
    p=pos(result, strLength-s, csPathTagWin);
    if (p>=0) return 0;
    return result;
}

const char* getFileNameNoPath(const char* oldfileNameAndPath,long strLength){
    long p=posFromEnd(oldfileNameAndPath, strLength, csPathTag);
    if (p<0) p=posFromEnd(oldfileNameAndPath, strLength, csPathTagWin);
    if (p<0)
        return oldfileNameAndPath;
    else
        return &oldfileNameAndPath[p+1];
}

template<class TString,class TChar>
inline TString tm_getFilePath(const TChar* oldfileNameAndPath,long strLength){
    long p=posFromEnd(oldfileNameAndPath, strLength, (TChar)csPathTag);
    if (p<0) p=posFromEnd(oldfileNameAndPath, strLength, (TChar)csPathTagWin);
    if (p<0)
        return TString();
    else{
        for (p=p-1;p>=0;--p){
            if ((oldfileNameAndPath[p]!=(TChar)csPathTag)&&(oldfileNameAndPath[p]!=(TChar)csPathTagWin)){
                TString result(oldfileNameAndPath);
                result.resize(p+1);
                return result;
            }
        }
        return TString();
    }
}

String getFilePath(const char* oldfileNameAndPath,long strLength){
    return tm_getFilePath<String,char>(oldfileNameAndPath,strLength);
}

#ifdef WINCE
W2String getFilePath(const UInt16* oldfileNameAndPath,long strLength){
    return tm_getFilePath<W2String,UInt16>(oldfileNameAndPath,strLength);
}
#endif


static unsigned char _charUpperCase_table[256]={
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
    0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};
static unsigned char _charLowerCase_table[256]={
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 
    0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF 
};
const  char*  charUpperCase_table=(const char*)&_charUpperCase_table[0];
const  char*  charLowerCase_table=(const char*)&_charLowerCase_table[0];
/*class _CAuto_inti_charCase_table
 {
 private:
 static inline long _charUpperCase(long c)
 {
 if ((c>='a')&&(c<='z'))
 return c+('A'-'a');
 else
 return c; 
 }
 static inline long _charLowerCase(long c)
 {
 if ((c>='A')&&(c<='Z'))
 return c+('a'-'A');
 else
 return c; 
 }
 public:
 _CAuto_inti_charCase_table() {
 for (int i=0;i<256;++i){
 _charUpperCase_table[i]=(char)_charUpperCase(i);
 _charLowerCase_table[i]=(char)_charLowerCase(i);
 }
 }
 };
 static _CAuto_inti_charCase_table _Auto_inti_charCase_table;*/

void upperCase(char* str,long strLength){
    long i=0;
    while (i<strLength){
        UInt8 aChar=(UInt8)str[i];
        if (aChar<=asciiCharMaxValue){
            str[i]=charUpperCase_table[aChar];
        }
        ++i;
    }
}

void lowerCase(char* str,long strLength){
    long i=0;
    while (i<strLength){
        UInt8 aChar=(UInt8)str[i];
        if (aChar<=asciiCharMaxValue){
            str[i]=charLowerCase_table[aChar];
        }
        ++i;
    }
}

/////////////////////////////////////////////////////////////
static GamePackFile* getGamePackFile(){
    static bool isInit=false;
    static GamePackFile packFile;
    if (!isInit){
        packFile.loadPackFile("data.pak");
        //载入默认语言 如果有的话
        String pfName=getLanguageID();
        pfName+=".pak";
        if (!packFile.loadPackFile(pfName.c_str())){
            packFile.loadPackFile("en.pak");
        }
        isInit=true;
    }
    return &packFile;
}
/*
 bool file_loadPackFile(const char* csPakFielName,const char* prefix){
 GamePackFile* packFile=getGamePackFile();
 return packFile->loadPackFile(csPakFielName,prefix);
 }*/

void* file_create(const char* fileName){
    long strLength=(long)strlen(fileName);
    String path=getFilePath(fileName, strLength);
    void* fHandle=sys_file_create(getFileNameNoPath(fileName, strLength),path.c_str());	
    if (fHandle!=csNullFileHandle){
        TPackHandleData* r=new TPackHandleData();
        r->fileHandle=fHandle;
        return r;
    }else
        return csNullFileHandle;
}

void* file_open_read(const char* fileName){
    GamePackFile* gpk=getGamePackFile();
    void* fHandle=gpk->file_open_read(fileName);
    if (fHandle!=csNullFileHandle){
        TPackHandleData* r=new TPackHandleData();
        r->fileHandle=fHandle;
        r->fileReadPos=0;
        r->packHandle=gpk;
        return r;
    }else{
        long strLength=(long)strlen(fileName);
        String path=getFilePath(fileName, strLength);
        void* fHandle=sys_file_open_read(getFileNameNoPath(fileName, strLength),path.c_str());
        if (fHandle!=csNullFileHandle){
            TPackHandleData* r=new TPackHandleData();
            r->fileHandle=fHandle;
            return r;
        }else{
            return csNullFileHandle;
        }
    }
}

bool file_exists(const char* fileName){
    long strLength=(long)strlen(fileName);
    String path=getFilePath(fileName, strLength);
    if (getGamePackFile()->file_exists(fileName))
        return true;
    else
        return 0!=sys_file_exists(getFileNameNoPath(fileName, strLength),path.c_str());
}

bool path_exists(const char* pathName){
    if (getGamePackFile()->path_exists(pathName))
        return true;
    else
        return 0!=sys_path_exists(pathName);
}

long get_file_size(void* fileHandle){
    if(fileHandle==csNullFileHandle)
        return 0;
    TPackHandleData* r=(TPackHandleData*)fileHandle;
    if (r->packHandle==0)
        return sys_get_file_size(r->fileHandle);
    else
        return r->packHandle->get_file_size(r->fileHandle);
}

long file_seek(void* fileHandle,long newPos){
    if(fileHandle==csNullFileHandle)
        return 0;
    TPackHandleData* r=(TPackHandleData*)fileHandle;
    if (r->packHandle==0)
        return sys_file_seek(r->fileHandle,newPos);
    else{
        long oldPos=r->fileReadPos;
        if (newPos>=0){
            const long endPos=r->packHandle->get_file_size(r->fileHandle);
            if (newPos>endPos)
                newPos=endPos;
            r->fileReadPos=newPos;
        }
        return oldPos;
    }
}

long file_read(void* fileHandle,UInt8* dst,unsigned long readSize){
    if(fileHandle==csNullFileHandle)
        return 0;
    TPackHandleData* r=(TPackHandleData*)fileHandle;
    if (r->packHandle==0)
        return sys_file_read(r->fileHandle,dst,readSize);
    else{
        long readed=r->packHandle->file_read(r->fileHandle,r->fileReadPos,dst,readSize);
        r->fileReadPos+=readed;
        return readed;
    }
}
long file_write(void* fileHandle,const UInt8* src,unsigned long writeSize){
    if(fileHandle==csNullFileHandle)
        return 0;
    TPackHandleData* r=(TPackHandleData*)fileHandle;
    assert(r->packHandle==0);
    
    return sys_file_write(r->fileHandle,src,writeSize);
}
void file_close(void* fileHandle){
    if(fileHandle==csNullFileHandle)
        return;
    TPackHandleData* r=(TPackHandleData*)fileHandle;
    if (r->packHandle==0)
        sys_file_close(r->fileHandle);
    else{
        r->packHandle->file_close(r->fileHandle);
    }
    delete r;
}

long file_writeMyData(const char* fileName,const UInt8* src,unsigned long writeSize){
    return sys_file_writeMyData(fileName,src,writeSize);
}
long file_readMyData(const char* fileName,UInt8* dst,unsigned long readSize){
    return sys_file_readMyData(fileName,dst,readSize);
}

const long hexStr_Error=0xFF;
static unsigned char _hexToInt_table[256]={
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    0x0,          0x1,          0x2,          0x3,          0x4,          0x5,          0x6,          0x7, 
    0x8,          0x9, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error,          0xA,          0xB,          0xC,          0xD,          0xE,          0xF, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error,          0xA,          0xB,          0xC,          0xD,          0xE,          0xF, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, 
    hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error, hexStr_Error 
};
static unsigned char _intTohex_table[16]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
const  unsigned char*  hexToInt_table=&_hexToInt_table[0];
const  unsigned char*  intTohex_table=&_intTohex_table[0];

/*class _CAuto_inti_hexToInt_table
 {
 private:
 static inline long _AHexToInt(long x)
 {
 if ((x>='a')&&(x<='f'))
 return x+(10-'a');
 else if ((x>='A')&&(x<='F'))
 return x+(10-'A');
 else if ((x>='0')&&(x<='9'))
 return x-'0';
 else
 return hexStr_Error; 		
 }
 public:
 _CAuto_inti_hexToInt_table() {
 for (int i=0;i<256;++i)
 _hexToInt_table[i]=(unsigned char)_AHexToInt(i);
 for (int j=0;j<16;++j){
 if (j<=9)
 _intTohex_table[j]=j+'0';
 else
 _intTohex_table[j]=j+('A'-10);					
 }
 }
 };
 static _CAuto_inti_hexToInt_table _Auto_inti_hexToInt_table;*/

long HexToInt(const char* hex,long length){
    long result=0;
    for (long i=0;i<length;++i){
        result<<=4;	
        result+=AHexToInt(hex[i]);	
    }
    return result;
}


long checkHexStrLength(const char* hex,long length){
    for (long i=0;i<length;++i){
        if (hexToInt_table[(UInt8)hex[i]]==hexStr_Error)
            return i;
    }
    return length;
}


long checkIntStrLength(const char* intStr,long length){
    for (long i=0;i<length;++i){
        long ch=(UInt8)intStr[i];
        if ((ch<'0')||(ch>'9'))
            return i;
    }
    return length;
}

void IntToHex(unsigned long src,char* hex,long hexSize){
    hex+=hexSize;
    for (long i=0;i<hexSize;++i){
        --hex;
        *hex=intTohex_table[src&0xF];
        src>>=4;
    }
}

void DataToHex(const void* _src,char* hex,long hexSize){
    const UInt8* src=(const UInt8*)_src;
    long fastSize=hexSize>>1<<1;
    for (long i=0;i<fastSize;i+=2){
        UInt8 c=src[i>>1];
        hex[i+0]=intTohex_table[c>>4];
        hex[i+1]=intTohex_table[c&0xF];
    }
    if (fastSize<hexSize){
        long i=fastSize;
        UInt8 c=src[i>>1];
        hex[i+0]=intTohex_table[c>>4];
    }
}

void hexStrToData(const String& hexStr, String& str){
    str = "";
    const char* csSrcData = hexStr.c_str();
    char* csDestData = 0;
    long hexStrLength = hexStr.size();
    if (hexStrLength > 0){
        long destLength = (hexStrLength + 1) / 2;
        str.resize(destLength, '\0');
        csDestData = &str[0];
    }
    while (hexStrLength > 0){
        long tmpLength = 2;
        if (hexStrLength < tmpLength){
            tmpLength = hexStrLength;
        }
        char val = (char)(HexToInt(csSrcData, tmpLength));
        *csDestData = val;
        csSrcData += tmpLength;
        hexStrLength -= tmpLength;
        ++csDestData;
    }
}

void dataCopyToString(const unsigned char* data,long it0,long it1,String& out_str){
    long count=it1-it0;
    data+=it0;
    out_str.resize(count);
    for (long i=0;i<count;++i)
        out_str[i]=data[i];
}

void ftoa(double f,long dot,long size,String& out_str){
    if (dot<0) dot=0;
    for (long i=0;i<dot;++i)
        f*=10;
    Int64 L=(Int64)(f+0.5);
    if (L==0) {
        out_str="0";
        return;
    }
    bool isRTag=L<0;
    if (isRTag) L=-L;
    
    long minSize;
    if (dot==0){
        minSize=1;
    } else {
        minSize=3;
    }
    if (isRTag) ++minSize;
    if (size<minSize) size=minSize;
    const long maxSize=128;
    if (size>maxSize) size=maxSize;
    
    char  data[maxSize+4];
    char* pData=&data[maxSize+4-1];
    *pData=0;
    long s=0;
    while ((L!=0)||(s<size)){
        --pData;
        if (s<dot){
            long n=(long)(L%10);
            L/=10;
            *pData=(char)(n+'0');
        }else{
            if ((s==dot)&&(dot>0)){
                *pData='.';
            }else{
                if (L>0){
                    long n=(long)(L%10);
                    L/=10;
                    *pData=(char)(n+'0');
                }else{
                    if (s==dot+1)
                        *pData='0';
                    else {
                        if (isRTag){
                            *pData='-';
                            isRTag=false;
                        }
                        else
                            *pData=' ';
                    }
                }
            }			
        }
        ++s;
    }
    out_str=pData;
}

template<class TInt>
must_inline void my_itoa(TInt L,String& out_str){
    if (L==0){
        out_str="0";
        return;
    }
    
    bool isRTag=L<0;
    if (isRTag) L=-L;
    
    const long maxSize=50;
    char  data[maxSize];
    char* pData=&data[maxSize-1];
    *pData=0;
    while (L>0){
        TInt cL=L/10;
        long n=(long)(L-cL*10);
        L=cL;
        --pData;
        *pData=(char)(n+'0');
    }
    if (isRTag){
        --pData;
        *pData='-';
    }
    out_str=pData;
}



void itoa64(Int64 L,String& out_str){
    my_itoa<Int64>(L, out_str);
}


void itoa(long L,String& out_str){
    const long  minLong    = -2147483647-1 ; 
    const char* minLongStr ="-2147483648";
    if (L==minLong){
        out_str=minLongStr;
        return;
    }
    my_itoa<long>(L, out_str);
}


template<class TInt>
must_inline bool my_atoi(const char* str,long strLength,TInt& out_i){
    if (strLength<=0) return false;
    
    //str as:  [+|-][0x|0X|0|[1..9]][0..9]
    
    //TODO:未处理溢出
    long sign=1;
    if (str[0]=='-'){
        sign=-1;
        ++str;
        --strLength;
    }else if (str[0]=='+'){
        ++str;
        --strLength;
    }
    TInt L=0;	
    if (str[0]=='0'){
        if (strLength>1){
            if ((str[1]=='x')||(str[1]=='X')){ //16进制
                for (long i=2;i<strLength;++i){
                    long d=str[i];
                    if ((d>='0')&&(d<='9')){
                        L*=16;
                        L+=(d-'0');
                    }else if ((d>='a')&&(d<='f')){
                        L*=16;
                        L+=(d+(10-'a'));
                    }else if ((d>='A')&&(d<='F')){
                        L*=16;
                        L+=(d+(10-'A'));
                    }else
                        return false;
                }
            }else{  //8进制
                for (long i=1;i<strLength;++i){
                    long d=str[i];
                    if ((d>='0')&&(d<='7')){
                        L*=8;
                        L+=(d-'0');
                    }else
                        return false;
                }
            }
        } //else L=0;
    }else{ //10进制
        for (long i=0;i<strLength;++i){
            long d=str[i];
            if ((d>='0')&&(d<='9')){
                L*=10;
                L+=(d-'0');
            }else
                return false;
        }
    }
    out_i=L*sign;
    return true;		
}


bool atoi(const char* str,long strLength,long& out_i){
    return my_atoi<long>(str,strLength,out_i);
}
bool atoi64(const char* str,long strLength,Int64& out_i){
    return my_atoi<Int64>(str,strLength,out_i);
}


static const char* atof_GetCharIntEnd(const char* int_begin,const char* str_end)
{
    long int_count=0;
    for (const char* i=int_begin;i<str_end;++i)
    {
        if ( ('0'<=(*i)) && ((*i)<='9') )
            ++int_count;
        else
            break;
    }
    return &int_begin[int_count];
}


bool atof(const char* str,long strLength,double& out_f){
    if (strLength<=0) return false;
    const char* pstr_begin=str;
    const char* pstr_end=&str[strLength];
    
    //str as:  [+|-][0..9][.][0..9][E|e][+|-][0..9] 
    bool is_have_sign=false;
    long  sign=1;
    bool is_have_int=false;
    const char* int_begin=0; const char* int_end=0; 
    bool is_have_dot=false;
    bool is_have_digits=false;
    const char* digits_begin=0; const char* digits_end=0; 
    bool is_have_ep=false;
    bool is_have_ep_sign=false;
    long  ep_sign=1;
    bool is_have_ep_int=false;
    const char* ep_int_begin=0; const char* ep_int_end=0; 
    
    //处理正负号
    if ((*pstr_begin)=='-') {
        sign = -1;
        is_have_sign=true;
        ++pstr_begin; 
    }else if ((*pstr_begin)=='+') {
        is_have_sign=true;
        ++pstr_begin; 
    }
    
    //处理前面的整数部分
    int_begin=pstr_begin;
    int_end=atof_GetCharIntEnd(int_begin,pstr_end);
    is_have_int=(int_begin!=int_end);
    if ((int_end-int_begin>=2)&&((*int_begin)=='0')) return false; //'0?'数字表示错误
    pstr_begin=int_end;
    if (pstr_begin!=pstr_end){
        if (((*pstr_begin)!='.')&&(!is_have_int)) return false; //没有任何数字
        
        //处理小数部分
        if ((*pstr_begin)=='.'){
            is_have_dot=true;
            ++pstr_begin;
            
            digits_begin=pstr_begin;
            digits_end=atof_GetCharIntEnd(digits_begin,pstr_end);
            is_have_digits=(digits_begin!=digits_end);
            if ((!is_have_digits)&&(!is_have_int)) return false; //没有任何数字
            pstr_begin=digits_end;
        }
        
        if (pstr_begin!=pstr_end){
            //处理指数部分
            if ( ((*pstr_begin)=='e')||((*pstr_begin)=='E') ){
                is_have_ep=true;
                ++pstr_begin;
                if (pstr_begin==pstr_end) return false; //指数没有后面的数
                //处理指数的正负号
                if ((*pstr_begin)=='-') {
                    ep_sign = -1;
                    is_have_ep_sign=true;
                    ++pstr_begin; 
                }else if((*pstr_begin)=='+') {
                    is_have_ep_sign=true;
                    ++pstr_begin; 
                }
                //处理指数中的整数
                ep_int_begin=pstr_begin;
                ep_int_end=atof_GetCharIntEnd(ep_int_begin,pstr_end);
                is_have_ep_int=(ep_int_begin!=ep_int_end);
                if (!is_have_ep_int) return false; //指数没有数字
                if ((ep_int_end-ep_int_begin>=2)&&((*ep_int_begin)=='0')) return false; //'0?'数字表示错误
                pstr_begin=ep_int_end;
            }
            if (pstr_begin!=pstr_end) return false; //未预料的字符
        }
    }
    
    //////////////////
    //实际的类型转换
    
    double f=0;
    for (const char* pi=int_begin;pi<int_end;++pi){
        f*=10;
        f+=((*pi)-'0');
    }
    double ep=1;
    for (const char* pd=digits_begin;pd<digits_end;++pd){
        ep/=10;
        f+=((*pd)-'0')*ep;
    }
    
    if (is_have_ep){
        long iep;
        if (!atoi(ep_int_begin,(long)(ep_int_end-ep_int_begin),iep)) return false;
        f*=MyBase::intPower<double>(10.0,iep*ep_sign);
    }
    f*=sign;
    out_f=f;
    return true;
}

UInt8 _charIsSpaceBuf[256]={
    0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
const UInt8* charIsSpaceBuf=_charIsSpaceBuf;
/*
 for (long i=-256;i<256;++i){
 if ((i<0)||(i>=127))
 _charIsSpaceBuf[i+256]=0;
 else
 _charIsSpaceBuf[i+256]=(UInt8)isspace(i);
 }
 charIsSpaceBuf=&_charIsSpaceBuf[256];
 */
UInt8 _charIsAlphabetBuf[256]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
const UInt8* charIsAlphabetBuf=_charIsAlphabetBuf;

UInt8 _charIsKeyCharBuf[256]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const UInt8* charIsKeyCharBuf=_charIsKeyCharBuf;


void trim(const char* strSrc,long strLength,String& out_str){
    const char* strEnd=&strSrc[strLength];
    for (;strSrc!=strEnd;++strSrc)
    {
        if (!charIsSpaceBuf[*(UInt8*)strSrc])
            break;
    }
    for (;strEnd!=strSrc;--strEnd)
    {
        if (!charIsSpaceBuf[(UInt8)strEnd[-1]])
            break;
    }
    if (strSrc==strEnd)
        out_str.clear();
    else{
        String tmp(strSrc,strEnd);
        out_str.swap(tmp);
    }
}

template<bool isIgnoreCase>
inline bool _strIsEqual(const char* csSrc, int srcLen, const char* csSub, int subLen){
    if (srcLen!=subLen) return false;
    if (!isIgnoreCase){
        for (long i=0;i<subLen;++i){
            if (csSrc[i]!=csSub[i]) return false;
        }
        return true;
    }
    
    long i=0;
    while (i<subLen){
        const char aChar0=csSrc[i];
        const char aChar1=csSub[i];
        if (aChar0!=aChar1){
            if (upperCaseChar(aChar0)!=upperCaseChar(aChar1)) return false;
            ++i;
        }else{
            ++i;
        }	
    }
    return true;
}

template<bool isIgnoreCase>
inline bool _findStrIsEqual(const char* csSrc, int srcLen, const char* csSub, int subLen){
    if (srcLen<subLen) return false;
    return _strIsEqual<isIgnoreCase>(csSrc,subLen,csSub,subLen);
}


template<bool isIgnoreCase>
inline long _findStr(const char* csSrc, int srcLen, const char* csSub, int subLen){
    const long csNotFind =-1;
    if (subLen<=0) return csNotFind;
    if (srcLen < subLen) return csNotFind;
    
    char subStartChar=csSub[0];
    long pos=0;
    while (true){
        pos=_posEx<isIgnoreCase,char>(csSrc,srcLen,pos,subStartChar);
        if (pos>=0){
            if (_findStrIsEqual<isIgnoreCase>(csSrc+pos,srcLen-pos,csSub,subLen))
                return pos;
            else
                ++pos;
        }else
            return csNotFind;
    }
}

long findStr(const char* csSrc, int srcLen, const char* csSub, int subLen){
    return _findStr<false>(csSrc, srcLen, csSub, subLen);
}
long findStrIgnoreCase(const char* csSrc, int srcLen, const char* csSub, int subLen){
    return _findStr<true>(csSrc, srcLen, csSub, subLen);
}


bool strIsEqual(const char* str0, int strLength0, const char* str1, int strLength1){
    return _strIsEqual<false>(str0,strLength0,str1,strLength1);
}
bool strIsEqualIgnoreCase(const char* str0, int strLength0, const char* str1, int strLength1){
    return _strIsEqual<true>(str0,strLength0,str1,strLength1);
}


bool replaceStr(const String& str,String& out_str,const char* csSub, int subLen,const char* csNewSub, int newSubLen){
    const char* pstr=str.c_str();
    long str_size=str.size();
    long p=findStr(pstr,str_size,csSub,subLen);
    if (p<0) return false;
    
    String r;
    r.reserve(str_size+newSubLen);
    r.insert(0,pstr,p);
    r.insert(p,csNewSub,newSubLen);
    r.insert(p+newSubLen,pstr+p+subLen,str_size-p-subLen);
    out_str.swap(r);
    return true;
}

inline String dateToStr(TDateTimeData dateTimeData){
    String year,month,day; 
    itoa(dateTimeData.year,year);
    itoa(dateTimeData.month,month);
    itoa(dateTimeData.day,day);
    if (month.size()<2) month='0'+month;
    if (day.size()<2) day='0'+day;
    return year+'.'+month+'.'+day;
}

inline String timeToStr(TDateTimeData dateTimeData){
    String hour,minute,second; 
    itoa(dateTimeData.hour,hour);
    itoa(dateTimeData.minute,minute);
    itoa((long)(dateTimeData.second),second);
    if (hour.size()<2) hour='0'+hour;
    if (minute.size()<2) minute='0'+minute;
    if (second.size()<2) second='0'+second;
    return hour+':'+minute+':'+second;
}

String dateToStr(const TDateTime dateTime){
    TDateTimeData dateTimeData;
    dateTimeToDateTimeData(dateTime,&dateTimeData);
    return dateToStr(dateTimeData);
}

String timeToStr(const TDateTime dateTime){
    TDateTimeData dateTimeData;
    dateTimeToDateTimeData(dateTime,&dateTimeData);
    return timeToStr(dateTimeData);
}
String dateTimeToStr(const TDateTime dateTime){
    TDateTimeData dateTimeData;
    dateTimeToDateTimeData(dateTime,&dateTimeData);
    return dateToStr(dateTimeData)+' '+timeToStr(dateTimeData);
}

static long subStr(const String& srcStr, char endChar, String& outStr, long& startIdx, long& len){
    long nPos = -1;
    outStr = "";
    if (len > 0){
        const char* csData = srcStr.c_str() + startIdx;
        nPos = pos(csData, len, endChar);
        if (nPos >= 0){
            outStr = srcStr.substr(startIdx, nPos);
            startIdx += (nPos + 1);
            len -= (nPos + 1);
        }
    }
    return nPos;
}

bool strToDateTime(const String& str, TDateTime& dt){
    TDateTimeData dtData;
    dtData.year = 0;
    dtData.month = 0;
    dtData.day = 0;
    dtData.hour = 0;
    dtData.minute = 0;
    dtData.second = 0.0;
    long len = str.size();
    long srcLen = len;
    long idx = 0;
    long startIdx = 0;
    //year month
    while (len > 0 && idx < 2 && startIdx < srcLen){
        String strSub;
        if (subStr(str, '-', strSub, startIdx, len) >= 0){
            if (idx == 0){
                dtData.year = strToInt(strSub);
            }
            else if (idx == 1){
                dtData.month = strToInt(strSub);
            }
            ++idx;
        }
        else{
            break;
        }
    }
    //day
    String strDay;
    if (len > 0 && subStr(str, ' ', strDay, startIdx, len) >= 0){
        dtData.day = strToInt(strDay);
    }
    idx = 0;
    //hour minute
    while (len > 0 && idx < 2 && startIdx < srcLen){
        String strSub;
        if (subStr(str, ':', strSub, startIdx, len) >= 0){
            if (idx == 0){
                dtData.hour = strToInt(strSub);
            }
            else if (idx == 1){
                dtData.minute = strToInt(strSub);
            }
            ++idx;
        }
        else{
            break;
        }
    }    
    //sec
    if (len > 0 && startIdx < srcLen){
        atof(str.substr(startIdx, len), dtData.second);
    }
    return dateTimeDataToDataTime(dtData, dt);
}

long getStrValueCount(const String& Values,const char spaceChar){
    long size=(long)Values.size();
    if (size==0) return 0;
    long count=0;
    for (long i=0;i<size;++i){
        if (Values[i]==spaceChar)
            ++count;
    }
    return count+1;
}

void getNextStrValue(String& Values,const char spaceChar,String& out_result){
    long i=pos(Values.c_str(),(long)Values.size(),spaceChar);
    if (i<0){
        out_result=Values;
        Values.clear();
    }else{
        out_result=Values.substr(0,i);
        Values=Values.substr(i+1,Values.size()-(i+1));
    }	
}


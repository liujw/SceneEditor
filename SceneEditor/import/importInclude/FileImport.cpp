//FileImport.cpp
#if defined(_MM_FILE_USE_C_FileImport) || !(defined(_IOS) || defined(_MACOSX)) 
//#if !(defined(_IOS) || defined(_MACOSX))

#include "FileImport.h"
#include "SysImportType.h"
#ifdef Set
#undef Set
#endif
#include <stdio.h>
#ifdef WINCE
#include <windows.h> 
#else
#ifdef _IOS
#import <UIKit/UIKit.h>
#else 
#ifdef _MACOSX
#import <Cocoa/Cocoa.h>
#else 
#include <io.h>
#endif
#endif
#endif

#pragma warning(disable:4996)

#ifdef WINCE
inline const W2String& getExePath(){
    static W2String exePath;
    static bool isInit=false;
    if (!isInit){
        wchar_t exeFileName_w[MAX_PATH+1];
        ::GetModuleFileName(0,exeFileName_w,MAX_PATH);
        exePath=exeFileName_w;
        exePath=getFilePath(exePath.c_str(),exePath.size());
        isInit=true;
    }
    return exePath;
}
#endif

#if (defined(_IOS) || defined(_MACOSX))

static NSString* getNSSourcesPath(){
    return [[NSBundle mainBundle] resourcePath];//bundlePath
}

inline const char* getSourcesPath(){
    static const char* _bundlePath=0;
    if (_bundlePath==0){
        const long csflLength=1024;
        static char bundlePath[csflLength+1];
        NSString* bsBundlePath=getNSSourcesPath();
        if ([bsBundlePath getCString:bundlePath maxLength:csflLength encoding:NSUTF8StringEncoding])
            _bundlePath=bundlePath;
    }
    return _bundlePath;
}
#else
inline const char* getSourcesPath(){
    return "";
}
#endif

inline bool pushcstr(char*& buf,long& blength,const char* src){
    if (src==0) return true;
    for (;blength>0;){
        if (*src!=0){
            --blength;
            *buf=*src;
            ++buf;
            ++src;
        }else 
            return true;
    }
    return false;
}

inline bool pushcstr(char*& buf,long& blength,char src){
    if (blength>0){
        --blength;
        *buf=src;
        ++buf;
        return true;
    }else 
        return false;
}

static bool pushCStrPathAndFileName(char*& fl,long& flLength,const char* fileName,const char* filePath,bool is_flEmpty){
    if (filePath!=0){
        if (!is_flEmpty) { if (!pushcstr(fl,flLength,'/')) return false; }
        if (!pushcstr(fl,flLength,filePath)) return false;
        is_flEmpty=false;
    }
    if (fileName!=0){
        if (!is_flEmpty){ if (!pushcstr(fl,flLength,'/')) return false; }
        if (!pushcstr(fl,flLength,fileName)) return false;
    }
    if (!pushcstr(fl,flLength,(char)0)) return false;
    return true;
}

static bool addCStrPathAndFileName(char* buf,long bufLength,const char* fileName,const char* filePath){
    long flLength=bufLength;
    char* fl=buf;
    return pushCStrPathAndFileName(fl,flLength,fileName,filePath,true);
}

static bool getCStrPathAndFileName(char* buf,long bufLength,const char* fileName,const char* filePath){
    long flLength=bufLength;
    char* fl=buf;
    if (!pushcstr(fl,flLength,getSourcesPath())) return false;
    return pushCStrPathAndFileName(fl,flLength,fileName,filePath,flLength==bufLength);
}

/*
 #ifdef WINCE
 void sys_dir_list(const char* dir,StringList& dirList){
 dirList.clear();
 W2String wText=getExePath();
 if ((dir!=0)&&(strlen(dir)>0)){
 wchar_t* twchar=ansiCharToUnicode(dir);
 wText+=TEXT("/")+twchar;
 delete []twchar;
 }
 wText+=TEXT("/*.*");
 
 WIN32_FIND_DATA FindData;
 memset(&FindData,0,sizeof(WIN32_FIND_DATA));
 HANDLE Handle = FindFirstFile(wText.c_str(),&FindData);
 if (Handle== INVALID_HANDLE_VALUE) return;
 while (true){
 if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
 W2String dirName(FindData.cFileName);
 if ((dirName!=TEXT(".")) && (dirName!=TEXT("..")) ){
 dirList.push_back(dirName);
 
 if(!FindNextFile(Handle,&FindData)) break;
 }
 }
 FindClose(Handle);
 }
 #endif
 */

//declared deprecated
inline bool sys_file_or_path_exists(const char* fileNameOrPath){
#ifdef WINCE
    wchar_t* twchar=ansiCharToUnicode(fileNameOrPath);
    W2String wText=getExePath()+TEXT("/")+twchar;
    delete []twchar;
    long Code = ::GetFileAttributes(wText.c_str());
    bool isExists=(Code != -1) && ((FILE_ATTRIBUTE_DIRECTORY & Code) != 0);
    if (!isExists) {
        WIN32_FIND_DATA FindData;
        memset(&FindData,0,sizeof(WIN32_FIND_DATA));
        HANDLE Handle = FindFirstFile(wText.c_str(),&FindData);
        isExists= (Handle != INVALID_HANDLE_VALUE);
        if (isExists)
            FindClose(Handle);
    }
    return isExists;
#else
#if (defined(_IOS) || defined(_MACOSX))
    long csflLength=1024;
    char flName[csflLength+1];
    if (!getCStrPathAndFileName(flName,csflLength,fileNameOrPath,0))
        return 0;
    return (access(flName, 0) !=-1);
#else
    return (_access(fileNameOrPath, 0) !=-1);
#endif
#endif
}

inline void* sys_file_create(const char* fileName){
#ifdef WINCE
    wchar_t* twchar=ansiCharToUnicode(fileName);
    W2String wText=getExePath()+TEXT("/")+twchar;
    delete []twchar;
    FILE* file=_wfopen(wText.c_str(),TEXT("wb")); 
#else
#if (defined(_IOS) || defined(_MACOSX))
    long csflLength=1024;
    char flName[csflLength+1];
    if (!getCStrPathAndFileName(flName,csflLength,fileName,0))
        return 0;
    FILE* file=fopen(flName,"wb"); 
#else
    FILE* file=fopen(fileName,"wb"); 
#endif
#endif
    return file;
}
inline void* sys_file_open_read(const char* fileName){
#ifdef WINCE
    FILE* file=csNullFileHandle;
    if ((fileName!=0)&&(fileName[0]=='$'))
        file=fopen(&fileName[1],"rb"); 
    else {
        wchar_t* twchar=ansiCharToUnicode(fileName);
        W2String wText=getExePath()+TEXT("/")+twchar;
        delete []twchar;
        file=_wfopen(wText.c_str(),TEXT("rb")); 
    }
    
#else
#if (defined(_IOS) || defined(_MACOSX))
    long csflLength=1024;
    char flName[csflLength+1];
    if (!getCStrPathAndFileName(flName,csflLength,fileName,0))
        return 0;
    FILE* file=fopen(flName,"rb"); 
#else
    FILE* file=fopen(fileName,"rb"); 
#endif
#endif
    return file;
}

////////////

int sys_file_exists(const char* fileName,const char* filePath){
    /*
     void* file=_file_open_read(fileName,filePath);
     if (file!=csNullFileHandle)
     file_close(file);
     return (file!=csNullFileHandle);
     */
    if ((fileName==0)||(fileName[0]==0)) return 0;
    if ((filePath==0)||(filePath[0]==0)) 
        return sys_file_or_path_exists(fileName);
    
    const long csflLength=1024;
    char flName[csflLength+1];
    if (!addCStrPathAndFileName(flName,csflLength,fileName,filePath))
        return 0;
    return sys_file_or_path_exists(flName);
}

int sys_path_exists(const char* filePath){
    return sys_file_or_path_exists(filePath); 
}


void* sys_file_create(const char* fileName,const char* filePath){
    if ((filePath==0)||(filePath[0]==0)) 
        return sys_file_create(fileName);
    
    const long csflLength=1024;
    char flName[csflLength+1];
    if (!addCStrPathAndFileName(flName,csflLength,fileName,filePath))
        return 0;
    return sys_file_create(flName);
}

void* sys_file_open_read(const char* fileName,const char* filePath){
    if ((filePath==0)||(filePath[0]==0)) 
        return sys_file_open_read(fileName);
    
    const long csflLength=1024;
    char flName[csflLength+1];
    if (!addCStrPathAndFileName(flName,csflLength,fileName,filePath))
        return 0;
    return sys_file_open_read(flName); 
}

long sys_get_file_size(void* fileHandle){
    if (fileHandle==csNullFileHandle)
        return 0;
    else{ 
        FILE* file=(FILE*)fileHandle;
        long oldPos = ftell(file);   
        fseek(file,0,SEEK_END);
        int file_length = ftell(file);   
        fseek(file,oldPos,SEEK_SET); 
        return file_length;	
    }
}

long sys_file_seek(void* fileHandle,long newPos){
    if (fileHandle==csNullFileHandle)
        return 0;
    else{
        FILE* file=(FILE*)fileHandle;
        long oldPos = ftell(file); 
        if (newPos>=0){
            int fs=fseek(file,newPos,SEEK_SET); 
            assert(fs==0);
        }
        return oldPos;
    }
} 

long sys_file_read(void* fileHandle,UInt8* dst,unsigned long readSize){
    if (fileHandle==csNullFileHandle)
        return 0;
    else{
        FILE* file=(FILE*)fileHandle;
        long rt=fread(dst,1,readSize,file);
        assert(rt==readSize);
        return rt;
    }    
}



long sys_file_read_from(void* fileHandle,long flStartPos,UInt8* dst,unsigned long readSize){
    if (fileHandle==csNullFileHandle)
        return 0;
    else{
        FILE* file=(FILE*)fileHandle;
        int fs=fseek(file,flStartPos,SEEK_SET); 
        assert(fs==0);
        /*long rt=0;
         while (readSize>0){
         long rd=fread(dst,readSize,1,file);
         rt+=rd;
         readSize-=rd;
         }
         return rt;*/
        long rt=fread(dst,1,readSize,file);
        assert(rt==readSize);
        return rt;
    }    
}


long sys_file_write(void* fileHandle,const UInt8* src,unsigned long writeSize){
    if (fileHandle==csNullFileHandle)
        return 0;
    else{
        FILE* file=(FILE*)fileHandle;
        return (long)fwrite(src,1,writeSize,file);
    }    
}

void sys_file_close(void* fileHandle){
    if (fileHandle!=csNullFileHandle)
        fclose((FILE*)fileHandle); 
}


#if (defined(_IOS) || defined(_MACOSX))

long sys_file_writeMyData(const char* fileName,const UInt8* src,unsigned long writeSize){
    NSArray *paths =  NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    if (!documentsDirectory) {
        printf("Documents directory not found!");
        return NO;
    }	
#ifdef _MACOSX
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"4avol"];
    [[NSFileManager defaultManager] createDirectoryAtPath:documentsDirectory attributes:nil];
#endif
    NSString* fName= [NSString stringWithCString:fileName encoding:NSUTF8StringEncoding];
    NSString *appFile = [documentsDirectory stringByAppendingPathComponent:fName];
    NSData* data=[NSData dataWithBytes:src length:writeSize];
    return ([data writeToFile:appFile atomically:YES]);
}

long sys_file_readMyData(const char* fileName,UInt8* dst,unsigned long readSize){
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
#ifdef _MACOSX
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"4avol"];
#endif
    NSString* fName= [NSString stringWithCString:fileName encoding:NSUTF8StringEncoding];
    NSString *appFile = [documentsDirectory stringByAppendingPathComponent:fName];
    NSData *data = [[[NSData alloc] initWithContentsOfFile:appFile] autorelease];
    long fileSize=[data length];
    if (dst==0) 
        return fileSize;
    else{
        if (readSize>fileSize)
            readSize=fileSize;
        [data getBytes:dst length:readSize];
        return readSize;	
    }
}

#else

long sys_file_writeMyData(const char* fileName,const UInt8* src,unsigned long writeSize){
    void* fileHandle=sys_file_create(fileName,0);
    if (fileHandle==0) return 0;
    long result=sys_file_write(fileHandle,src,writeSize);
    sys_file_close(fileHandle);
    return result;		
}

long sys_file_readMyData(const char* fileName,UInt8* dst,unsigned long readSize){
    void* fileHandle=sys_file_open_read(fileName,0);
    if (fileHandle==0) return 0;
    long result=0;
    
    if ((dst==0)&&(readSize==0))  //return size;
        result=sys_get_file_size(fileHandle);
    else
        result=sys_file_read(fileHandle,dst,readSize);
    sys_file_close(fileHandle);
    return result;		
}
#endif

#endif
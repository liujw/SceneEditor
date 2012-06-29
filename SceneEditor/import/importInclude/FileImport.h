//
//  FileImport.h
//
//  Created by housisong on 08-4-14.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _FileImport_h_
#define _FileImport_h_
#include "SysImport.h"

#ifdef __cplusplus 
extern "C" {
#endif
    
#ifndef csNullFileHandle
#define csNullFileHandle 0
#endif
    
    void* sys_file_create(const char* fileName,const char* filePath);
    void* sys_file_open_read(const char* fileName,const char* filePath);
    long sys_get_file_size(void* fileHandle);
    long sys_file_read(void* fileHandle,UInt8* dst,unsigned long readSize);
    long sys_file_seek(void* fileHandle,long newPos);
    long sys_file_read_from(void* fileHandle,long flStartPos,UInt8* dst,unsigned long readSize);
    long sys_file_write(void* fileHandle,const UInt8* src,unsigned long writeSize);
    void sys_file_close(void* fileHandle);
    int  sys_file_exists(const char* fileName,const char* filePath);
    int  sys_path_exists(const char* filePath);
    
    long sys_file_writeMyData(const char* fileName,const UInt8* src,unsigned long writeSize);
    long sys_file_readMyData(const char* fileName,UInt8* dst,unsigned long readSize);
    
    
#ifdef __cplusplus 
}
#endif




#endif //_FileImport_h_
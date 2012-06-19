//MyMD5Import.h

#ifndef _MyMD5Import_h_
#define _MyMD5Import_h_

void getMyMD5StrBin8(const void* data,long byte_length,char* out_md5BinStr_8);
void getMyMD5StrHex16(const void* data,long byte_length,char* out_md5HexStr_16);
void getMD5StdHex32(const void* data,long byte_length,char* out_md5HexStr_32);

#endif// _MyMD5Import_h_

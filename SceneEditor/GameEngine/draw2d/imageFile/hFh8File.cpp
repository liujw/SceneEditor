/*
 *  hFh8File.cpp
 *
 *  Created by housisong on 08-6-25.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "hFh8File.h"
#ifdef IS_NEED_CODE_FH8

#include "../hDraw.h"


static void Tfh8File_raise_error(const char* const Error)
{
	assert(false);
	throw new TReadFileException(Error); 
}

static void invalid_load()
{
	Tfh8File_raise_error(" open \"*.fh8\" picture file ERROR! ");
}

//文件头////
//文件类型　'F8'       2 byte
//文件版本  '08'       2 byte
//文件格式  '0a' or '0d'   2 byte
//Width                4 byte
//Height               4 byte

//0a: PixelsData       Width*Height byte
//0d: RLEDataLength    4 byte
//    RLEData          head: 1bit Type +15bit Length + ?Byte Data


const UInt16 csFH8Tag   = 'F' | ('8' << 8);  //标志
const UInt16 csVersion  = '0' | ('8'<< 8);  //版本

enum t_RLEFormatType 
{
	ftGeneral = ('0') | (('a') << 8),   //直接储存
    ftRLE     = ('0') | (('d') << 8)  //RLE行程压缩储存
};


static void  unRLE(UInt8* dst,IInputStream&  aInputStream,Int32 RLENodeCount){
	for (long i=0;i<RLENodeCount;++i){
		UInt16 head;
		aInputStream.read_trust(&head,2);
		long dataLength=head & ((1<<15)-1);
		long isRLENode=(head>>15);
		if (isRLENode){
			UInt8 value;
			aInputStream.read_trust(&value,1);
			for (long j=0;j<dataLength;++j)	dst[j]=value;
			dst+=dataLength;
		}else{
			aInputStream.read_trust(dst,dataLength);
			dst+=dataLength;
		}
	}	
}

bool Tfh8File::isCanLoadFrom(const IInputStream&  aInputStream){
	//checkHeadType
	UInt16 tmpWord;
	if (!aInputStream.test_read(&tmpWord,2)) return false;
	return (tmpWord==csFH8Tag);
}

void  Tfh8File::loadFrom(IPixelsGray8Buf* dst,IInputStream&  aInputStream){
	if (dst==0) return;
	
	if (!isCanLoadFrom(aInputStream))
		invalid_load();
	aInputStream.skip_trust(sizeof(csFH8Tag));

	UInt16 tmpWord=0;
	aInputStream.read_trust(&tmpWord,2);
	if (tmpWord!=csVersion)
		invalid_load();
	
	//read  rle type
	aInputStream.read_trust(&tmpWord,2);
	t_RLEFormatType RType=(t_RLEFormatType)tmpWord;
	
	if ((RType!=ftGeneral)&&(RType!=ftRLE))
		invalid_load();
	
	Int32 width=0;
	Int32 height=0;
	//read size
    aInputStream.read_trust(&width,4);
    aInputStream.read_trust(&height,4);
	
	//resize 
	PixelsGray8Ref gray8Ref;
	dst->lockRef(width,height,gray8Ref);
	
	if (RType==ftGeneral)
	{
		if (gray8Ref.byte_width==gray8Ref.width)
			aInputStream.read_trust(gray8Ref.pdata,width*height);
		else{
			for (long y=0;y<height;++y)
				aInputStream.read_trust(gray8Ref.getLinePixels(y),width);
		}
		return;
	}
	
	//else
	//RType==ftRLE
	
	Int32 RLENodeCount=0;
    aInputStream.read_trust(&RLENodeCount,4);
	if (gray8Ref.byte_width==gray8Ref.width){
		unRLE(gray8Ref.pdata,aInputStream,RLENodeCount);
	}else{
		Vector<UInt8> tmpData;
		tmpData.resize(width*height);
		unRLE(&tmpData[0],aInputStream,RLENodeCount);
		PixelsGray8Ref dataRef(&tmpData[0],width,height,width);
		copy(gray8Ref,0,0,dataRef);
	}
}


#endif


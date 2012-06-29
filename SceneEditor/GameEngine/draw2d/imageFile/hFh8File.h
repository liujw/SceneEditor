/*
 *  hFh8File.h
 *
 *  Created by housisong on 08-6-25.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _hFh8File_h_
#define _hFh8File_h_
#include "hImgFile.h"
#ifdef IS_NEED_CODE_FH8


class Tfh8File{
protected:
	void operator =(const Tfh8File& tmp);//error
public:
    explicit Tfh8File(){}
    ~Tfh8File(){}
	static bool isCanLoadFrom(const IInputStream&  aInputStream);
    static void  loadFrom(IPixelsGray8Buf* dst,IInputStream&  aInputStream);

	static void  loadFrom(IPixelsGray8Buf* dst,const char* const aFileName){//*.fh8
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream);
	}
};


#endif
#endif //_hFh8File_h_


//hImgFile.h


#ifndef _hImgFile_h_
#define _hImgFile_h_

#include "../hColor32.h"
#include "../../base/hStream.h"


#define IS_NEED_CODE_FH  
#define IS_NEED_CODE_FH8 
#define IS_NEED_CODE_BMP 

#if (!defined(_IOS) && !defined(WINCE))
	#define IS_NEED_CODE_PNG
	#define IS_NEED_CODE_JPG
#endif

	class IPixelsGray8Buf{
	public:
		virtual void lockRef(long width,long height,PixelsGray8Ref& out_gray8Ref)=0;
		virtual ~IPixelsGray8Buf(){}
	};

	class IPixels32Buf{
	public:
		virtual void lockRef(long width,long height,Pixels32Ref& out_color32Ref)=0;
		virtual ~IPixels32Buf(){}
	};

	class TReadFileException:public Exception{
	private:
		String m_err;
	public:
	    TReadFileException() :Exception(){}
		TReadFileException(const char * Error_Msg) :Exception(),m_err(Error_Msg){ }
		virtual const char* what() const throw() { return m_err.c_str(); }
		
		virtual ~TReadFileException() throw(){}
	};


#endif //_hImgFile_h_
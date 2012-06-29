//hJpegFile.h


#ifndef _hJpegFile_h_
#define _hJpegFile_h_
#include "hImgFile.h"
#ifdef IS_NEED_CODE_JPG




class TJpegFileBase
{
protected:
    virtual void  fast_resize(long width,long height)=0;
	virtual Color32* get_pixels_line(long y)=0;
protected:
	void operator =(const TJpegFileBase& tmp);//error
public:
	explicit TJpegFileBase(){}
	virtual ~TJpegFileBase(){}	
	static bool checkHeadType(const IInputStream&  aInputStream);
    void   load_from(IInputStream&  aInputStream);
};

class TJpegFileColor32Base:public TJpegFileBase
{
private:
	IPixels32Buf* m_buf;
	Pixels32Ref   m_ref;
protected:
    void  fast_resize(long width,long height) { m_buf->lockRef(width,height,m_ref); }
	virtual Color32* get_pixels_line(long y){ return m_ref.getLinePixels(y); }
public:
    explicit TJpegFileColor32Base(IPixels32Buf* dst_buf):m_buf(dst_buf) {}
};


class TJpegColor32File{
protected:
	void operator =(const TJpegColor32File& tmp);//error
public:
    explicit TJpegColor32File(){}
    ~TJpegColor32File(){}
	static bool isCanLoadFrom(const IInputStream&  aInputStream){
		return TJpegFileBase::checkHeadType(aInputStream);
	}
	static void  loadFrom(IPixels32Buf* dst,IInputStream&  aInputStream){
		if (dst==0) return;
		TJpegFileColor32Base jpg(dst);
		jpg.load_from(aInputStream);
	}
	static void  loadFrom(IPixels32Buf* dst,const char* const aFileName){//*.jpg *.jpeg
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream);
	}
};


#endif
#endif//#ifndef _hJpegFile_h_

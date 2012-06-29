//hPngFile.h


#ifndef _hPngFile_h_
#define _hPngFile_h_
#include "hImgFile.h"
#ifdef IS_NEED_CODE_PNG


 
class TPngFileBase
{
public:
    virtual void  fast_resize(long width,long height)=0;
	virtual void* get_pixels_line(long y)=0;
	virtual long get_pixels_width()=0;
	virtual long get_pixels_height()=0;
public:
	virtual void load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine) =0;
protected:
	void operator =(const TPngFileBase& tmp);//error
public:
	explicit TPngFileBase(){}
	virtual ~TPngFileBase(){}	
	static bool checkHeadType(const IInputStream&  aInputStream);
    void   load_from(IInputStream&  aInputStream);
    void save_to(IOutputStream&  aOutputStream) const;
};

class TPngFileColor32Base:public TPngFileBase
{
private:
	IPixels32Buf* m_buf;
	Pixels32Ref   m_ref;
protected:
	void operator =(const TPngFileColor32Base& tmp);//error
public:
    void  fast_resize(long width,long height) { m_buf->lockRef(width,height,m_ref); }
	void* get_pixels_line(long y){ return m_ref.getLinePixels(y); }
	void load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine);
	long get_pixels_width(){ return m_ref.width; }
	long get_pixels_height(){ return m_ref.height; }
public:
    explicit TPngFileColor32Base(IPixels32Buf* dst_buf=0):m_buf(dst_buf) {}
	void save_to(const Pixels32Ref& srcRef, IOutputStream&  aOutputStream){
		m_ref=srcRef; m_buf=0;
		TPngFileBase::save_to(aOutputStream);
	}
};


class TPngFileGray8Base:public TPngFileBase
{
private:
	IPixelsGray8Buf* m_buf;
	PixelsGray8Ref   m_ref;
	UInt8  m_Pallette[256];
	bool   m_isInitPallette;
protected:
	void operator =(const TPngFileGray8Base& tmp);//error
public:
    void  fast_resize(long width,long height) { m_buf->lockRef(width,height,m_ref); }
	void* get_pixels_line(long y){ return m_ref.getLinePixels(y); }
	void load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine);
	long get_pixels_width(){ return m_ref.width; }
	long get_pixels_height(){ return m_ref.height; }
public:
    explicit TPngFileGray8Base(IPixelsGray8Buf* dst_buf):m_buf(dst_buf),m_isInitPallette(false) {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class TPngColor32File{
protected:
	void operator =(const TPngColor32File& tmp);//error
public:
    explicit TPngColor32File(){}
    ~TPngColor32File(){}
	static bool isCanLoadFrom(const IInputStream&  aInputStream){
		return TPngFileBase::checkHeadType(aInputStream);
	}
	static void  loadFrom(IPixels32Buf* dst,IInputStream&  aInputStream){
		if (dst==0) return;
		TPngFileColor32Base png(dst);
		png.load_from(aInputStream);
	}
	static void  loadFrom(IPixels32Buf* dst,const char* const aFileName){//*.png
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream);
	}
	static void saveTo(const Pixels32Ref& srcRef, IOutputStream&  aOutputStream){
		TPngFileColor32Base png;
		png.save_to(srcRef,aOutputStream);
	}
	static void saveTo(const Pixels32Ref& srcRef,const char* const aFileName){
		FileOutputStream aOutputStream(aFileName);
		saveTo(srcRef,aOutputStream);
	}
};




class TPngGray8File{
protected:
	void operator =(const TPngGray8File& tmp);//error
public:
    explicit TPngGray8File(){}
    ~TPngGray8File(){}
	static bool isCanLoadFrom(const IInputStream&  aInputStream){
		return TPngFileBase::checkHeadType(aInputStream);
	}
	static void  loadFrom(IPixelsGray8Buf* dst,IInputStream&  aInputStream){
		if (dst==0) return;
		TPngFileGray8Base png(dst);
		png.load_from(aInputStream);
	}
	static void  loadFrom(IPixelsGray8Buf* dst,const char* const aFileName){//*.png
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream);
	}
};


#endif
#endif//#ifndef _hPngFile_h_

//hBmpFile.h
//  "hBmpFile.cpp"


#ifndef _hBmpFile_h_
#define _hBmpFile_h_
#include "hImgFile.h"
#ifdef IS_NEED_CODE_BMP


//注意,不支持压缩bmp格式
class TBmpFileBase{
protected:
    static void raise_error(const char* const Error);
protected:
	virtual void fast_resize(long width,long height) =0;
	virtual long get_bmp_byte_width() const =0;
	virtual long get_width() const =0;
	virtual long get_height() const =0;
    virtual long  write_palette(IOutputStream&  aOutputStream) const { return 0; }
    virtual long  get_palette_byte_size() const { return 0; }
    virtual long  get_bmp_bit_count() const=0;
	virtual void* get_pixels_line(long y) const =0;
protected:
	virtual void save_copy_line(IOutputStream& aOutputStream,const void* pixelsLine)const=0;
	virtual void load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine) =0;
protected:
	void operator =(const TBmpFileBase& tmp);//error
public:
    TBmpFileBase(){}
    virtual ~TBmpFileBase(){}
	static bool checkHeadType(const IInputStream&  aInputStream);
    void load_from(IInputStream&  aInputStream,bool IsKeepAlpha);
    void save_to(IOutputStream&  aOutputStream) const;
};

class TBmpFileColor32Base:public TBmpFileBase{
protected:
	IPixels32Buf* m_buf;
	Pixels32Ref   m_ref;
protected:
	virtual void fast_resize(long width,long height){
		//assert(m_buf!=0);
		m_buf->lockRef(width,height,m_ref);
	}
	virtual long get_bmp_byte_width() const{ return m_ref.width*4; }
	virtual long get_width() const { return m_ref.width; }
	virtual long get_height() const { return m_ref.height; }
    virtual long  write_palette(IOutputStream&  aOutputStream) const { return 0; }
    virtual long  get_palette_byte_size() const { return 0; }
	virtual long  get_bmp_bit_count() const { return 32; }
	virtual void* get_pixels_line(long y) const{ return m_ref.getLinePixels(y); }
protected:
	virtual void save_copy_line(IOutputStream& aOutputStream,const void* pixelsLine)const;
	virtual void load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine);
public:
	TBmpFileColor32Base():m_buf(0){}
	void load_from(IPixels32Buf* dstBuf,IInputStream&  aInputStream,bool IsKeepAlpha){
		m_buf=dstBuf; m_ref.setNullRef();
		TBmpFileBase::load_from(aInputStream,IsKeepAlpha);
	}
	void save_to(const Pixels32Ref& srcRef, IOutputStream&  aOutputStream){
		m_ref=srcRef; m_buf=0;
		TBmpFileBase::save_to(aOutputStream);
	}
};


class TBmpFileGray8Base:public TBmpFileBase{
protected:
	IPixelsGray8Buf* m_buf;
	PixelsGray8Ref   m_ref;	
	UInt8  m_Pallette[256];
	bool   m_isInitPallette;

protected:
	virtual void fast_resize(long width,long height){
		//assert(m_buf!=0);
		m_buf->lockRef(width,height,m_ref);
	}
	virtual long get_bmp_byte_width() const{ return ((m_ref.width+3)>>2)<<2; }
	virtual long get_width() const { return m_ref.width; }
	virtual long get_height() const { return m_ref.height; }
    virtual long  write_palette(IOutputStream&  aOutputStream) const;
    virtual long  get_palette_byte_size() const;
	virtual long  get_bmp_bit_count() const { return 8; }
	virtual void* get_pixels_line(long y) const{ return m_ref.getLinePixels(y); }
protected:
	virtual void save_copy_line(IOutputStream& aOutputStream,const void* pixelsLine)const;
	virtual void load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine);
public:
	TBmpFileGray8Base():m_buf(0),m_isInitPallette(0){}
	void load_from(IPixelsGray8Buf* dstBuf,IInputStream&  aInputStream){
		m_buf=dstBuf; m_ref.setNullRef();
		m_isInitPallette=false;
		TBmpFileBase::load_from(aInputStream,false);
	}
	void save_to(const PixelsGray8Ref& srcRef, IOutputStream&  aOutputStream){
		m_ref=srcRef; m_buf=0;
		TBmpFileBase::save_to(aOutputStream);
	}
};


//////////////


class TBmpColor32File{
protected:
	void operator =(const TBmpColor32File& tmp);//error
public:
    explicit TBmpColor32File(){}
    ~TBmpColor32File(){}

	static bool isCanLoadFrom(const IInputStream&  aInputStream){
		return TBmpFileBase::checkHeadType(aInputStream);
	}

	static void  loadFrom(IPixels32Buf* dst,IInputStream&  aInputStream,bool IsKeepAlpha=false){
		if (dst==0) return;
		TBmpFileColor32Base bmp;
		bmp.load_from(dst,aInputStream,IsKeepAlpha);
	}
	static void  loadFrom(IPixels32Buf* dst,const char* const aFileName,bool IsKeepAlpha=false){//*.bmp
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream,IsKeepAlpha);
	}
	static void saveTo(const Pixels32Ref& srcRef, IOutputStream&  aOutputStream){
		TBmpFileColor32Base bmp;
		bmp.save_to(srcRef,aOutputStream);
	}
	static void saveTo(const Pixels32Ref& srcRef,const char* const aFileName){
		FileOutputStream aOutputStream(aFileName,false);
		saveTo(srcRef,aOutputStream);
	}
};

class TBmpGray8File{
protected:
	void operator =(const TBmpGray8File& tmp);//error
public:
	explicit TBmpGray8File(){}
    ~TBmpGray8File(){}	
	
	static bool isCanLoadFrom(const IInputStream&  aInputStream){
		return TBmpFileBase::checkHeadType(aInputStream);
	}

	static void  loadFrom(IPixelsGray8Buf* dst,IInputStream&  aInputStream){
		if (dst==0) return;
		TBmpFileGray8Base bmp;
		bmp.load_from(dst,aInputStream);
	}

	static void  loadFrom(IPixelsGray8Buf* dst,const char* const aFileName){//*.bmp
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream);
	}

	static void saveTo(const PixelsGray8Ref& srcRef, IOutputStream&  aOutputStream){
		TBmpFileGray8Base bmp;
		bmp.save_to(srcRef,aOutputStream);
	}
	static void saveTo(const PixelsGray8Ref& srcRef,const char* const aFileName){
		FileOutputStream aOutputStream(aFileName,false);
		saveTo(srcRef,aOutputStream);
	}
};

#endif
#endif //_hBmpFile_h_
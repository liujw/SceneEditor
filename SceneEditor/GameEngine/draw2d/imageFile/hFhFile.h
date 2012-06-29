//hFhFile.h


#ifndef _hFhFile_h_
#define _hFhFile_h_
#include "hImgFile.h"
#ifdef IS_NEED_CODE_FH

class TFhFileBase
{
private:
	void* m_Impot;
protected:
	void operator =(const TFhFileBase& tmp);//error
public:
    explicit TFhFileBase();
    ~TFhFileBase();
    void   loadFrom(const char* const aFileName);//后缀名称*.fh
    void   loadFrom(IInputStream&  aInputStream);		
	static bool checkHeadType(const IInputStream&  aInputStream);
	long   getWidth() const;
	long   getHeight() const;
	Color32 getColorFast(long x,long y)const;
	void copyTo(const Pixels32Ref& dst,long x0,long y0) const;
	void blendTo(const Pixels32Ref& dst,long x0,long y0) const;
	must_inline void copyTo(const Pixels32Ref& dst) const { copyTo(dst,0,0); }
	must_inline void blendTo(const Pixels32Ref& dst) const { blendTo(dst,0,0);  }
	void copyTo(long scrx0,long scry0,long scrx1,long scry1,const Pixels32Ref& dst,long x0,long y0) const;
	void blendTo(long scrx0,long scry0,long scrx1,long scry1,const Pixels32Ref& dst,long x0,long y0) const;
};


class TfhFile{
protected:
	void operator =(const TfhFile& tmp);//error
public:
    explicit TfhFile(){}
    ~TfhFile(){}

	static bool isCanLoadFrom(const IInputStream&  aInputStream){
		return TFhFileBase::checkHeadType(aInputStream);
	}

	static void  loadFrom(IPixels32Buf* dst,IInputStream&  aInputStream){
		if (dst==0) return;
		TFhFileBase fh;
		fh.loadFrom(aInputStream);

		Pixels32Ref buf;
		dst->lockRef(fh.getWidth(),fh.getHeight(),buf);
		fh.copyTo(buf);
	}

	static void  loadFrom(IPixels32Buf* dst,const char* const aFileName){//*.fh
		FileInputStream aInputStream(aFileName);
		loadFrom(dst,aInputStream);
	}
};

#endif
#endif //_hFhFile_h_
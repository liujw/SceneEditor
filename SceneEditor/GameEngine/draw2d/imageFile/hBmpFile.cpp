//hBmpFile.cpp

#include "hBmpFile.h"
#ifdef IS_NEED_CODE_BMP
//#include <Windows.h>
#include "../hColorConvert.h"


///////////

typedef UInt16 WORD;
typedef UInt32 DWORD;
typedef long   LONG;
typedef UInt8  BYTE;

struct BITMAPFILEHEADER_NO_TYPE {
	//WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} ;

struct BITMAPINFOHEADER{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
};

struct RGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
};

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L


////////

//
void TBmpFileBase::raise_error(const char* const Error)
{
	assert(false);
	throw new TReadFileException(Error); 
}

const UInt16 csBmpTag='B'|('M'<<8);
const long bmpTagSize=sizeof(csBmpTag); //BM

bool TBmpFileBase::checkHeadType(const IInputStream&  aInputStream){
	char tag[bmpTagSize];
	if (!aInputStream.test_read(&tag[0],bmpTagSize)) return false;
	return ((tag[0]=='b')||(tag[0]=='B')) && ((tag[1]=='m')||(tag[1]=='M'));
}

void TBmpFileBase::load_from(IInputStream&  aInputStream,bool IsKeepAlpha)
{
	long ReadPos=0;
	
	if (!checkHeadType(aInputStream))
		this->raise_error("bmp file \"BM\" tag error!");
	aInputStream.skip_trust(bmpTagSize);
	ReadPos+=bmpTagSize;
	
	//位图文件头结构和位图信息结构
	BITMAPFILEHEADER_NO_TYPE bmFileHeader;//读出BITMAPFILEHEADER结构
	memset(&bmFileHeader,0,sizeof(bmFileHeader));
	aInputStream.read_trust((unsigned char *)&bmFileHeader,sizeof(bmFileHeader));
	ReadPos+=sizeof(bmFileHeader);
	
	BITMAPINFOHEADER bmpInfo;//
	memset(&bmpInfo,0,sizeof(bmpInfo));
	aInputStream.read_trust((unsigned char *)&bmpInfo.biSize,sizeof(bmpInfo.biSize));
	aInputStream.read_trust((unsigned char *)&(((DWORD*)(&(bmpInfo.biSize)))[1]),STD::min((long)(sizeof(bmpInfo)-sizeof(DWORD)),(long)(bmpInfo.biSize-sizeof(DWORD))));
	if (sizeof(bmpInfo)<bmpInfo.biSize) 
		aInputStream.skip_trust(bmpInfo.biSize-sizeof(bmpInfo));
	ReadPos+=bmpInfo.biSize;
	
	bool IsReFor=bmpInfo.biHeight>0;
	bmpInfo.biHeight=abs(bmpInfo.biHeight);
	
	DWORD nPalletteNumColors=0;
	if (bmpInfo.biBitCount<=8)
	{
		nPalletteNumColors=bmpInfo.biClrUsed;
		if (nPalletteNumColors==0 )
			nPalletteNumColors=1 << bmpInfo.biBitCount;
	}
	
	
	long incBmpByteWidth=(((bmpInfo.biWidth*bmpInfo.biBitCount)+31) / (1<<5) ) << 2;
	if (bmpInfo.biSizeImage == 0) 
		bmpInfo.biSizeImage = abs(incBmpByteWidth * bmpInfo.biHeight);
	
	RGBQUAD Pallette[256];
	//调色板
	if (nPalletteNumColors>0 )
	{
		aInputStream.read_trust(&Pallette[0],nPalletteNumColors*sizeof(RGBQUAD));
		ReadPos+=nPalletteNumColors*sizeof(RGBQUAD);
		for (long i=0;i<(long)nPalletteNumColors;++i)
			Pallette[i].rgbReserved=0xFF;
	}
	else if (bmpInfo.biBitCount==16 )
	{
		//区分 RGB 15bit,RGB 16bit
		if (bmpInfo.biCompression==BI_BITFIELDS )
		{
			DWORD RMask,GMask,BMask;
			aInputStream.read_trust(&RMask,sizeof(DWORD));
			aInputStream.read_trust(&GMask,sizeof(DWORD));
			aInputStream.read_trust(&BMask,sizeof(DWORD));
			ReadPos+=(3*sizeof(DWORD));
			if (GMask==0x03E0 )
			{
				bmpInfo.biBitCount=15;
				//assert(RMask==0x7C00);
				//assert(BMask==0x001F);
			}
			else
			{
				bmpInfo.biBitCount=16;
				//assert(RMask==0xF800);
				//assert(GMask==0x07E0);
				//assert(BMask==0x001F);
			}
		}
		else
			bmpInfo.biBitCount=15;
	}
	
	assert(bmpInfo.biPlanes==1);
	assert(bmpInfo.biCompression!=BI_RLE4);//不支持压缩格式
	assert(bmpInfo.biCompression!=BI_RLE8);//不支持压缩格式
	
	this->fast_resize(bmpInfo.biWidth,bmpInfo.biHeight);
	
	incBmpByteWidth=abs(incBmpByteWidth);
	
    aInputStream.skip_trust(bmFileHeader.bfOffBits-ReadPos);
    ReadPos=bmFileHeader.bfOffBits;
    UInt8* pLinePixels=(UInt8*)aInputStream.read_trust(bmpInfo.biSizeImage);
    ReadPos+=bmpInfo.biSizeImage;
	
    for (long y=0;y<bmpInfo.biHeight;++y)
    {
		long py;
		if (IsReFor) 
			py=bmpInfo.biHeight-1-y;
		else
			py=y;
		this->load_copy_line(pLinePixels,bmpInfo.biBitCount,(Color32 *)(&Pallette[0]),IsKeepAlpha,this->get_pixels_line(py));
        pLinePixels+=incBmpByteWidth;
    }
}

void  TBmpFileBase::save_to(IOutputStream&  aOutputStream) const
{
	long BmpByteWidth=this->get_bmp_byte_width();
	
	WORD    bfType=csBmpTag;
    //写入类型信息
    aOutputStream.write(&bfType, sizeof(bfType));
	
    //填充BITMAPFILEHEADER结构
	BITMAPFILEHEADER_NO_TYPE bmFileHeader;//位图文件头结构和位图信息结构
    memset(&bmFileHeader,0,sizeof(bmFileHeader));
	DWORD dwDibBitsSize=this->get_height()*MyBase::abs(BmpByteWidth);
    DWORD dwOffBits =sizeof(bfType)+ sizeof(BITMAPFILEHEADER_NO_TYPE) + sizeof(BITMAPINFOHEADER) +this->get_palette_byte_size();
	
    DWORD dwFileSize = dwOffBits - dwDibBitsSize;
    bmFileHeader.bfSize = dwFileSize;
    bmFileHeader.bfReserved1 = 0;
    bmFileHeader.bfReserved2 = 0;
    bmFileHeader.bfOffBits = dwOffBits;
	
    //写入文件头和位图信息
    aOutputStream.write(&bmFileHeader, sizeof(BITMAPFILEHEADER_NO_TYPE));
	
    //
	BITMAPINFOHEADER bmpInfo; 
    memset(&bmpInfo,0,sizeof(bmpInfo));
	bmpInfo.biSize= sizeof(BITMAPINFOHEADER);
	bmpInfo.biWidth=this->get_width();
	bmpInfo.biHeight=this->get_height();
	bmpInfo.biPlanes=1;
	bmpInfo.biBitCount=(WORD)this->get_bmp_bit_count();
	bmpInfo.biCompression=0;
	bmpInfo.biSizeImage=0;
	bmpInfo.biXPelsPerMeter=0;
	bmpInfo.biYPelsPerMeter=0;
	bmpInfo.biClrUsed=0;
	bmpInfo.biClrImportant=0;
    aOutputStream.write(&bmpInfo, sizeof(BITMAPINFOHEADER));
	
	this->write_palette(aOutputStream);
	
    //写入数据             
    for (long y=bmpInfo.biHeight-1;y>=0;--y)
		save_copy_line(aOutputStream,this->get_pixels_line(y));
}


////////////////

void TBmpFileColor32Base::save_copy_line(IOutputStream& aOutputStream,const void* pixelsLine)const{
	long BmpByteWidth=this->get_bmp_byte_width();
    aOutputStream.write(pixelsLine,BmpByteWidth);
}

void TBmpFileColor32Base::load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine)
{
	long DstWidth=get_width();
    if ((!IsKeepAlpha)&&(SrcBitCount==32))
        convertLine_Color32bitNoAlpha_to_Color32((Color32*)(pixelsLine),(Color32*)pSrcLinePixels,DstWidth);
	
    else
        convertLine_Colorbit_to_Color32_array[SrcBitCount]((Color32*)(pixelsLine),pSrcLinePixels,DstWidth,Pallette);
}


//////////////

void TBmpFileGray8Base::save_copy_line(IOutputStream& aOutputStream,const void* pixelsLine)const{
	long BmpByteWidth=this->get_bmp_byte_width();
	long pixelsWidth=this->get_width();
    aOutputStream.write(pixelsLine,pixelsWidth);
	if (pixelsWidth<BmpByteWidth)
		aOutputStream.skip_fill(BmpByteWidth-pixelsWidth);
}

void TBmpFileGray8Base::load_copy_line(const void* pSrcLinePixels,const long SrcBitCount,const Color32 *Pallette,bool IsKeepAlpha,void* pixelsLine)
{
	long DstWidth=get_width();
	if ((SrcBitCount<=8)&&(!m_isInitPallette)){
		for (long i=0;i<(1<<SrcBitCount);++i)
			m_Pallette[i]=Pallette[i].toGrayFast();
		m_isInitPallette=true;
	}
	convertLine_Colorbit_to_Gray8_array[SrcBitCount]((Gray8*)(pixelsLine),pSrcLinePixels,DstWidth,m_Pallette);
}

long  TBmpFileGray8Base::write_palette(IOutputStream&  aOutputStream) const{
	aOutputStream.write(gray_paltette256,256*4);
	return 256*4; 
}
long  TBmpFileGray8Base::get_palette_byte_size() const{
	return 256*4;
}

#endif

#include "hFhFile.h"
#ifdef IS_NEED_CODE_FH

#include "../hBlendAPixel.h"
#include "../hBlend.h"
///////////////////////////////////////////////////////////////////////////////////////

/////Fh file type 

const  UInt16 csFHTag  = 'F' | ('H'<<8);	//文件标志
//const UInt16 csHRTag = 'H' | ('R' << 8);  //文件标志{停止支持}
const  UInt16 csVersion= '0' | ('6'<<8);	//版本

enum t_RLEFormatType 
{
	ftGeneral = ('0') | (('a') << 8),   //直接储存
    //ftZip     = ('0') | (('b') << 8),   //zip压缩储存{停止支持}
    //ftLZMA    = ('0') | (('c') << 8),   //LZMA压缩储存//不支持
    ftRLE     = ('0') | (('d') << 8)  //RLE行程压缩储存
};


/////fh Data type----------------------------------------------------------------------

const  UInt32	csColorWidth  = 8;

//NodeType
const  UInt16	niDefult		=  0x00;
//const UInt16	ni noUsed		=  0x01;  //保留
const  UInt16	niEmpty			=  0x02;  //Alpha=0
const  UInt16	niNoAlpha		=  0x04;  //Alpha=255
const  UInt16	niSingleColor	=  0x08;
const  UInt16	niBitAlpha		=  0x10;  //Alpha=0 or Alpha=255
const  UInt16	niNoRLE			=  0x20;  //没有压缩，8x7颜色数据放到了调色板中,后8个颜色数据放到了索引数据中

const  UInt32	niMaxNodeType	= niEmpty | niNoAlpha | niSingleColor | niBitAlpha | niNoRLE;

struct t_RLEInfoSet 
{
    UInt16 NodeType;
    UInt16 noUsed;//保留
};

struct  t_RLEPixelsNode
{
    t_RLEInfoSet	InfoSet;  //set of node info
    Int32			TableIndex;
    UInt8			ColorIndexs[((csColorWidth*csColorWidth) / 2)];

    inline UInt32  get_index(const Int32 x,const Int32 y) const	
				{
					UInt32 TwoColorIndex=ColorIndexs[(x>>1)+y*(csColorWidth>>1)];
					if ( (x & 0x01)==0 )
						return (TwoColorIndex & 0x0F);
					else
						return (TwoColorIndex >> 4);
				}
    inline void	 set_index(const Int32 x,const Int32 y,const UInt32 IndexValue)
				{
					UInt8* PIndex=&ColorIndexs[(x>>1)+y*(csColorWidth>>1)];
					if ( (x & 0x01)==0 )
						PIndex[0]=(UInt8)( (PIndex[0] & 0xF0) | IndexValue );
					else
						PIndex[0]=(UInt8)( (PIndex[0] & 0x0F) | (IndexValue << 4) );
				}
    inline Color32  get_color(const Color32* const PublicTable,const Int32 x,const Int32 y) const
				{
				  if ((this->InfoSet.NodeType & niNoRLE)!=0)
				  {
					if (y!=(csColorWidth-1))
					  return PublicTable[this->TableIndex+(x+y*csColorWidth)];
					else
					  return ( (Color32*)(&ColorIndexs[0]) )[x];
				  }
				  else
					return PublicTable[this->TableIndex+get_index(x,y)];
				}

    inline Color32  get_color_X_0_Y_0(const Color32* const PublicTable) const
				{
				  if ((this->InfoSet.NodeType & niNoRLE)!=0)
					  return PublicTable[this->TableIndex];
				  else
					return PublicTable[this->TableIndex+(ColorIndexs[0] & 0x0F)];
				}
};

	static bool bufClip(long x0,long y0,long x1,long y1,long& clip_x0,long& clip_y0,long& clip_x1,long& clip_y1){
		if (clip_x0<x0) clip_x0=x0;
		if (clip_x1>x1) clip_x1=x1;
		if (clip_x0>=clip_x1) return false;
		
		if (clip_y0<y0) clip_y0=y0;
		if (clip_y1>y1) clip_y1=y1;
		if (clip_y0>=clip_y1) return false;
		
		return true;
	}

struct t_RLEPixelsBufRef
{
    Int32		Dx;	//偏移的x
    Int32		Dy; //偏移的y
    Int32		width; //像素的宽
    Int32		height; //像素的高
    Color32*	PTables;
    Int32		PixelsNodeWidth  ;
    Int32		PixelsNodeHeight ;
    t_RLEPixelsNode* PPixelsNodes ;
    inline Color32 get_pixel(const Int32 x,const Int32 y) const
				{
				  Int32 px=this->Dx+x;			Int32 py=this->Dy+y;
				  t_RLEPixelsNode* PNode=&this->PPixelsNodes[(px>>3)/*div csColorWidth*/+(py>>3)/*div csColorWidth*/ * this->PixelsNodeWidth];
				  return PNode->get_color(PTables,(px & 0x07),(py & 0x07));
				}
    t_RLEPixelsBufRef  get_sub_RLEPixelsBufRef(long x0,long y0,long x1,long y1) const
	{
	  t_RLEPixelsBufRef	result=*this;
	  if (!bufClip(Dx,Dy,Dx+width,Dy+height, x0,y0,x1,y1) )
	  {
			result.width=0;
			result.height=0;
	  }
	  else
	  {
			result.Dx=x0;
			result.Dy=y0;
			result.width=x1-x0;
			result.height=y1-y0;
	  }
	  return result;
	}
};


/////RLE Draw--------------------------------------------------------------------------


typedef  void (*_t_DrawProc_RLE_Proc)       (const Pixels32Ref& Dst,const Color32* srcTable,const t_RLEPixelsNode* SrcNode);
typedef  void (*_t_DrawProc_RLE_Border_Proc)(const Pixels32Ref& Dst,const Color32* srcTable,const t_RLEPixelsNode* SrcNode,Int32 DstX0,Int32 DstY0,Int32 DstX1,Int32 DstY1);
const UInt32 _TDProc_Array_Length=niMaxNodeType+1;

static _t_DrawProc_RLE_Proc			_CopyRLE_Fast_Proc_Array[_TDProc_Array_Length];
static _t_DrawProc_RLE_Proc			_BlendRLE_Fast_Proc_Array[_TDProc_Array_Length];
static _t_DrawProc_RLE_Border_Proc	_CopyRLE_Border_Proc_Array[_TDProc_Array_Length];
static _t_DrawProc_RLE_Border_Proc	_BlendRLE_Border_Proc_Array[_TDProc_Array_Length];



static void _CopyRLE_Border_SingleColor(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{
  Color32* pDstData=&DstPixels.pdata[-x0];
  Color32 tmpColor=SrcNode->get_color_X_0_Y_0(srcTable);
  for (long y=y0 ; y<y1 ;++y)
  {
    for (long x=x0 ; x<x1;++x)
      pDstData[x]=tmpColor;
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
  }
}

static void _CopyRLE_Border_NoRLE(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{
  Color32* pDstData=&DstPixels.pdata[-x0];
  const Color32* pTable=&srcTable[SrcNode->TableIndex];

  for (long y=y0 ; y<y1 ;++y)
  {
    if (y==(csColorWidth-1)) 
    {
      Color32* PSrcNodeColorIndexs=(Color32*)&SrcNode->ColorIndexs[0];
	  for (long x=x0 ; x<x1;++x)
        pDstData[x]=PSrcNodeColorIndexs[x];
    }
    else
    {
	  for (long x=x0 ; x<x1;++x)
        pDstData[x]=pTable[(x+y*csColorWidth)];
    }
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
  }
}

static void _CopyRLE_Border(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{
  Color32* pDstData=&DstPixels.pdata[-x0];
  const Color32* pTable=&srcTable[SrcNode->TableIndex];

  const UInt8* PSrcNodeColorIndexs=&SrcNode->ColorIndexs[y0*(csColorWidth>>1)];
  for (long y=y0 ; y<y1 ;++y)
  {
    for (long x=x0 ; x<x1;++x)
    {
      UInt32 tmpColorIndex=PSrcNodeColorIndexs[x>>1];
      if ((x & 0x01)==0 )
        pDstData[x]=pTable[tmpColorIndex & 0x0F];
      else
        pDstData[x]=pTable[tmpColorIndex >> 4];
    }
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
    ((UInt8*&)PSrcNodeColorIndexs)+=(csColorWidth>>1);
  }
}

inline void _copy_Color32_width8(Color32* pDstData,const Color32* pSrcData)
{
	pDstData[0]=pSrcData[0];
	pDstData[1]=pSrcData[1];
	pDstData[2]=pSrcData[2];
	pDstData[3]=pSrcData[3];
	pDstData[4]=pSrcData[4];
	pDstData[5]=pSrcData[5];
	pDstData[6]=pSrcData[6];
	pDstData[7]=pSrcData[7];
}

static void _CopyRLE_Fast_NoRLE(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  //assert((niNoRLE and SrcNode.InfoSet.NodeType)!=0);
  Color32* pDstData=DstPixels.pdata;
  const Color32* pSrcData=&srcTable[SrcNode->TableIndex];
  for (long y=0 ; y<(8-1) ;++y)
  {
	  _copy_Color32_width8(pDstData,pSrcData);
    ((UInt8*&)pSrcData)+=(8*sizeof(Color32));
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
  }
  pSrcData=(const Color32*)&SrcNode->ColorIndexs[0];
  _copy_Color32_width8(pDstData,pSrcData);
}
static void _CopyRLE_Fast_SingleColor(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  //assert((niSingleColor and SrcNode.InfoSet.NodeType)!=0);
  Color32 tmpColor=SrcNode->get_color_X_0_Y_0(srcTable);
  Color32* pDstData=DstPixels.pdata;
  for (long y=0 ; y<8 ;++y)
  {
    pDstData[0]=tmpColor;
    pDstData[1]=tmpColor;
    pDstData[2]=tmpColor;
    pDstData[3]=tmpColor;
    pDstData[4]=tmpColor;
    pDstData[5]=tmpColor;
    pDstData[6]=tmpColor;
    pDstData[7]=tmpColor;
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
  }
}

inline void PackIndexToColor(UInt32 PackIndex,Color32* pDstData,const Color32* pTable)
{
	pDstData[0]=pTable[PackIndex & 0x0F];
	pDstData[1]=pTable[PackIndex >> 4];
}

static void _CopyRLE_Fast(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  //assert(DstPixels.width=csColorWidth);
  long DHeight=DstPixels.height;
  //long DByteWidth=DstPixels.byte_width;

  const Color32* pTable=&srcTable[SrcNode->TableIndex];
  Color32* pDstData=DstPixels.pdata;
  const UInt8* PColorIndexs=&SrcNode->ColorIndexs[0];

  for (long y=0 ; y<DHeight ;++y)
  {
      PackIndexToColor(PColorIndexs[0],&pDstData[0],pTable);
      PackIndexToColor(PColorIndexs[1],&pDstData[2],pTable);
      PackIndexToColor(PColorIndexs[2],&pDstData[4],pTable);
      PackIndexToColor(PColorIndexs[3],&pDstData[6],pTable);
      ((UInt8*&)pDstData)+=DstPixels.byte_width;
      ((UInt8*&)PColorIndexs)+=(csColorWidth>>1);
  }
}

static void _BlendRLE_Border_Empty(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{
  //empty
}

static void _BlendRLE_Border_SingleColor(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{
  Color32* pDstData=DstPixels.pdata;
  Color32 tmpColor=SrcNode->get_color_X_0_Y_0(srcTable);
  long width=x1-x0;
  TSetPixelFillBlend op(tmpColor);
  for (long y=y0 ; y<y1 ;++y)
  {
	  tm_fillLine(pDstData,op,width);
      ((UInt8*&)pDstData)+=DstPixels.byte_width;
  }
}


static void _BlendRLE_Border_NoRLE(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{
  Color32* pDstData=DstPixels.pdata;
  const Color32* pTable=&srcTable[SrcNode->TableIndex+(y0*csColorWidth)];

  const UInt8* PColorIndexs=&SrcNode->ColorIndexs[0];
  long width=x1-x0;
  TSetPixelBlend op;
  for (long y=y0 ; y<y1 ;++y)
  {
    if (y!=(csColorWidth-1)) 
	   tm_copyLine(pDstData,pTable,op,width);
    else
	   tm_copyLine(pDstData,&(((Color32*)PColorIndexs)[x0]),op,width);

    ((UInt8*&)pDstData)+=DstPixels.byte_width;
    ((UInt8*&)pTable)+=(sizeof(Color32)*csColorWidth);
  }
}


static void _BlendRLE_Border(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode,Int32 x0,Int32 y0,Int32 x1,Int32 y1)
{

  Color32* pDstData=&DstPixels.pdata[-x0];
  const Color32*pTable=&srcTable[SrcNode->TableIndex];

  const UInt8* PColorIndexs=&SrcNode->ColorIndexs[y0*(csColorWidth>>1)];
  for (long y=y0 ; y<y1 ;++y)
  {                                
    for (long x=x0;x<x1;++x)
    {
      UInt32 tmpColorIndex=PColorIndexs[x>>1];
      if ((x & 0x01)==0)
        TSetPixelBlend::set_pixel(&pDstData[x],pTable[tmpColorIndex & 0x0F]);
      else
        TSetPixelBlend::set_pixel(&pDstData[x],pTable[tmpColorIndex >> 4]);
    }
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
    ((UInt8*&)PColorIndexs)+=(csColorWidth>>1);
  }
  do_emms();
}

static void _BlendRLE_Fast_Empty(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
//Empty
}

static void _BlendRLE_Fast_NoRLE(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  //assert((niNoRLE and SrcNode.InfoSet.NodeType)!=0);

  Color32* pDstData=DstPixels.pdata;
  const Color32* pSrcData=&srcTable[SrcNode->TableIndex];
  TSetPixelBlend op;
  for (long y=0 ; y<(8-1) ;++y)
  {
    tm_copyLine(pDstData,pSrcData,op,8);
    ((UInt8*&)pDstData)+=DstPixels.byte_width;
    ((UInt8*&)pSrcData)+=(8*sizeof(Color32));
  }
  pSrcData=(const Color32*)(&SrcNode->ColorIndexs[0]);
  tm_copyLine(pDstData,pSrcData,op,8);
}


static void _BlendRLE_Fast_SingleColor(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  //assert((niSingleColor and SrcNode.InfoSet.NodeType)!=0);
  Color32 tmpColor=SrcNode->get_color_X_0_Y_0(srcTable);
  Color32* pDstData=DstPixels.pdata; 
  TSetPixelFillBlend op(tmpColor);
  for (long y=0 ; y<8 ;++y)
  {
      tm_fillLine(pDstData,op,8);
      ((UInt8*&)pDstData)+=DstPixels.byte_width;
  }
}

must_inline static void PackIndexBitblend(UInt32 PackIndex,Color32* pDstData,const Color32* pTable)
{
	Color32 tmpColor=pTable[PackIndex & 0x0F];
	if (tmpColor.a!=0)
		pDstData[0]=tmpColor;

	tmpColor=pTable[PackIndex >> 4];
	if (tmpColor.a!=0)
		pDstData[1]=tmpColor;
}


static void _BlendRLE_Fast_BitAlpha(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  //assert(niBitAlpha and SrcNode.InfoSet.NodeType!=0);

  Color32* pDstData=DstPixels.pdata;
  const Color32* pTable=&srcTable[SrcNode->TableIndex];
  const UInt8* PColorIndexs=&SrcNode->ColorIndexs[0];

    for (long y=0;y<DstPixels.height;++y)  //bit Blend
    {
		PackIndexBitblend(PColorIndexs[0],&pDstData[0],pTable);
		PackIndexBitblend(PColorIndexs[1],&pDstData[2],pTable);
		PackIndexBitblend(PColorIndexs[2],&pDstData[4],pTable);
		PackIndexBitblend(PColorIndexs[3],&pDstData[6],pTable);
      ((UInt8*&)pDstData)+=DstPixels.byte_width;
      ((UInt8*&)PColorIndexs)+=(csColorWidth>>1);
    }
}


must_inline static void PackIndexBlend(UInt32 PackIndex,Color32* pDstData,const Color32* pTable)
{
   TSetPixelBlend::set_pixel(&pDstData[0],pTable[PackIndex & 0x0F]);
   TSetPixelBlend::set_pixel(&pDstData[1],pTable[PackIndex >> 4]);
}

static void _BlendRLE_Fast(const Pixels32Ref& DstPixels,const Color32* srcTable,
								 const t_RLEPixelsNode* SrcNode)
{
  Color32* pDstData=DstPixels.pdata;
  const Color32* pTable=&srcTable[SrcNode->TableIndex];
  const UInt8* PColorIndexs=&SrcNode->ColorIndexs[0];

    for (long y=0;y<DstPixels.height;++y)  //bit Blend
    {
		PackIndexBlend(PColorIndexs[0],&pDstData[0],pTable);
		PackIndexBlend(PColorIndexs[1],&pDstData[2],pTable);
		PackIndexBlend(PColorIndexs[2],&pDstData[4],pTable);
		PackIndexBlend(PColorIndexs[3],&pDstData[6],pTable);
      ((UInt8*&)pDstData)+=DstPixels.byte_width;
      ((UInt8*&)PColorIndexs)+=(csColorWidth>>1);
    }
	do_emms();
}







//////////////////////////////


static void _Inti_DrawRLE_Proc_Array()
{
	static bool _Is_Inti=false;
	if (_Is_Inti) return;

	long i;
	for (i=0;i<_TDProc_Array_Length;++i){
		_CopyRLE_Border_Proc_Array[i]=0;
		_BlendRLE_Border_Proc_Array[i]=0;
		_CopyRLE_Fast_Proc_Array[i]=0;
		_BlendRLE_Fast_Proc_Array[i]=0;
	}
   
	for (i=0;i<_TDProc_Array_Length;++i)
    {
      if ( (i & niSingleColor)!=0 )
        _CopyRLE_Border_Proc_Array[i]=_CopyRLE_Border_SingleColor;
      else if ((i & niNoRLE)!=0 )
        _CopyRLE_Border_Proc_Array[i]=_CopyRLE_Border_NoRLE;
      else
        _CopyRLE_Border_Proc_Array[i]=_CopyRLE_Border;
    }

	for (i=0;i<_TDProc_Array_Length;++i)
    {
      if ((i & niEmpty)!=0 )
        _BlendRLE_Border_Proc_Array[i]=_BlendRLE_Border_Empty;
      else if ((i & niNoAlpha)!=0 )
        _BlendRLE_Border_Proc_Array[i]=_CopyRLE_Border_Proc_Array[i];//copy
      else if ((i & niSingleColor)!=0 )
        _BlendRLE_Border_Proc_Array[i]=_BlendRLE_Border_SingleColor;
      else if ((i & niNoRLE)!=0 )
        _BlendRLE_Border_Proc_Array[i]=_BlendRLE_Border_NoRLE;
      //else if ((i & niBitAlpha)!=0 )
      //  BlendRLE_Border_Proc_Array[i]=_BlendRLE_Border_BitAlpha;
      else
        _BlendRLE_Border_Proc_Array[i]=_BlendRLE_Border;
    }


	for (i=0;i<_TDProc_Array_Length;++i)
    {
      if ((i & niSingleColor)!=0 )
        _CopyRLE_Fast_Proc_Array[i]=_CopyRLE_Fast_SingleColor;
      else if ((i & niNoRLE)!=0 )
        _CopyRLE_Fast_Proc_Array[i]=_CopyRLE_Fast_NoRLE;
      else
        _CopyRLE_Fast_Proc_Array[i]=_CopyRLE_Fast;
    }


	for (i=0;i<_TDProc_Array_Length;++i)
    {
      if ((i & niEmpty)!=0 )
        _BlendRLE_Fast_Proc_Array[i]=_BlendRLE_Fast_Empty;
      else if ((i & niNoAlpha)!=0 )
        _BlendRLE_Fast_Proc_Array[i]=_CopyRLE_Fast_Proc_Array[i];//copy
      else if ((i & niSingleColor)!=0 )
        _BlendRLE_Fast_Proc_Array[i]=_BlendRLE_Fast_SingleColor;
      else if ((i & niNoRLE)!=0 )
        _BlendRLE_Fast_Proc_Array[i]=_BlendRLE_Fast_NoRLE;
      else if ((i & niBitAlpha)!=0 )
        _BlendRLE_Fast_Proc_Array[i]=_BlendRLE_Fast_BitAlpha;
      else
        _BlendRLE_Fast_Proc_Array[i]=_BlendRLE_Fast;
    }
 
	_Is_Inti=true;
}

  static bool draw_clip(long dst_width,long dst_height,long& dst_x0,long& dst_y0,long& src_x0,long& src_y0,long& src_x1,long& src_y1){
	if (dst_x0<0) 
	{
		src_x0-=dst_x0;
		dst_x0=0;
	}
	if (dst_y0<0)
	{
		src_y0-=dst_y0;
		dst_y0=0;
	}

	if (dst_x0+(src_x1-src_x0)>=dst_width)
		src_x1=dst_width-dst_x0+src_x0;
	if (dst_y0+(src_y1-src_y0)>=dst_height)
		src_y1=dst_height-dst_y0+src_y0;
	if (src_x0>=src_x1) return false;
	if (src_y0>=src_y1) return false;
  
	return true;		
  }


static void _RLEProc(const Pixels32Ref& Dst,long DstPoint_x0,long DstPoint_y0,const t_RLEPixelsBufRef& RLESrc,
			  const _t_DrawProc_RLE_Border_Proc* const Proc_RLE_Border_Array,const _t_DrawProc_RLE_Proc* const Proc_RLE_Fast_Array)
{
  _Inti_DrawRLE_Proc_Array();
  if (Dst.pdata==0)  return;
	//TODO:

  long SrcRect_x0=RLESrc.Dx;
  long SrcRect_y0=RLESrc.Dy;
  long SrcRect_x1=RLESrc.Dx+RLESrc.width;
  long SrcRect_y1=RLESrc.Dy+RLESrc.height;
  if (!draw_clip(Dst.width,Dst.height,DstPoint_x0,DstPoint_y0,SrcRect_x0,SrcRect_y0,SrcRect_x1,SrcRect_y1) )
    return;
  long DstPoint_x1=DstPoint_x0+(SrcRect_x1-SrcRect_x0);
  long DstPoint_y1=DstPoint_y0+(SrcRect_y1-SrcRect_y0);

  Int32 nodeX0=(SrcRect_x0>>3); //div csColorWidth;
  Int32 nodeY0=(SrcRect_y0>>3); //div csColorWidth;
  Int32 nodeX1=((SrcRect_x1+csColorWidth-1)>>3);// div csColorWidth;
  Int32 nodeY1=((SrcRect_y1+csColorWidth-1)>>3);// div csColorWidth;

  Pixels32Ref SubDst;
  t_RLEPixelsNode* PSrcNode=&RLESrc.PPixelsNodes[nodeY0*RLESrc.PixelsNodeWidth];
  for (Int32 nodeY=nodeY0; nodeY<=nodeY1 ;++nodeY)
  {
    for (Int32 nodeX=nodeX0; nodeX<=nodeX1 ;++nodeX)
    {
        Int32 sDx=0;
        Int32 sDy=0;
        Int32 dx0=DstPoint_x0+(nodeX*csColorWidth-SrcRect_x0);
        Int32 dy0=DstPoint_y0+(nodeY*csColorWidth-SrcRect_y0);
        Int32 dx1=dx0+csColorWidth;
        Int32 dy1=dy0+csColorWidth;
        if (dx0<DstPoint_x0)
        {
          sDx=DstPoint_x0-dx0;
          dx0=DstPoint_x0;
        }
        if (dx1>DstPoint_x1 )
          dx1=DstPoint_x1;
        if (dy0<DstPoint_y0 )
        {
          sDy=DstPoint_y0-dy0;
          dy0=DstPoint_y0;
        }
        if (dy1>DstPoint_y1 )
          dy1=DstPoint_y1;
        if (dx0>=dx1 ) break;
        if (dy0>=dy1 ) break;

        SubDst.pdata=Dst.pdata;
          ((UInt8*&)(SubDst.pdata))+=(Dst.byte_width*dy0+4*dx0);
        SubDst.byte_width=Dst.byte_width;
        SubDst.width=dx1-dx0;
        SubDst.height=dy1-dy0;
        t_RLEPixelsNode* ptmpSrcNode=&PSrcNode[nodeX];
        if ( (sDx>0) || (sDy>0) || (SubDst.width!=csColorWidth) || (SubDst.height!=csColorWidth)  )
          Proc_RLE_Border_Array[(ptmpSrcNode->InfoSet.NodeType)](SubDst,RLESrc.PTables, ptmpSrcNode,sDx,sDy,sDx+SubDst.width,sDy+SubDst.height);
        else
          Proc_RLE_Fast_Array[(ptmpSrcNode->InfoSet.NodeType)](SubDst,RLESrc.PTables, ptmpSrcNode);
        //SubDst.pixels[0,0]=ToColor32(random(256),random(256),random(256)); //test
    }
    ((UInt8*&)PSrcNode)+=(RLESrc.PixelsNodeWidth*sizeof(t_RLEPixelsNode));
  }
  do_emms();
}


inline void RLECopy(const Pixels32Ref& Dst,long x,long y,const t_RLEPixelsBufRef& RLESrc)
{
  _RLEProc(Dst,x,y,RLESrc,_CopyRLE_Border_Proc_Array,_CopyRLE_Fast_Proc_Array);
}
inline void RLEBlend(const Pixels32Ref& Dst,long x,long y,const t_RLEPixelsBufRef& RLESrc)
{
  _RLEProc(Dst,x,y,RLESrc,_BlendRLE_Border_Proc_Array,_BlendRLE_Fast_Proc_Array);
}



///////////////////////////////////////////////////////////////////////////////////////

class TFhFileBaseBase
{

private:
	t_RLEPixelsBufRef m_RLEBufRef;
protected:
	void operator =(const TFhFileBaseBase& tmp);//error
    void raise_error(const char* const Error);
	void invalid_load();
	void clear();
public:
    TFhFileBaseBase();
    ~TFhFileBaseBase();
	static bool checkHeadType(const IInputStream&  aInputStream);
    void   load_from(const char* const  aFileName);//后缀名称*.fh
    void   load_from(IInputStream&  aInputStream);
	inline const t_RLEPixelsBufRef& get_RLEPixelsBufRef() const { return m_RLEBufRef; }
};

TFhFileBaseBase::TFhFileBaseBase()
{
	m_RLEBufRef.PTables=0;
	m_RLEBufRef.PPixelsNodes=0;
	clear();
}
void TFhFileBaseBase::clear()
{
	m_RLEBufRef.Dx=0;
	m_RLEBufRef.Dy=0;
	m_RLEBufRef.width=0;
	m_RLEBufRef.height=0;
	m_RLEBufRef.PixelsNodeWidth=0;
	m_RLEBufRef.PixelsNodeHeight=0;

	delete []m_RLEBufRef.PTables;
	m_RLEBufRef.PTables=0;
	delete [] m_RLEBufRef.PPixelsNodes;
	m_RLEBufRef.PPixelsNodes=0;
}


TFhFileBaseBase::~TFhFileBaseBase()
{
	clear();
}


void TFhFileBaseBase::raise_error(const char* const Error)
{
	assert(false);
	throw new TReadFileException(Error); 
//
}

void TFhFileBaseBase::invalid_load()
{
	raise_error(" open \"*.fh\" picture file ERROR! ");
}


typedef UInt8 TFileNodeRLEType;
const TFileNodeRLEType rtDefult			=0;
const TFileNodeRLEType rtSingleColor	=1;
const TFileNodeRLEType rtSameX			=2;
const TFileNodeRLEType rtSameY			=3;


void  TFhFileBaseBase::load_from(const char* const  aFileName)//后缀名称*.fh
{
	FileInputStream aInputStream(aFileName);
	load_from(aInputStream);
}
 
bool TFhFileBaseBase::checkHeadType(const IInputStream&  aInputStream){
	UInt16 tmpWord;
	if (!aInputStream.test_read(&tmpWord,2)) return false;
	return (tmpWord==csFHTag);
}

void  TFhFileBaseBase::load_from(IInputStream&  aInputStream)
{
	//read fh tag
	if (!checkHeadType(aInputStream))
		invalid_load();
	aInputStream.skip_trust(sizeof(csFHTag));

	UInt16 tmpWord=0;
	aInputStream.read_trust(&tmpWord,2);
	if (tmpWord!=csVersion)
		invalid_load();

	//read  rle type
	aInputStream.read_trust(&tmpWord,2);
	t_RLEFormatType RType=(t_RLEFormatType)tmpWord;

	if ((RType!=ftGeneral)&&(RType!=ftRLE))
		invalid_load();

	this->clear();

	//read head data
    aInputStream.read_trust(&this->m_RLEBufRef.Dx,4);
    aInputStream.read_trust(&this->m_RLEBufRef.Dy,4);
    aInputStream.read_trust(&this->m_RLEBufRef.width,4);
    aInputStream.read_trust(&this->m_RLEBufRef.height,4);

	//resize color table
	UInt32 TableLength=0;
    aInputStream.read_trust(&TableLength,4);
	this->m_RLEBufRef.PTables=new Color32[TableLength];

	//resize PixelsNode
    aInputStream.read_trust(&this->m_RLEBufRef.PixelsNodeWidth,4);
	UInt32 PixelsNodeLength=0;
    aInputStream.read_trust(&PixelsNodeLength,4);
	this->m_RLEBufRef.PixelsNodeHeight=PixelsNodeLength/this->m_RLEBufRef.PixelsNodeWidth;
	this->m_RLEBufRef.PPixelsNodes=new t_RLEPixelsNode[PixelsNodeLength];
	
	//read color table
	aInputStream.read_trust(this->m_RLEBufRef.PTables,TableLength*sizeof(Color32));
	if (RType==ftGeneral)
	{
		aInputStream.read_trust(this->m_RLEBufRef.PPixelsNodes,PixelsNodeLength*sizeof(t_RLEPixelsNode));
		return;
	}
	
	//else
	//RType==ftRLE
    long RLESize=0;
    aInputStream.read_trust(&RLESize,sizeof(RLESize));
	TFileNodeRLEType* FileNodeRLETypes=0;
	try
	{
		FileNodeRLETypes =new TFileNodeRLEType[PixelsNodeLength];
		
		const UInt8* FileNodeRLETypesBuf=(const UInt8*)aInputStream.read_trust(RLESize*sizeof(TFileNodeRLEType));
		long j=0;
		for (long cvReadIndex=0 ;cvReadIndex<RLESize;++cvReadIndex)
		{
		  UInt8 tmpA=FileNodeRLETypesBuf[cvReadIndex];
		  TFileNodeRLEType NodeType=(tmpA & 0x03);
		  long i=j+(tmpA >> 2);
		  while (j<i)
		  {
			FileNodeRLETypes[j]=NodeType;
			++j;
		  }
		} 
		//assert(j==PixelsNodeLength);

		t_RLEPixelsNode* PNodes=this->m_RLEBufRef.PPixelsNodes;
		//todo:

		for (long i=0;i<(long)PixelsNodeLength;++i)
		{
		  switch  (FileNodeRLETypes[i])
		  {
		   case rtSameX:
			   {
			   PNodes[i]=PNodes[i-1];
			   break;
			   }
		   case rtSameY:
			   {
			   PNodes[i]=PNodes[i-this->m_RLEBufRef.PixelsNodeWidth];
			   break;
			   }
		   case rtSingleColor:
			   {
				   UInt8* PColorIndexs=&(PNodes[i].ColorIndexs[0]);
				   PColorIndexs[0]=0; PColorIndexs[1]=0;
				   PColorIndexs[2]=0; PColorIndexs[3]=0;
				   PColorIndexs[4]=0; PColorIndexs[5]=0;
				   PColorIndexs[6]=0; PColorIndexs[7]=0;
				   aInputStream.read_trust(&PNodes[i].TableIndex,4);
				   //assert(length(FTables)>PNodes[i].TableIndex);
				   //assert(0<=PNodes[i].TableIndex);
				   UInt8 tmpA=this->m_RLEBufRef.PTables[PNodes[i].TableIndex].a;
				   UInt16 NodeType=niSingleColor;
				   if (tmpA==0 ) 
					   NodeType=(NodeType | niEmpty);
				   else if (tmpA==255) 
					   NodeType=(NodeType | niNoAlpha);
				   PNodes[i].InfoSet.NodeType=NodeType;
				   PNodes[i].InfoSet.noUsed=0;
			   }
			   break;
		   default: // rtDefult
			   {
			   aInputStream.read_trust(&PNodes[i],sizeof(t_RLEPixelsNode));
			   break;
			   }
		  }

		}
	 
	}
    catch(...)
	{
		if (FileNodeRLETypes!=0){
			delete [] FileNodeRLETypes;
			FileNodeRLETypes=0;
		}
		throw;
	}
	if (FileNodeRLETypes!=0){
		delete [] FileNodeRLETypes;
		FileNodeRLETypes=0;
	}
}



///////////

TFhFileBase::TFhFileBase():m_Impot(new TFhFileBaseBase()){
}
TFhFileBase::~TFhFileBase(){
	delete ((TFhFileBaseBase*)m_Impot);
}

long   TFhFileBase::getWidth() const{
	return ((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef().width;
}
long   TFhFileBase::getHeight() const{
	return ((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef().height;
}


void  TFhFileBase::loadFrom(const char* const  aFileName){
	((TFhFileBaseBase*)m_Impot)->load_from(aFileName);
}

void  TFhFileBase::loadFrom(IInputStream&  aInputStream){
	((TFhFileBaseBase*)m_Impot)->load_from(aInputStream);
}


bool TFhFileBase::checkHeadType(const IInputStream&  aInputStream){
	return TFhFileBaseBase::checkHeadType(aInputStream);
}


Color32 TFhFileBase::getColorFast(long x,long y)const{
	return ((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef().get_pixel(x,y);
}

void TFhFileBase::copyTo(const Pixels32Ref& dst,long x0,long y0) const{
  RLECopy(dst,x0,y0,((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef());
}
void TFhFileBase::blendTo(const Pixels32Ref& dst,long x0,long y0) const{
  RLEBlend(dst,x0,y0,((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef());
}

void TFhFileBase::copyTo(long scrx0,long scry0,long scrx1,long scry1,const Pixels32Ref& dst,long x0,long y0) const{
	RLECopy(dst,x0,y0,((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef().get_sub_RLEPixelsBufRef(scrx0,scry0,scrx1,scry1));
}

void TFhFileBase::blendTo(long scrx0,long scry0,long scrx1,long scry1,const Pixels32Ref& dst,long x0,long y0) const{
	RLEBlend(dst,x0,y0,((TFhFileBaseBase*)m_Impot)->get_RLEPixelsBufRef().get_sub_RLEPixelsBufRef(scrx0,scry0,scrx1,scry1));
}


#endif
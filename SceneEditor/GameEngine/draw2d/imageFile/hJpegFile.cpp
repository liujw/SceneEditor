//hJpegFile.cpp

#include "hJpegFile.h"
#ifdef IS_NEED_CODE_JPG

extern "C" {
 #include "STDIO.h"
 #include "jpeg/jpeglib.h"
 #include "jpeg/jerror.h"
}


/*
 * HouSisong@263.net edit 2006.6.17
 */


extern "C" {
 #include "jpeg/jmorecfg.h" 
 #include <setjmp.h>
}

typedef UInt8 BYTE;

struct jpg_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;		/* for return to caller */
	char* buffer;				/* error message <CSC>*/
};
typedef jpg_error_mgr *jpg_error_ptr;

////////////////////////////////////////////////////////////////////////////////
// Here's the routine that will replace the standard error_exit method:
////////////////////////////////////////////////////////////////////////////////
static void
ima_jpeg_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	jpg_error_ptr myerr = (jpg_error_ptr) cinfo->err;
	/* Create the message */
	myerr->pub.format_message (cinfo, myerr->buffer);
	/* Send it to stderr, adding a newline */
	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}



////////////////////////////////////////////////////////////////////////////////////////

// thanks to Chris Shearer Cooper <cscooper(at)frii(dot)com>
class CxFileJpg : public jpeg_destination_mgr, public jpeg_source_mgr
	{
public:
	enum { eBufSize = 4096 };

	CxFileJpg(IInputStream* pFile)
	{
        m_pFile = pFile;

		init_destination = InitDestination;
		empty_output_buffer = EmptyOutputBuffer;
		term_destination = TermDestination;

		init_source = InitSource;
		fill_input_buffer = FillInputBuffer;
		skip_input_data = SkipInputData;
		resync_to_restart = jpeg_resync_to_restart; // use default method
		term_source = TermSource;
		next_input_byte = NULL; //* => next byte to read from buffer 
		bytes_in_buffer = 0;	//* # of bytes remaining in buffer 

		m_pBuffer = new unsigned char[eBufSize];
	}
	~CxFileJpg()
	{
		delete [] m_pBuffer;
	}

	static void InitDestination(j_compress_ptr cinfo)
	{
		CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
		pDest->next_output_byte = pDest->m_pBuffer;
		pDest->free_in_buffer = eBufSize;
	}

	static boolean EmptyOutputBuffer(j_compress_ptr cinfo)
	{/*
		CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
		if (pDest->m_pFile->Write(pDest->m_pBuffer,1,eBufSize)!=(size_t)eBufSize)
			ERREXIT(cinfo, JERR_FILE_WRITE);
		pDest->next_output_byte = pDest->m_pBuffer;
		pDest->free_in_buffer = eBufSize;*/
		return TRUE;
	}

	static void TermDestination(j_compress_ptr cinfo)
	{
		/*
		CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
		size_t datacount = eBufSize - pDest->free_in_buffer;
		// Write any data remaining in the buffer 
		if (datacount > 0) {
			if (!pDest->m_pFile->Write(pDest->m_pBuffer,1,datacount))
				ERREXIT(cinfo, JERR_FILE_WRITE);
		}
		pDest->m_pFile->Flush();
		// Make sure we wrote the output file OK 
		if (pDest->m_pFile->Error()) ERREXIT(cinfo, JERR_FILE_WRITE);*/
		return;
	}

	static void InitSource(j_decompress_ptr cinfo)
	{
		CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
		pSource->m_bStartOfFile = TRUE;
	}

	static boolean FillInputBuffer(j_decompress_ptr cinfo)
	{
		size_t nbytes;
		CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
		nbytes=eBufSize;
		pSource->m_pFile->read(pSource->m_pBuffer,(unsigned long&)nbytes);
		if (nbytes <= 0){
			if (pSource->m_bStartOfFile)	//* Treat empty input file as fatal error 
				ERREXIT(cinfo, JERR_INPUT_EMPTY);
			WARNMS(cinfo, JWRN_JPEG_EOF);
			// Insert a fake EOI marker 
			pSource->m_pBuffer[0] = (JOCTET) 0xFF;
			pSource->m_pBuffer[1] = (JOCTET) JPEG_EOI;
			nbytes = 2;
		}
		pSource->next_input_byte = pSource->m_pBuffer;
		pSource->bytes_in_buffer = nbytes;
		pSource->m_bStartOfFile = FALSE;
		return TRUE;
	}

	static void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
	{
		CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
		if (num_bytes > 0){
			while (num_bytes > (long)pSource->bytes_in_buffer){
				num_bytes -= (long)pSource->bytes_in_buffer;
				FillInputBuffer(cinfo);
				// note we assume that fill_input_buffer will never return FALSE,
				// so suspension need not be handled.
			}
			pSource->next_input_byte += (size_t) num_bytes;
			pSource->bytes_in_buffer -= (size_t) num_bytes;
		}
	}

	static void TermSource(j_decompress_ptr cinfo)
	{
		return;
	}
protected:
    IInputStream  *m_pFile;
	unsigned char *m_pBuffer;
	bool m_bStartOfFile;
};


const long jpgTagSize=3; //FFD8FF

bool TJpegFileBase::checkHeadType(const IInputStream&  aInputStream){
	UInt8 tag[jpgTagSize];
	if (!aInputStream.test_read(&tag[0],jpgTagSize)) return false;
	return (tag[0]==0xFF) && (tag[1]==0xD8) && (tag[2]==0xFF);
}

////////////////////////////////////////////////////////////////////////////////
void TJpegFileBase::load_from(IInputStream&  aInputStream)
{
	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler. <CSC> */
	struct jpg_error_mgr jerr;

	char info_szLastError[256];
	jerr.buffer=info_szLastError;
	/* More stuff */
	JSAMPARRAY buffer;	/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	/* Step 1: allocate and initialize JPEG decompression object */
	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = ima_jpeg_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		jpeg_destroy_decompress(&cinfo);
		return;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	//jpeg_stdio_src(&cinfo, infile);
	CxFileJpg srcJpg(&aInputStream);
    cinfo.src = &srcJpg;

	/* Step 3: read file parameters with jpeg_read_header() */
	(void) jpeg_read_header(&cinfo, TRUE);

	/* Step 4 <chupeev> handle decoder options*/

	// Set the scale <ignacio>
	//cinfo.scale_denom = GetJpegScale();

	/* Step 5: Start decompressor */
	jpeg_start_decompress(&cinfo);

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	*/
	//Create the image using output dimensions <ignacio>

	this->fast_resize(cinfo.output_width, cinfo.output_height);

	/*
		if (cinfo.density_unit==2){
			SetXDPI((long)floor(cinfo.X_density * 254.0 / 10000.0 + 0.5));
			SetYDPI((long)floor(cinfo.Y_density * 254.0 / 10000.0 + 0.5));
		} else {
			SetXDPI(cinfo.X_density);
			SetYDPI(cinfo.Y_density);
		}
	*/

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */
	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/

	long y=0;//cinfo.output_height-1;
	while (cinfo.output_scanline < cinfo.output_height) 
	{		
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		// info.nProgress = (long)(100*cinfo.output_scanline/cinfo.output_height);
		
		Color32 *SrcTempLinePixels=this->get_pixels_line(y);
		long DstWidth=0;
		if (cinfo.num_components==4)
		{
			BYTE*   src=buffer[0];
			DstWidth=row_stride/4;
			const unsigned long Mul_22=(1<<22)/255;
			for(long x=0,x4=0; x4<row_stride; ++x, x4+=4)
			{
				//<DP> Step 6a: CMYK->RGB */ 
				BYTE k=src[x4+3];
				//SrcTempLinePixels[x].R=(BYTE)((k * src[x4+0])/255);
				//SrcTempLinePixels[x].G=(BYTE)((k * src[x4+1])/255);
				//SrcTempLinePixels[x].B=(BYTE)((k * src[x4+2])/255);
				SrcTempLinePixels[x].r=(BYTE)((k * src[x4+0]*Mul_22)>>22);
				SrcTempLinePixels[x].g=(BYTE)((k * src[x4+1]*Mul_22)>>22);
				SrcTempLinePixels[x].b=(BYTE)((k * src[x4+2]*Mul_22)>>22);
				SrcTempLinePixels[x].a=255;
			}
		} 
		else if (cinfo.num_components==3) 
		{
			/* Assume put_scanline_someplace wants a pointer and sample count. */
			BYTE*   src=buffer[0];
			DstWidth=row_stride/3;
			for(long x=0,x3=0; x3<row_stride; ++x, x3+=3)
			{
				SrcTempLinePixels[x].r=src[x3+0];
				SrcTempLinePixels[x].g=src[x3+1];
				SrcTempLinePixels[x].b=src[x3+2];
				SrcTempLinePixels[x].a=255;
			}
		}
		else
		{
			assert(false);
		}
		++y;
	}

	/* Step 7: Finish decompression */
	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/


	/* Step 8: Release JPEG decompression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	/* And we're done! */
	return;
}

#endif
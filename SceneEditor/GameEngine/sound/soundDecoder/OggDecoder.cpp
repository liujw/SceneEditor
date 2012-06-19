
#include "OggDecoder.h"
#include "../wavDispose.h"

#include "ogg/os_types.h"
#include "ogg/codec.h"
#include "ogg/vorbisfile.h"

#define READ_SIZE (1024*4)

OggVorbis_File RandInputStream_T0_OggVorbis_File(IRandInputStream* inputStream);

/* This downmixes multi-channel vorbis to stereo
 */

class CImportOggDecoder
{
private:
	long             m_sound_time_ms;
	long             m_sound_rate;
    OggVorbis_File   m_OggVorbisFile;
	TSoundDataBuf    m_sound_data;

	unsigned int	m_loop_section_2;
	double			m_loop_samples_done;
	int				m_loop_eof;
	vorbis_info*	m_loop_vi;
	int				m_loop_dont_read_this_time;
	int				m_loop_current_section;
	int				m_pcm_channels;

	void doReadFinish()
	{
		m_sound_time_ms=0;
		m_sound_rate=0;
		m_loop_eof=1;
	    if (m_sound_data.pcm_buf!=0)
		{
			delete []m_sound_data.pcm_buf;
		}
		m_sound_data.pcm_buf=0;
		m_sound_data.pcm_count=0;
		m_sound_data.pcm_readed=0;
		m_sound_data.pcm_buf_max_count=0;
		if (m_OggVorbisFile.datasource!=0)
		{
			ov_clear(&m_OggVorbisFile);
			m_OggVorbisFile.datasource=0;
		}
	}
public:
    CImportOggDecoder():m_sound_time_ms(0),m_sound_rate(0)
	{
		m_OggVorbisFile.datasource=0;
		m_sound_data.pcm_buf=0;
		m_sound_data.pcm_buf_max_count=0;
		m_sound_data.pcm_count=0;
		m_sound_data.pcm_readed=0;
		m_loop_eof=1;
		m_pcm_channels=0;
	}
    ~CImportOggDecoder() 
	{ 
		doReadFinish();
	}
public:
	//ISoundDecoder
	bool init(IRandInputStream* inputStream);
	long get_sound_time_ms() const {  
		return m_sound_time_ms; }
	long get_sound_rate() const {  
		return m_sound_rate; }
    long get_sound_BitsPerSample() const {  
		return 16; }
    long get_sound_Channels() const {  
		return m_pcm_channels; }
	bool seek_time_ms(long Pos_ms);
	long tell_time_ms();
	TSoundDataBuf* decode();
	static bool checkHeadType(const IRandInputStream& aInputStream);
};

bool CImportOggDecoder::checkHeadType(const IRandInputStream& aInputStream){
	unsigned char tag[4];
	if (!aInputStream.test_read(&tag[0],4)) return false;

	if ( (tag[0]=='O') && (tag[1]=='g')&&(tag[2]=='g')&&(tag[3]=='S') ) return true;  //'OggS'
	return false;
}


bool CImportOggDecoder::init(IRandInputStream* inputStream)
{
    doReadFinish();
	m_OggVorbisFile=RandInputStream_T0_OggVorbis_File(inputStream);
	if  (ov_streams(&m_OggVorbisFile)<=0) 
	{
		return false;
	}
	m_loop_eof=0;
	m_loop_section_2 = 0;
	m_loop_samples_done = 0;
	m_loop_dont_read_this_time = 0;
	m_loop_current_section=0;

	m_loop_vi=ov_info(&m_OggVorbisFile,-1);
	m_sound_time_ms = (long)(ov_time_total(&m_OggVorbisFile,0)*1000.0);
	m_sound_rate=m_loop_vi->rate;

	decode(); //to get heard info 
	return true;
}

bool CImportOggDecoder::seek_time_ms(long Pos_ms)
{
	if (m_OggVorbisFile.datasource==0)
		return false;
	m_sound_data.pcm_count=0;
	m_sound_data.pcm_readed=0;
	ov_time_seek(&m_OggVorbisFile,Pos_ms*0.001);
	m_loop_eof=0;
	return true;
}
	
long CImportOggDecoder::tell_time_ms()
{
	if (m_OggVorbisFile.datasource==0)
		return 0;
    return (long)(ov_time_tell(&m_OggVorbisFile)*1000.0);
}


TSoundDataBuf* CImportOggDecoder::decode()
{
	while((!m_loop_eof) &&(m_sound_data.pcm_readed==m_sound_data.pcm_count))
	{
		float **pcm;
		long ret;

		if(m_loop_dont_read_this_time)
			m_loop_dont_read_this_time = 0;
		else
			ret = ov_read_float(&m_OggVorbisFile, &pcm, READ_SIZE, &m_loop_current_section);

		unsigned int section_1 = m_loop_current_section;
		if(section_1 != m_loop_section_2) {
			m_loop_section_2 = section_1;
			m_loop_eof = 1;
			m_loop_dont_read_this_time = 1;
		}
		else 
		{
			m_loop_samples_done += ret;
			if (ret == 0) 
				m_loop_eof = 1;
			else if (ret < 0)
				;// error in the stream.  Not a problem, just reporting it in case we (the app) cares.  In this case, we don't. 
			else
			{
				/* we don't bother dealing with sample rate changes, etc, but
				 * you'll have to
				 */
				int     convsize=READ_SIZE;
				long    bout=(ret<convsize?ret:convsize);

				m_pcm_channels=m_loop_vi->channels;
				if (m_loop_vi->channels>2)
				{
					downmix_2_stereo(m_loop_vi->channels, pcm, bout);
					m_pcm_channels=2;
			    }

				long new_pcm_count=2*bout*m_pcm_channels;
				if (new_pcm_count>m_sound_data.pcm_buf_max_count)
				{
					m_sound_data.pcm_buf_max_count=0;
					if (m_sound_data.pcm_buf!=0)
						delete []m_sound_data.pcm_buf;
					m_sound_data.pcm_buf=new unsigned char[new_pcm_count];
					m_sound_data.pcm_buf_max_count=new_pcm_count;
				}
				m_sound_data.pcm_count=new_pcm_count;
				m_sound_data.pcm_readed=0;

				short* OutputPtr=(short*)m_sound_data.pcm_buf;
				FixedToInt16(m_pcm_channels,pcm,bout,m_pcm_channels,OutputPtr);
			}
		}
	}

	return &m_sound_data;
}

////////////////
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

class CReadStream
{
public:
	IRandInputStream* m_inputStream;
	long m_pos;
	unsigned long   m_streamPos0;
	CReadStream(IRandInputStream* readStream){ 
		m_inputStream=readStream; 
		m_pos=0; 
		m_streamPos0=m_inputStream->getPos();  
	}
	~CReadStream(){
		clear();
	}
	
	void clear(){		
		if (m_inputStream!=0){
			delete m_inputStream;
			m_inputStream=0;
		}
	}
	inline unsigned char* streamRead(unsigned long pos,unsigned long& readCount){
		m_inputStream->setPos(pos+m_streamPos0);
		return (unsigned char*)m_inputStream->read(readCount);
	}

	inline void streamReadTo(void* dst,unsigned long pos,unsigned long& readCount){
		void* data=streamRead(pos,readCount);
		memcpy(dst,data,readCount);
	}
	inline unsigned long getStreamSize()const{
		return m_inputStream->getDataSize()-m_streamPos0;
	}
};

	#ifdef __cplusplus
	extern "C"
	{
	#endif
	    static size_t IReadStream_read_func (void *ptr, size_t size, size_t nmemb, void *datasource)
	    {
			unsigned long readedsize=(unsigned long)(size*nmemb);
			((CReadStream*)datasource)->streamReadTo(ptr,((CReadStream*)datasource)->m_pos,readedsize);
			((CReadStream*)datasource)->m_pos+=readedsize;
			return readedsize;
	    }
	    static int    IReadStream_seek_func (void *datasource, ogg_int64_t offset, int whence)
	    {
			long newpos=((CReadStream*)datasource)->m_pos;
			if (whence==FILE_BEGIN)
				newpos=(long)offset;
			else if (whence==FILE_CURRENT)
				newpos+=(long)offset;
			else if (whence==FILE_END)
				newpos=((CReadStream*)datasource)->getStreamSize()-(long)offset;
			((CReadStream*)datasource)->m_pos=newpos;
			return 0;
	    }
	    static int    IReadStream_close_func(void *datasource)
	    {
			delete ((CReadStream*)datasource);
			return 0;
	    }
	    static long   IReadStream_tell_func (void *datasource)
	    {
			return ((CReadStream*)datasource)->m_pos;
	    }
	#ifdef __cplusplus
	}
	#endif /* __cplusplus */


OggVorbis_File RandInputStream_T0_OggVorbis_File(IRandInputStream* inputStream)
{
	ov_callbacks callbacks;
	callbacks.read_func=IReadStream_read_func;
	callbacks.seek_func=IReadStream_seek_func;
	callbacks.close_func=IReadStream_close_func;
	callbacks.tell_func=IReadStream_tell_func;
	CReadStream* aReadStream=new CReadStream(inputStream);
    OggVorbis_File OggVorbisFile;
	ov_open_callbacks(aReadStream,&OggVorbisFile,0,0,callbacks);
	return OggVorbisFile;
}


///////////

bool COggDecoder::init(IRandInputStream* inputStream)
{
	return ((CImportOggDecoder*)m_ImportDecoder)->init(inputStream);
}

long COggDecoder::get_sound_time_ms() const
{
	return ((CImportOggDecoder*)m_ImportDecoder)->get_sound_time_ms();
}

long COggDecoder::get_sound_rate() const
{
	return ((CImportOggDecoder*)m_ImportDecoder)->get_sound_rate();
}
long COggDecoder::get_sound_BitsPerSample() const
{
	return ((CImportOggDecoder*)m_ImportDecoder)->get_sound_BitsPerSample();
}
long COggDecoder::get_sound_Channels() const
{
	return ((CImportOggDecoder*)m_ImportDecoder)->get_sound_Channels();
}

TSoundDataBuf* COggDecoder::decode()
{
	return ((CImportOggDecoder*)m_ImportDecoder)->decode();
}

COggDecoder::COggDecoder() :m_ImportDecoder(0){
	m_ImportDecoder=new CImportOggDecoder();
}

COggDecoder::~COggDecoder(){
	CImportOggDecoder* tmpImportDecoder=(CImportOggDecoder*)m_ImportDecoder;
	m_ImportDecoder=0;
	delete tmpImportDecoder;
}

bool COggDecoder::seek_time_ms(long Pos_ms){
	return ((CImportOggDecoder*)m_ImportDecoder)->seek_time_ms(Pos_ms);
}

long COggDecoder::tell_time_ms(){
	return ((CImportOggDecoder*)m_ImportDecoder)->tell_time_ms();
}


bool COggDecoder::isCanLoadFrom(const IRandInputStream&  aInputStream){
	return CImportOggDecoder::checkHeadType(aInputStream);
}

#include "Mp3Decoder.h"
#include "mp3/mad.h" //用了libmad的mp3解码库

#include <string.h>
#include <limits.h>

#define READ_SIZE	(16*1024)

class CImportMp3Decoder
{
private:
	long             m_sound_time_ms;
	long             m_sound_rate;
	TSoundDataBuf    m_sound_data;
	IRandInputStream* m_inputStream;
	unsigned long   m_streamPos0;

	int				m_loop_eof;
	int				m_loop_read_pos;
	long            m_Mp3DataBegin_Offset;
	mad_stream		m_Stream;
	mad_frame		m_Frame;
	mad_synth		m_Synth; 
	mad_timer_t		m_DecTime;
	long            m_frameTime;
	long            m_frameCount;
	unsigned char	m_InputBuffer[READ_SIZE+MAD_BUFFER_GUARD];
	unsigned long	m_FramePos;
	long            m_bitrate;
	long			m_channels;

	void doReadFinish(){	
		m_sound_time_ms=0;
		m_sound_rate=0;
		m_loop_eof=1;
	    if (m_sound_data.pcm_buf!=0)
		{
			delete []m_sound_data.pcm_buf;
			m_sound_data.pcm_buf=0;
		}
		memset(&m_sound_data,0,sizeof(m_sound_data));
	    mad_synth_finish(&m_Synth);
	    mad_frame_finish(&m_Frame);
	    mad_stream_finish(&m_Stream);
	}
	void clear(){
		doReadFinish();
		if (m_inputStream!=0){
			delete m_inputStream;
			m_inputStream=0;
		}
	}
	void doReadBegin(){
		mad_stream_init(&m_Stream);
		mad_stream_options(&m_Stream, MAD_OPTION_IGNORECRC);
		mad_frame_init(&m_Frame);
		mad_synth_init(&m_Synth);
		mad_timer_reset(&m_DecTime);
		m_loop_eof=0;
		m_loop_read_pos=0;
		m_sound_data.pcm_count=0;
		m_sound_data.pcm_readed=0;
		m_FramePos=0;
	}
public:
    CImportMp3Decoder():m_sound_time_ms(0),m_sound_rate(0),m_inputStream(0){
		memset(&m_sound_data,0,sizeof(m_sound_data));
		doReadBegin();	
		m_streamPos0=0;
		m_channels=0;
	}
    ~CImportMp3Decoder() { 
		clear();
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

public:
	//ISoundDecoder
	bool init(IRandInputStream* inputStream);
	long get_sound_time_ms() const {  
		return m_sound_time_ms;
	}
	long get_sound_rate() const {  
		return m_sound_rate; 
	}
    long get_sound_BitsPerSample() const {  return 16; }
    long get_sound_Channels() const {  
		return m_channels; 
	}
	bool seek_time_ms(long Pos_ms);
	long tell_time_ms();
	TSoundDataBuf* decode();
	static bool checkHeadType(const IRandInputStream& aInputStream);
};


bool CImportMp3Decoder::checkHeadType(const IRandInputStream& aInputStream){
	unsigned char tag[3];
	if (!aInputStream.test_read(&tag[0],3)) return false;

	if ( (tag[0]=='I') && (tag[1]=='D')&&(tag[2]=='3') ) return true;  //'ID3'
	if ( (tag[0]==0xFF) && ((tag[1]>>4)==0xF) ) return true;  //FFF
	
	unsigned long maxCheckSize=1024*4;
	unsigned char* data=(unsigned char*)aInputStream.test_read(maxCheckSize);
	for (long i=0;i<(long)maxCheckSize-1;++i){
		if ( (data[i]==0xFF) && ((data[i+1]>>4)==0xF) ) return true;  //FFF
		if (data[i]!=0) return false;
	}

	return false;
}
bool CImportMp3Decoder::init(IRandInputStream* inputStream)
{
    clear();
	m_inputStream=inputStream;
	m_streamPos0=m_inputStream->getPos();

	m_sound_time_ms =0;
	m_sound_rate=0;
	doReadBegin();
	decode(); //to get heard info 

	return true;
}

bool CImportMp3Decoder::seek_time_ms(long Pos_ms)
{
    doReadFinish();
	doReadBegin();
	decode(); //to get heard info 
	m_sound_data.pcm_readed=m_sound_data.pcm_count;
	m_loop_read_pos=m_Mp3DataBegin_Offset + Pos_ms*(m_bitrate/8)/1000;
	m_FramePos=Pos_ms/m_frameTime+1;
	mad_stream_buffer(&m_Stream,&m_InputBuffer[0],0);
	m_Stream.error = MAD_ERROR_NONE;
	return true;
}
	
long CImportMp3Decoder::tell_time_ms()
{
	return (long)((m_loop_read_pos-m_Mp3DataBegin_Offset)*1000.0/(m_bitrate/8));
}


	inline signed short MadFixedToSshort(mad_fixed_t Fixed)
	{
		if(Fixed>=MAD_F_ONE)
			return(SHRT_MAX);
		else if(Fixed<=-MAD_F_ONE)
			return(-SHRT_MAX);
		else
			return (signed short)(Fixed>>(MAD_F_FRACBITS-15));//Conversion
	}


TSoundDataBuf* CImportMp3Decoder::decode()
{
	unsigned char*	GuardPtr=0;
	while((!m_loop_eof) &&(m_sound_data.pcm_readed==m_sound_data.pcm_count))
	{
		if(m_Stream.buffer==NULL || m_Stream.error==MAD_ERROR_BUFLEN)
		{
			unsigned long	ReadSize,
							Remaining;
			unsigned char	*ReadStart;

			if(m_Stream.next_frame!=NULL)
			{
				Remaining=(unsigned long)(m_Stream.bufend-m_Stream.next_frame);
				memmove(m_InputBuffer,m_Stream.next_frame,Remaining);
				ReadStart=m_InputBuffer+Remaining;
				ReadSize=READ_SIZE-Remaining;
			}
			else
			{
				ReadSize=READ_SIZE;
				ReadStart=m_InputBuffer;
				Remaining=0;
			}

			streamReadTo(ReadStart,m_loop_read_pos,ReadSize);
			m_loop_read_pos+=ReadSize;
			if(ReadSize<=0)
			{
				m_loop_eof=1;
				break;
			}

			if(m_loop_read_pos>=(long)getStreamSize())
			{
				GuardPtr=ReadStart+ReadSize;
				memset(GuardPtr,0,MAD_BUFFER_GUARD);
				ReadSize+=MAD_BUFFER_GUARD;
				m_loop_read_pos+=MAD_BUFFER_GUARD;
			}

			mad_stream_buffer(&m_Stream,m_InputBuffer,ReadSize+Remaining);
			m_Stream.error=MAD_ERROR_NONE;
		}

		if(mad_frame_decode(&m_Frame,&m_Stream))
		{
			if(MAD_RECOVERABLE(m_Stream.error))
				continue;// recoverable m_Frame level error 
			else
			{
				if(m_Stream.error==MAD_ERROR_BUFLEN)
					continue;
				else	
				{
					m_loop_eof=1;
					break;//unrecoverable m_Frame level error
				}
			}
		}

		if(m_FramePos==0)
		{
			m_sound_rate=m_Frame.header.samplerate;
			m_bitrate=m_Frame.header.bitrate;

			//get all time
			mad_timer_t duration = m_Frame.header.duration;
			m_frameTime  = mad_timer_count(duration,MAD_UNITS_MILLISECONDS);
			m_frameCount = 0;

			long offset = m_loop_read_pos;
			if(m_Stream.this_frame!=NULL)
				offset-= (long)(m_Stream.bufend-m_Stream.this_frame);
			else
				offset-= (long)(m_Stream.bufend-m_Stream.buffer);
			if (offset<0) offset=0;
			m_Mp3DataBegin_Offset=offset;
			if(offset <= (long)getStreamSize() ) {
				m_sound_time_ms =(long)( (((long)getStreamSize()-m_Mp3DataBegin_Offset)*1000.0) / (m_bitrate/8) );
				m_frameCount =(long) ((m_sound_time_ms+m_frameTime-1)/m_frameTime);
			}
		}
		
		m_FramePos++;
		mad_timer_add(&m_DecTime,m_Frame.header.duration);


		mad_synth_frame(&m_Synth,&m_Frame);

		m_channels=MAD_NCHANNELS(&m_Frame.header);
		if (m_channels>2) m_channels=2;

		long new_pcm_count=sizeof(short)*m_Synth.pcm.length*m_channels;
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
		if(m_channels==2)
		{
			for(long i=0;i<m_Synth.pcm.length;i++)
			{
				OutputPtr[i*2  ]=MadFixedToSshort(m_Synth.pcm.samples[0][i]);
				OutputPtr[i*2+1]=MadFixedToSshort(m_Synth.pcm.samples[1][i]);
			}
		}
		else
		{
			for(long i=0;i<m_Synth.pcm.length;i++) {
				OutputPtr[i]=MadFixedToSshort(m_Synth.pcm.samples[0][i]);
			}
		}
	}

	return &m_sound_data;
}


///////////

bool CMp3Decoder::init(IRandInputStream* inputStream){
	return ((CImportMp3Decoder*)m_ImportDecoder)->init(inputStream);
}

long CMp3Decoder::get_sound_time_ms() const
{
	return ((CImportMp3Decoder*)m_ImportDecoder)->get_sound_time_ms();
}

long CMp3Decoder::get_sound_rate() const
{
	return ((CImportMp3Decoder*)m_ImportDecoder)->get_sound_rate();
}
long CMp3Decoder::get_sound_BitsPerSample() const
{
	return ((CImportMp3Decoder*)m_ImportDecoder)->get_sound_BitsPerSample();
}
long CMp3Decoder::get_sound_Channels() const
{
	return ((CImportMp3Decoder*)m_ImportDecoder)->get_sound_Channels();
}

TSoundDataBuf* CMp3Decoder::decode()
{
	return ((CImportMp3Decoder*)m_ImportDecoder)->decode();
}

CMp3Decoder::CMp3Decoder():m_ImportDecoder(0){
	m_ImportDecoder=new CImportMp3Decoder();
}

CMp3Decoder::~CMp3Decoder(){
	CImportMp3Decoder* tmpImportDecoder=(CImportMp3Decoder*)m_ImportDecoder;
	m_ImportDecoder=0;
	delete tmpImportDecoder;
}

bool CMp3Decoder::seek_time_ms(long Pos_ms){
	return ((CImportMp3Decoder*)m_ImportDecoder)->seek_time_ms(Pos_ms);
}

long CMp3Decoder::tell_time_ms(){
	return ((CImportMp3Decoder*)m_ImportDecoder)->tell_time_ms();
}


bool CMp3Decoder::isCanLoadFrom(const IRandInputStream&  aInputStream){
	return CImportMp3Decoder::checkHeadType(aInputStream);
}
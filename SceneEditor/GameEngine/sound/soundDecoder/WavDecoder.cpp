
#include "WavDecoder.h"
#include "../wavDispose.h"
#include <string.h>


#define READ_SIZE (1024*64)

	struct RIFF{    
				 char  riffID[4];         //RIFF标识    
		unsigned long  riffSIZE;          //文件长度减8 字节 
				 char  riffFORMAT[4];     //WAVE标识："WAVE"    
	};  


	struct WAVEFORM{    
		unsigned short wFormatTag;          //数字声音的格式   WAVE_FORMAT_PCM  \ WAVE_FORMAT_ADPCM ...
		unsigned short nChannels;           //声道的数量（1为单声道、2为立体声）    
		unsigned long  nSamplesPerSec;      //每秒样本数    
		unsigned long  nAvgBytesPerSec;     //每秒平均字节数    
		unsigned short nBlockAlign;         //播放的最小字节数    
		unsigned short wBitsPerSample;      //每样本位数(8 或16)    
	};  
	const unsigned short WAVE_FORMAT_PCM=1;

	struct FMT{    
		char			fmtID[4];           //FMT标识: "fmt " (含空格)    
		unsigned long   fmtSIZE;            //块大小(对于PCM16而言)                               
		WAVEFORM		fmtFORMAT;          //WAVEFORMATEX结构（但是没有cbSize）    
	};

	struct WAVHEAD{
		RIFF aRIFF;
		FMT  aFMT;
	};

	struct DATA{    
		char		  dataID[4];         //DATA块表示: "data"    
		unsigned long dataSIZE;           //数据大小    
	}; 

const long dec_time_ms=140;  //末尾衰减时间 减少爆音的可能

class CImportWavDecoder{
private:
	long            m_sound_time_ms;
	long            m_sound_rate;
	TSoundDataBuf   m_sound_data;
	IRandInputStream*m_inputStream;
	unsigned long   m_streamPos0;

	RIFF			m_RIFF;
	FMT             m_FMT;
	DATA            m_DATA;
	long            m_WavDataBegin_Offset;
	int				m_loop_eof;
	int				m_loop_read_pos;
	long            m_dec_pos;
	double          m_dec_DIV_REV;

	void clear(){
		m_sound_time_ms=0;
		m_sound_rate=0;
		m_loop_eof=1;
		memset(&m_sound_data,0,sizeof(m_sound_data));
		memset(&m_RIFF,0,sizeof(RIFF));
		memset(&m_FMT,0,sizeof(FMT));
		
		m_streamPos0=0;
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

public:
    CImportWavDecoder():m_inputStream(0){
		clear();
	}
    ~CImportWavDecoder() { 
		clear();
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
    long get_sound_BitsPerSample() const {  
		return m_FMT.fmtFORMAT.wBitsPerSample; 
	}
    long get_sound_Channels() const {  
		return m_FMT.fmtFORMAT.nChannels; 
	}
	bool seek_time_ms(long Pos_ms);
	long tell_time_ms();
	TSoundDataBuf* decode();

	static bool checkHeadType(const IRandInputStream& aInputStream);
};


bool CImportWavDecoder::checkHeadType(const IRandInputStream& aInputStream){
	WAVHEAD wavHead;
	if (!aInputStream.test_read(&wavHead,sizeof(wavHead))) return false;
	RIFF& aRIFF=wavHead.aRIFF;
	FMT&  aFMT=wavHead.aFMT;
	if ( (aRIFF.riffID[0]!='R')||(aRIFF.riffID[1]!='I')||(aRIFF.riffID[2]!='F')||(aRIFF.riffID[3]!='F') ) return false;
	if (aFMT.fmtFORMAT.wFormatTag!=1) return false;
	if ( (aFMT.fmtID[0]!='f')||(aFMT.fmtID[1]!='m')||(aFMT.fmtID[2]!='t')/*||(aFMT.fmtID[3]!=' ')*/ ) return false;
	return true;
}

bool CImportWavDecoder::init(IRandInputStream* inputStream){
    clear();

	m_inputStream=inputStream;
	m_streamPos0=inputStream->getPos();
	m_WavDataBegin_Offset=0;
	m_loop_eof=1;

	if (!checkHeadType(*m_inputStream)) return false;
	m_inputStream->read_trust(&m_RIFF,sizeof(m_RIFF));
	m_inputStream->read_trust(&m_FMT,sizeof(m_FMT));

	//find "data" tag
	unsigned long readed=READ_SIZE;
	unsigned char* pwavdata=(unsigned char*)streamRead(sizeof(WAVHEAD),readed);
	long dataTag=findStr((const char*)pwavdata,readed,"data",4);
	if (dataTag>=0){
		m_WavDataBegin_Offset=sizeof(WAVHEAD)+dataTag;
		readed=sizeof(DATA);
		streamReadTo(&m_DATA,m_WavDataBegin_Offset,readed);
		m_WavDataBegin_Offset+=readed;
		m_loop_read_pos=0;

	}else
		return false;

	m_loop_eof=0;

	m_sound_time_ms =(long)(m_DATA.dataSIZE*1000.0/m_FMT.fmtFORMAT.nAvgBytesPerSec);
	m_sound_rate=m_FMT.fmtFORMAT.nSamplesPerSec;
	m_dec_pos=(long)(m_DATA.dataSIZE-(dec_time_ms*(1.0/1000)*m_FMT.fmtFORMAT.nAvgBytesPerSec));
	if (m_dec_pos<(long)m_DATA.dataSIZE*7/8) m_dec_pos=m_DATA.dataSIZE*7/8;
	m_dec_pos=((unsigned long)m_dec_pos)/4*4;
	if (m_DATA.dataSIZE==m_dec_pos) m_dec_pos-=4;
	m_dec_pos+=m_WavDataBegin_Offset;
	m_dec_DIV_REV=1.0/((long)m_DATA.dataSIZE+m_WavDataBegin_Offset-m_dec_pos);

	return true;
}

bool CImportWavDecoder::seek_time_ms(long Pos_ms){
	m_sound_data.pcm_count=0;
	m_sound_data.pcm_readed=0;
	//todo:
	if (m_WavDataBegin_Offset<=0)
		m_loop_eof=1;
	else
		m_loop_eof=0;

	m_loop_read_pos=(long)(m_WavDataBegin_Offset+Pos_ms*(1.0/1000)*m_FMT.fmtFORMAT.nAvgBytesPerSec);

	return true;
}
	
long CImportWavDecoder::tell_time_ms(){
    return (long)(m_loop_read_pos*1000.0/m_FMT.fmtFORMAT.nAvgBytesPerSec);
}

TSoundDataBuf* CImportWavDecoder::decode(){
	while((!m_loop_eof) &&(m_sound_data.pcm_readed==m_sound_data.pcm_count))
	{
		m_sound_data.pcm_buf_max_count=READ_SIZE;
		long new_pcm_count=READ_SIZE;
		m_sound_data.pcm_buf=streamRead(m_WavDataBegin_Offset+m_loop_read_pos,(unsigned long&)new_pcm_count);
		if (m_loop_read_pos+new_pcm_count>m_dec_pos)
		{
			unsigned char* pBuf=m_sound_data.pcm_buf;
			long Pos=m_loop_read_pos;
			if (m_loop_read_pos<m_dec_pos)
			{
				pBuf=&pBuf[m_dec_pos-m_loop_read_pos];
				Pos=m_dec_pos;
			}
			long byte_size=m_loop_read_pos+new_pcm_count-Pos;
			wavDispose_DEC(m_FMT.fmtFORMAT.wBitsPerSample,pBuf,byte_size,
				((long)m_DATA.dataSIZE-Pos)*m_dec_DIV_REV,
				((long)m_DATA.dataSIZE-(m_loop_read_pos+new_pcm_count))*m_dec_DIV_REV);
		}
		m_loop_read_pos+=new_pcm_count;
		if (new_pcm_count==0)
			m_loop_eof=1;
		m_sound_data.pcm_count=new_pcm_count;
		m_sound_data.pcm_readed=0;
	}
	return &m_sound_data;
}

///////////

bool CWavDecoder::init(IRandInputStream* inputStream){
	return ((CImportWavDecoder*)m_ImportDecoder)->init(inputStream);
}

long CWavDecoder::get_sound_time_ms() const{
	return ((CImportWavDecoder*)m_ImportDecoder)->get_sound_time_ms();
}

long CWavDecoder::get_sound_rate() const{
	return ((CImportWavDecoder*)m_ImportDecoder)->get_sound_rate();
}
long CWavDecoder::get_sound_BitsPerSample() const
{
	return ((CImportWavDecoder*)m_ImportDecoder)->get_sound_BitsPerSample();
}
long CWavDecoder::get_sound_Channels() const{
	return ((CImportWavDecoder*)m_ImportDecoder)->get_sound_Channels();
}

TSoundDataBuf* CWavDecoder::decode(){
	return ((CImportWavDecoder*)m_ImportDecoder)->decode();
}

CWavDecoder::CWavDecoder():m_ImportDecoder(0){
	m_ImportDecoder=new CImportWavDecoder();
}

CWavDecoder::~CWavDecoder(){
	CImportWavDecoder* tmpImportDecoder=(CImportWavDecoder*)m_ImportDecoder;
	m_ImportDecoder=0;
	delete tmpImportDecoder;
}

bool CWavDecoder::seek_time_ms(long Pos_ms){
	return ((CImportWavDecoder*)m_ImportDecoder)->seek_time_ms(Pos_ms);
}

long CWavDecoder::tell_time_ms(){
	return ((CImportWavDecoder*)m_ImportDecoder)->tell_time_ms();
}

bool CWavDecoder::isCanLoadFrom(const IRandInputStream&  aInputStream){
	return CImportWavDecoder::checkHeadType(aInputStream);
}


/////////////


	static unsigned long getBlockAlign(unsigned long blockBit){
		for (unsigned long i=1;i<8;++i){
			if (((blockBit*i)%8)==0)
				return (blockBit*i)/8;
		}
		return blockBit; //blockBit*8/8;
	}

	const unsigned short csHeadToDataSkip=0;

unsigned long CWavEncoder::getWavFileSize(unsigned long wavPCMDataSize){
	return sizeof(WAVHEAD)+sizeof(csHeadToDataSkip)+sizeof(DATA)+wavPCMDataSize;
}

void CWavEncoder::encoder(IOutputStream* outputStream,const TWavDataInfo& wavInfo,unsigned long wavPCMDataSize,const void* wavData){

	WAVHEAD wavHead;
	wavHead.aRIFF.riffFORMAT[0]='R';
	wavHead.aRIFF.riffFORMAT[1]='I';
	wavHead.aRIFF.riffFORMAT[2]='F';
	wavHead.aRIFF.riffFORMAT[3]='F';
	wavHead.aRIFF.riffSIZE=getWavFileSize(wavPCMDataSize)-8;
	wavHead.aRIFF.riffID[0]='W';
	wavHead.aRIFF.riffID[1]='A';
	wavHead.aRIFF.riffID[2]='V';
	wavHead.aRIFF.riffID[3]='E';
	wavHead.aFMT.fmtID[0]='f';
	wavHead.aFMT.fmtID[1]='m';
	wavHead.aFMT.fmtID[2]='t';
	wavHead.aFMT.fmtID[3]=' ';
	wavHead.aFMT.fmtSIZE=sizeof(WAVEFORM)+sizeof(csHeadToDataSkip);
	wavHead.aFMT.fmtFORMAT.wFormatTag=WAVE_FORMAT_PCM;       
	wavHead.aFMT.fmtFORMAT.nChannels=(unsigned short)wavInfo.channels;              
	wavHead.aFMT.fmtFORMAT.nSamplesPerSec=wavInfo.hz;     
	wavHead.aFMT.fmtFORMAT.nAvgBytesPerSec=(wavInfo.hz*wavInfo.bit*wavInfo.channels+7)/8;   
	wavHead.aFMT.fmtFORMAT.nBlockAlign=(unsigned short)getBlockAlign(wavInfo.bit*wavInfo.channels);   
	wavHead.aFMT.fmtFORMAT.wBitsPerSample=(unsigned short)wavInfo.bit;

	DATA aData;
	aData.dataID[0]='d';
	aData.dataID[0]='a';
	aData.dataID[0]='t';
	aData.dataID[0]='a';
	aData.dataSIZE=wavPCMDataSize;

	outputStream->write(&wavHead,sizeof(wavHead));
	outputStream->write(&csHeadToDataSkip,sizeof(csHeadToDataSkip));
	outputStream->write(&aData,sizeof(aData));
	if (wavData!=0)
		outputStream->write(wavData,wavPCMDataSize);
}

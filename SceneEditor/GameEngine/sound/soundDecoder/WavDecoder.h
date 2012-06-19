//WavDecoder.h

#ifndef __WavDecoder_H_
#define __WavDecoder_H_

#include "ISoundDecoder.h"

class CWavDecoder:public ISoundDecoder
{
private:
	void*         m_ImportDecoder;
public:
	//ISoundDecoder
	virtual bool init(IRandInputStream* inputStream);
	virtual long get_sound_time_ms() const;
	virtual long get_sound_rate() const;
    virtual long get_sound_BitsPerSample() const; 
    virtual long get_sound_Channels() const;
	virtual TSoundDataBuf* decode(); 
	virtual bool seek_time_ms(long Pos_ms);
	virtual long tell_time_ms();
public:
	CWavDecoder();
	virtual ~CWavDecoder();	
	static bool isCanLoadFrom(const IRandInputStream&  aInputStream);
};

struct TWavDataInfo{
	long channels;
	long hz;
	long bit;
};

class CWavEncoder
{
public:
	static unsigned long getWavFileSize(unsigned long wavPCMDataSize);
	static void encoder(IOutputStream* outputStream,const TWavDataInfo& wavInfo,unsigned long wavPCMDataSize,const void* wavData=0);
};

#endif //__WavDecoder_H_

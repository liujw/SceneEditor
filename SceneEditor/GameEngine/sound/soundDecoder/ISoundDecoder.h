//ISoundDecoder.h

#ifndef __ISoundDecoder_H_
#define __ISoundDecoder_H_

#include "../../base/hStream.h"

#define IS_NEED_CODE_WAV

#if (!defined(_IOS) && !defined(WINCE))
	#define IS_NEED_CODE_OGG
	#define IS_NEED_CODE_MP3
	//#ifdef WIN32
	//	#define IS_NEED_CODE_MIDI
	//#endif
#else
	#ifdef WINCE
		#define IS_NEED_CODE_MP3
	#endif
#endif


struct TSoundDataBuf
{
	UInt8* pcm_buf;
	long   pcm_buf_max_count;
	long   pcm_count;
	long   pcm_readed;
};
 
class ISoundDecoder{
public:
    virtual bool init(IRandInputStream* inputStream)=0;
    virtual long get_sound_time_ms() const=0;
    virtual long get_sound_rate() const=0;
    virtual long get_sound_BitsPerSample() const=0; // 8 bit or 16 bit
    virtual long get_sound_Channels() const=0;   // 1 Channels or 2 Channels
    virtual TSoundDataBuf* decode()=0;
	virtual bool seek_time_ms(long Pos_ms)=0;
	virtual long tell_time_ms()=0;
    virtual ~ISoundDecoder() {}

    unsigned long getPcmDataBytesCount(long time_ms){
        return (unsigned long) (0.001*get_sound_rate()*time_ms*(get_sound_BitsPerSample()>>3)*get_sound_Channels());
    }
};

#endif //__ISoundDecoder_H_

//Mp3Decoder.h

#ifndef __Mp3Decoder_H_
#define __Mp3Decoder_H_

#include "ISoundDecoder.h"

class CMp3Decoder:public ISoundDecoder
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
	CMp3Decoder();
	virtual ~CMp3Decoder();
	static bool isCanLoadFrom(const IRandInputStream&  aInputStream);
};

#endif //__Mp3Decoder_H_

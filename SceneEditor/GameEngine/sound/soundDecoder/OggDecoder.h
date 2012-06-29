//OggDecoder.h

#ifndef __OggDecoder_H_
#define __OggDecoder_H_

#include "ISoundDecoder.h"

class COggDecoder:public ISoundDecoder
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
	COggDecoder();
	virtual ~COggDecoder();
	static bool isCanLoadFrom(const IRandInputStream&  aInputStream);
};

#endif //__OggDecoder_H_

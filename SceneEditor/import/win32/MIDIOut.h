//MIDIOut.h

#ifndef __MIDIOut_H_
#define __MIDIOut_H_
#ifndef WINCE

#include "../../GameEngine/sound/ISoundOut.h"

#ifdef IS_NEED_CODE_MIDI

class ISoundMIDIBuffer:public ISoundBufferBase{
public:
	virtual void init(RandInputStream* inputStream)=0;
};

class CMIDIOut{
	void *  m_Import;
public:
	virtual ISoundMIDIBuffer* getNewSoundBuffer();
	virtual void  updateBufData(ISoundMIDIBuffer* SoundBuffer);
	virtual void  deleteSoundBuffer(ISoundMIDIBuffer* SoundBuffer);

    CMIDIOut(void* pDirectSound);
    virtual ~CMIDIOut();
public:
	static void setWinHandle(void* wHandle);
	static bool isCanLoadFrom(const RandInputStream&  aInputStream);
};

#endif
#endif
#endif //__MIDIOut_H_

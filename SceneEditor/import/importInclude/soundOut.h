//soundOut.h

#ifndef __soundOut_H_
#define __soundOut_H_

#include "../../GameEngine/sound/ISoundOut.h"

#ifndef _SOUND_PALY_ONE_

class CSoundOut:public ISoundOut{
private:
    void*  m_SoundOutImport;
public:
    CSoundOut();
    virtual ~CSoundOut();
    
    virtual bool init(); 
    virtual ISoundBuffer* getNewSoundBuffer();
    virtual void  updateBufData(ISoundBuffer* SoundBuffer);
    virtual void deleteSoundBuffer(ISoundBuffer* SoundBuffer);
public:
    void* getSoundOutHandle();
    static void setWinHandle(void* wHandle);
};

#endif

#endif //__soundOut_H

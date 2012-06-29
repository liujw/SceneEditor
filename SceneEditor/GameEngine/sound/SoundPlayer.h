//SoundPlayer.h

#ifndef __SoundPlayer_H_
#define __SoundPlayer_H_

#include "../base/hStream.h"

#ifndef _SOUND_PALY_ONE_

typedef void* TSoundHandle;

class CSoundPlayer{
    void *  m_Import;
public:
    TSoundHandle play(IRandInputStream* inputStream);
    TSoundHandle play(const char* fileName);
    TSoundHandle loop_play(IRandInputStream* inputStream);
    TSoundHandle loop_play(const char* fileName);
    TSoundHandle loop_play(IRandInputStream* inputStream,const long loopCount,const long msTime);
    TSoundHandle loop_play(const char* fileName,const long loopCount,const long msTime);
    bool stop(const void* sound_handle);
    void stop_all();
    CSoundPlayer();
    virtual ~CSoundPlayer();
public:
    static void setWinHandle(void* wHandle);
};

#endif

#endif //__SoundPlayer_H_

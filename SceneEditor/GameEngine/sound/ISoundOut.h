//ISoundOut.h

#ifndef __ISoundOut_H_
#define __ISoundOut_H_

#include "SoundDecoder/ISoundDecoder.h"


class ISoundBufferBase{
public:
    virtual void setID(void* ID)=0;
    virtual void* getID()=0;
    virtual void play(bool is_loop_play,long loopCount,long play_time_ms)=0;
    virtual bool is_paly_finished()=0;
    virtual void stop()=0;
    virtual ~ISoundBufferBase(){}
};

class ISoundBuffer:public ISoundBufferBase{
public:
    virtual void init(ISoundDecoder* soundDecoder)=0;
    virtual ~ISoundBuffer(){}
};

class ISoundOut
{
public:
    virtual bool init()=0; 
    virtual ISoundBuffer* getNewSoundBuffer()=0;
    virtual void  updateBufData(ISoundBuffer* soundBuffer)=0;
    virtual void deleteSoundBuffer(ISoundBuffer* soundBuffer)=0;
    virtual ~ISoundOut() {}
};


#endif //__ISoundOut_H_

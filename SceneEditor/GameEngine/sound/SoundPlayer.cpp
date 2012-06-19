//SoundPlayer.cpp

#include "SoundPlayer.h"
#ifndef _SOUND_PALY_ONE_

#define USES_TIMER_UPDATE_SOUND 1

#ifdef USES_TIMER_UPDATE_SOUND
#include "MMTimer.h"
#endif

#include "VThread.h"

#include "soundDecoder/OggDecoder.h"
#include "soundDecoder/WavDecoder.h"
#include "soundDecoder/Mp3Decoder.h"

#ifdef IS_NEED_CODE_MIDI
#include "../../import/win32/MIDIOut.h"
#endif 
#include "../../import/importInclude/soundOut.h"

const long csSoundIDMin=10000;

static void* g_sound_wHandle=0;
void CSoundPlayer::setWinHandle(void* wHandle)
{
    g_sound_wHandle=wHandle;
    CSoundOut::setWinHandle(wHandle);
#ifdef IS_NEED_CODE_MIDI
    CMIDIOut::setWinHandle(wHandle);
#endif
}

class CImportSoundPlayer
{
private:
    Vector<ISoundBufferBase*> m_SoundBufList;
    long				m_SoundHandleID;
    ISoundOut*			m_ISoundOut;
#ifdef IS_NEED_CODE_MIDI
    CMIDIOut*			m_MIDIOut;
#endif
#ifdef USES_TIMER_UPDATE_SOUND
    TAbstractTimer*		m_Timer;
#endif
    VLock				m_lock;
    inline long SoundBufCount()const { return (long)m_SoundBufList.size(); }
    void ClearASoundBuf(long Index);
    void* getNewSoundHandleID () {  
        ++m_SoundHandleID; 
        if (m_SoundHandleID<csSoundIDMin) 
            m_SoundHandleID=csSoundIDMin; 
        return (void*)m_SoundHandleID; 
    }
    
#ifdef IS_NEED_CODE_MIDI
    inline bool is_MIDIBuf(ISoundBufferBase* buf) const {  return ((long)(buf->getID())<0); }
    void* AsMidiHandleID (void* SoundID) {
        return (void*)(-abs((long)SoundID)); 
    }
#endif
    
    TSoundHandle newPlayer(IRandInputStream* inputStream,const long loopCount,long msTime,bool is_loop);
public:
    TSoundHandle play(IRandInputStream* inputStream);
    TSoundHandle loop_play(IRandInputStream* inputStream,const long loopCount,const long msTime);
    bool stop(const void* sound_handle);
    void stop_all();
    CImportSoundPlayer();
    virtual ~CImportSoundPlayer();
    
    void UpdateBufData();
};

TSoundHandle CImportSoundPlayer::play(IRandInputStream* inputStream)
{
    TSoundHandle result=newPlayer(inputStream,-1,-1,false);
    return result;
}

TSoundHandle CImportSoundPlayer::loop_play(IRandInputStream* inputStream,const long loopCount,const long msTime)
{
    TSoundHandle result=newPlayer(inputStream,loopCount,msTime,true);
    return result;
}

TSoundHandle CImportSoundPlayer::newPlayer(IRandInputStream* inputStream,const long loopCount,long msTime,bool is_loop)
{
    void* NewSoundHandleID=getNewSoundHandleID();
    ISoundBufferBase* newbuf=0;
#ifdef IS_NEED_CODE_MIDI
    if (CMIDIOut::isCanLoadFrom(*inputStream)){
        NewSoundHandleID=AsMidiHandleID(NewSoundHandleID);
        newbuf=m_MIDIOut->getNewSoundBuffer();
        ((ISoundMIDIBuffer*)newbuf)->init(inputStream);
    } 
    else 
#endif	
    {
        ISoundDecoder* SoundDecoder=0;
        // sound type Decoder
        if (COggDecoder::isCanLoadFrom(*inputStream))
            SoundDecoder=new COggDecoder();
        else if (CMp3Decoder::isCanLoadFrom(*inputStream))
            SoundDecoder=new CMp3Decoder();
        else if (CWavDecoder::isCanLoadFrom(*inputStream))
            SoundDecoder=new CWavDecoder();
        
        if (SoundDecoder==0) {
            delete inputStream;
            return 0;
        }
        SoundDecoder->init(inputStream);
        
        //CCriticalSectionAutoLock AutoLock(&m_lock);
        newbuf=m_ISoundOut->getNewSoundBuffer();
        if (newbuf==0){
            if (SoundDecoder!=0) delete SoundDecoder;
            return 0;
        }
        ((ISoundBuffer*)newbuf)->init(SoundDecoder);
    }
    if (newbuf==0) return 0;
    
    VAutoLock autoLock(m_lock);
    
    newbuf->setID(NewSoundHandleID);
    newbuf->play(is_loop,loopCount,msTime);
    m_SoundBufList.push_back(newbuf);
    return newbuf->getID();
}

bool CImportSoundPlayer::stop(const void* sound_handle)
{
    VAutoLock autoLock(m_lock);
    
    for (long i=0;i<SoundBufCount();++i)
    {
        if (m_SoundBufList[i]==0) continue;
        if (m_SoundBufList[i]->getID()==sound_handle)
        {
            ClearASoundBuf(i);
            return true;
        }
    }
    return false;
}

void CImportSoundPlayer::stop_all()
{
    VAutoLock autoLock(m_lock);
    
    for (long i=SoundBufCount()-1;i>=0;--i)
        ClearASoundBuf(i);
}

void CImportSoundPlayer::ClearASoundBuf(long Index)
{
    if (Index>=(long)m_SoundBufList.size()) return;
    if (Index<0) return;
    if (m_SoundBufList[Index]!=0)
    {
#ifdef IS_NEED_CODE_MIDI
        if (is_MIDIBuf(m_SoundBufList[Index]))
            m_MIDIOut->deleteSoundBuffer((ISoundMIDIBuffer*)m_SoundBufList[Index]);
        else
#endif
            m_ISoundOut->deleteSoundBuffer((ISoundBuffer*)m_SoundBufList[Index]);
        m_SoundBufList[Index]=0;
        m_SoundBufList.erase(m_SoundBufList.begin()+Index);
    }
}

#ifdef USES_TIMER_UPDATE_SOUND
static void OnTimer_Proc(TAbstractTimer* sender,const void* callBackData)
{
    ((CImportSoundPlayer*)callBackData)->UpdateBufData();
}
#endif

CImportSoundPlayer::CImportSoundPlayer()
:m_SoundHandleID(csSoundIDMin),m_ISoundOut(0)
#ifdef USES_TIMER_UPDATE_SOUND
,m_Timer(0)
#endif
{
    m_ISoundOut=new CSoundOut();
    m_ISoundOut->init();
#ifdef IS_NEED_CODE_MIDI
    m_MIDIOut=new CMIDIOut(((CSoundOut*)m_ISoundOut)->getSoundOutHandle());
#endif
    
#ifdef USES_TIMER_UPDATE_SOUND
#ifndef WIN_TIMER
    m_Timer=new TMMTimer();
#else
    m_Timer=new TMMTimer(g_sound_wHandle);
#endif
    m_Timer->setInterval(10);
    m_Timer->setOnTimer(OnTimer_Proc,this);
    m_Timer->setEnabled(true);
#endif
}

CImportSoundPlayer::~CImportSoundPlayer()
{
    {
#ifdef USES_TIMER_UPDATE_SOUND
        VAutoLock AutoLock(m_lock);
        if (m_Timer!=0)
            delete m_Timer;
        m_Timer=0;
#endif
    }
    stop_all();
#ifdef IS_NEED_CODE_MIDI
    if (m_MIDIOut!=0)
        delete m_MIDIOut;
#endif
    if (m_ISoundOut!=0)
        delete m_ISoundOut;
}


void CImportSoundPlayer::UpdateBufData()
{
    if (m_lock.lock_count()>0) return;
    VAutoLock AutoLock(m_lock);
    if (m_lock.lock_count()!=1)	 return;
    for (long i=SoundBufCount()-1;i>=0;--i)
    {
        if (m_SoundBufList[i]==0) continue;
        if (m_SoundBufList[i]->is_paly_finished())
            ClearASoundBuf(i);
        else
        {
#ifdef IS_NEED_CODE_MIDI
            if (is_MIDIBuf(m_SoundBufList[i]))
                m_MIDIOut->updateBufData((ISoundMIDIBuffer*)m_SoundBufList[i]);
            else
#endif
                m_ISoundOut->updateBufData((ISoundBuffer*)m_SoundBufList[i]);
        }
    }
}


//////////////////////////////////////////////////////////////


TSoundHandle CSoundPlayer::play(IRandInputStream* inputStream){
    return ((CImportSoundPlayer*)m_Import)->play(inputStream);
}

TSoundHandle CSoundPlayer::play(const char* fileName){
    return play(new FileInputStream(fileName));
}

TSoundHandle CSoundPlayer::loop_play(IRandInputStream* inputStream,const long loopCount,const long msTime){
    return ((CImportSoundPlayer*)m_Import)->loop_play(inputStream,loopCount,msTime);
}

TSoundHandle CSoundPlayer::loop_play(const char* fileName,const long loopCount,const long msTime){
    return loop_play(new FileInputStream(fileName),loopCount,msTime);
}

TSoundHandle CSoundPlayer::loop_play(IRandInputStream* inputStream){
    return loop_play(inputStream,-1,-1);
}

TSoundHandle CSoundPlayer::loop_play(const char* fileName){
    return loop_play(new FileInputStream(fileName),-1,-1);
}

bool CSoundPlayer::stop(const void* sound_handle){
    return ((CImportSoundPlayer*)m_Import)->stop(sound_handle);
}

void CSoundPlayer::stop_all(){
    ((CImportSoundPlayer*)m_Import)->stop_all();
}

CSoundPlayer::CSoundPlayer():m_Import(0){
    m_Import=new CImportSoundPlayer();
}

CSoundPlayer::~CSoundPlayer(){
    if (m_Import!=0){
        delete (CImportSoundPlayer*)m_Import;
        m_Import=0;
    }
}

#endif

//////////////////////////////////////////////////////////////////////////////
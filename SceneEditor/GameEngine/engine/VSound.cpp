//VSound.cpp

#include "VSound.h"
#include "../base/SysImportType.h"

#include "../../import/importInclude/SoundImport.h"

#ifdef _SOUND_PALY_ONE_

class VSoundPlayer{
protected:
    void*   m_soundImport;
    long    m_loopPlayCout;
    bool    m_isPlaying;
    double  m_playBeginTime_s;
    double  m_playTime_s;
    static void* create_sondImport(const char* fileName);
    static void delete_sondImport(void* soundImport);
protected:
    void init(){
        m_soundImport=0;
        m_loopPlayCout=0;
        m_isPlaying=false;
        m_playBeginTime_s=0;
        m_playTime_s=0;
    }
    void doOnPlayOnceFinish();
public:
    explicit VSoundPlayer(){ init(); }
    explicit VSoundPlayer(const char* fileName){ init(); open(fileName); }
    void open(const char* fileName){
        delete_sondImport(m_soundImport);
        m_soundImport=create_sondImport(fileName);
    }
    void play();
    void playLoopCount(const long loopCount);
    void playLoopTime(double playTime_s);
    void playLoop(const long loopCount,double playTime_s);
    
    bool getIsPlaying()const{
        return m_isPlaying;
    }
    void updateStep();
    virtual ~VSoundPlayer(){ delete_sondImport(m_soundImport); }
};


void* VSoundPlayer::create_sondImport(const char* fileName){
    long strLength=(long)strlen(fileName);
    String path=getFilePath(fileName, strLength);
    return sound_create(getFileNameNoPath(fileName, strLength),path.c_str());
}

void VSoundPlayer::delete_sondImport(void* soundImport){
    if (soundImport!=0)
        sound_delete(soundImport);
}



void VSoundPlayer::play(){
    playLoopCount(1);
}
void VSoundPlayer::playLoopCount(const long loopCount){
    playLoop(loopCount,1<<30);
}

void VSoundPlayer::playLoopTime(double playTime_s){
    playLoop(1<<30,playTime_s);
}

class VSoundPlayerCB:public VSoundPlayer{
public:
    must_inline void doOnPlayOnceFinish(){
        VSoundPlayer::doOnPlayOnceFinish();
    }
};

void _doOnPlayOnceFinish(void* soundHandleImport,void *callBackData){
    VSoundPlayerCB* soundPlayer=(VSoundPlayerCB*)callBackData;
    soundPlayer->doOnPlayOnceFinish();
}

void VSoundPlayer::playLoop(const long loopCount,double playTime_s){
    m_loopPlayCout=loopCount;
    m_playTime_s=playTime_s;
    if ((!m_isPlaying)&&(m_loopPlayCout>0)&&(m_playTime_s>0)){
        m_isPlaying=true;
        m_playBeginTime_s=getMMTimerCount()*0.001;
        sound_play(m_soundImport,_doOnPlayOnceFinish,this);//play once
    }
}

void VSoundPlayer::updateStep(){
    if (!m_isPlaying) return;
    
    if (m_soundImport==0){
        m_isPlaying=false;
    }else if (m_loopPlayCout<=0){
        m_isPlaying=false;
    }else{
        double nowTime_s=getMMTimerCount()*0.001;
        if (nowTime_s>=m_playBeginTime_s+m_playTime_s){
            delete_sondImport(m_soundImport);
            m_soundImport=0;
            m_isPlaying=false;
        }
    }
}
void VSoundPlayer::doOnPlayOnceFinish(){
    --m_loopPlayCout;
    updateStep();
    if (m_isPlaying)
        sound_play(m_soundImport,_doOnPlayOnceFinish,this);
}


/////////////////////////////////////////////

TSoundID getNewSoundID(){
    const unsigned long csMinSoundID=1000;
    static unsigned long sSoundID=csMinSoundID;
    ++sSoundID;
    if (sSoundID<csMinSoundID)
        sSoundID=csMinSoundID;
    return (TSoundID)sSoundID;
}

class VSoundPlayerEx:public VSoundPlayer{
public:
    TSoundID soundID;
    void init(){
        VSoundPlayer::init();
    }
    explicit VSoundPlayerEx(){ init(); }
    explicit VSoundPlayerEx(const char* fileName){ init(); open(fileName); }
};

typedef Vector<VSoundPlayerEx*> TSoundList;
void* VSoundManager::createSoundList(){
    return new TSoundList();
}
void VSoundManager::deleteSoundList(void* soundList){
    if (soundList!=0){
        delete (TSoundList*)soundList;
    }
}

void VSoundManager::updateStep(){
    TSoundList& soundList=*(TSoundList*)m_soundList;
    for (long i=0;i<(long)soundList.size();++i){
        if (soundList[i]!=0){
            soundList[i]->updateStep();
        }
    }	
}

VSoundManager::VSoundManager(){ 
    init(); 
    m_soundList=createSoundList(); 
    VGame::registUpdateStep(this);
}

VSoundManager::~VSoundManager(){
    VGame::unRegistUpdateStep(this);
    stopAllSound(); 
    if (m_loopThread!=0){
        m_loopThread->fastRunAllWork();
        VThread::tryExitThread((VThread *&)m_loopThread);
    }
    deleteSoundList(m_soundList);
}

void VSoundManager::stopAllSound(){
    TSoundList& soundList=*(TSoundList*)m_soundList;
    for (long i=0;i<(long)soundList.size();++i){
        if (soundList[i]!=0){
            delete soundList[i];
            soundList[i]=0;
        }
    }
    soundList.clear();
    //stop_soundBackgroundMusic();
}

TSoundID VSoundManager::play(const char* fileName){
    return playLoopCount(fileName, 1);
}


TSoundID VSoundManager::playLoop(const char* fileName,long loopCount,double playTime_s){
    deleteSoundPlayerByStop();
    if(!m_isCanPlay) return 0;
    
    TSoundList& soundList=*(TSoundList*)m_soundList;
    
    VSoundPlayerEx* result=new VSoundPlayerEx(fileName);
    soundList.push_back(result);
    result->playLoop(loopCount,playTime_s);
    return result->soundID;
}

TSoundID VSoundManager::playLoopCount(const char* fileName,long loopCount){
    return playLoop(fileName,loopCount,1<<30);
}


TSoundID VSoundManager::playLoopTime(const char* fileName,double playTime_s){
    return playLoop(fileName,1<<30,playTime_s);
}

void VSoundManager::stopSound(TSoundID soundID){
    long index=findSoundPlayer(soundID);
    if (index>=0)
        deleteSoundPlayerByIndex(index);
}


long VSoundManager::findSoundPlayer(TSoundID soundID){
    TSoundList& soundList=*(TSoundList*)m_soundList;
    for (long i=0;i<(long)soundList.size();++i){
        if ((soundList[i]!=0)&&(soundList[i]->soundID==soundID))
            return i;
    }
    return -1;
}

void VSoundManager::deleteSoundPlayerByIndex(long index){
    TSoundList& soundList=*(TSoundList*)m_soundList;
    if ((index<0)||(index>=(long)soundList.size())) return;
    if (soundList[index]!=0){
        delete soundList[index];
        soundList[index]=0;
    }
}

void VSoundManager::deleteSoundPlayerByStop(){
    TSoundList& soundList=*(TSoundList*)m_soundList;
    long insettIndex=0;
    for (long i=0;i<(long)soundList.size();++i){
        if ((soundList[i]==0)||(!soundList[i]->getIsPlaying())){
            if (soundList[i]!=0) delete soundList[i];
        }else{
            soundList[insettIndex]=soundList[i];
            ++insettIndex;
        }
    }
    soundList.resize(insettIndex);
}

void VSoundManager::phoneVibrate(){
    phone_vibrate();
}

#else 

#include "../sound/SoundPlayer.h"

extern const void* getMainFormHandle();


//////////////////////////////////

class TWorkStopSound:public ILoopWork{
private:
    void*		m_soundList;
    HashMap<long,TSoundID>& m_soundIDMap;
    TSoundID	m_soundMapID;
public:
    TWorkStopSound(void* soundList,HashMap<long,TSoundID>& soundIDMap,TSoundID	soundMapID)
    :m_soundList(soundList),m_soundIDMap(soundIDMap),m_soundMapID(soundMapID){}
    virtual void doWork(){
        if (m_soundMapID==0)
            ((CSoundPlayer*)m_soundList)->stop_all();
        else{
            ((CSoundPlayer*)m_soundList)->stop(m_soundIDMap[(long)m_soundMapID]);
            m_soundIDMap.erase((long)m_soundMapID);
        }
    }
};
class TWorkPlaySound:public ILoopWork{
private:
    void*		m_soundList;
    HashMap<long,TSoundID>& m_soundIDMap;
    TSoundID	m_soundMapID;
    String		m_fileName;
    long		m_loopCount;
    long		m_playTime;
public:
    TWorkPlaySound(void* soundList,HashMap<long,TSoundID>& soundIDMap,TSoundID	soundMapID,const char* fileName,long loopCount,long	playTime)
    :m_soundList(soundList),m_soundIDMap(soundIDMap),m_soundMapID(soundMapID),m_fileName(fileName),m_loopCount(loopCount),m_playTime(playTime){}
    virtual void doWork(){
        TSoundID id=0;
        if ((m_loopCount==1)&&(m_playTime<0))
            id=((CSoundPlayer*)m_soundList)->play(m_fileName.c_str());
        else
            id=((CSoundPlayer*)m_soundList)->loop_play(m_fileName.c_str(),m_loopCount,m_playTime);
        m_soundIDMap[(long)m_soundMapID]=id;
    }
};

static void* getNewMapSoundID () {  
    const long csSoundIDMin=50000;
    static long m_SoundHandleID=csSoundIDMin;
    ++m_SoundHandleID; 
    if (m_SoundHandleID<csSoundIDMin) 
        m_SoundHandleID=csSoundIDMin; 
    return (void*)m_SoundHandleID;
}


////

#define IS_SOUND_USE_LoopThread


#ifdef IS_SOUND_USE_LoopThread
VLoopThread g_loopThread;
bool g_loopThread_isRuning=false;
#endif

void VSoundManager::init(){
    m_isCanPlay=true;
    m_soundList=0;
    m_loopThread=0;
#ifdef IS_SOUND_USE_LoopThread
    if (!g_loopThread_isRuning){
        g_loopThread.start();
        g_loopThread_isRuning=true;
    }
    m_loopThread=&g_loopThread;
#endif
}
void* VSoundManager::createSoundList(){
    CSoundPlayer::setWinHandle((void*)getMainFormHandle());
    CSoundPlayer* result=new CSoundPlayer();
    return result;
}
void VSoundManager::deleteSoundList(void* soundList){
    if (soundList!=0){
        delete (CSoundPlayer*)soundList;
    }
}

void VSoundManager::updateStep(){
}

VSoundManager::VSoundManager(){
    init();
    m_soundList=createSoundList(); 
}

VSoundManager::~VSoundManager(){
    stopAllSound(); 
#ifdef IS_SOUND_USE_LoopThread
    while (!m_loopThread->workListEmpty()){
        VThread::sleep_s(0.001);
    }
#endif
    deleteSoundList(m_soundList);
}


void VSoundManager::stopAllSound(){
    
#ifdef IS_SOUND_USE_LoopThread
    m_loopThread->addWork(new TWorkStopSound(m_soundList,m_soundIDMap,0));
#else
    ((CSoundPlayer*)m_soundList)->stop_all();
#endif
}

TSoundID VSoundManager::play(const char* fileName){
    if(!m_isCanPlay) return 0;
#ifdef IS_SOUND_USE_LoopThread
    TSoundID rtMapID=getNewMapSoundID();
    m_soundIDMap[(long)rtMapID]=0;
    m_loopThread->addWork(new TWorkPlaySound(m_soundList,m_soundIDMap,rtMapID,fileName,1,-1));
    return rtMapID;
#else
    return ((CSoundPlayer*)m_soundList)->play(fileName);
#endif
}


TSoundID VSoundManager::playLoop(const char* fileName,long loopCount,double playTime_s){
    if(!m_isCanPlay) return 0;
#ifdef IS_SOUND_USE_LoopThread
    TSoundID rtMapID=getNewMapSoundID();
    m_soundIDMap[(long)rtMapID]=0;
    m_loopThread->addWork(new TWorkPlaySound(m_soundList,m_soundIDMap,rtMapID,fileName,loopCount,(long)(playTime_s*1000)));
    return rtMapID;
#else
    return ((CSoundPlayer*)m_soundList)->loop_play(fileName,loopCount,(long)(playTime_s*1000));
#endif
}

TSoundID VSoundManager::playLoopCount(const char* fileName,long loopCount){
    if(!m_isCanPlay) return 0;
    
#ifdef IS_SOUND_USE_LoopThread
    TSoundID rtMapID=getNewMapSoundID();
    m_soundIDMap[(long)rtMapID]=0;
    m_loopThread->addWork(new TWorkPlaySound(m_soundList,m_soundIDMap,rtMapID,fileName,loopCount,-1));
    return rtMapID;
#else
    return ((CSoundPlayer*)m_soundList)->loop_play(fileName,loopCount,-1);
#endif
}


TSoundID VSoundManager::playLoopTime(const char* fileName,double playTime_s){
    if(!m_isCanPlay) return 0;
#ifdef IS_SOUND_USE_LoopThread
    TSoundID rtMapID=getNewMapSoundID();
    m_soundIDMap[(long)rtMapID]=0;
    m_loopThread->addWork(new TWorkPlaySound(m_soundList,m_soundIDMap,rtMapID,fileName,-1,(long)(playTime_s*1000)));
    return rtMapID;
#else
    return ((CSoundPlayer*)m_soundList)->loop_play(fileName,-1,(long)(playTime_s*1000));
#endif
}

void VSoundManager::stopSound(TSoundID soundID){
#ifdef IS_SOUND_USE_LoopThread
    m_loopThread->addWork(new TWorkStopSound(m_soundList,m_soundIDMap,soundID));
#else
    ((CSoundPlayer*)m_soundList)->stop(soundID);
#endif
}

long VSoundManager::findSoundPlayer(TSoundID soundID){
    //not uses
    return -1;
}

void VSoundManager::deleteSoundPlayerByIndex(long index){
    //not uses
}

void VSoundManager::deleteSoundPlayerByStop(){
    //not uses
}

void VSoundManager::phoneVibrate(){
    phone_vibrate();
}


#endif
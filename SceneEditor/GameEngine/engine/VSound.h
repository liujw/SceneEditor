//
//  VSound.h
//
//  Created by housisong on 08-4-10.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VSound_h_
#define _VSound_h_

//请使用VSoundManager来管理声音播放
#include "VGame.h"
#include "VThread.h"

typedef void* TSoundID;
class VSoundManager:ICanUpdateStep{
private:
    bool			m_isCanPlay;
    void*			m_soundList;
    VLoopThread*	m_loopThread;
    HashMap<long,TSoundID> m_soundIDMap;
    void init();
    static void* createSoundList();
    static void deleteSoundList(void* soundList);
    virtual void updateStep();
    long findSoundPlayer(TSoundID soundID);
    void deleteSoundPlayerByIndex(long index);
    void deleteSoundPlayerByStop(); 
    TSoundID playLoop(const char* fileName,long loopCount,double playTime_s);
public:
    VSoundManager();
    virtual ~VSoundManager();
    
    bool getIsCanPlay(){
        return m_isCanPlay;
    }
    void setIsCanPlay(bool isCanPlay){
        if (isCanPlay!=m_isCanPlay){
            m_isCanPlay=isCanPlay;
            if (!m_isCanPlay) 
                stopAllSound();
        }
    }
    
    void stopAllSound();
    TSoundID play(const char* fileName);
    TSoundID playLoopCount(const char* fileName,long loopCount);
    TSoundID playLoopTime(const char* fileName,double playTime_s);
    void stopSound(TSoundID soundID);
    
    static void phoneVibrate();//让手机震动
};



#endif //_VSound_h_

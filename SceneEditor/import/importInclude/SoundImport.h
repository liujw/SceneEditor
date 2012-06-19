//
//  SoundImport.h
//
//  Created by housisong on 08-4-10.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _SoundImport_h_
#define _SoundImport_h_


#ifdef __cplusplus 
extern "C" {
#endif
    
    void* sound_create(const char* soundFileName,const char* soundFilePath);
    
    
    typedef void (*TPlayFinishCallBackProc)(void* soundHandleImport,void *callBackData);
    void  sound_play(void* soundHandleImport,TPlayFinishCallBackProc playFinishCallBackProc,void *callBackData);
    void  sound_delete(void* soundHandleImport);
    
    void  phone_vibrate();  //手机震动
    
    
#ifdef WINCE
    void  setWavOutActive(bool isActive);
    float getWavOutVolumeValue();
    float getWavOutVolume();
    void  setWavOutVolume(float volume);
    void  addWavOutVolumeStep();
    void  subWavOutVolumeStep();
#endif
    
#ifdef __cplusplus 
}
#endif




#endif //_SoundImport_h_
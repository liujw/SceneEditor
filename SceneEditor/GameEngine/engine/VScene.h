//
//  VScene.h
//
//  Created by housisong on 08-3-21.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VScene_h_
#define _VScene_h_

#include "VSprite.h"
#include "GameRes.h"

class VScene:public VSprite{
public:
    inline explicit VScene(){}
    virtual ~VScene(){ 
        m_spriteList.clear(); 
        m_GameRes.clear();
    }
    inline void loadRes(const char* fileName){
        m_GameRes.loadFromXml(fileName);
    }
    inline void doLoadAllRes(){
        m_GameRes.doLoadAllRes();
    }
    inline void clearRes(){
        m_GameRes.clear();
    }
    
    inline bool getIsExistClipCanvas(const String& strSurfaceName){
        return m_GameRes.getIsExistClipCanvas(strSurfaceName);
    }
    inline const VClipCanvas getClipCanvas(const String& strSurfaceName){
        return m_GameRes.getClipCanvas(strSurfaceName);
    }
    inline const VClipCanvasGray8 getClipCanvasGray8(const String& strSurfaceName){
        return m_GameRes.getClipCanvasGray8(strSurfaceName);
    }
    inline const VCanvas getCanvas(const String& strSurfaceName){
        return m_GameRes.getClipCanvas(strSurfaceName).getCanvas();
    }
    inline const VCanvasGray8 getCanvasGray8(const String& strSurfaceName){
        return m_GameRes.getClipCanvasGray8(strSurfaceName).getCanvas();
    }
private:
    GameResMgr m_GameRes;
};



#endif //_VScene_h_
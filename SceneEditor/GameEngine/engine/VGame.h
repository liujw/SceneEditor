//
//  VGame.h
//
//  Created by housisong on 08-3-21.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VGame_h_
#define _VGame_h_

#include "VDrawPipe.h"
#include "VSpriteEngine.h"
#include "INetIO.h"

class ICanUpdateStep{
public:
    virtual void updateStep()=0;
    virtual ~ICanUpdateStep(){}
};


class VGame{
private:
    double m_curStepTimeSum;
    double m_curStepTimeCount;
    VSprite* m_fps;
protected:
    bool   m_isShowFPS;
    INetIOFactory* m_netIOFactory;
    VSpriteEngine  m_spriteEngine;
public:
    virtual void update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight);
    virtual void draw(VDrawPipe& dst,long x0,long y0);
    virtual void pushNotification(const char* csMessage){};
public:
    explicit VGame(INetIOFactory* netIOFactory);
    virtual ~VGame();
    VSpriteEngine& getSpriteEngine(){
        return m_spriteEngine;
    }
    
    IInputIOListener* getInputIOListener() {
        return &m_spriteEngine;
    }
    
    static void registUpdateStep(ICanUpdateStep* canUpdater);
    static void unRegistUpdateStep(ICanUpdateStep* canUpdater);
    static void outDebugInfo(const String& text,bool isClearOld);
};



#endif //_VGame_h_
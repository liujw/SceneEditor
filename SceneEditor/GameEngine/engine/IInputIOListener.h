//
//  IInputIOListener.h
//
//  Created by housisong on 08-4-2.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _IInputIOListener_h_
#define _IInputIOListener_h_
#include "../../import/importInclude/SysImport.h"

////////////


enum TInputIOType{INPUT_NULL=0, INPUT_MOUSE_MOVED, INPUT_MOUSE_DOWN, INPUT_MOUSE_UP,INPUT_KEY_CHAR, INPUT_KEY_DOWN, INPUT_KEY_UP};

struct TKeyBoardState{
    unsigned long isShiftDown :1;
    unsigned long isCtrlDown :1;
    unsigned long isAltDown :1;
    TKeyBoardState(){ memset(this,0,sizeof(TKeyBoardState)); }
    TKeyBoardState(const TKeyBoardState& src){ *this=src; }
    explicit TKeyBoardState(const long src){ *this=*(TKeyBoardState*)&src; }
};

struct InputIORecord{
    
    TInputIOType  inputType;
    long   x;
    long   y;
    int  aChar;
    TKeyBoardState keyBoardState;
    TDateTime recordTime;
    inline InputIORecord():inputType(INPUT_NULL){}
    inline InputIORecord(TInputIOType inputType_,long x_,long y_,int aChar_,const TKeyBoardState& keyBoardState_){
        inputType=inputType_;
        x=x_;
        y=y_;
        aChar=aChar_;
        keyBoardState=keyBoardState_;
        recordTime=getNowTime_s();
    }
};

struct IInputIOListener{
public:
    virtual void disposeInputIOEvent(const InputIORecord& inputIORecord)=0;
    virtual ~IInputIOListener(){}
};

#endif //_IInputIOListener_h_
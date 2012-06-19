//
//  VAction.h
//
//  Created by housisong on 08-5-27.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VAction_h_
#define _VAction_h_

#include <math.h>
#include "../base/SysImportType.h"

class IEnableAlpha{
public:
    virtual void setAlpha(long alpha) {};
    virtual ~IEnableAlpha() {}
};


class VAction{
private:
    bool m_isRunBegin;
    bool m_isRunEnd;
    bool m_isSubRunEnd;
    bool m_isAllRunEnd;
    VAction* m_lastAction;
    Vector<VAction*> m_actionList;
    
    bool selfUpdate(double stepTime_s,double& out_usedTime_s);
    void subRun();
    void subFastRun();
    void subUpdate(double stepTime_s);
    void subClear();
protected:
    void init();
    virtual void doRun(){ } //开始执行   
    virtual void doFastRunEnd(bool isRunBegin){ } //如果没有执行则立即执行并返回，如果动作已经在执行则立即执行完毕返回
    virtual bool doUpdate(double stepTime_s,double& out_usedTime_s){ out_usedTime_s=stepTime_s; return true; } //更新状态  返回的时间true表示Action结束,将自动触发finish事件;
public:
    VAction(){ init(); }
    virtual ~VAction();
    bool getIsRunBegin()const { return m_isRunBegin; }
    bool getIsRunEnd()const { return m_isRunEnd; }
    bool getIsAllRunEnd();
    
    void fastRunEnd();
    void run();
    
    void update(double stepTime_s);	
    ///
    
    void addASubAction(VAction* action);
    void addASubActionAsLastAction(VAction* last_action);
};

//Action控制器
class VActionCtrl{
private:
    VAction* m_delayDelAction;
    VAction* m_activeAction;
    void clearCurAction(bool isFastRunEnd=true);
    void clearDelAction();
public:
    VActionCtrl():m_activeAction(0),m_delayDelAction(0){}
    ~VActionCtrl(){ clearCurAction(); clearDelAction(); }
    
    void setNewActionAndRunIt(VAction* aAction);
    void fastRunEnd();
    void updateAction(double stepTime_s);
};

//睡眠Action
class VSleepAction:public VAction{
    double m_lastSleepTime_s;
protected:
    virtual void doFastRunEnd(bool isRunBegin){ m_lastSleepTime_s=0; } 
    virtual bool doUpdate(double stepTime_s,double& out_usedTime_s);
public:
    VSleepAction(double sleepTime_s):m_lastSleepTime_s(sleepTime_s){}
};

//类成员函数的回调Action
template<class T,class TActionEventProc>
class VFactorAction:public VAction{
public:
private:
    T*					m_listener;
    TActionEventProc	m_actionEventProc;
    void*				m_callBackData;
    bool                m_isInvoked;
protected:
    void try_invoke(){
        if (!m_isInvoked){
            m_isInvoked=true;
            (m_listener->*m_actionEventProc)(m_callBackData);
        }
    }
    virtual void doRun(){ try_invoke(); }    
    virtual void doFastRunEnd(bool isRunBegin){ try_invoke(); } 
    virtual bool doUpdate(double stepTime_s,double& out_usedTime_s){
        out_usedTime_s=0;
        return m_isInvoked;
    }
    
public:	
    VFactorAction(T* listener,TActionEventProc aActionEventProc,void* callBackData=0)
    :m_listener(listener), m_actionEventProc(aActionEventProc) ,m_callBackData(callBackData),m_isInvoked(false) {}
};

template<class T,class TActionEventProc>
VAction* new_FactorAction(T* listener,TActionEventProc aActionEventProc,void* callBackData=0){
    return new VFactorAction<T,TActionEventProc>(listener,aActionEventProc,callBackData);
}

class IEnableMove{
public:
    virtual void setLeft(long left) = 0;
    virtual void setTop(long top) = 0;
    virtual ~IEnableMove(){}
};


//移动Action
class VMoveAction: public VAction{
protected:
    IEnableMove* m_mover;
    long       m_x0;
    long       m_x1;
    long       m_y0;
    long       m_y1;
    double       m_allTime;
    double       m_curTime;
    void init(IEnableMove* mover, double dMoveTime_s,const long srcX0, const long srcY0, const long destX0,  const long destY0);
    long getCurValue(const long srcValue,const long dstValue)const;
public:		
    //move
    VMoveAction(IEnableMove* mover, double dMoveTime_s, const long srcX0, const long srcY0, const long destX0,  const long destY0): m_mover(0)	 { 
        init(mover,dMoveTime_s,srcX0,srcY0,destX0,destY0); 
    }		
    virtual ~VMoveAction() {}
protected:
    virtual void doFastRunEnd(bool isRunBegin) { 
        m_curTime=m_allTime;
        doMove();
        m_mover=0;
    } 
    virtual bool doUpdate(double stepTime_s,double& out_usedTime_s);
    
    virtual void doMove();
    
};

class IEnableZoom{
public:
    virtual void setMoveX0(double x0) = 0;
    virtual void setMoveY0(double y0) = 0;
    virtual void setZoomX(double zoomX) = 0;
    virtual void setZoomY(double zoomY) = 0;
    virtual ~IEnableZoom(){}
};

//缩放移动Action
class VZoomMoveAction: public VAction{
protected:
    IEnableZoom* m_zoomer;
    double       m_x0;
    double       m_x1;
    double       m_y0;
    double       m_y1;
    double       m_zoomX0;
    double       m_zoomX1;
    double       m_zoomY0;
    double       m_zoomY1;
    double       m_allTime;
    double       m_curTime;
    double       m_isUseMove;
    double       m_isUseZoom;
    void init(IEnableZoom* zoomer, double dZoomTime_s, double dSrcZoomScale, double dDestZoomScale,
              double srcX0, double srcY0, double destX0,  double destY0,bool isUseZoom,bool isUseMove);
    double getCurValue(double srcValue,double dstValue)const;
public:
    //zoom
    VZoomMoveAction(IEnableZoom* zoomer, double dZoomTime_s, double dSrcZoomScale, double dDestZoomScale): m_zoomer(0)	 { 
        init(zoomer,dZoomTime_s,dSrcZoomScale,dDestZoomScale,0,0,0,0,true,false); 
    }		
    //move
    VZoomMoveAction(IEnableZoom* zoomer, double dZoomTime_s, double srcX0, double srcY0, double destX0,  double destY0): m_zoomer(0)	 { 
        init(zoomer,dZoomTime_s,1,1,srcX0,srcY0,destX0,destY0,false,true); 
    }		
    
    //zoom and move
    VZoomMoveAction(IEnableZoom* zoomer, double dZoomTime_s, double dSrcZoomScale, double dDestZoomScale, 
                    double srcX0, double srcY0, double destX0,  double destY0): m_zoomer(0)	 { 
        init(zoomer,dZoomTime_s,dSrcZoomScale,dDestZoomScale,srcX0,srcY0,destX0,destY0,true,true); 
    }		
    virtual ~VZoomMoveAction() {}
protected:
    virtual void doFastRunEnd(bool isRunBegin) { 
        m_curTime=m_allTime;
        doMoveZoom();
        m_zoomer=0;
    } 
    virtual bool doUpdate(double stepTime_s,double& out_usedTime_s);
    virtual void doMoveZoom();
};

//两点间晃动
class VShakeAction: public VAction{
public:
    explicit VShakeAction(IEnableMove* pShaker, double dShakeTime, double dCycleTime, const long& x0, const long& y0, const long& x1, const long& y1);
protected:
    virtual void doFastRunEnd(bool isRunBegin){ 
        m_PassTime = m_ShakeTime; 
        m_pShaker = 0;
    } 
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
    void doShaking();
    double lineMap(double t, double l0, double lt);
private:
    long m_X0;
    long m_Y0;
    long m_X1;
    long m_Y1;
    double m_dTempCycle;
    double m_CenterX;
    double m_CenterY;
    double m_CycleTime;
    double m_ShakeTime;
    double m_PassTime;
    IEnableMove* m_pShaker;
};

class VAlphaAction: public VAction
{
public:
    explicit VAlphaAction(IEnableAlpha* pAlphaer, const unsigned char& srcAlpha, const unsigned char& destAlpha, double dTotalTime, const bool& bUpdateAllChild = false);
protected:
    virtual void doFastRunEnd(bool isRunBegin){
        m_PassTime = m_TotalTime;
        doChangeAlpha();
        m_pAlphaer = 0;
    }
    
    unsigned char getAlpha();
    
    void doChangeAlpha();
    
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    double m_PerChange;
    double m_TotalTime;
    double m_PassTime;
    unsigned char  m_SrcAlpha;
    unsigned char  m_DestAlpha;
    bool m_UpdateAllChild;
    IEnableAlpha* m_pAlphaer;
};

class VAlphaMoveAction: public VMoveAction
{
public:
    explicit VAlphaMoveAction(IEnableMove* mover, IEnableAlpha* pAlphaer, double dTotalTime, long srcX0, long srcY0, long destX0,  long destY0, long srcAlpha, long destAlpha);
protected:
    virtual void doFastRunEnd(bool isRunBegin);
    virtual bool doUpdate(double stepTime_s,double& out_usedTime_s);
    virtual void doMove();
    long getAlpha();
protected:
    IEnableAlpha* m_pAlphaer;
    long m_SrcAlpha;
    long m_DestAlpha;
    double m_PerChangeAlpha;
};

class IEnableFlare
{
public:
    virtual void setIsVisible(bool isVisible) = 0;
    virtual bool getIsVisible() const = 0; 
    virtual ~IEnableFlare() {}
};

class VFlareAction: public VAction
{
public:
    explicit VFlareAction(IEnableFlare* pFlarer, double dTotalTime, double dPerShowTime, double dPerHideTime);
protected:
    virtual void doFastRunEnd(bool isRunBegin){
        m_PassTime = m_TotalTime;
        m_pFlarer = 0;
    }
    
    void doFlare(double stepTime_s);	
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    bool m_Visible;
    double m_PerShowPassTime;
    double m_PerHidePassTime;
    double m_TotalTime;
    double m_PassTime;
    double m_PerShowTime;
    double m_PerHideTime;
    IEnableFlare* m_pFlarer;
};

class VFlareSwapAction: public VAction
{
public:
    explicit VFlareSwapAction(IEnableFlare* pFlarer1, IEnableFlare* pFlarer2, double dTotalTime, double dShowTime1, double dShowTime2);
protected:
    virtual void doFastRunEnd(bool isRunBegin){
        m_PassTime = m_TotalTime;
        m_pFlarer1 = 0;
        m_pFlarer2 = 0;
    }
    
    void doFlare(double stepTime_s);	
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    double m_ShowPassTime1;
    double m_ShowPassTime2;
    double m_TotalTime;
    double m_PassTime;
    double m_ShowTime1;
    double m_ShowTime2;
    IEnableFlare* m_pFlarer1;
    IEnableFlare* m_pFlarer2;
};

class IEnableValueChange
{
public:
    virtual void setNumValue(const Int64 value) = 0;
    virtual ~IEnableValueChange() {}
};

class VNumChangeAction: public VAction
{
public:
    explicit VNumChangeAction(IEnableValueChange* pChanger, const Int64& srcVal, const Int64& destVal, double dTotalTime);
protected:
    virtual void doFastRunEnd(bool isRunBegin){
        m_PassTime = m_TotalTime;
        doChangeVal();
        m_pChanger = 0;
    }
    Int64 getCurValue();
    void doChangeVal();
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    double m_PerChange;
    double m_TotalTime;
    double m_PassTime;
    Int64  m_SrcVal;
    Int64  m_DestVal;
    IEnableValueChange* m_pChanger;
};
//移动的进度条接口
class IProgressSprite
{
public:
    virtual ~IProgressSprite() {}
    virtual void setProgress(double progress) = 0;
    virtual void setMaxProgress(double maxProgress) = 0;
};
//移动的进度条，进度条数据发生改变时，做进度移动的动画
class VProgressChangeAction: public VAction
{
public:
    explicit VProgressChangeAction(IProgressSprite* pProgressSprite, Int64 srcVal, Int64 destVal, Int64 maxVal, double dTotalTime);
protected:
    virtual void doFastRunEnd(bool isRunBegin){
        m_PassTime = m_TotalTime;
        doChangeVal();
        m_pProgressSprite = 0;
    }
    Int64 getCurValue();
    void doChangeVal();
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    double m_PerChange;
    double m_TotalTime;
    double m_PassTime;
    Int64  m_SrcVal;
    Int64  m_DestVal;
    IProgressSprite* m_pProgressSprite;
};

class IEnableRotation{
public:
    virtual ~IEnableRotation() {}
    virtual void setRotaryAngle(double angle) = 0;
};

class VRotationAction: public VAction
{
public:
    explicit VRotationAction(IEnableRotation* pDelegate, double srcAngle, double destAngle, double totalTime);
protected:
    virtual void doFastRunEnd(bool isRunBegin){
        m_ElaplsedTime = m_TotalTime;
        doRotation(m_ElaplsedTime);
        m_pDelegate = 0;
    }
    void doRotation(double elapsedTime);
    double getCurAngle(double elpasedTime);
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    double m_SrcAngle;
    double m_DestAngle;
    double m_PerChange;
    double m_TotalTime;
    double m_ElaplsedTime;
    IEnableRotation* m_pDelegate;
};

//计算掉落反弹动画
class VBacklashAction: public VAction
{
public:
    explicit VBacklashAction(IEnableMove* pMover, long nSrcY, long nHeight, double dFirstDropTime);
protected:
    void init();
    
    void doBacklash();
    
    virtual void doFastRunEnd(bool isRunBegin){
        m_PassTime = m_TotalTime;
        if (m_pMover) m_pMover->setTop(m_SrcY);
        m_pMover = 0;
    }
    
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    double m_TotalTime;
    double m_PassTime;
    double m_SumTime;
    Vector<double> m_TimeList;
    double m_A;				//下落加速度
    double m_DestSpeed;		//到达地面速度
    double m_Attenuation;	//衰减系数
    long m_SrcY;			//下落初位置
    long m_Height;			//下落高度
    IEnableMove* m_pMover;	
};

enum EMoveStateY {staticY = 0 ,upY, downY, overTopY, overBottomY};
enum EMoveStateX {staticX = 0 ,leftX, rightX, overLeftX, overRightX};
enum ESlowActionType {Quadratic = 0, Cubic, Quartic, Quintic, Sinusoidal, Exponential, Circular};
//Quadratic：二次方的缓动
//Cubic：三次方的缓动
//Quartic：四次方的缓动
//Quintic：五次方的缓动
//Sinusoidal：正弦曲线的缓动
//Exponential：指数曲线的缓动
//Circular：圆形曲线的缓动
class VTweenSlowAction : public VAction{
public:
    explicit VTweenSlowAction(IEnableMove* pMover, EMoveStateY moveStateY, long objCurPosY, double moveTimeY, long moveDistanceY){
        init(pMover, moveStateY, staticX, objCurPosY, moveTimeY, moveDistanceY, Exponential);
    }
    explicit VTweenSlowAction(IEnableMove* pMover, EMoveStateX moveStateX, long objCurPosX, double moveTimeX, long moveDistanceX){
        init(pMover, staticY, moveStateX, objCurPosX, moveTimeX, moveDistanceX, Exponential);
    }
    explicit VTweenSlowAction(IEnableMove* pMover, EMoveStateY moveStateY, long objCurPosY, double moveTimeY, long moveDistanceY, ESlowActionType slowActionType){
        init(pMover, moveStateY, staticX, objCurPosY, moveTimeY, moveDistanceY, slowActionType);
    }
    explicit VTweenSlowAction(IEnableMove* pMover, EMoveStateX moveStateX, long objCurPosX, double moveTimeX, long moveDistanceX, ESlowActionType slowActionType){
        init(pMover, staticY, moveStateX, objCurPosX, moveTimeX, moveDistanceX, slowActionType);
    }
private:
    void init(IEnableMove* pMover, EMoveStateY moveStateY, EMoveStateX moveStateX, long objCurPos, double moveTime, long moveDistance, ESlowActionType slowActionType);
    void doMove(double passTime);
    long slowActionFunction(double passTime, ESlowActionType slowActionType);
protected:
    virtual bool doUpdate(double stepTime_s, double& out_usedTime_s);
private:
    IEnableMove* m_pMover;
    EMoveStateY  m_MoveStateY;
    EMoveStateX  m_MoveStateX;
    
    long   m_BeginPos;//开始位置
    long   m_MoveDistance;//移动距离
    double m_PassTime;//移动走过的时间
    double m_MoveTime;//移动总时间
    ESlowActionType m_SlowActionType;
};
#endif //_VAction_h_
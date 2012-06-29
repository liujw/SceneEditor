//mmTimer.h
#ifndef _mmTimer_h_
#define _mmTimer_h_


//TAbstractTimer = class; //计时器基类
//TMMTimer=class;         //windows下的多媒体计时器, 可以精确到1毫秒

class TAbstractTimer;
typedef void (*TOnTimerProc)(TAbstractTimer* sender,const void* callBackData);

class TAbstractTimer
{
private:
protected:
    long			m_interval;
    TOnTimerProc	m_onTimerProc;
    const void*     m_callBackData;
    bool			m_enabled;
    //protected:
    virtual void updateTimer();
    virtual void setTimer();
    virtual void killTimer();
public:
    virtual void timer();
public:
    explicit TAbstractTimer();
    virtual ~TAbstractTimer();
    void setEnabled(bool enable);
    bool getEnabled() const { return m_enabled; }
    void setInterval(long interval) ;
    long getInterval() const { return m_interval; }
    void setOnTimer(const TOnTimerProc onTimerProc,const void* callBackData=0);
};


//#define WIN_TIMER

unsigned long getMMTimerCount();

class  TMMTimer :public TAbstractTimer
{
private:
    void*			m_MMTimerID;
    long			m_resolution;
#ifdef WIN_TIMER
    void*			m_winHandle;
#endif
protected:
    //void Timer();
    virtual void killTimer();
    virtual void setTimer();
public:
#ifndef WIN_TIMER
    explicit TMMTimer();
#else
    TMMTimer(void* winHandle);
    void setWinHandle(void* winHandle){ m_winHandle=winHandle; }
#endif
    virtual ~TMMTimer();
    long getResolution() const { return m_resolution; }
    void setResolution(long resolution);
};

#endif //#define _mmTimer_h_

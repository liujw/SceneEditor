//mmTimer.cpp
#include "mmTimer.h"
#include "Windows.h"

#ifndef WIN_TIMER
	#include "mmsystem.h"
	#ifndef WINCE
		#pragma comment (lib, "winmm.lib")
	#else
		#pragma comment (lib, "mmTimer.lib")
	#endif
#endif

unsigned long getMMTimerCount(){
#ifndef WIN_TIMER
	return timeGetTime();
#else
	return ::GetTickCount();
#endif
}

void TAbstractTimer::updateTimer(){
	killTimer();   
	if ( (m_interval>0) && (m_enabled) ) 
		setTimer();			
}

void TAbstractTimer::timer(){
	if (m_onTimerProc!=0) 
		m_onTimerProc(this,m_callBackData);		
}

void TAbstractTimer::setTimer(){
}
void TAbstractTimer::killTimer(){
}


TAbstractTimer::TAbstractTimer()
:m_interval(0),m_onTimerProc(),m_callBackData(0),m_enabled(false){
}

TAbstractTimer::~TAbstractTimer(){ 
	m_onTimerProc=0; 
    m_callBackData=0;
	m_enabled=false;  
	killTimer(); 
}

void TAbstractTimer::setEnabled(bool enabled){ 
	if (m_enabled!=enabled){ 
		m_enabled=enabled; 
		updateTimer(); 
	}  
}

void TAbstractTimer::setInterval(long interval) {
	if (m_interval!=interval) { 
		m_interval= interval; 
		updateTimer(); 
	} 
}

void TAbstractTimer::setOnTimer(const TOnTimerProc onTimerProc,const void* callBackData) 
{
	m_onTimerProc=onTimerProc; 
    m_callBackData=callBackData;
}

struct TMMTimerIDData{
	UINT mmTimerID;
	TMMTimerIDData(UINT _mmTimerID):mmTimerID(_mmTimerID){}
};

void _stdcall MMTimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	if (dwUser!=0)
		((TAbstractTimer*)dwUser)->timer();
}

void TMMTimer::setTimer() 
{
	if (m_MMTimerID!=0)
		killTimer();
	//assert(m_MMTimerID==0);

#ifndef WIN_TIMER
	UINT mmID= timeSetEvent(m_interval, m_resolution,&MMTimerProc,(DWORD_PTR)this,
		  TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
#else
	UINT mmID=::SetTimer((HWND)m_winHandle,1,m_interval,0);
#endif
	//assert(mmID!=0);
	m_MMTimerID=new TMMTimerIDData(mmID);
}

void TMMTimer::killTimer()
{
    if (m_MMTimerID!=0 ){
		UINT mmID=((TMMTimerIDData*)m_MMTimerID)->mmTimerID;
		delete (TMMTimerIDData*)m_MMTimerID;
        m_MMTimerID=0;

#ifndef WIN_TIMER
        MMRESULT MTResult=timeKillEvent(mmID);
#else
		::KillTimer((HWND)m_winHandle,1);
#endif
	    //assert(TIMERR_NOERROR==MTResult);
        //if (TIMERR_NOERROR<>MTResult)
        //raise ETimerKillTimerError.Create(csKillTimerError);
    }
}

#ifndef WIN_TIMER
	TMMTimer::TMMTimer()
	:m_MMTimerID(0),m_resolution(5){
	} 
#else
	TMMTimer::TMMTimer(void* winHandle)
	:m_MMTimerID(0),m_resolution(5),m_winHandle(winHandle){
	} 
#endif


TMMTimer::~TMMTimer(){
	killTimer();
}

void TMMTimer::setResolution(long resolution) { 
    if (resolution<=0) resolution=1;
	if (m_resolution!=resolution){ 
		m_resolution=resolution;
		this->updateTimer();
	} 
}
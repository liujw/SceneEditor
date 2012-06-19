//ThreadImport.cpp

#include "../importInclude/ThreadImport.h"
#ifndef WINCE
	#include <process.h>
#endif
#include <Windows.h>
#include <assert.h>

void curThread_exit(){
#ifndef WINCE
	_endthread();
#else
	ExitThread(0);
#endif
}

void curThread_sleep_s(double time_s){
	Sleep((DWORD)(time_s*1000+0.5));
}


struct TWorkThread
{
public:
    volatile HANDLE					thread_handle;
    volatile TThreadCallBackProc    callBack;
    void *							userData;   
	TWorkThread():thread_handle(0),callBack(0),userData(0) { }
};

void __cdecl thread_dowork(TWorkThread* thread_data) //void __stdcall thread_dowork(TWorkThread* thread_data)
{
	if (thread_data!=0){
		if (thread_data->callBack!=0)
			thread_data->callBack(thread_data,thread_data->userData);
		delete thread_data;
	}
}

TThreadImportHandle thread_create(TThreadCallBackProc callBack,void* userData){
	TWorkThread* result=new TWorkThread();
	result->callBack=callBack;
	result->userData=userData;
	result->thread_handle=0;
	return result;
}

void thread_start(TThreadImportHandle threadImportHandle){
	TWorkThread* thread_data=(TWorkThread*)threadImportHandle;
	if (thread_data!=0){
		assert(thread_data->thread_handle==0);
		#ifndef WINCE
			thread_data->thread_handle=(HANDLE)_beginthread((void (__cdecl *)(void *))thread_dowork, 0, (void*)thread_data);
		#else
			thread_data->thread_handle=CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_dowork,(void*)thread_data, 0, 0);
		#endif
	}
}

void thread_delete(TThreadImportHandle threadImportHandle){
	TWorkThread* thread_data=(TWorkThread*)threadImportHandle;
	if (thread_data!=0){
		if (thread_data->thread_handle==0)
			delete thread_data;
		else
			;//no thing
	}
}


long getThreadProcessorsCount(){
    SYSTEM_INFO SystemInfo; 
	::GetSystemInfo(&SystemInfo);
    return SystemInfo.dwNumberOfProcessors;
}

////////////////////////////////////////

	
TLockImportHandle lock_create(){
	RTL_CRITICAL_SECTION* result=new RTL_CRITICAL_SECTION();
	InitializeCriticalSection((LPCRITICAL_SECTION)result);
	return result;
}

void lock_delete(TLockImportHandle lockImportHandle){
	if (lockImportHandle==0) return;
	DeleteCriticalSection((LPCRITICAL_SECTION)lockImportHandle);
	delete [] (RTL_CRITICAL_SECTION*)lockImportHandle;
}

void lock_to_lock(TLockImportHandle lockImportHandle){
	if (lockImportHandle==0) return;
	EnterCriticalSection((LPCRITICAL_SECTION)lockImportHandle);
}

void lock_to_unlock(TLockImportHandle lockImportHandle){
	if (lockImportHandle==0) return;
	LeaveCriticalSection((LPCRITICAL_SECTION)lockImportHandle);
}

//ThreadImport.h

#ifndef _ThreadImport_h_
#define _ThreadImport_h_


#ifdef __cplusplus 
extern "C" {
#endif
    
    //线程
    typedef void* TThreadImportHandle;	
    void curThread_exit();
    void curThread_sleep_s(double time_s);
    typedef void (*TThreadCallBackProc)(TThreadImportHandle sender,void* userData);
    TThreadImportHandle thread_create(TThreadCallBackProc callBack,void* userData);
    void thread_start(TThreadImportHandle threadImportHandle);
    void thread_delete(TThreadImportHandle threadImportHandle);
    
    //CPU总线程数
    long getThreadProcessorsCount();
    
    //锁
    typedef void* TLockImportHandle;
    TLockImportHandle lock_create();
    void lock_delete(TLockImportHandle lockImportHandle);
    void lock_to_lock(TLockImportHandle lockImportHandle);
    void lock_to_unlock(TLockImportHandle lockImportHandle);
    
#ifdef __cplusplus 
}
#endif
#endif //_ThreadImport_h_
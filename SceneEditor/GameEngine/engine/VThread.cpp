//VThread.cpp

#include "VThread.h"
#include "../../import/importInclude/ThreadImport.h"


struct TThreadData{
    VThread*      m_thread;
    bool          m_isOwnerThread;
    void*         m_import;
    volatile bool m_isWantExit;	
    TThreadData():m_thread(0),m_import(0),m_isWantExit(false),m_isOwnerThread(false){}
    ~TThreadData(){
        if (m_import!=0)
            thread_delete(m_import);
    }
};



static void threadCallBack(TThreadImportHandle sender,void* userData){
    class VThread_:public VThread{
    public: 
        inline void _run(){
            run();
        }
        static inline void _tryDeleteThread(VThread*& thread){
            VThread::tryDeleteThread(thread);
        }
    };
    
    TThreadData* threadData=(TThreadData*)userData;
    VThread_* thread=(VThread_*)threadData->m_thread;
    thread->_run();
    if (threadData->m_isOwnerThread)
        VThread_::_tryDeleteThread(threadData->m_thread); //VThread类都创建在堆上,线程循环结束时自动析构
    delete threadData;
}

VThread::VThread():m_data(0),m_isRunning(false){
    TThreadData* threadData=new TThreadData();	
    threadData->m_thread=this;
    threadData->m_isWantExit=false;
    threadData->m_isOwnerThread=false;
    threadData->m_import=thread_create(threadCallBack,threadData);
    m_data=threadData;
}

VThread::~VThread(){
    TThreadData* threadData=(TThreadData*)m_data;
    m_data=0;
    if (threadData!=0) {
        threadData->m_thread=0;
        threadData->m_isOwnerThread=false;
        if (m_isRunning)
            threadData->m_isWantExit=true;
        else
            delete threadData;
    }
}

void VThread::start(){
    if (m_isRunning) return;
    
    m_isRunning=true;
    TThreadData* threadData=(TThreadData*)m_data;
    threadData->m_isOwnerThread=true;
    thread_start(threadData->m_import);
}


void VThread::exit(){
    curThread_exit();
}

void VThread::sleep_s(double time_s){
    curThread_sleep_s(time_s);
}


bool VThread::getIsWantExit()const{
    TThreadData* threadData=(TThreadData*)m_data;
    if (threadData!=0)
        return threadData->m_isWantExit;
    else 
        return true;
}

void VThread::setIsWantExit(){
    //assert(m_isRunning);
    TThreadData* threadData=(TThreadData*)m_data;
    if (threadData!=0){
        threadData->m_isWantExit=true;
    }
}

long VThread::getThreadProcessorsCount(){
    return ::getThreadProcessorsCount();
}

void VThread::tryExitThread(VThread*& pThread){
    VThread* thread=pThread;
    pThread=0;
    if (thread!=0){
        if (thread->getIsRunning())
            thread->setIsWantExit();
        else 
            delete thread;
    }
}

void VThread::tryDeleteThread(VThread*& pThread){
    VThread* thread=pThread;
    pThread=0;
    if (thread!=0)
        delete thread;
}

//////

VLock::VLock():m_import(0),m_lock_count(0){
    m_import=lock_create();
}
VLock::~VLock(){
    if(m_import!=0){
        lock_delete(m_import);
        m_import=0;
    }
}

void VLock::lock(){
    lock_to_lock(m_import);
    ++m_lock_count;
}
void VLock::unlock(){
    --m_lock_count;
    //assert(m_lock_count>=0);
    lock_to_unlock(m_import);
}

long VLock::lock_count() const{
    return m_lock_count;
}


////////////////////////////////////


bool VLoopThread::isWorking(){
    return (m_working!=0);
}
bool VLoopThread::workListEmpty(){
    VAutoLock _autoLock(m_lock);
    {
        return m_list.empty();
    }
}

bool VLoopThread::doAWork(){	//assert(m_working==0);
    {
        VAutoLock _autoLock(m_lock);
        {
            if (m_list.empty()) return false;
            m_working=m_list.front();
            m_list.pop_front();
        }
    }
    ((ILoopWork*)m_working)->doWork();
    delete (ILoopWork*)m_working;
    m_working=0;
    
    return true;
}

void VLoopThread::run(){
    while (!getIsWantExit()){
        if (!doAWork())
            sleep_s(m_emptySleepTime_s);
        else
            sleep_s(0);
    }
}

void VLoopThread::addWork(ILoopWork* work){
    VAutoLock _autoLock(m_lock);
    {
        m_list.push_back(work);
    }
}

void VLoopThread::fastRunAllWork(){
    VAutoLock _autoLock(m_lock);
    {
        while (isWorking()){
            sleep_s(0.001);
        }
        while (!m_list.empty()){
            m_working=m_list.front();
            m_list.pop_front();
            ((ILoopWork*)m_working)->doWork();
            delete (ILoopWork*)m_working;
            m_working=0;
        }
    }
}

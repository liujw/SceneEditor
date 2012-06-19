//
//  VThread.h
//
//  Created by housisong on 08-4-24.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VThread_h_
#define _VThread_h_

#include "../base/SysImportType.h"  //for Deque

class VThread{
private:
    void* m_data;
    volatile bool  m_isRunning;
protected:
    virtual void run()=0; //子类改写
    virtual ~VThread();  //VThread类都创建在堆上,线程循环结束时自动析构
    static void tryDeleteThread(VThread*& pThread);
public:
    VThread();  //警告: 只允许 new 出来,不允许创建在局部堆栈中 否则编译就会报错
    void start();  //启动线程
    bool getIsRunning()const{ return m_isRunning; }
    
    bool getIsWantExit()const;
    virtual void setIsWantExit();//设置成要求退出线程状态 子类线程循环通过getIsWantExit检测这个标志来退出循环
    static void exit(); //不建议使用 可能会泄露资源 而应该使用线程类的 tryExitThread
    static void sleep_s(double time_s);
    
    static long getThreadProcessorsCount();//返回处理器总线程数
    
    static void tryExitThread(VThread*& pThread);
};

class VLock{
protected:
    void* m_import;
    long m_lock_count;
    template <class T>
    VLock& operator=(const T&); //error
public:
    VLock();
    ~VLock();
    void lock();
    void unlock();
    long lock_count() const;
};	

class VAutoLock{
protected:
    VLock& m_lock;
public:
    inline  explicit VAutoLock(VLock& lock):m_lock(lock) { m_lock.lock(); }
    inline ~VAutoLock(){ m_lock.unlock(); }
};

class ILoopWork{
public:
    virtual void doWork()=0;
    virtual ~ILoopWork(){}
};
typedef Deque<ILoopWork*> TLoopWorkList;

//异步工作线程
class VLoopThread:public VThread{
private:
    double			m_emptySleepTime_s;
    TLoopWorkList	m_list;
    VLock			m_lock;
    volatile void*	m_working;
    bool doAWork();
protected:
    virtual void run();
    virtual bool isWorking();
public:
    explicit VLoopThread(double emptySleepTime_s=0.020):m_emptySleepTime_s(emptySleepTime_s),m_working(0){}
    void addWork(ILoopWork* work);
    void fastRunAllWork();
    bool workListEmpty();
};


#endif //_VThread_h_
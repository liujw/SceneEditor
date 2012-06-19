//
//  IEventListener.h
//
//  Created by housisong on 08-3-25.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _IEventListener_h_
#define _IEventListener_h_

struct IEventListener{
public:
    virtual void event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen){}
    virtual ~IEventListener(){}
};

struct VEventDisposer{
private:
    void*			m_sender;
    IEventListener* m_listener;
    void*			m_callBackData;
public:
    inline VEventDisposer():m_sender(0),m_listener(0),m_callBackData(0){}
    inline void setEventListener(void* sender,IEventListener* listener ,void* callBackData){
        m_sender=sender;
        m_listener=listener;
        m_callBackData=callBackData;
    }
    inline void doEvent(void* pdata,long data0,long data1)  {
        if (m_listener!=0){
            bool isContinueListen=true;
            m_listener->event(m_sender, m_callBackData,pdata,data0,data1,isContinueListen);
            if (!isContinueListen){
                m_listener=0;
                m_callBackData=0;
            }
        }
    }
};

#endif //_IEventListener_h_
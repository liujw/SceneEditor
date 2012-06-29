//
//  VDispatcher.h
//
//  Created by housisong on 08-5-20.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VDispatcher_h_
#define _VDispatcher_h_

#include "../base/SysImportType.h"
#include "VCom.h"

class DispatcherError:public Exception{
public:
    virtual const char* what() const throw() { return "class DispatcherError"; }
};

//警告:消息处理子类必须先继承IMsgDisposer,多继承的其他基类放在后面
class IMsgDisposer{
public:
    virtual ~IMsgDisposer(){}
};
typedef void (*TMsgDisposerProc)(IMsgDisposer* msgDisposer,const StringList& strList);
struct TMsgDisposerData{
    IMsgDisposer*    msgDisposer;
    TMsgDisposerProc msgDisposerProc;
    inline TMsgDisposerData(IMsgDisposer* aMsgDisposer,TMsgDisposerProc aMsgDisposerProc):msgDisposer(aMsgDisposer),msgDisposerProc(aMsgDisposerProc){ }
    inline TMsgDisposerData():msgDisposer(0),msgDisposerProc(0){ }
};

class VDispatcher;

class IDispatcheListener{
public:
    virtual void disposeDispatchMsg(VDispatcher* sender,const StringList& msg){}
    virtual ~IDispatcheListener(){}
};

class VDispatcher{
private:
    volatile long		m_isBarrageMsg;
    volatile long		m_isGenericBarrageMsg;
    volatile long		m_isSecondBarrageMsg;
    volatile long		m_isSerialStrikeMsg;
    IDispatcheListener* m_listener;
    
    VCom*				m_com; 
    HashSet<String>	m_strikeIDSet;
    HashSet<String>	m_serialIDSet;
    HashSet<String>	m_secondIDSet;
    Deque<StringList>	m_dataList;
    typedef HashMap<String,TMsgDisposerData> TDisposerMap;
    TDisposerMap	m_disposerMap;
    volatile bool	m_isInExit;
    inline bool getIsCanDisposeMsg(const String& Msg_ID){
        if (m_isBarrageMsg > 0) return false;
        if (m_secondIDSet.count(Msg_ID) > 0) return (m_isSecondBarrageMsg==0);
        if (m_isGenericBarrageMsg == 0) return true;
        return ((m_isSerialStrikeMsg==0)&&(m_strikeIDSet.count(Msg_ID)>0));
    }
    bool readMsg(StringList& datas);
public:
    VDispatcher(VCom* com):m_isBarrageMsg(0),m_isGenericBarrageMsg(0),m_isSecondBarrageMsg(0),
    m_isSerialStrikeMsg(0),m_com(com),m_listener(0),m_isInExit(false){}
    
    //强制阻塞时任何消息都会被保存在队列里
    void barrageMsg(){ ++m_isBarrageMsg; } //强制阻塞   累加阻塞计数，所有消息会被暂时保存起来
    void rushDownMsg() { --m_isBarrageMsg; if (m_isBarrageMsg<0) throw new DispatcherError(); } //释放强制阻塞  强制阻塞计数为0时执行释放阻赛的消息
    
    bool registerDisposer(const String& Msg_ID,IMsgDisposer* msgDisposer,TMsgDisposerProc msgDisposerProc){ m_disposerMap[Msg_ID]=TMsgDisposerData(msgDisposer,msgDisposerProc); return true; }
    
    void strikeMsg(const String& Msg_ID) {  m_strikeIDSet.insert(Msg_ID); } //是否允许穿透； 也就是允许消息在普通阻塞状态下穿过，从而得到提前处理
    //普通阻塞时 穿透消息允许继续处理
    void genericBarrageMsg() { ++m_isGenericBarrageMsg; } //一般阻塞   累加普通阻塞计数，普通消息会被暂时保存起来
    void genericRushDownMsg(){ --m_isGenericBarrageMsg; if (m_isGenericBarrageMsg<0) throw new DispatcherError(); } //释放一般阻塞  阻塞计数为0时执行释放阻赛的消息
    
    void serialMsg(const String& Msg_ID) { m_serialIDSet.insert(Msg_ID); } //需要串行化消息处理 该消息被处理前,后面的消息不允许被提前执行
    
    //第二个消息通道
    void secondMsg(const String& Msg_ID) { m_secondIDSet.insert(Msg_ID); } //设置第二通道消息  //第二通道不支持 Strike消息
    void secondBarrageMsg(){ ++m_isSecondBarrageMsg; } //阻塞   累加阻塞计数，所有消息会被暂时保存起来
    void secondRushDownMsg(){ --m_isSecondBarrageMsg; if (m_isSecondBarrageMsg<0) throw new DispatcherError(); } //释放阻塞  阻塞计数为0时执行释放阻赛的消息
    
    void dispatchMsg();
    
    
    IDispatcheListener* getDispatcheListener()const{
        return m_listener;
    }
    void setDispatcheListener(IDispatcheListener* listener){
        m_listener=listener;
    }
    void setIsInExit(){
        m_isInExit=true;
    }
    bool getIsInExit()const{
        return m_isInExit;
    }
};


#endif //_VDispatcher_h_

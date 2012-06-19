//VDispatcher.cpp

#include "VDispatcher.h"

bool VDispatcher::readMsg(StringList& datas){
    StringList  tmp_datas;
    while (m_com->readData(tmp_datas)) { 
        const String& Msg_ID=tmp_datas[0];
        if (m_serialIDSet.count(Msg_ID)>0) 
            ++m_isSerialStrikeMsg; 		
        m_dataList.push_back(StringList());
        m_dataList.back().swap(tmp_datas);
    }; 
    
    if (m_isBarrageMsg>0) return false; 
    while (m_dataList.size()>0) {
        if (m_dataList[0].size()==0)
            m_dataList.pop_front();
        else
            break;
    }
    
    long size=(long)m_dataList.size();
    long i = 0;
    while (i<size) {
        StringList& Msg =m_dataList[i];
        if (Msg.size()==0) {
            ++i;
        }else{
            const String& Msg_ID=Msg[0];
            if ((i > 0) && (m_serialIDSet.count(Msg_ID) > 0)) return false; 
            if (getIsCanDisposeMsg(Msg_ID)) {
                if (m_serialIDSet.count(Msg_ID)>0)
                    --m_isSerialStrikeMsg; 
                datas.swap(Msg);
                Msg.resize(0); //del
                return true;
            }else
                ++i;
        }
    }
    return false;
}


void VDispatcher::dispatchMsg(){
    StringList datas;
    while ((!m_isInExit)&&readMsg(datas)){
        const String& Msg_ID=datas[0];
        TDisposerMap::iterator it=m_disposerMap.find(Msg_ID);
        if (it!=m_disposerMap.end()){
            IMsgDisposer*    msgDisposer=it->second.msgDisposer;
            TMsgDisposerProc msgDisposerProc=it->second.msgDisposerProc;
            if (m_listener!=0)
                m_listener->disposeDispatchMsg(this,datas);
            msgDisposerProc(msgDisposer,datas);
        }else{
            //assert(false);
        }
    }
}

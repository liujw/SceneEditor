//UndoRedo.cpp

#include "UndoRedo.h"
#include <assert.h>

void  TUndoRedo::delFront(){
    long size=(long)m_historyList.size();
    assert(size>0);
    
    IStateData* stateData=m_historyList.front();
    m_historyList.pop_front();
    
    --m_useHistoryIndex;
    if (m_useHistoryIndex<0)
        m_useHistoryIndex=0;
    
    delete stateData;
}


void TUndoRedo::clear(){
    while (!m_historyList.empty())
        delFront();
    m_useHistoryIndex=0;
}

void TUndoRedo::toFitMaxHistoryCount(){
    while (((long)m_historyList.size()) > m_maxHistoryCount)
        delFront();
}


void TUndoRedo::setMaxHistoryCount(long maxHistoryCount){
    assert(maxHistoryCount>=0);
    m_maxHistoryCount=maxHistoryCount;
    toFitMaxHistoryCount();
}

void TUndoRedo::saveState(IStateDataOwner* src){
    IStateData* stateData=src->getNowStateData();
    
    if ( (m_historyList.empty())||( (!stateData->isEqual(m_historyList.back())) && (!stateData->isEqual(m_historyList[m_useHistoryIndex])) ) ){
        m_historyList.push_back(stateData);
        m_useHistoryIndex=(long)m_historyList.size()-1;
        toFitMaxHistoryCount();
    }else{
        delete stateData;
    }
}

void TUndoRedo::undo(IStateDataOwner* dst){
    if (getIsCanUndo()){
        --m_useHistoryIndex;
        dst->setNowStateData(m_historyList[m_useHistoryIndex]);
    }
}
void TUndoRedo::redo(IStateDataOwner* dst){
    if (getIsCanRedo()){
        ++m_useHistoryIndex;
        dst->setNowStateData(m_historyList[m_useHistoryIndex]);
    }
}


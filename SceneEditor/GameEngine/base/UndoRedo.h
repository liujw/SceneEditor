//UndoRedo.h

#ifndef _UndoRedo_h_
#define _UndoRedo_h_

//TUndoRedo 撤销 重做 的逻辑实现

#include "SysImportType.h"

class TUndoRedo{
public:
    struct IStateData{
        virtual bool isEqual(const IStateData* stateData)=0;
        virtual ~IStateData(){}
    };
    struct IStateDataOwner{
        virtual IStateData* getNowStateData()=0;
        virtual void setNowStateData(const IStateData* stateData)=0;
        virtual ~IStateDataOwner(){}
    };
protected:
    Deque<IStateData*> m_historyList;
    long		m_maxHistoryCount;
    long        m_useHistoryIndex;
    void toFitMaxHistoryCount();
    void  delFront();
public:
    TUndoRedo(const long maxHistoryCount=20) :m_maxHistoryCount(maxHistoryCount),m_useHistoryIndex(0){}
    virtual ~TUndoRedo() { clear(); }
    
    void clear();
    long  getHistoryCount()const{
        return (long)m_historyList.size();
    }
    
    long getMaxHistoryCount(){
        return m_maxHistoryCount;
    }
    void setMaxHistoryCount(long maxHistoryCount);
    
    void saveState(IStateDataOwner* src);
    void undo(IStateDataOwner* dst);
    void redo(IStateDataOwner* dst);
    bool getIsCanUndo()const{
        return m_useHistoryIndex>0;
    }
    bool getIsCanRedo()const{
        return m_useHistoryIndex<(long)m_historyList.size()-1;
    }
};


struct TAutoUndoRedo:private TUndoRedo::IStateDataOwner{
protected:
    typedef TUndoRedo::IStateData IStateData;
private:
    bool		m_isInUndoRedo;
    TUndoRedo	m_undoRedo;
    
    //IStateDataOwner
    virtual void setNowStateData(const IStateData* stateData){
        m_isInUndoRedo=true;
        doSetStateData(stateData);
        m_isInUndoRedo=false;
    }
    virtual IStateData* getNowStateData(){
        return doCreateNowStateData();
    }
protected:
    virtual IStateData* doCreateNowStateData()=0;
    virtual void doSetStateData(const IStateData* stateData)=0;
public:
    TAutoUndoRedo():m_isInUndoRedo(false){}
    void stateUndo(){
        m_undoRedo.undo(this);
    }
    void stateRedo(){
        m_undoRedo.redo(this);
    }
    void stateChanged(){
        if (!m_isInUndoRedo){
            m_undoRedo.saveState(this);
        }
    }
    bool getIsCanUndo()const{
        return m_undoRedo.getIsCanUndo();
    }
    bool getIsCanRedo()const{
        return m_undoRedo.getIsCanRedo();
    }
    virtual ~TAutoUndoRedo(){}
};


#endif //_UndoRedo_h_

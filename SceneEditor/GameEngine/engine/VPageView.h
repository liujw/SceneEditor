//
//  VPageView.h
//
//  Created by housisong on 08-6-6.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VPageView_h_
#define _VPageView_h_

#include "VSprite.h"
#include "VBmpButtonSprite.h" 

class VCellValue{//一个单元格的值类型(基类)
public:
    VCellValue(){}
    virtual ~VCellValue(){}
};

class VCellSprite:public VSprite{//一个显示单元格(基类)(显示)
protected:
public:
    VCellSprite(){}
    virtual ~VCellSprite(){}
    
    virtual void updateCellView(const VCellValue* cellValue){ 
    }
    
    void setValueToView(const VCellValue* cellValue){
        updateCellView(cellValue);
        changed();
    }
};

class ICellDocLisener{
public:
    virtual void cellDocChanged();
    virtual ~ICellDocLisener(){}
};

class VCellDoc{ //(Cell List)单元格组成的文档
private:
    Vector<VCellValue*>  m_list;
    ICellDocLisener*  m_cellDocLisener;
    void doDocChanged();
public:
    VCellDoc():m_cellDocLisener(0){}
    void setCellDocLisener(ICellDocLisener* cellDocLisener){ m_cellDocLisener=cellDocLisener; }
    virtual ~VCellDoc(){ clear(); }
    void addACellValue(VCellValue* cellValue);
    void deleteCellValueByIndex(long index);
    void clear();
    VCellValue* getCellValue(long index)const;
    long getCount()const{
        return (long)m_list.size();
    }
    long indexOf(VCellValue* cellValue) const;
};

class VPageView:public VSprite,IEventListener,ICellDocLisener{
private:
protected:
    long						m_viewCellBeginIndex;
    long						m_viewCellCount; //能够显示的单元格数量
    Vector<VCellSprite*>	m_viewList;
    VCellDoc					m_doc;
    VBmpButtonSprite*			m_nextBtn;
    VBmpButtonSprite*			m_previousBtn;
    bool						m_isAutoHideBotton;
    bool                        m_docChanged;
    
    void init(){
        m_viewCellBeginIndex=0;
        m_nextBtn=0;
        m_previousBtn=0;
        m_isAutoHideBotton=true;
        m_doc.setCellDocLisener(this);
        m_docChanged = false;
    }
    virtual void doUpdateChange();
protected:	
    virtual VCellSprite* createNewCellSprite(){ //子类改写
        return new VCellSprite();
    }
    virtual void resetCellSpritePos(){ //子类改写  设置单元格的位置
    }
public:
    //ICellDocLisener
    virtual void cellDocChanged();
    //IEventListener
    virtual void event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen);
public:
    explicit  VPageView(long viewCellCount):m_viewCellCount(viewCellCount){ init(); }
    long getViewCellCount()const{ 
        return m_viewCellCount;
    }
    void setViewCellCount(long viewCellCount);
    VCellDoc& getDoc(){
        return m_doc;
    }
    
    long getPageCount() const;
    long getPageIndex() const;
    void setPageIndex(long pageIndex);
    
    void refresh();
    void toFirst();
    void toLast();
    void toNext();
    bool getIsFirstPage()const;
    bool getIsLastPage()const;
    void toPrevious();
    
    void setNextButton(VBmpButtonSprite* nextBtn);
    
    void setPreviousBotton(VBmpButtonSprite* previousBtn);
    
    bool getIsAutoHideBotton()const { //只有一页的时候是否自动隐藏翻页按钮
        return m_isAutoHideBotton;
    }
    void setIsAutoHideBotton(bool isAutoHideBotton);
};


class VListView:public VPageView{
protected:
    long	m_cellHeight;
protected:	
    virtual void resetCellSpritePos();
public:
    explicit  VListView(long listCellCount,long cellHeight):VPageView(listCellCount),m_cellHeight(cellHeight){  }
};


class VGridView:public VPageView{
protected:
    long	m_cellHeight;
    long	m_cellWidth;
    long    m_colCount;
protected:	
    virtual void resetCellSpritePos();
public:
    explicit  VGridView(long rowCount,long colCount,long cellWidth,long cellHeight)
    :VPageView(rowCount*colCount),m_cellHeight(cellHeight),m_cellWidth(cellWidth),m_colCount(colCount){  }
};

#endif //_VPageView_h_
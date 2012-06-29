//VPageView.cpp

#include "VPageView.h"

void ICellDocLisener::cellDocChanged()
{
}

/////////////////////////////////

void VCellDoc::addACellValue(VCellValue* cellValue){
    m_list.push_back(cellValue);
    doDocChanged();
}

void VCellDoc::doDocChanged(){
    if (m_cellDocLisener!=0)
        m_cellDocLisener->cellDocChanged();
}

void VCellDoc::deleteCellValueByIndex(long index){
    if ((index<0)||(index>=(long)m_list.size())) return;
    if (m_list[index]!=0)
        delete m_list[index];
    m_list.erase(m_list.begin()+index);
    doDocChanged();
}

void VCellDoc::clear(){
    long size=(long)m_list.size();
    if (size==0) return;
    
    for (long i=0;i<size;++i){
        if (m_list[i]!=0)
            delete m_list[i];
    }
    m_list.clear();
    doDocChanged();
}

VCellValue* VCellDoc::getCellValue(long index)const{
    if ((index<0)||(index>=(long)m_list.size())) return 0;
    return m_list[index];
}

long VCellDoc::indexOf(VCellValue* cellValue) const{ //失败返回-1
    long size=(long)m_list.size();
    for (long i=0;i<size;++i){
        if (m_list[i]==cellValue)
            return i;
    }
    return -1;
}


///////////////////

void VPageView::setPageIndex(long pageIndex){
    if (pageIndex<0) pageIndex=0;
    const long maxPageIndex=getPageCount()-1;
    if (pageIndex>maxPageIndex) pageIndex=maxPageIndex;
    
    long new_viewCellBeginIndex=m_viewCellCount*pageIndex;
    if (m_viewCellBeginIndex!=new_viewCellBeginIndex){
        m_viewCellBeginIndex=new_viewCellBeginIndex;
        changed();
    }
}

void VPageView::doUpdateChange(){
    if(!m_docChanged) return;
    
    if (m_viewCellCount<=0) m_viewCellCount=1;
    if (m_viewCellBeginIndex<0) m_viewCellBeginIndex=0;
    if (m_viewCellBeginIndex>m_doc.getCount()) m_viewCellBeginIndex=m_doc.getCount();
    
    if ((long)m_viewList.size()>m_viewCellCount){
        long size=(long)m_viewList.size();
        for (long i=m_viewCellCount;i<size;++i)
            m_viewList[i]->kill();
        m_viewList.resize(m_viewCellCount);
    }else if ((long)m_viewList.size()<m_viewCellCount){
        long addCount=m_viewCellCount-(long)m_viewList.size();
        for (long i=0;i<addCount;++i){
            VCellSprite* cellSprite=createNewCellSprite();
            this->addASprite(cellSprite);
            m_viewList.push_back(cellSprite);
        }
    }
    
    for (long i=0;i<m_viewCellCount;++i){
        long docIndex=m_viewCellBeginIndex+i;
        m_viewList[i]->setValueToView(m_doc.getCellValue(docIndex));
    }
    
    resetCellSpritePos();
    
    
    if (m_nextBtn!=0)
        m_nextBtn->setIsEnableSelf(!getIsLastPage());
    if (m_previousBtn!=0)
        m_previousBtn->setIsEnableSelf(!getIsFirstPage());
    
    bool isVisible=true;
    if (m_isAutoHideBotton) {
        isVisible=getPageCount()>1;
    }
    if (m_nextBtn!=0)
        m_nextBtn->setIsVisible(isVisible);
    if (m_previousBtn!=0)
        m_previousBtn->setIsVisible(isVisible);
    
    m_docChanged = false;
}

//IEventListener
void VPageView::event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen){
    if (sender==m_nextBtn)
        toNext();
    else if (sender==m_previousBtn)
        toPrevious();
    isContinueListen=true;
}	

void VPageView::setViewCellCount(long viewCellCount){
    if (viewCellCount!=m_viewCellCount){
        m_viewCellCount=viewCellCount;
        changed();
    }
}

void VPageView::cellDocChanged()
{
    changed();
    m_docChanged = true;
}	

long VPageView::getPageCount() const {
    return (m_doc.getCount()+m_viewCellCount-1)/m_viewCellCount;
} 
long VPageView::getPageIndex() const {
    return m_viewCellBeginIndex/m_viewCellCount;
}
void VPageView::refresh(){
    changed();
    updateChange();
}
void VPageView::toFirst(){
    setPageIndex(0);
}
void VPageView::toLast(){
    setPageIndex(getPageCount()-1);
}
void VPageView::toNext(){
    setPageIndex(getPageIndex()+1);
}
bool VPageView::getIsFirstPage()const{
    return m_viewCellBeginIndex<=0;
}
bool VPageView::getIsLastPage()const{
    return (m_viewCellBeginIndex+m_viewCellCount)>=m_doc.getCount();
}
void VPageView::toPrevious(){
    setPageIndex(getPageIndex()-1);
}

void VPageView::setNextButton(VBmpButtonSprite* nextBtn){
    m_nextBtn=nextBtn;
    m_nextBtn->setIsVisible(false);
    this->addASprite(nextBtn);
    nextBtn->setClickEventListener(this, 0);
    changed();
}

void VPageView::setPreviousBotton(VBmpButtonSprite* previousBtn){
    m_previousBtn=previousBtn;
    m_previousBtn->setIsVisible(false);
    this->addASprite(previousBtn);
    previousBtn->setClickEventListener(this, 0);
    changed();
}

void VPageView::setIsAutoHideBotton(bool isAutoHideBotton) {
    if (m_isAutoHideBotton!=isAutoHideBotton){
        m_isAutoHideBotton=isAutoHideBotton;
        changed();
    }
}


////////////

void VListView::resetCellSpritePos(){ 
    long size=(long)m_viewList.size();
    long y=0;
    for (long i=0;i<size;++i){
        m_viewList[i]->setTop(y);
        y+=m_cellHeight;
    }
}

//////////

void VGridView::resetCellSpritePos(){ 
    if (m_colCount<=0) m_colCount=1;
    long size=(long)m_viewList.size();
    for (long i=0;i<size;++i){
        long yp=i/m_colCount;
        m_viewList[i]->setTop(yp*m_cellHeight);
        long xp=i-yp*m_colCount;
        m_viewList[i]->setLeft(xp*m_cellWidth);
    }
}
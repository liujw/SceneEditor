/*
 *  VDragPageGridView.cpp
 *  Ddz4.0SDK
 *
 *  Created by wsh on 11-2-16.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "VDragPageGridView.h"
#include "VBitmapSprite.h"

VGridViewPageIdxFlags::VGridViewPageIdxFlags(const VClipCanvas& idxFrames){
    idxFrames.getSubCanvasByClipIndex(0, m_SelectedCanvas);
    idxFrames.getSubCanvasByClipIndex(1, m_NormalCanvas);
    this->setHeight(m_NormalCanvas.getHeight());
    m_ItemOffset = 0;
    m_SelectedIdx = -1;
}

void VGridViewPageIdxFlags::setPageCount(long count){
    this->getSpriteList().clear();
    for (int i = 0; i < count; ++i){
        VBitmapSprite* pItem = new VBitmapSprite(m_NormalCanvas);
        this->addASprite(pItem);
        pItem->setFitSize();
        pItem->setLeft(i * (m_NormalCanvas.getWidth() + m_ItemOffset));
    }
    this->setWidth((m_NormalCanvas.getWidth() + m_ItemOffset) * count - m_ItemOffset);
}

void VGridViewPageIdxFlags::setAlpha(long alpha){
    long itemCount = this->getSpriteList().getSpriteCount();
    for (long i = 0; i < itemCount; ++i){
        VSprite* pItem = this->getSubSprite(i);
        pItem->setAlpha(alpha);
    }
}

void VGridViewPageIdxFlags::setSelectedPageIdx(long selectedIdx){
    if (selectedIdx != m_SelectedIdx){
        long itemsCount = this->getSpriteList().getSpriteCount();
        if (m_SelectedIdx >= 0 && m_SelectedIdx < itemsCount){
            VBitmapSprite* pItem = (VBitmapSprite*)this->getSubSprite(m_SelectedIdx);
            pItem->setBitmap(m_NormalCanvas);
        }
        if (selectedIdx >= 0 && selectedIdx < itemsCount){
            VBitmapSprite* pItem = (VBitmapSprite*)this->getSubSprite(selectedIdx);
            pItem->setBitmap(m_SelectedCanvas);
        }		
        m_SelectedIdx = selectedIdx;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VDragPageGridView::VDragPageGridView(long perPageRowCount, long perPageColCount, 
                                     long cellWidth, long cellHeight): m_PerPageRowCount(perPageRowCount), 
m_PerPageColCount(perPageColCount), m_CellWidth(cellWidth), m_CellHeight(cellHeight)
{
    m_PageCount = 0;
    m_pPageIdxFlags = 0;
    m_DecelerateTime = 0.4;
    m_CountDownSqrTime = 1.0 / (m_DecelerateTime * m_DecelerateTime);
    m_SrcDeceleratePos = 0.0;
    m_DestDeceleratePos = 0.0;
    m_CurSpeed = 0.0;
    m_ElpasedDecelerateTime = 0.0;
    m_CurAcceleration = 0.0;
    m_IsDecelerating = false;	
    m_StartingMouseDownTime = 0;
    m_MayBeClicking = false;
    m_MaxClickingItemOffset = mapGamePos(6);
    m_MouseDownFlagItemIdx = -1;
    m_pLastMouseDownFlagItem = 0;
    m_pMouseDownFlagItem = 0;
    m_pDelegate = 0;
    m_SrcMousePos = 0;
    m_SrcContainerPos = 0;
    m_MinLeft = 0;
    m_MaxLeft = 0;
    m_SelectedPageIdx = -1;
    m_PerPageItemsCount = m_PerPageColCount * m_PerPageRowCount;
    m_MouseMoving = false;
    this->setWidth(m_PerPageColCount * cellWidth);
    this->setHeight(m_PerPageRowCount * cellHeight);
    this->setIsEnableSelf(true);
    this->setMouseEventListener(this);
    m_pContainer = new VSprite();
    this->addASprite(m_pContainer);
}

void VDragPageGridView::doSpriteEventMouseDown(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo)
{
    m_SrcContainerPos = m_pContainer->getLeft();
    m_IsDecelerating = false;
    m_SrcMousePos = mouseEventInfo.screenX0;
    m_MouseMoving = true;
    m_StartingMouseDownTime = mouseEventInfo.recordTime;
    m_MayBeClicking = true;
    m_pMouseDownFlagItem = 0;
    m_MouseDownFlagItemIdx = getMouseDownItemIdx(mouseEventInfo);
    if (m_MouseDownFlagItemIdx != -1){
        m_pMouseDownFlagItem = (void*)getItem(m_MouseDownFlagItemIdx);
    }
    if (m_pDelegate != 0){
        m_pDelegate->didPageGridViewItemMouseDown(m_pLastMouseDownFlagItem, m_pMouseDownFlagItem);
    }
    if (m_pPageIdxFlags){
        m_pPageIdxFlags->setSelectedPageIdx(getSelectedPageIdx(m_pContainer->getLeft()));
    }		
}

void VDragPageGridView::setPageIdxFlags(VGridViewPageIdxFlags* pFlags){
    if (pFlags != 0 && m_pPageIdxFlags != pFlags){
        m_pPageIdxFlags = pFlags;
        this->addASprite(m_pPageIdxFlags);
    }
}

long VDragPageGridView::getMouseDownItemIdx(const TSpriteMouseEventInfo& mouseEventInfo){
    long rowIdx = mouseEventInfo.clientY0 / m_CellHeight;
    long colIdx = (mouseEventInfo.clientX0 - m_SrcContainerPos) / m_CellWidth;
    long pageIdx = colIdx / m_PerPageColCount;
    colIdx = colIdx % m_PerPageColCount;
    long cellIdx = pageIdx * m_PerPageItemsCount + rowIdx * m_PerPageColCount + colIdx;
    if (cellIdx >= 0 && cellIdx < getItemCount()){
        return cellIdx;
    }	
    return -1;
}

void VDragPageGridView::doSpriteEventMouseMoved(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo)
{
    if (m_MouseMoving){
        long offset = mouseEventInfo.screenX0 - m_SrcMousePos;
        m_pContainer->setLeft(getCurValidPos(offset));
        if (m_MayBeClicking){
            if (MyBase::abs(offset) > m_MaxClickingItemOffset){
                m_MayBeClicking = false;
                if (m_pDelegate != 0){
                    m_pDelegate->didPageGridViewItemMouseUp(m_pLastMouseDownFlagItem, m_pMouseDownFlagItem);
                }				
            }
        }
        if (m_pPageIdxFlags){
            m_pPageIdxFlags->setSelectedPageIdx(getSelectedPageIdx(m_pContainer->getLeft()));
        }			
    }
}

long VDragPageGridView::getCurValidPos(long offset){
    long newPos = m_SrcContainerPos + offset;
    if (newPos < m_MinLeft){
        newPos = m_MinLeft;
    }
    else if (newPos > m_MaxLeft){
        newPos = m_MaxLeft;
    }
    return newPos;
}

void VDragPageGridView::setSelectedItem(long selectedIdx){
    if (selectedIdx >= 0 && selectedIdx < getItemCount()){
        long pageIdx = getPageIdx(selectedIdx);
        if (pageIdx != m_SelectedPageIdx){
            m_SelectedPageIdx = pageIdx;
            m_pContainer->setLeft(-m_SelectedPageIdx * this->getWidth());
            if (m_pPageIdxFlags){
                m_pPageIdxFlags->setSelectedPageIdx(m_SelectedPageIdx);
            }			
        }
    }
}

void VDragPageGridView::doSpriteEventMouseUp(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo)
{
    if (m_MouseMoving){
        bool isClickItem = false;
        long offset = mouseEventInfo.screenX0 - m_SrcMousePos;
        if (m_MayBeClicking && MyBase::abs(offset) <= m_MaxClickingItemOffset){
            if (m_MouseDownFlagItemIdx != -1 && m_MouseDownFlagItemIdx == getMouseDownItemIdx(mouseEventInfo)){
                isClickItem = true;
            }
        }
        m_MayBeClicking = false;
        if (isClickItem){
            long newPos = getCurValidPos(offset);
            m_SelectedPageIdx = getSelectedPageIdx(newPos);
            newPos = -m_SelectedPageIdx * this->getWidth();
            m_pContainer->setLeft(newPos);
            if (m_pPageIdxFlags){
                m_pPageIdxFlags->setSelectedPageIdx(m_SelectedPageIdx);
            }
            if (m_pDelegate != 0){
                m_pDelegate->didPageGridViewItemSelected(m_MouseDownFlagItemIdx, m_pMouseDownFlagItem);
            }
            m_MayBeClicking = false;
        }
        else {
            double incTime = 0.14;
            double elapsedTime = mouseEventInfo.recordTime - m_StartingMouseDownTime;
            double mouseOffset = (double)(mouseEventInfo.screenX0 - m_SrcMousePos);
            double speed = mouseOffset / (elapsedTime + 0.00000001);
            if (elapsedTime > 0.1)
            {
                mouseOffset += speed * incTime;
            }
            long newPos = getCurValidPos((long)mouseOffset);
            m_SelectedPageIdx = getSelectedPageIdx(newPos);
            newPos = -m_SelectedPageIdx * this->getWidth();
            
            m_SrcDeceleratePos = m_pContainer->getLeft();
            m_DestDeceleratePos = newPos;
            if (MyBase::abs(m_DestDeceleratePos - m_SrcDeceleratePos) > (double)m_MaxClickingItemOffset)
            {
                m_IsDecelerating = true;
                m_CurSpeed = speed;
                m_ElpasedDecelerateTime = 0.0;
                m_CurAcceleration = (m_DestDeceleratePos - m_SrcDeceleratePos - speed * m_DecelerateTime) * 2.0 * m_CountDownSqrTime;				
            }
            else {
                m_pContainer->setLeft(newPos);
                if (m_pPageIdxFlags){
                    m_pPageIdxFlags->setSelectedPageIdx(m_SelectedPageIdx);
                }				
                m_IsDecelerating = false;
            }
        }
        if (m_pDelegate != 0){
            m_pDelegate->didPageGridViewItemMouseUp(m_pLastMouseDownFlagItem, m_pMouseDownFlagItem);
        }
        m_pLastMouseDownFlagItem = m_pMouseDownFlagItem;
        m_pMouseDownFlagItem = 0;
        m_MouseMoving = false;
    }
}

void VDragPageGridView::doUpdate(double stepTime_s){
    if (m_IsDecelerating && (!m_MouseMoving))
    {
        m_ElpasedDecelerateTime += stepTime_s;
        if (m_ElpasedDecelerateTime >= m_DecelerateTime)
        {
            m_ElpasedDecelerateTime = 0.0;
            m_pContainer->setLeft((long)m_DestDeceleratePos);
            if (m_pPageIdxFlags){
                m_pPageIdxFlags->setSelectedPageIdx(m_SelectedPageIdx);
            }				
            m_IsDecelerating = false;			
        }
        else {
            double y = m_SrcDeceleratePos + (m_CurSpeed * m_ElpasedDecelerateTime) + 0.5 * (m_CurAcceleration * m_ElpasedDecelerateTime * m_ElpasedDecelerateTime);
            m_pContainer->setLeft((long)y);
            if (m_pPageIdxFlags){
                m_pPageIdxFlags->setSelectedPageIdx(getSelectedPageIdx(m_pContainer->getLeft()));
            }		
        }
    }
}


VSprite* VDragPageGridView::getItem(long idx)
{
    if (idx >= 0 && idx < m_pContainer->getSpriteList().getSpriteCount())
    {
        return m_pContainer->getSubSprite(idx);
    }
    return 0;
}

long VDragPageGridView::getPageCount() const{
    return m_PageCount;
}

long VDragPageGridView::getPageIdx(long itemIdx){
    return (itemIdx / m_PerPageItemsCount);
}

void VDragPageGridView::didAddItemsCompletion()
{
    m_pContainer->setLeft(0);
    long itemsCount = m_pContainer->getSpriteList().getSpriteCount();
    m_PageCount =  itemsCount / m_PerPageItemsCount;
    if ((itemsCount % m_PerPageItemsCount) > 0){
        ++m_PageCount;
    }
    for (long i = 0; i < itemsCount; ++i)
    {
        setItemPos(i);
    }
    
    if (itemsCount <= 0){
        m_SelectedPageIdx = -1;
        m_MinLeft = 0;
        m_MaxLeft = 0;
    }
    else {
        m_SelectedPageIdx = 0;
        long rowIdx, colIdx;
        getRowAndColIdx(itemsCount - 1, rowIdx, colIdx);
        m_MaxLeft = m_CellWidth * (m_PerPageColCount - 1);
        m_MinLeft = -colIdx * m_CellWidth;
    }
    if (m_pPageIdxFlags != 0){
        m_pPageIdxFlags->setPageCount(getPageCount());
        m_pPageIdxFlags->setSelectedPageIdx(m_SelectedPageIdx);
        m_pPageIdxFlags->setLeft((this->getWidth() - m_pPageIdxFlags->getWidth()) / 2);
        m_pPageIdxFlags->setIsVisible(getPageCount() > 1);
        //m_pPageIdxFlags->setTop(this->getHeight());
    }
}

void VDragPageGridView::setAlpha(long alpha){
    long itemsCount = m_pContainer->getSpriteList().getSpriteCount();
    for (long i = 0; i < itemsCount; ++i){
        VSprite* pItem = m_pContainer->getSubSprite(i);
        pItem->setAlpha(alpha);
    }
    if (m_pPageIdxFlags != 0){
        m_pPageIdxFlags->setAlpha(alpha);
    }
}

long VDragPageGridView::getSelectedPageIdx(long newPos){
    long pageCount = getPageCount();
    long selectedPageIdx = 0;
    if (newPos < m_MinLeft){
        newPos = m_MinLeft;
        selectedPageIdx = pageCount - 1;
    }
    else if (newPos > m_MaxLeft){
        newPos = m_MaxLeft;
        selectedPageIdx = 0;
    }
    else {
        selectedPageIdx= -newPos / this->getWidth();
        long tmpIdx = selectedPageIdx;
        long offset = MyBase::abs(selectedPageIdx * this->getWidth() + newPos);
        if (tmpIdx - 1 >= 0){
            long offset1 = MyBase::abs((tmpIdx - 1) * this->getWidth() + newPos);
            if (offset1 < offset){
                selectedPageIdx = tmpIdx - 1;
                offset = offset1;
            }
        }
        
        if (tmpIdx + 1 < pageCount){
            long offset2 = MyBase::abs((tmpIdx + 1) * this->getWidth() + newPos);
            if (offset2 < offset){
                selectedPageIdx = tmpIdx + 1;
            }		
        }		
    }
    if (selectedPageIdx >= pageCount){
        selectedPageIdx = pageCount - 1;
    }
    if (selectedPageIdx < 0){
        selectedPageIdx = 0;
    }
    return selectedPageIdx;
}

void VDragPageGridView::setItemPos(long idx){
    long rowIdx, colIdx;
    getRowAndColIdx(idx, rowIdx, colIdx);
    VSprite* pItem = getItem(idx);
    pItem->setLeft(colIdx * m_CellWidth);
    pItem->setTop(rowIdx * m_CellHeight);
}

void VDragPageGridView::getRowAndColIdx(long itemIdx, long& rowIdx, long& colIdx){
    long pageIdx = getPageIdx(itemIdx);
    long remainIdx = itemIdx - (pageIdx * m_PerPageColCount * m_PerPageRowCount);
    colIdx = (pageIdx * m_PerPageColCount) + (remainIdx % m_PerPageColCount);
    rowIdx = remainIdx / m_PerPageColCount;
}

void VDragPageGridView::addItem(VSprite* pItem)
{
    if (pItem != 0)
    {
        m_pContainer->addASprite(pItem);
    }
}

void VDragPageGridView::clearItems()
{
    m_pContainer->getSpriteList().clear();
    m_pLastMouseDownFlagItem = 0;
    m_pMouseDownFlagItem = 0;
    m_SrcContainerPos = 0;
    m_SrcMousePos = 0;
    m_SrcMousePos = 0;
    m_SrcContainerPos = 0;	
    m_MouseMoving = false;
    m_pContainer->setLeft(0);
    m_pContainer->setTop(0);
    m_SelectedPageIdx = -1;
    m_SrcDeceleratePos = 0.0;
    m_DestDeceleratePos = 0.0;
    m_CurSpeed = 0.0;
    m_ElpasedDecelerateTime = 0.0;
    m_CurAcceleration = 0.0;
    m_IsDecelerating = false;	
    m_StartingMouseDownTime = 0;
    m_MayBeClicking = false;
    didAddItemsCompletion();
}

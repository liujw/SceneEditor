/*
 *  VDragPageGridView.h
 *  Ddz4.0SDK
 *
 *  Created by wsh on 11-2-16.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VDragPageGridView_h_
#define _VDragPageGridView_h_

#include "VSprite.h"

class IDragPageGridViewDelegate{
public:
    virtual ~IDragPageGridViewDelegate() {}
    virtual void didPageGridViewItemMouseDown(void* pLastItem, void* pItem) = 0;
    virtual void didPageGridViewItemMouseUp(void* pLastItem, void* pItem) = 0;
    virtual void didPageGridViewItemSelected(long selectedIdx, void* pItem) = 0;
};

class VGridViewPageIdxFlags: public VSprite{
public:
    explicit VGridViewPageIdxFlags(const VClipCanvas& idxFrames);
    void setPageCount(long count);
    void setSelectedPageIdx(long selectedIdx);
    void setItemOffset(long offset){
        m_ItemOffset = offset;
    }
public:
    virtual void setAlpha(long alpha);
private:
    long m_SelectedIdx;
    VCanvas m_SelectedCanvas;
    VCanvas m_NormalCanvas;
    long m_ItemOffset;
};

class VDragPageGridView : public VSprite, ISpriteMouseEventListener {
public:
    explicit VDragPageGridView(long perPageRowCount, long perPageColCount, 
                               long cellWidth, long cellHeight);
    VSprite* getItem(long idx);
    void setMaxClickingItemOffset(long offset){
        m_MaxClickingItemOffset =  offset;
    }
    void addItem(VSprite* pItem);
    void didAddItemsCompletion();
    void setSelectedItem(long selectedIdx);
    void clearItems();
    long getItemCount() const {
        return m_pContainer->getSpriteList().getSpriteCount();
    }
    long getCellWidth() const{
        return m_CellWidth;
    }
    long getCellHeight() const{
        return m_CellHeight;
    }
    void getRowAndColIdx(long itemIdx, long& rowIdx, long& colIdx);
    long getPageCount() const;
    void setDelegate(IDragPageGridViewDelegate* pDelegate){
        m_pDelegate = pDelegate;
    }
    void setPageIdxFlags(VGridViewPageIdxFlags* pFlags);
public:
    virtual void setAlpha(long alpha);
public:
    virtual void doUpdate(double stepTime_s);
    virtual void doSpriteEventMouseDown(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void doSpriteEventMouseMoved(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void doSpriteEventMouseUp(VSprite* sender,const TSpriteMouseEventInfo& mouseEventInfo);	
protected:
    void setItemPos(long idx);
    long getPageIdx(long itemIdx);
    long getSelectedPageIdx(long newPos);
    long getCurValidPos(long offset);
    long getMouseDownItemIdx(const TSpriteMouseEventInfo& mouseEventInfo);
protected:
    long m_PageCount;
    VGridViewPageIdxFlags* m_pPageIdxFlags;
    double m_DecelerateTime;
    double m_CountDownSqrTime;
    double m_SrcDeceleratePos;
    double m_DestDeceleratePos;
    double m_CurSpeed;
    double m_ElpasedDecelerateTime;
    double m_CurAcceleration;
    bool m_IsDecelerating;
    double m_StartingMouseDownTime;
    bool m_MayBeClicking;
    long m_MaxClickingItemOffset;
    long m_MouseDownFlagItemIdx;
    void* m_pLastMouseDownFlagItem;
    void* m_pMouseDownFlagItem;
    IDragPageGridViewDelegate* m_pDelegate;
    long m_SelectedPageIdx;
    long m_PerPageItemsCount;
    long m_MinLeft;
    long m_MaxLeft;
    long m_PerPageRowCount;
    long m_PerPageColCount;
    long m_CellWidth;
    long m_CellHeight;
    long m_SrcMousePos;
    long m_SrcContainerPos;
    bool m_MouseMoving;
    VSprite* m_pContainer;
};

#endif

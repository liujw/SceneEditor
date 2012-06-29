/*
 *  VGrid9BitmapSprite.cpp
 *  Dzpk
 *
 *  Created by wsh on 08-11-13.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VGrid9BitmapSprite.h"

VGrid9BitmapSprite::VGrid9BitmapSprite(const VClipCanvas& bmpClip): m_BmpClip(bmpClip)
{
    
}

void VGrid9BitmapSprite::getValidRect(long& nWidth, long& nHeight)
{
    nWidth = this->getWidth();
    nHeight = this->getHeight();
    if (nWidth < (m_BmpClip.getClipWidth() * 2)) nWidth = m_BmpClip.getClipWidth() * 2;
    if (nHeight < (m_BmpClip.getClipHeight() * 2)) nHeight = m_BmpClip.getClipHeight() * 2;	
}

void VGrid9BitmapSprite::doUpdateChange()
{
    m_Pic.clear();	
    long nCellWidth = m_BmpClip.getClipWidth();
    long nCellHeight = m_BmpClip.getClipHeight();
    long nDrawWidth;
    long nDrawHeight;
    getValidRect(nDrawWidth, nDrawHeight);
    
    m_Pic.resizeFast(nDrawWidth, nDrawHeight);
    TRect rectArray[9];
    long srcY = 0;
    for (int nRow = 0; nRow < 3; ++nRow)
    {
        long nTmpHeight = nCellHeight;
        if (nRow == 1) nTmpHeight = nDrawHeight - nCellHeight * 2;
        long srcX = 0;
        for (int nCol = 0; nCol < 3; ++nCol)
        {
            long nTmpWidth = nCellWidth;
            if (nCol == 1) nTmpWidth = nDrawWidth - nCellWidth * 2;
            
            rectArray[nCol + nRow * 3].setRect(srcX, srcY, srcX + nTmpWidth, srcY + nTmpHeight);
            srcX += nTmpWidth;
        }
        srcY += nTmpHeight;
    }		
    
    for (int i = 0; i < 9; ++i)
    {
        if (!rectArray[i].getIsEmpty())
        {
            VCanvas srcCanvas;
            m_BmpClip.getSubCanvasByClipIndex(i, srcCanvas);
            
            VCanvas dstCanvas;
            m_Pic.getCanvasSub(rectArray[i].x0, rectArray[i].y0, rectArray[i].x1, rectArray[i].y1,dstCanvas);
            dstCanvas.zoomCopyFast(srcCanvas);
        }
    }
}

void VGrid9BitmapSprite::doDraw(const VCanvas& dst, long x0, long y0)
{
    if (m_Pic.getCanvas().getIsEmpty()) return;
    dst.blend(x0, y0, m_Pic.getCanvas());
}

void VGrid9BitmapSprite::getWantDrawRect(long x0, long y0, TRect& out_rect)
{
    out_rect.setRect(x0, y0, x0 + m_Pic.getWidth(), y0 + m_Pic.getHeight());
}
bool VGrid9BitmapSprite::getIsHit(long clientX0, long clientY0){
    return (clientX0 >= 0) && (clientY0 >= 0) && (clientX0 < getWidth()) && (clientY0 < getHeight());
}
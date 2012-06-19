/*
 *  VRadioButtonSprite.cpp
 *  Dzpk
 *
 *  Created by wsh on 08-10-15.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VRadioButtonSprite.h"

VRadioButtonSprite::VRadioButtonSprite(const VClipCanvas& btnBmpClip): m_RadioClipCanvas(btnBmpClip), m_pListener(0), m_pCallBackData(0), m_pSelectBmp(0), m_pUnSelectBmp(0), m_IsSelected(false), m_IsGroupBtn(false)
{
    initSprites();
}

void VRadioButtonSprite::setIsSelected(bool bSelected)
{
    if (m_IsSelected != bSelected)
    {
        m_IsSelected = bSelected;
        m_pSelectBmp->setIsVisible(m_IsSelected);
        m_pUnSelectBmp->setIsVisible(!m_IsSelected);
    }
}

void VRadioButtonSprite::setSelectEventListener(IRadioButtonListener* pListener, void* pCallBackData)
{
    m_pListener = pListener;
    m_pCallBackData = pCallBackData;
}

void VRadioButtonSprite::event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen)
{
    if (!m_IsGroupBtn)
    {
        setIsSelected(!m_IsSelected);
        doOnSelectChange();
    }
    else
    {
        if (!m_IsSelected)
        {
            setIsSelected(true);
            doOnSelectChange();	
        }
    }
}

void VRadioButtonSprite::doOnSelectChange()
{
    if (m_pListener) m_pListener->doOnSelectEvent(this, m_pCallBackData, m_IsSelected);
}

void VRadioButtonSprite::initSprites()
{
    assert(m_RadioClipCanvas.getClipCount() == 2);
    if (!m_pSelectBmp)
    {
        m_pSelectBmp = new VBitmapSprite(m_RadioClipCanvas.getSubCanvasByClipIndex(0));
        m_pSelectBmp->updateChange();
        m_pSelectBmp->setIsEnableSelf(true);
        m_pSelectBmp->setClickEventListener(this, 0);
        this->addASprite(m_pSelectBmp);
    }	
    m_pSelectBmp->setIsVisible(false);
    
    if (!m_pUnSelectBmp)
    {
        m_pUnSelectBmp = new VBitmapSprite(m_RadioClipCanvas.getSubCanvasByClipIndex(1));
        m_pUnSelectBmp->updateChange();
        m_pUnSelectBmp->setIsEnableSelf(true);
        m_pUnSelectBmp->setClickEventListener(this, 0);
        this->addASprite(m_pUnSelectBmp);		
    }
    m_pUnSelectBmp->setIsVisible(true);
    this->setWidth(m_pUnSelectBmp->getWidth());
    this->setHeight(m_pUnSelectBmp->getHeight());
}

/////////////////////

VRadioGroupSprite::VRadioGroupSprite(): m_VertSpace(0), m_HorzSpace(0), m_IsHorzType(true), m_SelectedIdx(-1)
{
}

long VRadioGroupSprite::addRadioButton(VRadioButtonSprite* pBtn)
{
    if (findItem(pBtn) != -1) return -1;
    this->addASprite(pBtn);
    pBtn->setIsGroupBtn(true);
    pBtn->setSelectEventListener(this, 0);
    resetPos();
    if (this->getSpriteList().getSpriteCount() == 1) setSelectedIndex(0);
    return (long)(this->getSpriteList().getSpriteCount()) - 1;
}

long VRadioGroupSprite::findItem(VRadioButtonSprite* pItem)
{
    for (int i = 0; i < this->getSpriteList().getSpriteCount(); ++i)
    {
        if (this->getSubSprite(i) == pItem) return i;
    }
    return -1;
}

void VRadioGroupSprite::resetPos()
{
    long nCount = this->getSpriteList().getSpriteCount();
    if (m_IsHorzType)
    {
        long x0 = 0;
        for (int i = 0; i < nCount; ++i)
        {
            this->getSubSprite(i)->setLeft(x0);	
            this->getSubSprite(i)->setTop(0);
            x0 += (this->getSubSprite(i)->getWidth() + m_HorzSpace);
        }
    }
    else
    {
        long y0 = 0;
        for (int i = 0; i < nCount; ++i)
        {
            this->getSubSprite(i)->setLeft(0);	
            this->getSubSprite(i)->setTop(y0);
            y0 += (this->getSubSprite(i)->getHeight() + m_VertSpace);
        }		
    }
}

void VRadioGroupSprite::setSelectedIndex(long nIdx)
{
    if ((nIdx < 0) || (nIdx >= (long)(this->getSpriteList().getSpriteCount()))) return;
    if (nIdx != m_SelectedIdx)
    {
        m_SelectedIdx = nIdx;
        for (int i = 0; i < this->getSpriteList().getSpriteCount(); ++i)
        {
            VRadioButtonSprite* pBtn = (VRadioButtonSprite*)this->getSubSprite(i);
            pBtn->setIsSelected(i == m_SelectedIdx);
        }
    }
}

void VRadioGroupSprite::doOnSelectEvent(void* sender, void* pCallBackData, bool bSelected)
{
    for (int i = 0; i < this->getSpriteList().getSpriteCount(); ++i)
    {
        if (this->getSubSprite(i) == sender)
        {
            m_SelectedIdx = i;
        }
        else
        {
            VRadioButtonSprite* pBtn = (VRadioButtonSprite*)this->getSubSprite(i);
            pBtn->setIsSelected(false);
        }
    }
}

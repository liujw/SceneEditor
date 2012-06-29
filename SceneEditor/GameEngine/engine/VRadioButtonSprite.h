/*
 *  VRadioButtonSprite.h
 *  Dzpk
 *
 *  Created by wsh on 08-10-15.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VRadioButtonSprite_h_
#define _VRadioButtonSprite_h_

#include "VBitmapSprite.h"

class IRadioButtonListener
{
public: 
    IRadioButtonListener() {}
    virtual ~IRadioButtonListener() {}
public:
    virtual void doOnSelectEvent(void* sender, void* pCallBackData, bool bSelected) = 0;
};

class VRadioButtonSprite: public VSprite, public IEventListener
{
public:
    explicit VRadioButtonSprite(const VClipCanvas& btnBmpClip);
    void setSelectEventListener(IRadioButtonListener* pListener, void* pCallBackData);
    virtual void event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen);
    void setIsSelected(bool bSelected);
    void setIsGroupBtn(bool bGroup) {m_IsGroupBtn = bGroup;}
    bool getIsGroupBtn() const {return m_IsGroupBtn;}
protected:
    void initSprites();
    virtual void doOnSelectChange();
protected:
    bool m_IsGroupBtn;
    bool m_IsSelected;
    VBitmapSprite* m_pSelectBmp;
    VBitmapSprite* m_pUnSelectBmp;
    VClipCanvas m_RadioClipCanvas;
    void* m_pCallBackData;
    IRadioButtonListener* m_pListener;
};

class VRadioGroupSprite: public VSprite, IRadioButtonListener
{
public:
    VRadioGroupSprite();
    virtual void doOnSelectEvent(void* sender, void* pCallBackData, bool bSelected);
public:
    long getSelectedIndex() const {return m_SelectedIdx;}
    void setSelectedIndex(long nIdx);
    long addRadioButton(VRadioButtonSprite* pBtn);
    long getHorzSpace() const {return m_HorzSpace;}
    void setHorzSpace(long nSpace) {
        if (m_HorzSpace != nSpace)
        {
            m_HorzSpace = nSpace;
            resetPos();
        }
    }
    long getVertSpace() const {return m_VertSpace;}
    void setVertSpace(long nSpace){
        if (m_VertSpace != nSpace)
        {
            m_VertSpace = nSpace;
            resetPos();
        }
    }
    void setIsHorzType(bool isHorzType)
    {
        if (m_IsHorzType != isHorzType)
        {
            m_IsHorzType = isHorzType;
            resetPos();
        }
    }
    bool getIsHorzType() const {return m_IsHorzType;}
protected:
    virtual void resetPos();
    long findItem(VRadioButtonSprite* pItem);
protected:
    long m_SelectedIdx;
    bool m_IsHorzType;
    long m_HorzSpace;
    long m_VertSpace;
};

#endif

/*
 *  VDateTimeLabel.h
 *  Ddz
 *
 *  Created by wsh on 08-7-25.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VDateTimeLabel_h_
#define _VDateTimeLabel_h_

#include "VLabelSprite.h"

class VDateTimeLabel: public VLabelSprite
{
public:
    explicit VDateTimeLabel();
    void setFormatStr(const String& str){
        if (m_FormatStr != str){
            m_FormatStr = str;
            changed();
        }
    }
    const String& getFormatStr() const {
        return m_FormatStr;
    }
    virtual void doUpdate(double stepTime_s);
    //"%4y-%2m-%2d %2h:%2n:%2s"，y表示年，m月，d日，h时，n分，s秒，数字表示显示位数，为0则表示显示为数字本身的长度
    virtual String formatDataTime(const String& strFormat, const TDateTime curDateTime);
protected:
    virtual bool getDateTimeSub(const TDateTimeData& curDtRec, const char cFlag, long nCount,String& result);
protected:
    String m_FormatStr;
};

#endif
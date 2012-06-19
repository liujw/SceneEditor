/*
 *  VDateTimeLabel.cpp
 *  Ddz
 *
 *  Created by wsh on 08-7-25.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VDateTimeLabel.h"

VDateTimeLabel::VDateTimeLabel()
{
    m_FormatStr = "%2h:%2n";
}

void VDateTimeLabel::doUpdate(double stepTime_s)
{
    this->setText(formatDataTime(m_FormatStr, getNowTime_s()));
}

bool VDateTimeLabel::getDateTimeSub(const TDateTimeData& curDtRec, const char cFlag, long nCount,String& result)
{
    long nVal = 0;
    switch (cFlag)
    {
        case 'y': 
        case 'Y': 
            nVal = curDtRec.year;
            break;
        case 'm':
        case 'M':
            nVal = curDtRec.month;
            break;
        case 'd':
        case 'D':
            nVal = curDtRec.day;
            break;
        case 'h':
        case 'H':
            nVal = curDtRec.hour;
            break;
        case 'n':
        case 'N':
            nVal = curDtRec.minute;
            break;
        case 's':
        case 'S':
            nVal = (long)curDtRec.second;
            break;
        default:
            return false;
            break;
    }
    String strResult = intToStr(nVal);
    long zCount=nCount-strResult.size();
    if (zCount>0)
        result.insert(result.end(),zCount,'0');
    result+=strResult;
    return true;
}

//example: %4y-%2m-%2d %2h:%2n:%2s
String VDateTimeLabel::formatDataTime(const String& strFormat, const TDateTime curDateTime)
{
    TDateTimeData curDtRec;
    dateTimeToDateTimeData(curDateTime, &curDtRec);
    
    String strResult;
    strResult.reserve(strFormat.size()*2+4);
    const char* csSrc = strFormat.c_str();
    const char* csEnd = csSrc + strFormat.size();
    long nPos = -1;
    while ((nPos = pos(csSrc, '%')) >=0){
        if (2< csEnd-csSrc-nPos){
            if (nPos > 0) strResult.insert(strResult.end(),csSrc, csSrc+nPos);
            csSrc+=nPos+1;
            long nCount = strToInt(csSrc, 1);
            ++csSrc;
            char dtSign = *csSrc;
            if (getDateTimeSub(curDtRec, dtSign, nCount,strResult))
                ++csSrc;
            else
                strResult.insert(strResult.end(),dtSign);
        }else{
            strResult.insert(strResult.end(),csSrc,csEnd);
            csSrc = csEnd;
        }
    }
    if (csEnd-csSrc>0) strResult.insert(strResult.end(),csSrc,csEnd);
    return strResult;
}

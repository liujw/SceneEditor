//
//  VMemoTextDragDrop.h
//  ddzHD
//
//  Created by chendengfeng on 11-9-23.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//
#ifndef _VMemoTextDragDrop_h_
#define _VMemoTextDragDrop_h_

#include "VSprite.h"
#include "VAction.h"
#include "VMemoSprite.h"
//主要针对长文本Memo的拖动及放手惯性，只支持上下移动
class VMemoTextDragDrop : public VSprite, ISpriteMouseEventListener
{
public:
    explicit VMemoTextDragDrop(VMemoSprite* memoSprite, const TRect& moveArea, double A = 1000);
    virtual ~VMemoTextDragDrop(){}
public:
    virtual void doSpriteEventMouseDown(VSprite* sender, const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void doSpriteEventMouseMoved(VSprite* sender, const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void doSpriteEventMouseUp(VSprite* sender, const TSpriteMouseEventInfo& mouseEventInfo);
    virtual void doUpdate(double stepTime_s);
public:
    void setMemoTop(long nTop){
        m_MemoSprite->setTop(nTop);
    }
    void setMemoLeft(long nLeft){
        m_MemoSprite->setLeft(nLeft);
    }
    void setEnableInertial(bool enableInertial){
        if (m_EnableInertial!=enableInertial){
            m_EnableInertial=enableInertial;
            changed();
        }
    }
private:
    void doMoveMemoByLine(double stepTime_s);
private:
    VMemoSprite* m_MemoSprite;
    TSpriteMouseEventInfo m_MouseDownInfo;
    TSpriteMouseEventInfo m_MouseMoveInfo;
    EMoveStateY m_MoveStateY;//memo的移动状态
    
    bool m_Moving;
    bool m_EnableInertial;//是否 允许放手惯性
    
    const TRect& m_MoveArea;//memo可移动的窗口
    long m_MoveAreaHeight;
    
    long m_LineHeight;//为了减少CPU占有率，这里是按行来移动，记录行高
    long m_CurTopLine;
    long m_AllDis;//放手后惯性移动距离
    double m_AllTime;//惯性移动时间
    double m_PassTime;
    double m_Speed;//放手时刻的速度
    double m_A;
};
#endif//_VMemoTextDragDrop_h_
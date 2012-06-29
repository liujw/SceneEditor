//
//  VMemoTextDragDrop.cpp
//  ddzHD
//
//  Created by chendengfeng on 11-9-23.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include "VMemoTextDragDrop.h"

VMemoTextDragDrop::VMemoTextDragDrop(VMemoSprite* memoSprite, const TRect& moveArea, double A):m_MemoSprite(memoSprite), m_MoveArea(moveArea), m_A(A)
{
    m_Moving = false;
    m_EnableInertial = true;
    m_MoveStateY = staticY;
    m_Speed = 0.0;
    m_AllTime = 0.0;
    m_PassTime = 0.0;
    m_AllDis = 0;
    this->setWidth(moveArea.getWidth());
    this->setHeight(moveArea.getHeight());
    this->setTop(moveArea.y0);
    this->setLeft(moveArea.x0);
    
    VSprite* dragedSprite = new VSprite();
    dragedSprite->setWidth(memoSprite->getWidth());
    dragedSprite->setHeight(memoSprite->getAllTextLinePixelsHeight());
    dragedSprite->setTop(0);
    dragedSprite->setLeft(0);
    dragedSprite->setIsEnableSelf(true);
    dragedSprite->setMouseEventListener(this);
    this->addASprite(m_MemoSprite);
    this->addASprite(dragedSprite);
    
    m_MoveAreaHeight = moveArea.getHeight();
    m_LineHeight = memoSprite->getAllTextLinePixelsHeight()/memoSprite->getLineCount();
}

void VMemoTextDragDrop::doSpriteEventMouseDown(VSprite *sender, const TSpriteMouseEventInfo &mouseEventInfo){
    m_Moving = true;
    m_MouseDownInfo = mouseEventInfo;
    m_CurTopLine = m_MemoSprite->getTopLine();
    m_PassTime = 0.0;
    m_AllTime = 0.0;
}

void VMemoTextDragDrop::doSpriteEventMouseMoved(VSprite *sender, const TSpriteMouseEventInfo &mouseEventInfo){
    if (m_Moving){
        m_MemoSprite->setTopLine(m_CurTopLine - (mouseEventInfo.screenY0 - m_MouseDownInfo.screenY0)/m_LineHeight);
        m_MouseMoveInfo = mouseEventInfo;
    } 
}

void VMemoTextDragDrop::doSpriteEventMouseUp(VSprite *sender, const TSpriteMouseEventInfo &mouseEventInfo){
    if (m_Moving){ 
        TDateTime useTime = mouseEventInfo.recordTime - m_MouseDownInfo.recordTime;
        
        if (m_EnableInertial){
            long distance = mouseEventInfo.screenY0 - m_MouseDownInfo.screenY0;
            m_Speed = MyBase::abs(2*distance/(double)useTime);
            
            if (mouseEventInfo.recordTime - m_MouseMoveInfo.recordTime < 0.1 && MyBase::abs(mouseEventInfo.screenY0 - m_MouseMoveInfo.screenY0) > 5){
                m_AllTime = m_Speed/m_A;
                m_AllDis = m_AllTime*(m_Speed - 5*m_AllTime);
                
                if (m_CurTopLine > m_MemoSprite->getTopLine()){//down
                    m_MoveStateY = downY;
                } else if (m_CurTopLine < m_MemoSprite->getTopLine()){//up
                    m_MoveStateY = upY;
                }
            }
            
            m_CurTopLine = m_MemoSprite->getTopLine();
            if (m_CurTopLine > m_MemoSprite->getLineCount() - m_MoveAreaHeight/m_LineHeight){
                m_MoveStateY = overBottomY;
                m_AllTime = m_Speed/m_A;
                m_AllDis = m_MoveAreaHeight - (m_MemoSprite->getLineCount() - m_MemoSprite->getTopLine())*m_LineHeight;
            }
        }
    }
    m_Moving = false;
}

void VMemoTextDragDrop::doMoveMemoByLine(double stepTime_s){
    long passDistance = m_AllDis*sin(stepTime_s/m_AllTime * (MyBase::PI/2));
    
    if (m_MoveStateY == downY || m_MoveStateY == overBottomY){
        m_MemoSprite->setTopLine(m_CurTopLine - (passDistance/m_LineHeight));
    } else if (m_MoveStateY == upY){
        long tempMoveLine = m_CurTopLine + (passDistance/m_LineHeight);
        long tempDisLine = m_MemoSprite->getLineCount() - m_MoveAreaHeight/m_LineHeight;
        if (tempMoveLine > tempDisLine){
            m_MemoSprite->setTopLine(tempDisLine);
        } else {
            m_MemoSprite->setTopLine(tempMoveLine);
        }
    }
}
void VMemoTextDragDrop::doUpdate(double stepTime_s){
    if (!m_Moving && m_PassTime < m_AllTime){
        if ((m_PassTime + stepTime_s) <= m_AllTime){
            m_PassTime += stepTime_s;
        } else {
            m_PassTime = m_AllTime;
        }
        doMoveMemoByLine(m_PassTime);
    }
}
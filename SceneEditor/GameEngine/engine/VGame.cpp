//VGame.cpp

#include "VGame.h"
#include "VLabelSprite.h"
#include "VCom.h"
#include "VMemoSprite.h"

Vector<ICanUpdateStep*> updateStepList;
void VGame::registUpdateStep(ICanUpdateStep* canUpdater){
    updateStepList.push_back(canUpdater);
}
void VGame::unRegistUpdateStep(ICanUpdateStep* canUpdater){
    for (long i=0;i<(long)updateStepList.size();++i){
        if (updateStepList[i]==canUpdater)
            updateStepList[i]=0;
    }
}

static void delNullUpdateStep(){
    long insertIndex=0;
    for (long i=0;i<(long)updateStepList.size();++i){
        if (updateStepList[i]!=0){
            updateStepList[insertIndex]=updateStepList[i];
            ++insertIndex;
        }
    }
    updateStepList.resize(insertIndex);
}

static void doUpdateStep(){
    delNullUpdateStep();
    
    for (long i=0;i<(long)updateStepList.size();++i){
        updateStepList[i]->updateStep();
    }
}

VGame::VGame(INetIOFactory* netIOFactory){
    m_netIOFactory=netIOFactory;
    m_curStepTimeSum=0;
    m_curStepTimeCount=0;
    m_isShowFPS=false;
    
    //m_isShowFPS=true; /////////////////////
    
    VLabelSprite* lb=new VLabelSprite();
    lb->setIsEnableSelf(false);
    lb->setZ(1<<30);
    lb->setLeft(mapGamePos(10));
    lb->setTop(mapGamePos(40));
    lb->getFont().setSize(15);
    lb->setColor(Color32(255,0,255));
    this->getSpriteEngine().addASprite(lb);
    m_fps=lb;
}

VGame::~VGame(){
}

static VMemoSprite* gMemo_DebugInfo=0;

void VGame::update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight){
    doUpdateStep();
    
    //fps
    m_curStepTimeSum+=stepTime_s;
    ++m_curStepTimeCount;
    if (m_curStepTimeSum>=1){
        m_fps->setIsVisible(m_isShowFPS);
        if (m_isShowFPS){
            double fps=m_curStepTimeCount/m_curStepTimeSum;
            String fpsStr;
            ftoa(fps,1,5,fpsStr);
            fpsStr+=" fps";
            ((VLabelSprite*)m_fps)->setText(fpsStr.c_str());
        }
        
        m_curStepTimeSum=0;
        m_curStepTimeCount=0;
    }
    
    m_spriteEngine.update(stepTime_s);
    
    if (gMemo_DebugInfo){
        gMemo_DebugInfo->setWidth(newWidth);
        gMemo_DebugInfo->setHeight(newHeight);
        gMemo_DebugInfo->setViewLineCount(newHeight/gMemo_DebugInfo->getFont().getSize());
        gMemo_DebugInfo->update(stepTime_s);
    }
}


void VGame::draw(VDrawPipe& dst,long x0,long y0){
    m_spriteEngine.draw(dst, x0, y0);
    
    if (gMemo_DebugInfo)
        dst.drawSprite(gMemo_DebugInfo);
}

static void VGame_outDebugInfo(const char* text,bool isClearOld){
    VGame::outDebugInfo(text,isClearOld);
}
void VGame::outDebugInfo(const String& text,bool isClearOld){
    if (gMemo_DebugInfo==0){
        gMemo_DebugInfo=new VMemoSprite(mapGamePos(480),60);
        gMemo_DebugInfo->setLeft(5);
        gMemo_DebugInfo->setTop(5);
        gMemo_DebugInfo->setIsEnableSelf(false);
    }
    if (isClearOld)
        gMemo_DebugInfo->clear();
    gMemo_DebugInfo->addLine(text);
}
//
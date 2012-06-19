//VAction.cpp

#include "VAction.h"


bool VAction::selfUpdate(double stepTime_s,double& out_usedTime_s){
    if ((m_isRunBegin)&&(!m_isRunEnd)){
        bool result=doUpdate(stepTime_s,out_usedTime_s);
        if (result){
            m_isRunEnd=true;
            subRun();
        }
        return result;
    }else{
        out_usedTime_s=0;
        return m_isRunEnd;
    }
}
void VAction::subRun(){
    long size=(long)m_actionList.size();
    for (long i=0;i<size;++i)
        m_actionList[i]->run();
}
void VAction::subFastRun(){
    long size=(long)m_actionList.size();
    for (long i=0;i<size;++i)
        m_actionList[i]->fastRunEnd();
}
void VAction::subUpdate(double stepTime_s){
    if (stepTime_s<=0) return;
    long size=(long)m_actionList.size();
    for (long i=0;i<size;++i)
        m_actionList[i]->update(stepTime_s);
}
void VAction::subClear(){
    Vector<VAction*> tmpList;
    tmpList.swap(m_actionList);
    long size=(long)tmpList.size();
    for (long i=0;i<size;++i)
        delete tmpList[i];
}

void VAction::init() {
    m_isRunBegin=false; 
    m_isRunEnd=false; 
    m_isSubRunEnd=false;
    m_isAllRunEnd=false;
    m_lastAction=this;
}

VAction::~VAction() { 
    fastRunEnd(); subClear(); 
}

void VAction::fastRunEnd() { 	
    if (m_isAllRunEnd) return;
    if (!m_isRunEnd){
        m_isRunEnd=true;
        doFastRunEnd(m_isRunBegin);
        m_isRunBegin=true;
    }
    if (!m_isSubRunEnd){
        m_isSubRunEnd=true;
        subFastRun();
    }
    m_isAllRunEnd=true;
}
void VAction::run()	{ 
    if ((!m_isRunEnd)&&(!m_isRunBegin)){
        m_isRunBegin=true; 
        doRun(); 
    }
} 


bool VAction::getIsAllRunEnd(){
    if (!m_isRunEnd) return false;
    if (m_isAllRunEnd) return true;
    
    long size=(long)m_actionList.size();
    for (long i=0;i<size;++i){
        if (!m_actionList[i]->getIsAllRunEnd())
            return false;
    }
    m_isAllRunEnd=true;
    return true;
}

void VAction::update(double stepTime_s){
    if (stepTime_s<=0) return;
    double selfUsedTime=0;
    bool isSelfFinish=selfUpdate(stepTime_s,selfUsedTime);
    if (isSelfFinish){
        if (selfUsedTime<=0)
            subUpdate(stepTime_s);
        else if (selfUsedTime>=stepTime_s)
            ;//nothing
        else
            subUpdate(stepTime_s-selfUsedTime);	
    }
}

///

void VAction::addASubAction(VAction* action){ 
    m_lastAction->m_actionList.push_back(action); 
}
void VAction::addASubActionAsLastAction(VAction* last_action){  
    m_lastAction->addASubAction(last_action); 
    m_lastAction=last_action; 
}


////////////

void VActionCtrl::clearCurAction(bool isFastRunEnd){
    clearDelAction();
    if (m_activeAction!=0){
        VAction* tmpAction=m_activeAction;
        m_activeAction=0;
        m_delayDelAction=tmpAction;
        if (isFastRunEnd)
            tmpAction->fastRunEnd();
    }
}

void VActionCtrl::clearDelAction(){
    if (m_delayDelAction!=0){
        VAction* tmpAction=m_delayDelAction;
        m_delayDelAction=0;
        delete tmpAction;
    }
}

void VActionCtrl::setNewActionAndRunIt(VAction* aAction){
    clearCurAction();
    if (aAction!=0){
        m_activeAction=aAction;
        m_activeAction->run();
    }
}
void VActionCtrl::fastRunEnd(){
    clearCurAction();
}

void VActionCtrl::updateAction(double stepTime_s){
    if (m_activeAction!=0)
        m_activeAction->update(stepTime_s);
    if (m_activeAction!=0){
        if(m_activeAction->getIsAllRunEnd())
            clearCurAction(false);
    }
}

/////////


bool VSleepAction::doUpdate(double stepTime_s,double& out_usedTime_s){
    if (stepTime_s<m_lastSleepTime_s){
        out_usedTime_s=stepTime_s;
        m_lastSleepTime_s-=stepTime_s;
    }else {
        out_usedTime_s=m_lastSleepTime_s;
        m_lastSleepTime_s=0;
    }
    return (m_lastSleepTime_s<=0);
} 

////

void VMoveAction::init(IEnableMove* mover, double dMoveTime_s,const long srcX0, const long srcY0, const long destX0,  const long destY0){
    m_mover=mover;
    m_x0=srcX0;
    m_x1=destX0;
    m_y0=srcY0;
    m_y1=destY0;
    m_allTime=dMoveTime_s;
    m_curTime=0;
}
long VMoveAction::getCurValue(const long srcValue,const long dstValue)const{
    long dValue=dstValue-srcValue;
    if (m_curTime>=m_allTime)
        return dstValue;
    else
        return (long) (0.5+srcValue+m_curTime*dValue/m_allTime);
}


bool VMoveAction::doUpdate(double stepTime_s,double& out_usedTime_s){
    bool isFinish=((m_curTime+stepTime_s)>=m_allTime);
    if (isFinish){
        out_usedTime_s=m_allTime-m_curTime;
        m_curTime=m_allTime;			
    }else{
        out_usedTime_s=stepTime_s;
        m_curTime+=stepTime_s;
    }
    
    doMove();
    if (isFinish)
        m_mover=0;
    return isFinish;
} 

void VMoveAction::doMove()
{
    if (!m_mover) return;
    m_mover->setLeft(getCurValue(m_x0,m_x1));
    m_mover->setTop(getCurValue(m_y0,m_y1));	
}

//////

void VZoomMoveAction::init(IEnableZoom* zoomer, double dZoomTime_s, double dSrcZoomScale, double dDestZoomScale,
                           double srcX0, double srcY0, double destX0,  double destY0,bool isUseZoom,bool isUseMove){
    m_zoomer=zoomer;
    m_x0=srcX0;
    m_x1=destX0;
    m_y0=srcY0;
    m_y1=destY0;
    m_zoomX0=dSrcZoomScale;
    m_zoomX1=dDestZoomScale;
    m_zoomY0=dSrcZoomScale;
    m_zoomY1=dDestZoomScale;
    m_allTime=dZoomTime_s;
    m_curTime=0;
    m_isUseMove=isUseMove;
    m_isUseZoom=isUseZoom;
}
double VZoomMoveAction::getCurValue(double srcValue,double dstValue)const{
    double dValue=dstValue-srcValue;
    if (m_curTime>=m_allTime)
        return dstValue;
    else
        return srcValue+m_curTime*dValue/m_allTime;
}


bool VZoomMoveAction::doUpdate(double stepTime_s,double& out_usedTime_s){
    bool isFinish=((m_curTime+stepTime_s)>=m_allTime);
    if (isFinish){
        out_usedTime_s=m_allTime-m_curTime;
        m_curTime=m_allTime;			
    }else{
        out_usedTime_s=stepTime_s;
        m_curTime+=stepTime_s;
    }
    
    doMoveZoom();
    if (isFinish)
        m_zoomer=0;
    return isFinish;
} 

void VZoomMoveAction::doMoveZoom()
{
    if (!m_zoomer) return;
    if (m_isUseZoom) {
        m_zoomer->setZoomX(getCurValue(m_zoomX0,m_zoomX1));
        m_zoomer->setZoomY(getCurValue(m_zoomY0,m_zoomY1));
    }
    if (m_isUseMove) {
        m_zoomer->setMoveX0(getCurValue(m_x0,m_x1));
        m_zoomer->setMoveY0(getCurValue(m_y0,m_y1));
    }		
}


///////

VShakeAction::VShakeAction(IEnableMove* pShaker, double dShakeTime, double dCycleTime, const long& x0, const long& y0, const long& x1, const long& y1)
: m_ShakeTime(dShakeTime), m_PassTime(0), m_pShaker(pShaker)
{
    m_X0 = x0;
    m_Y0 = y0;
    m_X1 = x1;
    m_Y1 = y1;
    m_CycleTime = dCycleTime;
    m_CenterX = (m_X0 + m_X1) / 2;
    m_CenterY = (m_Y0 + m_Y1) / 2;
    m_dTempCycle = 1 / dCycleTime;
}

double VShakeAction::lineMap(double t, double l0, double lt)
{
    return (l0 + 4 * t * (lt - l0) * m_dTempCycle);
}

bool VShakeAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_ShakeTime);
    if (isFinish) {
        out_usedTime_s = m_ShakeTime - m_PassTime;
        m_PassTime = m_ShakeTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doShaking();
    if (isFinish) m_pShaker = 0;
    return isFinish;
} 

void VShakeAction::doShaking()
{
    if (!m_pShaker) return;
    long nTmp =(long)(m_PassTime * m_dTempCycle);
    double t = m_PassTime - m_CycleTime * nTmp;
    double x = 0;
    double y = 0;
    if (t <= m_CycleTime * 0.25)
    {
        x = lineMap(t, m_CenterX, m_X0);
        y = lineMap(t, m_CenterY, m_Y0);
    }
    else if (t <= m_CycleTime * 0.5)
    {
        t -= m_CycleTime * 0.25;
        x = lineMap(t, m_X0, m_CenterX);
        y = lineMap(t, m_Y0, m_CenterY);			
    }
    else if (t <= m_CycleTime * 0.75)
    {
        t -= m_CycleTime * 0.5;
        x = lineMap(t, m_CenterX, m_X1);
        y = lineMap(t, m_CenterY, m_Y1);		
    }
    else
    {
        t -= m_CycleTime * 0.75;
        x = lineMap(t, m_X1, m_CenterX);
        y = lineMap(t, m_Y1, m_CenterY);						
    }
    m_pShaker->setLeft((long)x);
    m_pShaker->setTop((long)y);
}

////

VAlphaAction::VAlphaAction(IEnableAlpha* pAlphaer, const unsigned char& srcAlpha, const unsigned char& destAlpha, double dTotalTime, const bool& bUpdateAllChild)
: m_pAlphaer(pAlphaer), m_SrcAlpha(srcAlpha), m_DestAlpha(destAlpha),	m_TotalTime(dTotalTime), m_UpdateAllChild(bUpdateAllChild), m_PassTime(0)																																		
{
    m_PerChange = (destAlpha - srcAlpha) / dTotalTime;
}

unsigned char VAlphaAction::getAlpha()
{
    if (m_PassTime < m_TotalTime) return (unsigned char)(0.5 + m_SrcAlpha + m_PerChange * m_PassTime);
    return m_DestAlpha;
}

void VAlphaAction::doChangeAlpha()
{
    if (!m_pAlphaer) return;
    unsigned char uAlpha = getAlpha();
    m_pAlphaer->setAlpha((long)uAlpha);
}

bool VAlphaAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_PassTime;
        m_PassTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doChangeAlpha();
    if (isFinish) m_pAlphaer = 0;
    return isFinish;
} 

/////

long VAlphaMoveAction::getAlpha()
{
    if (m_curTime < m_allTime) return (long)(0.5 + m_SrcAlpha + m_PerChangeAlpha * m_curTime);
    return m_DestAlpha;
}	

VAlphaMoveAction::VAlphaMoveAction(IEnableMove* mover, IEnableAlpha* pAlphaer, double dTotalTime, long srcX0, long srcY0, long destX0,  long destY0, long srcAlpha, long destAlpha): 
VMoveAction(mover, dTotalTime, srcX0, srcY0, destX0, destY0), m_pAlphaer(pAlphaer), m_SrcAlpha(srcAlpha), m_DestAlpha(destAlpha)
{
    if (m_SrcAlpha < 0) m_SrcAlpha = 0;
    else if (m_SrcAlpha > 255) m_SrcAlpha = 255;
    
    if (m_DestAlpha < 0) m_DestAlpha = 0;
    else if (m_DestAlpha > 255) m_DestAlpha = 255;
    
    m_PerChangeAlpha = 0;
    if ((m_DestAlpha - m_SrcAlpha) != 0) m_PerChangeAlpha = ((double)(m_DestAlpha - m_SrcAlpha)) / dTotalTime;
}

void VAlphaMoveAction::doFastRunEnd(bool isRunBegin) 
{ 
    VMoveAction::doFastRunEnd(isRunBegin);
    m_pAlphaer = 0;
} 

bool VAlphaMoveAction::doUpdate(double stepTime_s,double& out_usedTime_s)
{
    bool isFinish = VMoveAction::doUpdate(stepTime_s, out_usedTime_s);
    if (isFinish) m_pAlphaer = 0;
    return isFinish;
}

void VAlphaMoveAction::doMove()
{
    VMoveAction::doMove();
    if (m_pAlphaer) m_pAlphaer->setAlpha(getAlpha());
}


//////////

VFlareAction::VFlareAction(IEnableFlare* pFlarer, double dTotalTime, double dPerShowTime, double dPerHideTime)																																		
{
    m_pFlarer = pFlarer;
    m_PassTime = 0;
    m_PerHideTime = dPerHideTime;
    m_PerShowTime = dPerShowTime;
    m_TotalTime = dTotalTime;
    m_PerShowPassTime = 0;
    m_PerHidePassTime = 0;
    m_Visible = false;
    if (pFlarer) pFlarer->setIsVisible(false);
}

void VFlareAction::doFlare(double stepTime_s)
{
    if (!m_pFlarer) return;
    if (m_Visible)
    {
        m_PerShowPassTime += stepTime_s;
        if (m_PerShowPassTime >= m_PerShowTime)
        {
            m_pFlarer->setIsVisible(false);
            m_PerShowPassTime = 0;
            m_PerHidePassTime = 0;
            m_Visible = false;
        }
    }
    else
    {
        m_PerHidePassTime += stepTime_s;
        if (m_PerHidePassTime >= m_PerHideTime)
        {
            m_pFlarer->setIsVisible(true);
            m_PerShowPassTime = 0;
            m_PerHidePassTime = 0;
            m_Visible = true;
        }
    }
}

bool VFlareAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_PassTime;
        m_PassTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doFlare(stepTime_s);
    if (isFinish) m_pFlarer = 0;
    return isFinish;
} 

//
VFlareSwapAction::VFlareSwapAction(IEnableFlare* pFlarer1, IEnableFlare* pFlarer2, double dTotalTime, double dShowTime1, double dShowTime2)																																		
{
    m_pFlarer1 = pFlarer1;
    m_pFlarer2 = pFlarer2;
    m_PassTime = 0;
    m_ShowTime1 = dShowTime1;
    m_ShowTime2 = dShowTime2;
    m_TotalTime = dTotalTime;
    m_ShowPassTime1 = 0;
    m_ShowPassTime2 = 0;
    if (pFlarer1) pFlarer1->setIsVisible(true);
    if (pFlarer2) pFlarer2->setIsVisible(false);
}

void VFlareSwapAction::doFlare(double stepTime_s)
{
    if (!m_pFlarer1 || !m_pFlarer2) return;
    if (m_pFlarer1->getIsVisible())
    {
        m_ShowPassTime1 += stepTime_s;
        if (m_ShowPassTime1 >= m_ShowTime1)
        {
            m_pFlarer1->setIsVisible(false);
            m_pFlarer2->setIsVisible(true);
            m_ShowPassTime2 = 0;
            m_ShowPassTime1 = 0;
        }		
    }
    else
    {
        m_ShowPassTime2 += stepTime_s;
        if (m_ShowPassTime2 >= m_ShowTime2)
        {
            m_pFlarer1->setIsVisible(true);
            m_pFlarer2->setIsVisible(false);
            m_ShowPassTime2 = 0;
            m_ShowPassTime1 = 0;
        }		
    }
}

bool VFlareSwapAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_PassTime;
        m_PassTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doFlare(stepTime_s);
    if (isFinish) 
    {
        m_pFlarer1 = 0;
        m_pFlarer2 = 0;
    }
    return isFinish;
} 

///////

bool VNumChangeAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_PassTime;
        m_PassTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doChangeVal();
    if (isFinish) m_pChanger = 0;
    return isFinish;
} 

VNumChangeAction::VNumChangeAction(IEnableValueChange* pChanger, const Int64& srcVal, const Int64& destVal, double dTotalTime)
: m_pChanger(pChanger), m_SrcVal(srcVal), m_DestVal(destVal),	m_TotalTime(dTotalTime), m_PassTime(0)																																		
{
    m_PerChange = (destVal - srcVal) / dTotalTime;
}
Int64 VNumChangeAction::getCurValue()
{
    if (m_PassTime < m_TotalTime) return ((Int64)(0.5 + m_SrcVal + m_PerChange * m_PassTime));
    return m_DestVal;
}

void VNumChangeAction::doChangeVal()
{
    if (m_pChanger) m_pChanger->setNumValue(getCurValue());
}

///////

bool VProgressChangeAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_PassTime;
        m_PassTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doChangeVal();
    if (isFinish) m_pProgressSprite = 0;
    return isFinish;
} 

VProgressChangeAction::VProgressChangeAction(IProgressSprite* pProgressSprite, Int64 srcVal, Int64 destVal, Int64 maxVal, double dTotalTime)
: m_pProgressSprite(pProgressSprite), m_SrcVal(srcVal), m_DestVal(destVal),	m_TotalTime(dTotalTime), m_PassTime(0)																																		
{
    if (pProgressSprite){
        pProgressSprite->setMaxProgress(maxVal);
    }
    m_PerChange = (destVal - srcVal) / dTotalTime;
}
Int64 VProgressChangeAction::getCurValue()
{
    if (m_PassTime < m_TotalTime) return ((Int64)(0.5 + m_SrcVal + m_PerChange * m_PassTime));
    return m_DestVal;
}

void VProgressChangeAction::doChangeVal()
{
    if (m_pProgressSprite) m_pProgressSprite->setProgress(getCurValue());
}

///////

bool VRotationAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_ElaplsedTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_ElaplsedTime;
        m_ElaplsedTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_ElaplsedTime += stepTime_s;
    }
    
    doRotation(m_ElaplsedTime);
    if (isFinish) m_pDelegate = 0;
    return isFinish;
} 

VRotationAction::VRotationAction(IEnableRotation* pDelegate, double srcAngle, double destAngle, double totalTime): 
m_pDelegate(pDelegate), m_SrcAngle(srcAngle), m_DestAngle(destAngle),	m_TotalTime(totalTime), m_ElaplsedTime(0.0)																																		
{
    m_PerChange = (destAngle - srcAngle) / totalTime;
}

double VRotationAction::getCurAngle(double elpasedTime)
{
    if (elpasedTime < m_TotalTime)
    {
        return (m_SrcAngle + m_PerChange * elpasedTime);
    }
    return m_DestAngle;
}

void VRotationAction::doRotation(double elapsedTime)
{
    if (m_pDelegate) m_pDelegate->setRotaryAngle(getCurAngle(elapsedTime));
}

///////////

VBacklashAction::VBacklashAction(IEnableMove* pMover, long nSrcY, long nHeight, double dFirstDropTime)
: m_pMover(pMover), m_TotalTime(dFirstDropTime), m_PassTime(0), m_Height(nHeight), m_SrcY(nSrcY)																																		
{
    
    init();
}

void VBacklashAction::init()
{
    m_A = m_Height * 2 / MyBase::sqr(m_TotalTime);
    m_DestSpeed = MyBase::sqr(m_A * m_TotalTime);
    m_Attenuation = 0.2;
    
    m_TimeList.resize(20);
    m_TimeList[0] = 0;
    int listSize=(int)m_TimeList.size();
    for (int i = 1; i <listSize ; ++i)
    {
        m_TimeList[i] = 2 * sqrt(MyBase::intPower(m_Attenuation, i) * m_DestSpeed) / m_A;
        m_TotalTime += m_TimeList[i];
    }
    
}

void VBacklashAction::doBacklash()
{
    if (!m_pMover) return;
    double dTmpTime = m_PassTime;
    int i = 0;
    int listSize=(int)m_TimeList.size();
    while (i <listSize )
    {
        if (m_TimeList[i] >= dTmpTime) break;
        else dTmpTime -= m_TimeList[i];
        ++i;
    }
    
    if (i == 2)
    {
        //do finish
    }
    
    double dH = 0;
    if (i >= listSize)
        dH = m_Height;
    else
    {
        if (i == 0) dH = 0.5 * m_A * MyBase::sqr(dTmpTime);
        else
        {
            dTmpTime = MyBase::abs(dTmpTime - m_TimeList[i] * 0.5);
            dH = m_Height - 0.5 * m_A * MyBase::sqr(m_TimeList[i] * 0.5) + 0.5 * m_A * MyBase::sqr(dTmpTime);
        }			
    }
    m_pMover->setTop(m_SrcY + (long)(0.5 + dH));
}


bool VBacklashAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_TotalTime);
    if (isFinish) {
        out_usedTime_s = m_TotalTime - m_PassTime;
        m_PassTime = m_TotalTime;			
    }else{
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    
    doBacklash();
    if (isFinish) m_pMover = 0;
    return isFinish;
} 


void VTweenSlowAction::init(IEnableMove* pMover, EMoveStateY moveStateY, EMoveStateX moveStateX, long objCurPos, double moveTime, long moveDistance, ESlowActionType slowActionType){
    m_pMover = pMover;
    m_BeginPos = objCurPos;
    m_MoveDistance = moveDistance;
    m_PassTime = 0.0;
    m_MoveTime = moveTime;
    m_MoveStateY = moveStateY;
    m_MoveStateX = moveStateX;
    m_SlowActionType = slowActionType;
}

long VTweenSlowAction::slowActionFunction(double passTime, ESlowActionType slowActionType){
    switch(slowActionType){
        case Quadratic:
            return -m_MoveDistance*(passTime/=m_MoveTime)*(passTime-2);
        case Cubic:
            return m_MoveDistance*((passTime=passTime/m_MoveTime-1)*passTime*passTime + 1);
        case Quartic:
            return -m_MoveDistance*((passTime=passTime/m_MoveTime-1)*passTime*passTime*passTime - 1);
        case Quintic:
            return m_MoveDistance*((passTime=passTime/m_MoveTime-1)*passTime*passTime*passTime*passTime + 1);
        case Sinusoidal:
            return m_MoveDistance*sin(passTime/m_MoveTime * (MyBase::PI/2));
        case Exponential:
            return (passTime==m_MoveTime) ? m_MoveDistance : m_MoveDistance*(-pow(2, -10 * passTime/m_MoveTime) + 1);
        case Circular:
            return m_MoveDistance*sqrt(1 - (passTime=passTime/m_MoveTime-1)*passTime);
        default:
            return m_MoveDistance*passTime/m_MoveTime;
    }
}

void VTweenSlowAction::doMove(double PassTime){
    if (!m_pMover) return;
    long passDistance = 0;
    if (m_MoveTime > 0){
        passDistance = slowActionFunction(PassTime, m_SlowActionType);
    }
    if (m_MoveStateY != staticY){
        if (m_MoveStateY == upY || m_MoveStateY == overTopY){
            m_pMover->setTop(m_BeginPos - passDistance);
        } else if (m_MoveStateY == downY || m_MoveStateY == overBottomY){
            m_pMover->setTop(m_BeginPos + passDistance);
        }
    } else if (m_MoveStateX != staticX){
        if (m_MoveStateX == leftX || m_MoveStateX == overLeftX){
            m_pMover->setLeft(m_BeginPos - passDistance);
        } else if (m_MoveStateX == rightX || m_MoveStateX == overRightX){
            m_pMover->setLeft(m_BeginPos + passDistance);
        }
    }
}

bool VTweenSlowAction::doUpdate(double stepTime_s, double& out_usedTime_s){
    bool isFinish = ((m_PassTime + stepTime_s) >= m_MoveTime);
    if (isFinish) {
        out_usedTime_s = m_MoveTime - m_PassTime;
        m_PassTime = m_MoveTime;			
    } else {
        out_usedTime_s = stepTime_s;
        m_PassTime += stepTime_s;
    }
    doMove(m_PassTime);
    if (isFinish) m_pMover = 0;
    return isFinish;
}

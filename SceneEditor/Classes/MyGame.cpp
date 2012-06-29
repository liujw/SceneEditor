//MyGame.cpp

#include "MyGame.h"
#include "VButtonSprite.h"
#include "VBmpButtonSprite.h"
#include "VLabelSprite.h"
#include "VEditSprite.h"
#include "math.h"
//#include "VHandCard.h"
#include "GameRes.h"
#include "VLightFlashEffect.h"
#include "VRectLightFlashEffectSprite.h"

#include "VWebBrowserSprite.h"
#include "VWaterSprite.h"
//#include "testDNA.h"

#include "SysImport.h"

TSpriteTree g_spriteTree;

class VTimeLabelSprite : public VLabelSprite {
public:
	virtual void doUpdate(double stepTime_s){
		this->setText(dateTimeToStr(getNowTime_s()));
	}
};

MyGame::MyGame(INetIOFactory* netIOFactory):VGame(netIOFactory)
{
	m_pMainScene = new VScene();
	this->getSpriteEngine().addASprite(m_pMainScene);
	
	m_isShowFPS=true;

	m_sumTime_s=0;
	 
	m_pSelectLabel = 0;

	loadSprite();
}

MyGame::~MyGame()
{
	if(m_pMainScene)
	{
		m_pMainScene->kill();
		m_pMainScene = 0;
	}
}

void MyGame::freeAllSprite(const String& rootSpriteName)
{
	if(m_spriteMap.size())
    {
		VSprite* pRootSprite = m_spriteMap[rootSpriteName];
		pRootSprite->kill();
		m_spriteMap.clear();
	}
}


VSprite* MyGame::createSprite(TSpriteTree* spriteNode,VSprite* pParent)
{
	return m_spriteFactory.createSprite(m_pMainScene,pParent,spriteNode);
}
void MyGame::gerenalSpriteTree(TSpriteTree* pCurSpriteNode)
{
	if(pCurSpriteNode == NULL)
		return;

	VSprite* pParent = GetSprite(pCurSpriteNode->getParentName());
	if(pParent == NULL)
		pParent = m_pMainScene;

	VSprite* pSprite = createSprite(pCurSpriteNode,pParent);
	if(pSprite)
	{
		addMapSprite(pCurSpriteNode->getName(),pSprite);

		if(pCurSpriteNode->getName() != "BackGround")
			pSprite->setIsEnableSelf(true);
	}
	
	for(long i = 0;i<pCurSpriteNode->getSpriteCount();i++)
	{
		gerenalSpriteTree(pCurSpriteNode->getSprite(i));
	}
}
void MyGame::loadSprite()
{
	if(g_spriteTree.getName() != "")
	{
		gerenalSpriteTree(&g_spriteTree);
	}
}

void MyGame::loadRes(const String& strXmlFile)
{
	freeAllSprite(g_spriteTree.getName());

	g_spriteTree.freeSpriteTree();

	m_pMainScene->loadRes(strXmlFile.c_str());
}

VSprite* MyGame::GetSprite(const String& spriteName)
{
	VSprite* sprite = NULL;
	if(spriteName != "")
		sprite = GetMapSprite(spriteName);

	return sprite;
}

VSprite* MyGame::GetSprite(long posx,long posy)
{
    VSprite* curSprite = this->getSpriteEngine().testCurSprite(posx,posy);

	return curSprite;
}

String MyGame::GetSpriteName(const VSprite* sprite)
{
	VSprite* pSprite = NULL;
    for (TSpriteMap::iterator it(m_spriteMap.begin()); it != m_spriteMap.end(); ++it) 
    {
        pSprite = (VSprite*)it->second;
        if (pSprite == sprite)
        {
            return it->first;
        }
    }

	return "";
}

TSpriteTree* MyGame::GetSpriteTree(const String& spriteName)
{
	//return findSprite(&g_spriteTree,spriteName);
	return g_spriteTree.getSprite(spriteName);
}

class TMyClickEventListener:public IEventListener{	
	virtual void event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen){
		MyGame* game=(MyGame*)callBackData;
		((VSprite*)sender)->kill();
	}
};

void MyGame::addBtn()
{
 	VBmpButtonSprite* m_pBtn = new VBmpButtonSprite();
	m_pBtn->loadFrom("../pic/button.png");
	m_pBtn->setLeft(100);
	m_pBtn->setTop(100);
	
	static TMyClickEventListener clickListenr; 
	m_pBtn->setClickEventListener(&clickListenr,this);
	m_pBack->addASprite(m_pBtn);   
}


void MyGame::update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight){

    VGame::update(stepTime_s,isScreenSizeChanged,newWidth,newHeight);
}


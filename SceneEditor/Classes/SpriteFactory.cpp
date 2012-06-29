#include "SpriteFactory.h"


StringList CSpriteFactory::m_creatCodeList;

CSpriteFactory::CSpriteFactory()
{
	m_spriteCreatorMap.clear();

	registerCreator("VBitmapSprite",CSpriteFactory::CreateBitmapSprite);
	registerCreator("VButtonSprite",CSpriteFactory::CreateButtonSprite);
	registerCreator("VMemoSprite",CSpriteFactory::CreateMemoSprite);	
	registerCreator("VLabelSprite",CSpriteFactory::CreateLabelSprite);	

}
CSpriteFactory::~CSpriteFactory()
{
	m_spriteCreatorMap.clear();
}

VSprite* CSpriteFactory::createSprite(VScene* pScene,VSprite* pParent,TSpriteNode* pSpriteNode)
{
	funSpriteCreator pSpriteCreator = m_spriteCreatorMap[pSpriteNode->getType()];
	if(pSpriteCreator)
	{
		VSprite* pSprite = (*pSpriteCreator)(pScene,pSpriteNode);
		if(pSprite)
		{
			pParent->addASprite(pSprite);
			pSpriteNode->setSprite(pSprite);

			setSprite(pSpriteNode);
			
			return pSprite;
		}
	}
	return NULL;
}
void CSpriteFactory::setSprite(TSpriteNode* pSpriteNode)
{
	m_creatCodeList.push_back("m_pSprite = setZ(pSpriteNode->_z);");
	long nPos = 0;
	if((nPos = pSpriteNode->getLeft()) != 0)
		pSpriteNode->setSpriteLeft(nPos);

	if((nPos = pSpriteNode->getTop()) != 0)
		pSpriteNode->setSpriteTop(nPos);
	
	if((nPos = pSpriteNode->getWidth()) != 0)
		pSpriteNode->setWidth(nPos);

	if((nPos = pSpriteNode->getHeight()) != 0)
		pSpriteNode->setHeight(nPos);

	if((nPos = pSpriteNode->getZ()) != 0)
		pSpriteNode->setZ(nPos);
}

VSprite* CSpriteFactory::CreateBitmapSprite(VScene* pScene,TSpriteNode* pSpriteNode)
{
	m_creatCodeList.push_back("m_pSprite = new VBitmapSprite();");

	if(!pScene->getIsExistClipCanvas(pSpriteNode->getName()))
	{
		pScene->loadSurface(pSpriteNode);
	}

	VCanvas canvas = pScene->getCanvas(pSpriteNode->getName());

	return new VBitmapSprite(canvas);
}

VSprite* CSpriteFactory::CreateButtonSprite(VScene* pScene,TSpriteNode* pSpriteNode)
{
	m_creatCodeList.push_back("m_pSprite = new VButtonSprite();");

	if(!pScene->getIsExistClipCanvas(pSpriteNode->getName()))
	{
		pScene->loadSurface(pSpriteNode);
	}

	VClipCanvas canvas = VClipCanvas(pScene->getCanvas(pSpriteNode->getName()),pSpriteNode->getRow(),pSpriteNode->getCol());
		
	return new VButtonSprite(canvas);
}
VSprite* CSpriteFactory::CreateMemoSprite(VScene* pScene,TSpriteNode* pSpriteNode)
{
	VMemoSprite* pSprite = new VMemoSprite(pSpriteNode->getWidth(),pSpriteNode->getHeight());
	
	pSprite->setIsAutoLine(true);
	pSprite->setIsAllowMultiLine(true);
	pSprite->getFont().setSize(20);
	pSprite->setColor(Color32(255,189,189));

	pSprite->addLine(pSpriteNode->getName().c_str());

	return pSprite;
}
VSprite* CSpriteFactory::CreateLabelSprite(VScene* pScene,TSpriteNode* pSpriteNode)
{
	VLabelSprite* pSprite = new VLabelSprite();
	pSprite->setText(pSpriteNode->getName());

	return pSprite;
}


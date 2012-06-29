#pragma once

#include "sysimport.h"

#include "VBitmapSprite.h"
#include "VMoveBitmapSprite.h"
#include "VButtonSprite.h"
#include "VMemoSprite.h"
#include "VLabelSprite.h"
#include "VScene.h"
#include "SpriteTree.h"

typedef TSpriteTree TSpriteNode;

//void setSpritePos(TSpriteNode* pSpriteNode);
//void setSpritePos(TSpriteNode* pSpriteNode,TSpriteNode* pSpriteParentNode);

typedef VSprite* (*funSpriteCreator)(VScene* pScene,TSpriteNode* pSpriteNode);  

class CSpriteFactory
{
public:
	CSpriteFactory();
	~CSpriteFactory();

public:
	virtual VSprite* createSprite(VScene* pScene,VSprite* pParent,TSpriteNode* pSpriteNode);
	
	void setSprite(TSpriteNode* m_pSpriteNode);

    static VSprite* CreateBitmapSprite(VScene* pScene,TSpriteNode* pSpriteNode);
    static VSprite* CreateButtonSprite(VScene* pScene,TSpriteNode* pSpriteNode);
    static VSprite* CreateMemoSprite(VScene* pScene,TSpriteNode* pSpriteNode);
    static VSprite* CreateLabelSprite(VScene* pScene,TSpriteNode* pSpriteNode);


	StringList getCreateCodeList()
	{
		return m_creatCodeList;
	}
private:
	typedef HashMap<String,funSpriteCreator> TSpriteCreatorMap;
	TSpriteCreatorMap	m_spriteCreatorMap;

	bool registerCreator(const String& SpriteType,funSpriteCreator pSpriteCreator)
	{ 
		m_spriteCreatorMap[SpriteType] = pSpriteCreator; 
		return true; 
	}

	static StringList m_creatCodeList;
};
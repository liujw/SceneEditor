//
//  MyGame.h
//
//  Created by housisong on 08-3-21.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _MyGame_h_
#define _MyGame_h_

#include "VGame.h"
#include "VScene.h"
#include "VBitmapSprite.h"
#include "VMoveBitmapSprite.h"
#include "VButtonSprite.h"
#include "Rand.h"
#include "VSound.h" 
#include "DownLoader.h"
#include "VMemoSprite.h"
#include "SpriteFactory.h"
#include "SpriteTree.h"

extern TSpriteTree g_spriteTree;

class MyGame:public VGame,IDownLoadListener{
public:
	VScene* m_pMainScene;
	CSpriteFactory m_spriteFactory;

	VBitmapSprite* m_pBack;
	VMoveBitmapSprite* m_pMoveBmp;
	VBitmapSprite* m_pHumen;
	double    m_sumTime_s;
	VRand     m_rand;	
	VSurface surface;
	VSurface surfaceQiJuRec;
	VSurface surfaceBmp;
	VSurface surfaceHumen;
	VBmpButtonSprite* m_pBtnStart;
	VBmpButtonSprite* m_pBtnQiJuRec;

	VLabelSprite* m_pSelectLabel;
public:
	explicit MyGame(INetIOFactory* netIOFactory);
	~MyGame();
	void addBtn();

	void selectSprite(String spriteName);
	VSprite* GetSprite(const String& spriteName);
	TSpriteTree* GetSpriteTree(const String& spriteName);
	VSprite* GetSprite(long posx,long posy);
	String GetSpriteName(const VSprite* sprite);

	void loadRes(const String& strXmlFile);
	void loadSprite();
	VSprite* createSprite(TSpriteTree* spriteNode,VSprite* pParent);
	void gerenalSpriteTree(TSpriteTree* pRootSprite);

public:
	virtual void update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight);
	void freeAllSprite(const String& rootSpriteName);
private:
	typedef HashMap<String, VSprite*> TSpriteMap;

    void addMapSprite(const String& spriteName,VSprite* sprite)
    {
         m_spriteMap[spriteName] = sprite;
    }

    VSprite* GetMapSprite(const String& spriteName)
    {
        //查找是否已经注册该消息映射
        if(m_spriteMap.size() && m_spriteMap.find(spriteName) != m_spriteMap.end())
        {
            return m_spriteMap[spriteName];
        }
        
        return NULL;
    }    

    TSpriteMap m_spriteMap;
};

#endif //_MyGame_h_
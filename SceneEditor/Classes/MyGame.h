//
//  MyGame.h
//
//  Created by housisong on 08-3-21.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _MyGame_h_
#define _MyGame_h_

#include "VGame.h"
#include "VBitmapSprite.h"
#include "VMoveBitmapSprite.h"
#include "Rand.h"
#include "VSound.h" 
#include "DownLoader.h"
#include "VMemoSprite.h"

class MyGame:public VGame,IDownLoadListener{
private:
	VBitmapSprite* m_pBack;
	VMoveBitmapSprite* m_pMoveBmp;
	double    m_sumTime_s;
	VRand     m_rand;		
	VSurface surfaceBmp;
	VSoundManager m_sound;
	VSurfaceGray8  m_surface8_a;
	VSurfaceGray8  m_surface8_b;
	VSurfaceGray8  m_surface8_c;
	VMemoSprite*   m_memo;
public:
	explicit MyGame(INetIOFactory* netIOFactory);
	void addBtn();
	void addLable(long fontSizei);

	virtual void downLoader_finish(IDownLoader* downLoader);
public:
	virtual void update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight);
};

#endif //_MyGame_h_
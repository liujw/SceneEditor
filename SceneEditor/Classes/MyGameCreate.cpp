//
//  MyGameCreate.cpp
//  testEngine
//
//  Created by housisong on 08-3-21.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#include "MyGameCreate.h"
#include "MyGame.h"
#include "VGameCtrl.h"

void MyGameCreate(IUpdateListenerBase* out_updateListener){
	VNetIOFactory* netIOFactory=new VNetIOFactory();
	MyGame*  game=new MyGame(netIOFactory);
	VGameCtrl* gameCtrl=new VGameCtrl(game,netIOFactory); 
	gameCtrl->getUpdateListener(out_updateListener);
}

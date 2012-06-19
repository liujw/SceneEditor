/*
 *  InAppPurchase.cpp
 *  Ddz
 *
 *  Created by housisong on 09-11-2.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "InAppPurchase.h"
#include "SysImport.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////

#ifdef _IS_USE_InAppPurchaseDocPlug
IInAppPurchaseDocPlug* g_InAppPurchaseDocPlug=0;
void VInAppPurchase::setInAppPurchaseDocPlug(IInAppPurchaseDocPlug* inAppPurchaseDocPlug){
	if ((g_InAppPurchaseDocPlug==0)||(inAppPurchaseDocPlug==0))
		g_InAppPurchaseDocPlug=inAppPurchaseDocPlug;
	else
		assert(inAppPurchaseDocPlug==g_InAppPurchaseDocPlug);
}
#endif

class TInAppPurchase{
private:
public:
	TInAppPurchase(IInAppPurchaseListener* listener){
#ifdef _IS_USE_InAppPurchaseDocPlug
		if (g_InAppPurchaseDocPlug!=0){
			g_InAppPurchaseDocPlug->initPurchase(listener);
		}
#endif
	}
	~TInAppPurchase(){	
#ifdef _IS_USE_InAppPurchaseDocPlug
		if (g_InAppPurchaseDocPlug!=0){
			g_InAppPurchaseDocPlug->endPurchase();
		}
#endif
	}
	void toGoodsList(const char*AppID){
#ifdef _IS_USE_InAppPurchaseDocPlug
		if (g_InAppPurchaseDocPlug!=0){
			g_InAppPurchaseDocPlug->toGoodsList(AppID);
		}
#endif
	}
	void toBuyGoods(const TGoodsInfo& goods){
#ifdef _IS_USE_InAppPurchaseDocPlug
		if (g_InAppPurchaseDocPlug!=0){
			g_InAppPurchaseDocPlug->toBuyGoods(goods);
		}
#endif
	}
	void finishReceipt(const void* receiptHandle){
#ifdef _IS_USE_InAppPurchaseDocPlug
		if (g_InAppPurchaseDocPlug!=0){
			g_InAppPurchaseDocPlug->finishReceipt(receiptHandle);
		}
#endif
	}
};



//////////////////////////////////////////////////////////////////////////////
//class VInAppPurchase

bool  VInAppPurchase::getIsCanBuyGoods(){
	return true;
}

VInAppPurchase::VInAppPurchase(IInAppPurchaseListener* listener):m_import(0){
	m_import=new TInAppPurchase(listener);
}
VInAppPurchase::~VInAppPurchase(){
	if (m_import!=0){
		delete m_import;
		m_import=0;
	}
}

void VInAppPurchase::toGoodsList(const char* AppID){
	((TInAppPurchase*)m_import)->toGoodsList(AppID);
}

void VInAppPurchase::toBuyGoods(const TGoodsInfo& goods){
	((TInAppPurchase*)m_import)->toBuyGoods(goods);
}

void VInAppPurchase::finishReceipt(const void* receiptHandle){
	((TInAppPurchase*)m_import)->finishReceipt(receiptHandle);
}

//void VInAppPurchase::toCancelBuyGoods(const TGoodsInfo& goods,const String& receiptData){} 


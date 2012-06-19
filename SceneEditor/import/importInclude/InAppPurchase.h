/*
 *  InAppPurchase.h
 *  Ddz
 *
 *  Created by housisong on 09-11-2.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _InAppPurchase_h_
#define _InAppPurchase_h_


#include "SysImportType.h"

struct TGoodsInfo{
    //void*           goodsHandle;
    String		name;	//名称
    String		ID;		//商品编码
    double			price;	//价格
    String		priceInfo;	//价格的字符串描述
    unsigned long	money;	//悟性或金币
    String		info;	//描述
};

//typedef Vector<TGoodsInfo> TGoodsList;

//购买结果(成功购买商品,商品进入购买流程,已经购买过该商品,交易取消,交易失败)
enum TReceiptState{ rs_ok,rs_purchasing,rs_restored,rs_cancel,rs_failed};
//购买错误类型(无,客户端不允许购买,商品无效,设备不允许购买,未知)
enum TPaymentErrorType { pe_nil,pe_clientInvalid,pe_paymentInvalid,pe_paymentNotAllowed,pe_unknown};

class IInAppPurchaseListener{
public:
    virtual void disposeGoodsList(const TGoodsInfo* goodsList,long goodsCount,const String& error_info)=0;
    virtual void disposeBuyGoodsReceipt(TReceiptState receiptState,const void* receiptHandle,const String& goodsID,const String& receiptData,TPaymentErrorType error_type)=0;
    virtual ~IInAppPurchaseListener(){}
};

#ifndef _IOS
#define _IS_USE_InAppPurchaseDocPlug
#endif

#ifdef _IS_USE_InAppPurchaseDocPlug
class IInAppPurchaseDocPlug{
public:
    virtual void initPurchase(IInAppPurchaseListener* listener){}
    virtual void endPurchase(){}
    
    virtual void toGoodsList(const char* AppID){}
    virtual void toBuyGoods(const TGoodsInfo& goods){}
    virtual void finishReceipt(const void* receiptHandle){}
    virtual ~IInAppPurchaseDocPlug(){}
};
#endif

class VInAppPurchase{
private:
    void* m_import;
public:
    static bool  getIsCanBuyGoods(); //是否允许购买
    
    VInAppPurchase(IInAppPurchaseListener* listener);
    virtual ~VInAppPurchase();
    
    void toGoodsList(const char* AppID);//获取商品列表
    void toBuyGoods(const TGoodsInfo& goods); //购买商品
    //void toCancelBuyGoods(const TGoodsInfo& goods,const String& receiptData); //取消购买的商品
    void finishReceipt(const void* receiptHandle);
    void updateRemainPurchasedTransactions();//由于苹果不再自动触发遗留订单，默认在获得商品成功后，程序调用该接口，处理遗留已支付订单
    
public:
#ifdef _IS_USE_InAppPurchaseDocPlug
    static void setInAppPurchaseDocPlug(IInAppPurchaseDocPlug* inAppPurchaseDocPlug);
#endif
};



#endif
//MyGame.cpp

#include "MyGame.h"
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

class VTimeLabelSprite : public VLabelSprite {
public:
	virtual void doUpdate(double stepTime_s){
		this->setText(dateTimeToStr(getNowTime_s()));
	}
};


MyGame::MyGame(INetIOFactory* netIOFactory):VGame(netIOFactory),m_pBack(NULL){
	m_isShowFPS=true;
	//getBestXY();

	m_sumTime_s=0;
	 
	/*VSurface* pic=new VSurface();
	pic->loadFrom("pic/ditu.JPG");
	
	VWaterSprite* water=new VWaterSprite(pic,true);
	water->setIsEnableSelf(true);
	this->getSpriteEngine().addASprite(water);
	m_back=(VBitmapSprite*)water;*/

	m_pBack = new VBitmapSprite();
	m_pBack->setIsDrawAsCopy(true);
	m_pBack->loadFrom("../pic/ditu.png");//"pic/qdq.png");
	this->getSpriteEngine().addASprite(m_pBack);
	
	VMemoSprite* memo=new VMemoSprite(80,13);
	//VLabelSprite* memo=new VLabelSprite();
	//memo->setWidth(200);
	memo->setIsAutoLine(true);
	memo->setIsAllowMultiLine(true);
	memo->addLine("v{\\\\anchor=hello;test@333.com}333444444444444444444{//}v{\\\\style=\\b\\u\\c0}hello{//}bbbbb");
	memo->setZ(1000);
	m_pBack->addASprite(memo);

	/*memo->setMaxLineCount(10000);
	String str;
	{
		FileInputStream txtFile("xieyi.txt");
		//while (txtFile.readLine(str))
		//	memo->addLine(str);
		unsigned long strLength=txtFile.getDataSize();
		str.resize(strLength);
		txtFile.read((void*)str.c_str(),strLength);
	}
	memo->setText(str);
	memo->setLeft(0);
	memo->setTop(0);
	memo->toFirstLine();
	this->getSpriteEngine().addASprite(memo);
	m_memo=memo;*/
	
	//*
	//m_zaka_surface.loadFrom("pic/qdq_button_png.fh");
	surfaceBmp.loadFrom("../pic/move.png");
	m_pMoveBmp=new VMoveBitmapSprite(surfaceBmp.getCanvas());
	//m_zaka->setColoring(Color32(200,255,100));
	//m_zaka->setAlpha(200);
	m_pMoveBmp->setZoom(0.88);
	m_pMoveBmp->setIsEnableSelf(true);
	m_pMoveBmp->setRotaryAngle(MyBase::PI*0.33333);
	m_pMoveBmp->setZ(10);
	m_pBack->addASprite(m_pMoveBmp);
	m_pMoveBmp->setLeft(400);
	m_pMoveBmp->setTop(400);

	addBtn();
	//for (long i=0;i<20;++i) addBtn();
	
	//for (long i=0;i<20;++i) addLable(i);
	
	
	/*VEditSprite* edit=new VEditSprite(20,100,300,30);
	edit->setText(getLanguageID());
	//edit->setShowHit("请输入你的名字");
	edit->getFont().setSize(20);
	edit->setColor(Color32(0,0,255));
	//edit->setIsUsePassWordChar(true);
	this->getSpriteEngine().addASprite(edit);*/
	
	
	//TSurfaceData* bmpClip=new TSurfaceData("pic/IPpai.png",1,55);
	//*/
	
	//VAutoZoomCtrlHandCard* handCard=new VAutoZoomCtrlHandCard();
	/*for (long i=0;i<13;++i){
		VCardSprite* card=new VCardSprite(bmpClip->getClipCanvas());
		card->setCardNo(getCardNo(i+1,ctHeiTao));
		card->setZoom(0.88);
		handCard->addCard(card);
	}
	handCard->setZ(10);
	handCard->setLeft(200);
	handCard->setTop(80);
	this->getSpriteEngine().addASprite(handCard);
	*/
	
	//*
	/*
	handCard=new VAutoZoomCtrlHandCard();
	handCard->setIsAutoZoomCardOnSelect(true);
	handCard->setCardDx(17);
	handCard->setMaxZoomSpace(0);
	for (long i=0;i<13;++i){
		VCardSprite* card=new VCardSprite(bmpClip->getClipCanvas());
		card->setCardNo(getCardNo(i+1,ctFangPian));
		handCard->addCard(card);
	}
	for (long i=0;i<7;++i){
		VCardSprite* card=new VCardSprite(bmpClip->getClipCanvas());
		card->setCardNo(getCardNo(i+1,ctHeiTao));
		handCard->addCard(card);
	}
	handCard->setZ(10);
	handCard->setLeft(240+25);
	handCard->setTop(270);
	this->getSpriteEngine().addASprite(handCard);
	//*/
	
	/*
	m_surface8_a.loadFrom("pic/zmbsguang.bmp");
	VClipCanvasGray8 clipCanvas8(m_surface8_a.getCanvas(),1,14);
	VLightFlashEffectRandom* lightFlash=new VLightFlashEffectRandom(clipCanvas8,1,1000000,450,480,320);
	//this->getSpriteEngine().addASprite(lightFlash);
	
	m_surface8_b.loadFrom("pic/sparkle_large.BMP"); 
	clipCanvas8.clip(m_surface8_b.getCanvas(),1,14);
	lightFlash=new VLightFlashEffectRandom(clipCanvas8,1.2,1000000,20,480,320);
	//this->getSpriteEngine().addASprite(lightFlash);
	
	m_surface8_c.loadFrom("pic/powerglow.BMP"); //10
	clipCanvas8.clip(m_surface8_c.getCanvas(),1,10);
	lightFlash=new VLightFlashEffectRandom(clipCanvas8,2,1000000,2,480,320);
	//this->getSpriteEngine().addASprite(lightFlash);
	
	m_surface8_a.loadFrom("pic/zmbsguang.bmp");
	m_surface8_b.loadFrom("pic/sparkle_large.BMP"); 
	m_surface8_c.loadFrom("pic/powerglow.BMP"); //10
	VRectLightFlashEffectSprite* shenji=new VRectLightFlashEffectSprite(
																		VClipCanvasGray8(m_surface8_a.getCanvas(),1,14),VClipCanvasGray8(m_surface8_b.getCanvas(),1,14),
																		VClipCanvasGray8(m_surface8_c.getCanvas(),1,10),3,63,81);
	shenji->setLeft(100);
	shenji->setTop(20);
	shenji->setZ(10000);
	this->getSpriteEngine().addASprite(shenji);
	 //*/
	
	/*
	
	VTimeLabelSprite* timeLabelSprite=new VTimeLabelSprite();
	timeLabelSprite->setLeft(110);
	timeLabelSprite->setTop(55);
	timeLabelSprite->getFont().setSize(25);
	timeLabelSprite->setColor(cl32Blue);
	this->getSpriteEngine().addASprite(timeLabelSprite);
	*/
	
	//VWebBrowserSprite* web=new VWebBrowserSprite(10,20,200,140);
	//web->openURL("http://192.168.0.72:5555/iphoneAccountManagement/Rules.html");
	//web->openURL("http://www.80166.com");
	//this->getSpriteEngine().addASprite(web);
	
	//VDownLoader*  downLoad=new VDownLoader();
	//downLoad->downLoadByURL(this,"http://hh.basecity.com/download/DDZInfo.vol");
	
	//
	
	//m_sound.play("sound/hai.ogg");  	
	//m_sound.play("sound/H_HJD.mp3");  
	
}

void MyGame::playXX(){
	//m_sound.playLoopCount("sound/H_HJDClick.wav",1);
	//m_sound.playLoopCount("sound/H_HJD.mp3",1);  
	//m_sound.play("sound/hai.ogg");  	
}

void MyGame::downLoader_finish(IDownLoader* downLoader){
	long fileSize=downLoader->getDownLoadedDataSize();
	char* data=new char[fileSize+1];
	data[fileSize]=0;
	downLoader->readDownLoadedData(data, 0, fileSize);
	m_memo->setText(data);
}

class TMyClickEventListener:public IEventListener{	
	virtual void event(void* sender,void* callBackData,void* pdata,long data0,long data1,bool& isContinueListen){
		MyGame* game=(MyGame*)callBackData;
		((VSprite*)sender)->kill();
		game->playXX();
	}

};
void MyGame::addLable(long fontSizei){
	
	VLabelSprite* lb=new VLabelSprite();
	long minFontSize=6;
	String s;
	itoa(fontSizei+minFontSize,s);
	lb->setText(s+"号字体  你好Hello!");
	lb->setZ(100);
	lb->setLeft(200);
	lb->setTop(-10+(fontSizei+minFontSize)*(fontSizei+minFontSize)/2);
	lb->getFont().setSize((fontSizei+minFontSize));
	lb->setColor(Color32(255,0,0));
	m_pBack->addASprite(lb);
}
void MyGame::addBtn()
{
 	VBmpButtonSprite* pBtn = new VBmpButtonSprite();
	pBtn->loadFrom("../pic/button.png");
	pBtn->setLeft(400);
	pBtn->setTop(100);
	
	static TMyClickEventListener clickListenr; 
	pBtn->setClickEventListener(&clickListenr,this);
	m_pBack->addASprite(pBtn);   
}

/*
#include "RotaryZoom.h"
TPixelsRef cvRef(const Pixels32Ref& ref){
    TPixelsRef result;
    result.pdata=ref.pdata;
    result.byte_width=ref.byte_width;
    result.width=ref.width;
    result.height=ref.height;
    result.color_type=ctRGB32;
    return result;
}
*/

void MyGame::update(double stepTime_s,bool isScreenSizeChanged,long newWidth,long newHeight){
	/*m_zaka->setIsVisible(false);
	
	m_sumTime_s+=stepTime_s;
	
	double x0=(100-60*m_sumTime_s);
	double y0=(200-20*m_sumTime_s);
	long w=m_zaka->getWidth()+m_back->getWidth();
	long h=m_zaka->getHeight()+m_back->getHeight();
	x0=fmod(x0,w);
	if (x0<0) x0+=w;
	y0=fmod(y0,h);
	if (y0<0) y0+=h;
	m_zaka->setMoveX0(x0-m_zaka->getWidth()/2);
	m_zaka->setMoveY0(y0-m_zaka->getHeight()/2);
	//m_zaka->setMoveX0(300);
	//m_zaka->setMoveY0(150);
	*/
    VGame::update(stepTime_s,isScreenSizeChanged,newWidth,newHeight);
}
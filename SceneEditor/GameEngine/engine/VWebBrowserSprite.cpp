/*
 *  VWebBrowserSprite.cpp
 *
 *  Created by housisong on 08-7-8.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VWebBrowserSprite.h"
#include "../../import/importInclude/WebImport.h"

void* VWebBrowserSprite::createImport(long x0,long y0,long width,long height,bool isShare){
    return web_create(x0,y0,width,height,(long)isShare);
}
void VWebBrowserSprite::deleteImport(void* webImportHandle){
    if (webImportHandle!=0)
        web_delete(webImportHandle);
}


class _VWebBrowserSprite:public VWebBrowserSprite{
public:
    must_inline void _diposeWebLoadFinished(){ diposeWebLoadFinished(); }
    must_inline void _diposeWebLoadErrored(){ diposeWebLoadErrored(); }
};

static void _g_diposeWebLoadFinished(void* callBackData){ ((_VWebBrowserSprite*)callBackData) ->_diposeWebLoadFinished(); }
static void _g_diposeWebLoadErrored(void* callBackData){ ((_VWebBrowserSprite*)callBackData) ->_diposeWebLoadErrored(); }

VWebBrowserSprite::VWebBrowserSprite(long x0,long y0,long width,long height,bool isShare){
    init();
    setLeft(x0);
    setTop(y0);
    setWidth(width);
    setHeight(height);
    m_webImportHandle=createImport(x0,y0,width,height,isShare);
    web_setListener(m_webImportHandle, this, _g_diposeWebLoadFinished, _g_diposeWebLoadErrored);
}


void VWebBrowserSprite::doUpdateChange(){
    long left; long top;
    this->getInSpriteListTreePos(left,top);
    web_setPosAndSize(m_webImportHandle,left,top,getWidth(),getHeight());
    web_setVisible(m_webImportHandle, (long)getIsVisible());
    web_setBackGroundColor(m_webImportHandle,m_backColor.r,m_backColor.g,m_backColor.b,m_backColor.a);
} 



void VWebBrowserSprite::doDraw(const VCanvas& dst,long x0,long y0){
    //do nothing
}

void VWebBrowserSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    //do nothing
    out_rect.setEmpty();
}

void VWebBrowserSprite::openURL(const String& url){
    web_openURL(m_webImportHandle, url.c_str());
}


void VWebBrowserSprite::openHtmlFile(const String& htmlFileName){
    String path=getFilePath(htmlFileName.c_str(), (long)htmlFileName.size());
    const char* fileName=getFileNameNoPath(htmlFileName.c_str(), (long)htmlFileName.size());	
    web_openHtmlFile(m_webImportHandle, fileName, path.c_str());
}


void VWebBrowserSprite::openURLWithSystem(const String& url){
    web_openURLWithSystem(url.c_str());	
}
void VWebBrowserSprite::openSmsWithSystem(const String& phoneNumber,const String& text){
    web_openSmsWithSystem(phoneNumber.c_str(),text.c_str());	
}
void VWebBrowserSprite::openPhoneWithSystem(const String& phoneNumber){
    web_openPhoneWithSystem(phoneNumber.c_str());	
}
void VWebBrowserSprite::openMailWithSystem(const String& mailAddress,const String& title,const String& text){
    web_openMailWithSystem(mailAddress.c_str(),title.c_str(),text.c_str());	
}

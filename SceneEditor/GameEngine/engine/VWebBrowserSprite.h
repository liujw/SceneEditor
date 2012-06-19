/*
 *  VWebBrowserSprite.h
 *
 *  Created by housisong on 08-7-8.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VWebBrowserSprite_h_
#define _VWebBrowserSprite_h_

#include "VSprite.h"


class VWebBrowserSprite;

class IWebBrowserListener{
public:
    virtual void disposeWebBrowserLoadFinished(VWebBrowserSprite* sender) {}
    virtual void disposeWebBrowserLoadErrored(VWebBrowserSprite* sender) {}
    virtual ~IWebBrowserListener(){}
};

class VWebBrowserSprite:public VSprite{
protected:
    void*		m_webImportHandle;
    Color32		m_backColor;
    IWebBrowserListener* m_listener;
    void init(){
        m_webImportHandle=0;
        m_backColor=cl32Black;
        m_listener=0;
    }
protected:
    static void* createImport(long x0,long y0,long width,long height,bool isShare);
    static void deleteImport(void* webImportHandle);
    
    void diposeWebLoadFinished(){
        if (m_listener!=0){
            m_listener->disposeWebBrowserLoadFinished(this);
        }
    }
    void diposeWebLoadErrored(){
        if (m_listener!=0){
            m_listener->disposeWebBrowserLoadErrored(this);
        }
    }
protected:
    virtual void doUpdateChange();
    virtual void doDraw(const VCanvas& dst,long x0,long y0);
    virtual void getWantDrawRect(long x0,long y0,TRect& out_rect);
public:
    VWebBrowserSprite(long x0,long y0,long width,long height,bool isShare=true);
    virtual	~VWebBrowserSprite(){ deleteImport(m_webImportHandle); }
    
    Color32 getBackColor()const{
        return m_backColor;
    }
    void setBackColor(const Color32& color){
        if (m_backColor!=color){
            m_backColor=color;
            changed();
        }
    }
    
    void setListener(IWebBrowserListener* listener){
        m_listener=listener;
    }
    IWebBrowserListener* getListener()const{
        return m_listener;
    }
    
    void openURL(const String& url);
    void openHtmlFile(const String& htmlFileName);
    static void openURLWithSystem(const String& url);
    static void openSmsWithSystem(const String& phoneNumber,const String& text);
    static void openPhoneWithSystem(const String& phoneNumber);
    static void openMailWithSystem(const String& mailAddress,const String& title,const String& text);
};

#endif //_VWebBrowserSprite_h_

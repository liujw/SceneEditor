//
//  EditImport.h
//
//  Created by housisong on 08-4-18.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _EditImport_h_
#define _EditImport_h_

#if defined(_IOS) || defined(_MACOSX)

#ifdef __cplusplus 
extern "C" {
#endif
    
    typedef void (*TEditOnBeginEditEventProc)(void* callBackData);
    typedef void (*TEditOnEndEditEventProc)(void* callBackData);
    typedef void (*TEditOnInputEventProc)(void* callBackData,int selPos0,int selLength,const char* newInputStr,int* isCanInput);
    typedef void* TEditImportHandle;
    
    TEditImportHandle edit_create(long x0,long y0,long width,long height,
                                  void* callBackData,TEditOnBeginEditEventProc onBeginEditEventProc,TEditOnEndEditEventProc onEndEditEventProc,TEditOnInputEventProc onInputEventProc);
    void edit_delete(TEditImportHandle editImportHandle);
    void edit_setPosAndSize(TEditImportHandle editImportHandle,long x0,long y0,long width,long height);
    void edit_setFont(TEditImportHandle editImportHandle,const char* fontName,long fontSize,long isBold);
    void edit_setShowHit(TEditImportHandle editImportHandle,const char* hitText);
    void edit_setVisible(TEditImportHandle editImportHandle,long isVisible);
    void edit_setText(TEditImportHandle editImportHandle,const char* text);
    long edit_getText(TEditImportHandle editImportHandle,char* out_text,long textMaxSize);
    void edit_setColor(TEditImportHandle editImportHandle,unsigned char R,unsigned char G,unsigned char B,unsigned char A);
    void edit_setIsUsePassWordChar(TEditImportHandle editImportHandle,long isUsePassWordChar);
    void edit_setKeyBordIsTransparence(TEditImportHandle editImportHandle,long keyBordIsTransparence);   
    void edit_setInInputing(TEditImportHandle editImportHandle,long isEditing); 
    
    //private
    void importView_setVisible(void* _view,long isVisible);
    
    
#ifdef __cplusplus 
}
#endif

#endif
#endif //_EditImport_h_
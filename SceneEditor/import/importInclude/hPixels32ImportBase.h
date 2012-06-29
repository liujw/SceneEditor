//hPixels32ImportBase.h

#ifndef _hPixels32ImportBase_h_
#define _hPixels32ImportBase_h_

#ifdef __cplusplus 
extern "C" {
#endif
    
    //import CGBitmapContext or DIB
    void getNewPixels_Import(long width,long height,void** pColorData,long* byte_width,void** out_contextHandleImport);
    typedef struct TContextDataBase{
        void* context;
    } TContextDataBase;
    
    void deletePixels_Import(void* contextHandleImport,void* pColorData);
    
    typedef void* TImageHandleImport;
    TImageHandleImport _loadImageFile(const char* fileName,const char* pathName,long* out_width,long* out_height);
    void drawImage(void* contextHandleImport,TImageHandleImport imageHandleImport);
    void deleteImageHandle(TImageHandleImport imageHandleImport);
    
    //text out 
    
    void ContextHandle_setFont(void* contextHandleImport,const char* fontName,long fontSize,int isBlod);
    void ContextHandle_GetDrawATextSize(void* contextHandleImport,int aChar,long* out_width,long* out_height);
    void ContextHandle_DrawAText(void* contextHandleImport,int aChar);
    
    
#ifdef __cplusplus 
}
#endif
#endif //_hPixels32ImportBase_h_

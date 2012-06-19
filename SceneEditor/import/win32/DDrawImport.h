//DDrawImport.h
#ifndef __DDraw_Import_H_
#define __DDraw_Import_H_

struct _TPixelsRef{
    void*		pdata;        
    long        byte_width;   
	long        width;        
	long        height; 
};
	
class CDDrawImprot
{
private:
    void* m_Import;
    void  clearImportPointer();
public:
    CDDrawImprot();
    ~CDDrawImprot();

    bool intiScreen(long ScreenWidth,long ScreenHeight,long ScreenColorBit,void * main_window_handle); 
    bool drawFrame(const _TPixelsRef* src32,long* pDstRect=0,long rectCount=0);
	bool setActive(bool isActive);
};


#endif //__DDraw_Import_H_
//hPixels32Import.h


#ifndef _hPixels32Import_h_
#define _hPixels32Import_h_

#include "hPixels32.h"

class Pixels32Import:public Pixels32
{
protected:
    virtual TPixelsRefEx  getNewMemory(const long width,const long height);
    virtual void delMemory(TPixelsRefEx& ref);
    void import_loadFrom(const char* filename);
public:
    virtual  ~Pixels32Import() { delMemory(m_data); }
    static void loadFileTo(Pixels32& dst,const char* filename);
};


#endif //_hPixels32Import_h_

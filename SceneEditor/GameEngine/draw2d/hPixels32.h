//hPixels32.h


#ifndef _hPixels32_h_
#define _hPixels32_h_

#include "hColor32.h"
#include "hPixels.h"
#include "imageFile/hImgFile.h"


class Pixels32:public TMemPixels<Pixels32Ref>,public IPixels32Buf {
public:
    virtual void loadFrom(const char* filename);
    virtual void loadFrom(IInputStream& stream);
    virtual void lockRef(long width,long height,Pixels32Ref& out_Ref);
};



class PixelsGray8:public TMemPixels<PixelsGray8Ref>,public IPixelsGray8Buf {
public:
    virtual void loadFrom(const char* filename);	
    virtual void loadFrom(IInputStream& stream);
    virtual void lockRef(long width,long height,PixelsGray8Ref& out_gray8Ref);
};


#endif //hPixels32_h_

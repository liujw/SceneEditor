//hPixels32.cpp

#include "hPixels32.h"
#include "imageFile/hFh8File.h"

#include "hPixels32Import.h" //for Pixels32Import::loadFileTo
#include "hDraw.h"
#include "imageFile/hFhFile.h"
#include "imageFile/hFh8File.h"
#include "imageFile/hBmpFile.h"
#include "imageFile/hJpegFile.h"
#include "imageFile/hPngFile.h"

void Pixels32::loadFrom(const char* filename){
    const char* afileSuffix=getFileSuffix(filename);
    long   aStrLength=0;
    if (afileSuffix!=0) aStrLength=(long)strlen(afileSuffix);
#ifdef IS_NEED_CODE_FH
    if (isFileSuffixEqual(afileSuffix,aStrLength,".FH",3)){ //*.fh
        TfhFile::loadFrom(this,filename);
    } else 
#endif
#ifdef IS_NEED_CODE_FH8
        if (isFileSuffixEqual(afileSuffix,aStrLength,".FH8",4)){  //*.fh8
            PixelsGray8 tmp;
            Tfh8File::loadFrom(&tmp,filename);
            this->resizeFast(tmp.getWidth(),tmp.getHeight());
            copyFromGray(this->getRef(),0,0,tmp.getRef());
        } else
#endif
#ifdef IS_NEED_CODE_PNG
            if (isFileSuffixEqual(afileSuffix,aStrLength,".PNG",4)){  //*.png
                TPngColor32File::loadFrom(this,filename);
            } else 
#endif
#ifdef IS_NEED_CODE_BMP
                if (isFileSuffixEqual(afileSuffix,aStrLength,".BMP",4)){  //*.bmp
                    TBmpColor32File::loadFrom(this,filename);
                } else 
#endif
#ifdef IS_NEED_CODE_JPG
                    if (isFileSuffixEqual(afileSuffix,aStrLength,".JPG",4) || isFileSuffixEqual(afileSuffix,aStrLength,".JPEG",5)){  //*.jpg *.jpeg
                        TJpegColor32File::loadFrom(this,filename);
                    } else
#endif
                        Pixels32Import::loadFileTo(*this,filename);
}

void Pixels32::loadFrom(IInputStream& stream){
#ifdef IS_NEED_CODE_FH
    if (TfhFile::isCanLoadFrom(stream)){  //*.fh
        TfhFile::loadFrom(this,stream);
    } else 
#endif
#ifdef IS_NEED_CODE_FH8
        if (Tfh8File::isCanLoadFrom(stream)){  //*.fh8
            PixelsGray8 tmp;
            Tfh8File::loadFrom(&tmp,stream);
            this->resizeFast(tmp.getWidth(),tmp.getHeight());
            copyFromGray(this->getRef(),0,0,tmp.getRef());
        } else
#endif
#ifdef IS_NEED_CODE_PNG
            if (TPngColor32File::isCanLoadFrom(stream)){  //*.png
                TPngColor32File::loadFrom(this,stream);
            } else 
#endif
#ifdef IS_NEED_CODE_BMP
                if (TBmpColor32File::isCanLoadFrom(stream)){  //*.bmp
                    TBmpColor32File::loadFrom(this,stream);
                } else 
#endif
#ifdef IS_NEED_CODE_JPG
                    if (TJpegColor32File::isCanLoadFrom(stream)){  //*.jpg *.jpeg
                        TJpegColor32File::loadFrom(this,stream);
                    } else
#endif
                        assert(false); //no can load
}

void Pixels32::lockRef(long width,long height,Pixels32Ref& out_Ref){
    this->resizeFast(width,height);
    out_Ref=this->getRef();
}

void PixelsGray8::lockRef(long width,long height,PixelsGray8Ref& out_gray8Ref){
    this->resizeFast(width,height);
    out_gray8Ref=this->getRef();
}


void PixelsGray8::loadFrom(const char* filename){
    const char* afileSuffix=getFileSuffix(filename);
    long   aStrLength=0;
    if (afileSuffix!=0) aStrLength=(long)strlen(afileSuffix);
    
#ifdef IS_NEED_CODE_FH8
    if (isFileSuffixEqual(afileSuffix,aStrLength,".FH8",4)){  //*.fh8
        Tfh8File::loadFrom(this,filename);
    }
    else
#endif
#ifdef IS_NEED_CODE_BMP
        if (isFileSuffixEqual(afileSuffix,aStrLength,".BMP",4)){  //*.bmp
            TBmpGray8File::loadFrom(this,filename);
        }
        else
#endif
#ifdef IS_NEED_CODE_PNG
            if (isFileSuffixEqual(afileSuffix,aStrLength,".PNG",4)){  //*.png
                TPngGray8File::loadFrom(this,filename);
            }
            else
#endif
            {
                Pixels32 tmp;
                tmp.loadFrom(filename);
                this->resizeFast(tmp.getWidth(),tmp.getHeight());
                copyToGray(this->getRef(),0,0,tmp.getRef());
            }
}


void PixelsGray8::loadFrom(IInputStream& stream){
#ifdef IS_NEED_CODE_FH8
    if (Tfh8File::isCanLoadFrom(stream)){  //*.fh8
        Tfh8File::loadFrom(this,stream);
    }
    else
#endif
#ifdef IS_NEED_CODE_BMP
        if (TBmpGray8File::isCanLoadFrom(stream)){  //*.bmp
            TBmpGray8File::loadFrom(this,stream);
        }
        else
#endif
#ifdef IS_NEED_CODE_PNG
            if (TPngGray8File::isCanLoadFrom(stream)){  //*.png
                TPngGray8File::loadFrom(this,stream);
            }
            else
#endif
            {
                Pixels32 tmp;
                tmp.loadFrom(stream);
                this->resizeFast(tmp.getWidth(),tmp.getHeight());
                copyToGray(this->getRef(),0,0,tmp.getRef());
            }
}

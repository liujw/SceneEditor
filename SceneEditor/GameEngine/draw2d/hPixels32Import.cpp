//hPixels32Import.cpp

#include "hPixels32Import.h"
#include "../../import/importInclude/hPixels32ImportBase.h"

Pixels32::TPixelsRefEx Pixels32Import::getNewMemory(const long width,const long height)
{ 
    TPixelsRefEx ref;
    getNewPixels_Import(width,height,(void**)&ref.pdata,&ref.byte_width,&ref.user_data);
    ref.width=width;
    ref.height=height;
    return ref;
}
void Pixels32Import::delMemory(TPixelsRefEx& ref) 
{ 
    if (ref.user_data!=0){
        deletePixels_Import(ref.user_data,ref.pdata);
        ref.setNullRef();
    }
}

inline static void* loadImageFile(const char* fileName,long* out_width,long* out_height){
    long strLength=(long)strlen(fileName);
    String path=getFilePath(fileName, strLength);
    return _loadImageFile(getFileNameNoPath(fileName, strLength),path.c_str(),out_width,out_height);
}

void Pixels32Import::import_loadFrom(const char* filename){
    long width=0; long height=0;
    TImageHandleImport image=loadImageFile(filename,&width,&height);
    this->resizeFast(width,height);
    drawImage(m_data.user_data,image);
    deleteImageHandle(image);
}

void Pixels32Import::loadFileTo(Pixels32& dst,const char* filename){
    Pixels32Import pic;
    pic.import_loadFrom(filename);
    dst.assign(pic);
}
//VCharCanvas.cpp

#include "VCharCanvas.h"	

struct TCharCanvasData:public TCharCanvasDataBase{
protected:
    mutable  int     m_RefCount; //引用计数
public:
    must_inline void incRef()const { ++m_RefCount; }
    must_inline long decRef()const { --m_RefCount; return m_RefCount; }
    must_inline long getRefCount()const{ return m_RefCount; }
protected:
    mutable  VSurfaceGray8 m_pic;
    static void newCharSurface(const VFont& font,int aChar,VSurfaceGray8& out_pic);
    inline void init() const{
        newCharSurface(m_font,m_char,m_pic);
        m_canvas=m_pic.getCanvas();
    }  
public:
    inline TCharCanvasData(const VFont& font,int aChar):TCharCanvasDataBase(font,aChar),m_RefCount(1){ init(); }
    virtual ~TCharCanvasData(){}
};

struct TEqualCharKey{
    const TCharCanvasDataKey* key;
    must_inline TEqualCharKey(const TCharCanvasDataKey* _key):key(_key){ }
    must_inline bool operator==(const TEqualCharKey& charKey1)const{
        return ((key->m_char==charKey1.key->m_char)&&(key->m_font==charKey1.key->m_font));
    }
    must_inline bool operator <(const TEqualCharKey& charKey1)const{
        if  (key->m_char!=charKey1.key->m_char)
            return (key->m_char<charKey1.key->m_char);
        else
            return (key->m_font < charKey1.key->m_font);
    }
};

must_inline static size_t hash_value(const TEqualCharKey& charKey){
    return charKey.key->m_font.hashCode(charKey.key->m_char);
}

#ifdef __GNUC__
namespace __gnu_cxx {
    template<>
    struct hash<TEqualCharKey>
    {
        must_inline	size_t operator()(const TEqualCharKey& charKey) const{
            return hash_value(charKey);
        }
    };
}
#endif

typedef HashSet<TEqualCharKey> TCharBufSet;

static TCharBufSet m_charBufSet; 
/*must_inline void CharCanvasData_debug_out(const char* str,const void* p,long refCount){
 String strData;
 strData.resize(8);
 IntToHex((Int32)p,&strData[0], 8);
 strData=str+String(" 0x")+strData+"  " +intToStr(refCount)+'\n';
 debug_out(strData.c_str());
 }*/
inline const TCharCanvasData* insertNew(const VFont& font,int aChar){
    const TCharCanvasData* result=new TCharCanvasData(font,aChar);
    m_charBufSet.insert(TEqualCharKey(result));
    //CharCanvasData_debug_out("new -- ",result,result->getRefCount());
    return result;
}

const TCharCanvasDataBase* TCharCanvasDataBase::getCharCanvasData(const VFont& font,int aChar){
    TCharCanvasDataKey charKey(font,aChar);
    TCharBufSet::iterator it=m_charBufSet.find(TEqualCharKey((const TCharCanvasDataKey*)&charKey));
    if (it!=m_charBufSet.end())
        return copyCharCanvasData((const TCharCanvasDataBase*)((*it).key));
    else
        return insertNew(font,aChar);
}

const TCharCanvasDataBase* TCharCanvasDataBase::copyCharCanvasData(const TCharCanvasDataBase* src){
    const TCharCanvasData* data=(const TCharCanvasData*)src;
    data->incRef();
    //CharCanvasData_debug_out("inc -- ",data,data->getRefCount());
    return data;
}

void TCharCanvasDataBase::releaseCharCanvasData(const TCharCanvasDataBase* _dst){
    const TCharCanvasData* data=(const TCharCanvasData*)_dst;
    assert(data->getRefCount()>=1);
    assert(m_charBufSet.find(TEqualCharKey(data))!=m_charBufSet.end());
    //CharCanvasData_debug_out("dec -- ",data,data->getRefCount()-1);
    if (data->decRef()==0){ 
        m_charBufSet.erase(TEqualCharKey(data));
        delete data;
    }
}

/*
 const TCharCanvasDataBase* TCharCanvasDataBase::getCharCanvasData(const VFont& font,int aChar){
 return insertNew(font,aChar);
 }
 
 const TCharCanvasDataBase* TCharCanvasDataBase::copyCharCanvasData(const TCharCanvasDataBase* src){
 return insertNew(src->m_font,src->m_char);
 }
 
 void TCharCanvasDataBase::releaseCharCanvasData(const TCharCanvasDataBase* _dst){
 const TCharCanvasData* data=(const TCharCanvasData*)_dst;
 delete data;
 }*/



//////////////////////////////////////
#include "../draw2d/hPixels32Import.h"
#include "../draw2d/hDraw.h"
#include "../../import/importInclude/hPixels32ImportBase.h"

class TPixels32ImportTextOut:public Pixels32Import{
protected:
    virtual Pixels32::TPixelsRefEx getNewMemory(const long width,const long height) { 
        Pixels32::TPixelsRefEx ref=Pixels32Import::getNewMemory(width,height);
        if (ref.pdata!=0){
            ref.reversal();
        }
        return ref;
    }
    virtual void delMemory(TPixelsRefEx& ref)  { 
        if (ref.pdata!=0){
            ref.reversal();
            Pixels32Import::delMemory(ref);
        }
    }
private: 
    VFont m_font;
    void init(){
        resizeFast(mapGamePos(m_font.getSize())*2+30, mapGamePos(m_font.getSize())*2+30); 
        setCurFont();
    }
    inline void setCurFont(){
        ContextHandle_setFont(m_data.user_data,m_font.getName().c_str(),m_font.getSize(),(int)m_font.getIsBold());
    }
    inline void ATextOut(int aChar,long& out_width,long& out_height){
        ContextHandle_GetDrawATextSize(m_data.user_data,aChar,&out_width,&out_height);
#ifdef TEXTOUT_AA
        out_width=(out_width+TEXTOUT_AA-1)/TEXTOUT_AA*TEXTOUT_AA;
        out_height=(out_height+TEXTOUT_AA-1)/TEXTOUT_AA*TEXTOUT_AA;
#endif
        if ((this->getWidth()<out_width)||(this->getHeight()<out_height)){
            this->resizeFast(out_width, out_height);
            setCurFont();
        }
        ::fill(m_data.getSubRef(0,0,out_width,out_height),cl32Empty);
        ContextHandle_DrawAText(m_data.user_data,aChar);
        
    }
public:
    inline TPixels32ImportTextOut(){ init(); }
    virtual ~TPixels32ImportTextOut(){ delMemory(m_data); }
    inline void TextOut(const VFont& font,int aChar,long& out_width,long& out_height){
        if (m_font!=font){
            m_font=font;
            setCurFont();
        }
        ATextOut(aChar,out_width,out_height);
    }
};




void TCharCanvasData::newCharSurface(const VFont& font,int aChar,VSurfaceGray8&	out_pic){
    static TPixels32ImportTextOut pixels32ImportTextOut;
    long out_width=0;
    long out_height=0;
#ifdef TEXTOUT_AA
    VFont fontAA(font);
    fontAA.setSize(font.getSize()*TEXTOUT_AA);
    pixels32ImportTextOut.TextOut(fontAA,aChar,out_width,out_height);
    
    VSurfaceGray8 mip_pic;
    mip_pic.resizeFast(out_width,out_height);
    mip_pic.getCanvas().copyAsTextPixels(0,0,pixels32ImportTextOut.getRef());
    
    out_pic.resizeFast((out_width+TEXTOUT_AA-1)/TEXTOUT_AA,(out_height+TEXTOUT_AA-1)/TEXTOUT_AA);
    mipZoomBilinearGray8(out_pic.getCanvas().getRef(),mip_pic.getCanvas().getRef());
    //zoomBilinearAsCopy(out_pic.getCanvas().getRef(),mip_pic.getCanvas().getRef());
#else
    pixels32ImportTextOut.TextOut(font,aChar,out_width,out_height);
    out_pic.resizeFast(out_width,out_height);
    Pixels32Ref ref=pixels32ImportTextOut.getRef();
#ifdef _MACOSX
    ref.width=out_width;
    ref.height=out_height;
    ref.reversal();
#endif
    out_pic.getCanvas().copyAsTextPixels(0,0,ref);
#endif
}

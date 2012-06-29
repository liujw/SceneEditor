//hPixels.h


#ifndef _hPixels_h_
#define _hPixels_h_

#include "hColor32.h"
#include "hDraw.h"

template<class TPixelsRef>
class TMemPixels
{
protected:
    struct TPixelsRefEx:public TPixelsRef
    {
        void * user_data;
        TPixelsRefEx():TPixelsRef(),user_data(0){}
        void setNullRef(){
            TPixelsRef::setNullRef();
            user_data=0;
        }
    };
    TPixelsRefEx m_data;
public: 
    typedef typename TPixelsRef::TColor TColor; 
    typedef TPixelsRefEx TPixelsRefType;
protected:
    virtual TPixelsRefEx  getNewMemory(const long width,const long height){
        TPixelsRefEx result;
        if ((width>0)&&(height>0))
        {
            long byte_width=width*sizeof(TColor);
            result.pdata=(TColor*)allocPixelsMemery(byte_width*height);   
            result.byte_width=byte_width;
            result.width=width;
            result.height=height;
        }
        return result;
    }
    virtual void delMemory(TPixelsRefEx& ref){
        UInt8* pMemData=((UInt8*)ref.pdata);
        ref.setNullRef();
        if (pMemData!=0){
            freePixelsMemery(pMemData);
        }
    }
    inline void setNewSize(int width, int height) {
        delMemory(m_data);
        m_data=getNewMemory(width, height);
    }
public:
    template<class T>
    TMemPixels& operator=(const T&);//error
public:
    inline explicit TMemPixels():m_data() { }
    inline explicit TMemPixels(const long width,const long height,const TColor& fillColor=TColorAttribute<TColor>::getEmptyColor()):m_data()
    { resizeAndClear(width,height,fillColor); }
    virtual  ~TMemPixels() { delMemory(m_data); }
    void resize(const long width,const long height,const TColor& fillColor=TColorAttribute<TColor>::getEmptyColor()){
        if ((width > 0) && (height > 0)) {
            if ((width != m_data.width) || (height != m_data.height)) {
                TPixelsRefEx oldRef = m_data;
                m_data=getNewMemory(width, height);
                copy(m_data,0,0,oldRef);
                //fill border
                if (m_data.height>oldRef.height)
                    fill(m_data.getSubRef(0,oldRef.height,m_data.width,m_data.height),fillColor);
                if (m_data.width>oldRef.width)
                    fill(m_data.getSubRef(oldRef.width,0,m_data.width,oldRef.height),fillColor);
                delMemory(oldRef);
            } 
        } else
            delMemory(m_data);
    }
    
    void resizeAndClear(const long width,const long height,const TColor& fillColor=TColorAttribute<TColor>::getEmptyColor()){
        resizeFast(width,height);
        fill(m_data,fillColor);
    }
    
    void resizeFast(const long width,const long height){
        if ((width > 0) && (height > 0)) {
            if ((width != m_data.width) || (height != m_data.height))
                setNewSize(width,height);
        } else
            delMemory(m_data);
    }
    
    void assign(const TPixelsRef& src){  resizeFast(src.width,src.height);  copy(m_data,0,0,src); }
    inline void assign(const TMemPixels& src) { assign(src.getRef()); }
    inline void swap(TMemPixels& pixels) { STD::swap(m_data,pixels.m_data); }
    inline long getWidth() const { return m_data.width; }
    inline long getHeight() const { return m_data.height; }
    inline const TPixelsRef& getRef() const { return m_data; }
    inline bool getIsEmpty()const{ return (m_data.width<=0)||(m_data.height<=0); }
};


#endif //hPixels_h_

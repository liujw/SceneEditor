//Rect.h


#ifndef _Rect_h_
#define _Rect_h_

#include "SysImportType.h"
struct TPoint{
    long x;
    long y;
    must_inline TPoint(){}
    must_inline TPoint(long _x,long _y):x(_x),y(_y){}
    must_inline TPoint(const TPoint& _pt):x(_pt.x),y(_pt.y){}
    must_inline bool operator ==(const TPoint& pt)const{
        return (x==pt.x)&&(y==pt.y);
    }
    must_inline bool operator !=(const TPoint& pt)const{
        return !((*this)==pt);
    }
};

struct TRect{
public:
    //union{
    //	struct{
    //		TPoint p0;
    //		TPoint p1;			
    //	};
    //	struct{
    long x0;
    long y0;
    long x1;
    long y1;			
    //	};
    //};
public:
    must_inline TRect(){}
    must_inline TRect(long _x0,long _y0,long _x1,long _y1):x0(_x0),y0(_y0),x1(_x1),y1(_y1){	}
    must_inline TRect(const TRect& src){ *this=src; }
    must_inline long getWidth() const { return x1-x0; }
    must_inline long getHeight() const { return y1-y0; }
    must_inline bool getIsHit(long pointX0, long pointY0) const{
        return ((pointX0>=x0)&&(pointX0<x1)&&(pointY0>=y0)&&(pointY0<y1));
    }
    must_inline bool getIsHit(const TRect& r) const{ //是否相交
        return (STD::max(x0,r.x0)<STD::min(x1,r.x1))&&(STD::max(y0,r.y0)<STD::min(y1,r.y1));
    }
    must_inline bool getIsCover(const TRect& r) const{//是否覆盖r
        return (x0<=r.x0)&&(x1>=r.x1)&&(y0<=r.y0)&&(y1>=r.y1);
    }
    must_inline bool getIsEmpty()const { return ((x0>=x1)||(y0>=y1)); }
    must_inline void setEmpty() { x0=0; y0=0; x1=0; y1=0; }
    must_inline void setEmpty(long _x0,long _y0) { x0=_x0; y0=_y0; x1=_x0; y1=_y0; }
    must_inline void setRect(const TRect& src){ *this=src; }
    must_inline void setRect(long _x0,long _y0,long _x1,long _y1){  x0=_x0; y0=_y0; x1=_x1; y1=_y1;  }
    must_inline void max(const TRect& src){  //并集
        max(src.x0,src.y0,src.x1,src.y1);	
    }  
    must_inline void max(long _x0,long _y0,long _x1,long _y1){  //并集
        if (_x0<x0) x0=_x0;
        if (_y0<y0) y0=_y0;
        if (_x1>x1) x1=_x1;
        if (_y1>y1) y1=_y1;
    } 
    must_inline void min(const TRect& src){  //交集
        min(src.x0,src.y0,src.x1,src.y1);
    }  
    must_inline void min(long _x0,long _y0,long _x1,long _y1){  //交集
        long ox0=x0;
        long oy0=y0;
        if (_x0>x0) x0=_x0;
        if (_y0>y0) y0=_y0;
        if (_x1<x1) x1=_x1;
        if (_y1<y1) y1=_y1;
        if (getIsEmpty()){
            setEmpty(ox0,oy0);
        } 
    }
    must_inline void max(long _x0,long _y0){  //并集
        max(_x0,_y0,_x0+1,_y0+1);
    } 
    must_inline void min(long _x0,long _y0){  //并集
        min(_x0,_y0,_x0+1,_y0+1);
    } 
    friend must_inline TRect max(const TRect& r0,const TRect& r1){
        TRect r(r0);
        r.max(r1);
        return r;
    }
    friend must_inline TRect min(const TRect& r0,const TRect& r1){
        TRect r(r0);
        r.min(r1);
        return r;
    }
    must_inline bool operator ==(const TRect& rect)const{
        return (x0==rect.x0)&&(y0==rect.y0)&&(x1==rect.x1)&&(y1==rect.y1);
    }
    must_inline bool operator !=(const TRect& rect)const{
        return !((*this)==rect);
    }
    must_inline long getArea()const{
        if (getIsEmpty())
            return 0;
        else 
            return getAreaFast();
    }
    must_inline long getAreaFast()const{
        return (x1-x0)*(y1-y0);
    }
    must_inline void move(long movex,long movey){
        x0+=movex; x1+=movex;
        y0+=movey; y1+=movey;
    }
};

typedef Vector<TRect> TRectArray;


//多个矩形区域合并成一个最小的集合矩形
class VRectCalcBox{
    TRect m_rectClip;
    TRect m_rectBox;
public:
    VRectCalcBox() {}
    
    void init(const TRect& rectClip){
        m_rectBox.setEmpty(0,0);
        m_rectClip=rectClip;
    }
    void addRect(const TRect& rect){
        m_rectBox.max(rect);
    }
    void flush(){
        m_rectBox.min(m_rectClip);
    }
    
    long getRectCount()const{
        if (m_rectBox.getIsEmpty())
            return 0;
        else
            return 1;
    }
    void getRectBox(long index,TRect& out_rectBox){
        out_rectBox=m_rectBox;
    }
};


//合并出最佳的不重叠的脏矩形列表
class VRectCalc{
    long  m_maxRectCount;
    TRect m_rectClip;
    TRectArray m_rectList;
public:
    VRectCalc() {  }
    void init(const TRect& rectClip,long maxRectCount){
        if (maxRectCount<1) maxRectCount=1;
        m_rectList.clear();
        m_rectList.reserve((maxRectCount+4)*2);
        m_rectClip=rectClip;
        m_maxRectCount=maxRectCount;
    }
    void addRect(const TRect& rect){
        TRect r(rect);
        r.min(m_rectClip);
        if (!r.getIsEmpty())
            m_rectList.push_back(r);
    }
    void flush();
    
    TRectArray& getRectList(){
        return m_rectList;
    }
};
//*/


#endif //_Rect_h_

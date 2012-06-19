//Rect.cpp

#include "Rect.h"
//using namespace MyBase;


struct TRectAreaCompare{
    must_inline bool operator()(const TRect& a,const TRect& b)const{
        return a.getAreaFast()>b.getAreaFast();
    }
};

struct TRectXYOrderCompare{
    must_inline bool operator()(const TRect& a,const TRect& b)const{
        if (a.x0==b.x0)
            return a.y0<b.y0;
        else
            return a.x0<b.x0;
    }
};

struct TRectYXOrderCompare{
    must_inline bool operator()(const TRect& a,const TRect& b)const{
        if (a.y0==b.y0)
            return a.x0<b.x0;
        else
            return a.y0<b.y0;
    }
};


struct TY;
struct TX;

const long max_value=(((Int64)1)<<31)-2;
const long nil_value=max_value+1;

//Y方向的链表结点 并包含一条X链表
struct TY{
    long y0;
    long y1;
    TY*  nextY;
    TX*  xLine;
};

//X方向的链表结点
struct TX{
    long x0;
    long x1;
    TX* nextX;
    long meldY0; //用于最后的合并
    must_inline long getY0(TY* Y)const{
        if (meldY0==nil_value)
            return Y->y0;
        else
            return meldY0;
    }
    must_inline long getY0(long Y_y0)const{
        if (meldY0==nil_value)
            return Y_y0;
        else
            return meldY0;
    }
};

//负责内存分配
class TXYMem{
private:
    enum { csMemNodeSize=1024*32 };
    Vector<UInt8*> m_usedDatas;
    UInt8* m_dataBegin;
    UInt8* m_dataEnd;
    must_inline UInt8* getMem(long memSize){
        if (m_dataBegin+memSize>m_dataEnd){
            m_dataBegin=new UInt8[csMemNodeSize];
            m_dataEnd=&m_dataBegin[csMemNodeSize];
            m_usedDatas.push_back(m_dataBegin);
        }
        UInt8* result=m_dataBegin;
        m_dataBegin+=memSize;
        return result;		
    }
public:
    TXYMem():m_dataBegin(0),m_dataEnd(0){ }
    ~TXYMem(){ releaseAllMem(); }
    
    must_inline TX* newTX(){
        return (TX*)getMem(sizeof(TX));
    }
    must_inline TY* newTY(){
        return (TY*)getMem(sizeof(TY));
    }
    
    void releaseAllMem(){
        m_dataBegin=0;
        m_dataEnd=0;
        long size=(long)m_usedDatas.size();
        for (long i=0;i<size;++i)
            delete [] m_usedDatas[i];
        m_usedDatas.clear();
    }
};

class TYX{
private:
    TY*     m_head;
    TXYMem  m_mem;
private:
    must_inline TX* getNewX(long x0,long x1){
        TX* pX=m_mem.newTX();
        pX->x0=x0;
        pX->x1=x1;
        pX->nextX=0;
        pX->meldY0=nil_value;
        return pX;
    }
    must_inline void insertXAtFront(TX** ppX,long x0,long x1){
        TX* pNewX=getNewX(x0,x1);
        pNewX->nextX=*ppX;
        *ppX=pNewX;
    }
    must_inline void insertXAtBack(TX* pX,long x0,long x1){
        TX* pNewX=getNewX(x0,x1);
        pX->nextX=pNewX;
    }
    must_inline void insertXNext(TX* pX,long x0,long x1){
        if (pX->nextX==0)
            insertXAtBack(pX,x0,x1);
        else
            insertX(&pX->nextX,x0,x1);		
    }
    
    void insertX(TX** ppX,long x0,long x1){
        TX* pX=*ppX;
        if (x1 < pX->x0){ //之前
            insertXAtFront(ppX,x0,x1);
        }else if (x0 > pX->x1){ //之后
            insertXNext(pX,x0,x1);
        }else{ //相交
            if (x0 < pX->x0)
                pX->x0=x0;
            if (x1 > pX->x1){
                TX* pCvrX=pX;
                while (pCvrX->nextX!=0){
                    TX* pNX=pCvrX->nextX;
                    if (x1< pNX->x0) break;
                    pCvrX=pNX;
                }
                if (x1 >= pCvrX->x1)
                    pX->x1=x1;
                else
                    pX->x1=pCvrX->x1;
                pX->nextX=pCvrX->nextX; //可能会丢弃很多节点
            }
        }
        //
    }
    must_inline void YAddAX(TY* pY,long x0,long x1){
        insertX(&pY->xLine,x0,x1);
    }
    
private:
    
    must_inline TY* getNewY(long y0,long y1){
        TY* pY=m_mem.newTY();
        pY->y0=y0;
        pY->y1=y1;
        pY->nextY=0;
        pY->xLine=0;
        return pY;
    }
    must_inline TY* getNewY(const TRect& rect){
        TY* pY=getNewY(rect.y0,rect.y1);
        pY->xLine=getNewX(rect.x0,rect.x1);
        return pY;
    }
    inline void clipY(TY* oY,long y){
        TY* pY=getNewY(y,oY->y1);
        pY->nextY=oY->nextY;
        
        oY->nextY=pY;
        oY->y1=y;
        
        TX*  oX=oY->xLine;
        TX** ppX=&pY->xLine;
        while (oX!=0){
            TX* pNewX=getNewX(oX->x0,oX->x1);
            *ppX=pNewX;
            
            ppX=&pNewX->nextX;
            oX=oX->nextX;
        }
    }
    
    
    must_inline void insertAtFront(TY** ppY,const TRect& rect){
        TY* pNewY=getNewY(rect);
        pNewY->nextY=*ppY;
        *ppY=pNewY;
    }
    must_inline void insertAtBack(TY* pY,const TRect& rect){
        TY* pNewY=getNewY(rect);
        pY->nextY=pNewY;
    }
    must_inline void insertRectNext(TY* pY,const TRect& rect){
        if (pY->nextY==0)
            insertAtBack(pY,rect);
        else
            insertRect(&pY->nextY,rect);		
    }
    static must_inline bool getIsCover(TY* pY,const TRect& r){//是否了覆盖r
        if ((pY->y0>r.y0)||(pY->y1<r.y1)) return false;
        TX* pX=pY->xLine;
        while (true){
            if ((pX->x0<=r.x0)&&(pX->x1>=r.x1))
                return true;
            else{
                pX=pX->nextX;
                if (pX==0) return false;
            }
        }
    }
    
    void insertRect(TY** ppY,const TRect& rect){
        TY* pY=*ppY;
        if (rect.y1 <= pY->y0){  //之前
            insertAtFront(ppY,rect);
        }else if (rect.y0 >= pY->y1){ //之后
            insertRectNext(pY,rect);
        }else if (getIsCover(pY,rect)){ //包含
            return;
        }else{ //相交
            TRect nRect(rect);
            if (nRect.y1 > pY->y1){ //长出一截的部分交给下一个Y
                TRect backRect(nRect.x0,pY->y1,nRect.x1,nRect.y1);
                nRect.y1=pY->y1;
                insertRectNext(pY,backRect);
            }else if (nRect.y1< pY->y1){ //短一截,分裂Y
                clipY(pY,nRect.y1);
            }
            //nRect.y1==pY->y1
            
            if (nRect.y0 < pY->y0){ 
                TRect frontRect(nRect.x0,nRect.y0,nRect.x1,pY->y0);
                nRect.y0=pY->y0;
                YAddAX(pY,nRect.x0,nRect.x1);				
                insertAtFront(ppY,frontRect);
            }else if (nRect.y0< pY->y0){
                clipY(pY,nRect.y0);
                YAddAX(pY->nextY,nRect.x0,nRect.x1);
            }else{ //nRect.y0== pY->y0			
                YAddAX(pY,nRect.x0,nRect.x1);
            }
        }
    }
    
    inline static bool getIsEqualXLine(TX* pX,TX* pNX){
        while (true){
            if ( (pX->x0 != pNX->x0) || (pX->x1 != pNX->x1) )
                return false;
            pX=pX->nextX;
            pNX=pNX->nextX;
            if (pX==0)
                return (pNX==0);
            else{
                if (pNX==0)
                    return false;
            }
        }
    }
    
    inline void meldYLine(long yBegin,long yEnd){
        //尝试合并一些Y; 满足这样的条件:相邻,并且xLine一样
        if (m_head==0) return;
        TY* pY=m_head;
        while (pY->nextY!=0){
            if (pY->y1<yBegin){
                pY=pY->nextY;
                continue;
            }
            else if (pY->y0>yEnd)
                break;
            
            TY* pNY=pY->nextY;
            if ( (pY->y1==pNY->y0)&&(getIsEqualXLine(pY->xLine,pNY->xLine)) ){
                pY->y1=pNY->y1;
                pY->nextY=pNY->nextY;
            }else
                pY=pNY;			
        }
    }
    
    inline static void meldXLine(TY* meldY,TY* pNY){
        //合并相邻的矩形 满足 (a.x0==b.x0) && (a.x1==b.x1)  
        TX** ppX=&meldY->xLine;
        TX* pNX=pNY->xLine;
        while (true){
            TX* pX=*ppX;
            if ((pX==0)||(pNX==0)) return;
            
            if (pX->x0 < pNX->x0){
                ppX=&pX->nextX;
            }else if (pX->x0 > pNX->x0){
                pNX=pNX->nextX;
            }else{ // pX->x0 == pNX->x0
                if ((pX->x1 != pNX->x1)||(pX->getY0(meldY)!=pNX->getY0(pNY))){
                    ppX=&pX->nextX;
                    pNX=pNX->nextX;					
                }else{//合并矩形
                    pNX->meldY0=pX->getY0(meldY);
                    pNX=pNX->nextX;
                    
                    //del one X
                    *ppX=pX->nextX;
                    ppX=&pX->nextX;
                }				
            }
        }
    }
    
    void tryMeldRect(){
        //合并相邻的矩形 满足:Y相邻并且xLine满足(Ya.x0==Yb.x0) && (Ya.x1==Yb.x1)  
        if(m_head==0) return;
        
        TY** ppY=&m_head;
        while ((*ppY)->nextY!=0){
            TY* pY=*ppY;
            TY* pNY=pY->nextY;
            if (pY->y1 == pNY->y0){
                meldXLine(pY,pNY); 
                if (pY->xLine==0){
                    *ppY=pNY;//del one Y
                }
            }
            ppY=&pY->nextY;
        }
    }
    
    void faceMeldARect(){
        TX* minX=0;
        TY* minX_Y=0;
        long minRect=max_value;//width*height 
        TRect r0,r1;
        
        //x方向  查找最小的多出面积
        for (TY* Y=m_head;Y!=0;Y=Y->nextY){
            long y0=Y->y0; 
            long y1=Y->y1; 
            r0.y1=y1;
            r1.y1=y1;
            for (TX* X=Y->xLine;(X!=0)&&(X->nextX!=0);X=X->nextX){
                TX* NX=X->nextX;
                if ((y1-y0)*(NX->x0 - X->x1)>=minRect) continue; //不可能获得更小的代价了
                
                r0.x0=X->x0;
                r0.x1=X->x1;
                r0.y0=X->getY0(y0);
                r1.x0=NX->x0;
                r1.x1=NX->x1;
                r1.y0=NX->getY0(y0);
                
                long subRect=max(r0,r1).getAreaFast()-r0.getAreaFast()-r1.getAreaFast()+min(r0,r1).getAreaFast();
                if (subRect<minRect){
                    minRect=subRect;
                    minX=X;
                    minX_Y=Y;
                }
            }	
        }
        
        //Y方向
        TY* minY=0;
        TX* minY_X=0;
        TX* minNX=0;
        for (TY* Y=m_head;(Y!=0)&&(Y->nextY!=0);Y=Y->nextY){
            TY* NY=Y->nextY;
            r0.y1=Y->y1;
            r1.y1=NY->y1;
            for (TX* NX=NY->xLine;(NX!=0);NX=NX->nextX){
                long nxY0=NX->getY0(NY);
                if ((nxY0 - Y->y1)*(NX->x1 - NX->x0)>=minRect) continue; //不可能获得更小的代价了
                
                r0.x0=NX->x0;
                r0.x1=NX->x1;
                r0.y0=NX->getY0(NY);
                //警告:算法复杂度有点高
                for (TX* X=Y->xLine;(X!=0);X=X->nextX){
                    r1.x0=X->x0;
                    r1.x1=X->x1;
                    r1.y0=X->getY0(Y);
                    long subRect=max(r0,r1).getAreaFast()-r0.getAreaFast()-r1.getAreaFast()+min(r0,r1).getAreaFast();
                    subRect=subRect*5/4; //增大这种合并方式的代价
                    if (subRect<minRect){
                        minRect=subRect;
                        minNX=NX;
                        minY=Y;
                        minY_X=X;
                    }					
                }	
            }	
        }
        
        if (minY!=0){						
            minNX->x0=STD::min(minNX->x0,minY_X->x0);
            minNX->x1=STD::max(minNX->x1,minY_X->x1);
            minNX->meldY0=STD::min(minNX->getY0(minY->nextY),minY_X->getY0(minY));
            
            for (TX** pX=&minY->xLine;((*pX)!=0);pX=&(*pX)->nextX){
                TX* X=(*pX);
                if (X==minY_X){
                    *pX=X->nextX;  //del A X node 
                    if (minY->xLine==0){
                        for (TY** pY=&m_head;(*pY)!=0;pY=&(*pY)->nextY){
                            TY* Y=(*pY); 
                            if (Y==minY){
                                *pY=Y->nextY;  //del A Y node 
                                break;
                            }
                        }
                    }
                    break;
                }
            }	
        }else if (minX!=0){
            TY* Y=minX_Y;
            for (TX* X=Y->xLine;X!=0;X=X->nextX){
                if (X==minX){
                    TX* NX=X->nextX;
                    X->x0=STD::min(X->x0,NX->x0);
                    X->x1=STD::max(X->x1,NX->x1);
                    
                    long xY0=STD::min(X->getY0(Y),NX->getY0(Y));
                    if (xY0 < Y->y0)
                        X->meldY0=xY0;
                    
                    X->nextX=NX->nextX; //del A X node 
                    return;
                }
            }	
        }		
    }
    
    void faceMeldRect(long maxRectCount){
        //矩形还是太多的情况下,增大面积(代价最小)来合并
        long rectCount=0;
        for (TY* Y=m_head;Y!=0;Y=Y->nextY){
            for (TX* X=Y->xLine;X!=0;X=X->nextX){
                ++rectCount;
            }
        }
        while (rectCount>maxRectCount){
            faceMeldARect();
            --rectCount;
        }		
    }
public:
    TYX():m_head(0){}
    must_inline void addRect(const TRect& rect){	
        if (m_head==0){
            m_head=getNewY(rect);
            return;			
        }else{		
            insertRect(&m_head,rect);
            meldYLine(rect.y0,rect.y1);
        }
    }
    
    void getRectList(Vector<TRect>& out_rectList,long maxRectCount){
        tryMeldRect();
        //faceMeldRect(maxRectCount);
        
        out_rectList.clear();
        TRect rect; 
        for (TY* Y=m_head;Y!=0;Y=Y->nextY){
            rect.y1=Y->y1;
            long y0=Y->y0;
            for (TX* X=Y->xLine;X!=0;X=X->nextX){
                rect.x0=X->x0;
                rect.x1=X->x1;
                rect.y0=X->getY0(y0);
                out_rectList.push_back(rect);
            }
        }
    }
};

void VRectCalc::flush(){
    long old_size=(long)m_rectList.size();
    if (old_size==0)
        return;
    else if (m_maxRectCount<=1){
        for (long i=1;i<old_size;++i){
            m_rectList[0].max(m_rectList[i]);
        }
        m_rectList.resize(1);
        m_rectList[0].min(m_rectClip);
        return;
    }
    
    //删除区域外的和空的矩形
    long size=(long)m_rectList.size();
    long insertIndex=0;
    for (long i=0;i<size;++i){
        if (m_rectClip.getIsHit(m_rectList[i])){
            m_rectList[insertIndex]=m_rectList[i];
            m_rectList[insertIndex].min(m_rectClip);
            ++insertIndex;
        }
    }
    m_rectList.resize(insertIndex);
    size=insertIndex;
    
    //按面积排序 从大到小
    STD::sort(m_rectList.begin(),m_rectList.end(),TRectAreaCompare());
    
    
    TYX xy;
    for (long i=0;i<size;++i)
        xy.addRect(m_rectList[i]);
    xy.getRectList(m_rectList,m_maxRectCount);
    
    //如果改变了顺序： 按位置排序 从上到下 从左到右
    //assert(MyBase::is_sorted(m_rectList.begin(),m_rectList.end(),TRectYXOrderCompare()));
    //STD::sort(m_rectList.begin(),m_rectList.end(),TRectYXOrderCompare());
    
    
    /* 检查是否有重叠情况
     for (long i=0;i<m_rectList.size();++i){
     for (long j=0;j<m_rectList.size();++j){
     if (i!=j)
     assert(!m_rectList[i].getIsHit(m_rectList[j]));
     }
     }//*/
}

//VSprite.cpp

#include "VSprite.h"
#include "VSpriteEngine.h"

//#define _DEBUG_SPRITE_POS

#ifdef _DEBUG_SPRITE_POS
#include "../base/Rand.h"
static VRand debug_rand;
#endif

void ISprite::draw(const VCanvas& dst,long x0,long y0){
    doDraw(dst,x0,y0);
    //is for debug
#ifdef _DEBUG_SPRITE_POS
    VSprite* self=(VSprite*)this;
    dst.rect(x0, y0, x0+self->getWidth(), y0+self->getHeight(), 
             Color32(debug_rand.next()%256,debug_rand.next()%256,debug_rand.next()%256));
#endif
}
//out_rect.setRect(x0,y0,x0+getWidth(),y0+getHeight());

struct TSpriteSortCompare{
    must_inline bool operator () (const ISprite* a,const ISprite* b){
        return ((const VSprite*)a)->getZ() < ((const VSprite*)b)->getZ();
    }
};

void VSpriteList::sortSpriteList(){ 
    if (!MyBase::is_sorted(m_list.begin(),m_list.end(),TSpriteSortCompare()))
        STD::stable_sort(m_list.begin(),m_list.end(),TSpriteSortCompare()); 
}

long VSpriteList::findASprite(ISprite* sprite)const{
    long spriteCount=(long)m_list.size();
    for (long i=0;i<spriteCount;++i){
        if (m_list[i]==sprite)
            return i;
    }
    return -1;
}
void VSpriteList::clear(){ 
    TList list;
    m_list.swap(list);
    for (long i=list.size()-1;i>=0;--i)
        freeSprite(list[i]);
}

void VSpriteList::delKilledSprite(){
    long spriteCount=(long)m_list.size(); 
    long delCount=0;
    for (long i=0;i<spriteCount;++i){
        ISprite*& sprite=m_list[i];
        if ( (sprite==0)||(((VSprite*)sprite)->getIsKilled()) ){
            freeSprite(sprite);
            ++delCount;
        }
    }
    if (delCount>0){
        long insertIndex=0;
        for (long i=0;i<spriteCount;++i){
            ISprite* sprite=m_list[i];
            if (sprite!=0){
                m_list[insertIndex]=sprite;
                ++insertIndex;
            }
        }
        m_list.resize(insertIndex);
    }
}


void VSpriteList::outASprite(ISprite* sprite){
    long index=findASprite(sprite);
    if (index>=0){
        m_list[index]=0;
        delKilledSprite();
    }
}


void VSprite::drawWidthChild(const VCanvas& dst,long x0,long y0){
    VSpriteEngine::drawSpriteWithChilds(dst,x0,y0,this,true);
}


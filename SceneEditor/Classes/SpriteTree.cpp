#include "SpriteTree.h"

void VChildSpriteList::clear()
{
	long spriteCount=(long)m_list.size();
    for (long i=0;i<spriteCount;++i){
       freeSprite(m_list[i]);
    }
	m_list.clear();
}

ISpriteTreeBase* VChildSpriteList::getSprite(const String& name) 
{
	for (long i = m_list.size() - 1; i >= 0; --i)
	{
		if (m_list[i]->getName() == name)
		{
			return m_list[i];
		}
	}
	return 0;
}

long VChildSpriteList::findASprite(ISpriteTreeBase* sprite)const{
    long spriteCount=(long)m_list.size();
    for (long i=0;i<spriteCount;++i){
        if (m_list[i]==sprite)
            return i;
    }
    return -1;
}

void VChildSpriteList::outASprite(ISpriteTreeBase* sprite)
{
	long index = findASprite(sprite);
	if (index>=0){
    	m_list[index] = 0;
		delKilledSprite();
    }
}

void VChildSpriteList::delKilledSprite(){
    long spriteCount=(long)m_list.size(); 
    long delCount=0;
    for (long i=0;i<spriteCount;++i){
        ISpriteTreeBase*& sprite=m_list[i];
        if (sprite == 0){
            //freeSprite(sprite);
            ++delCount;
        }
    }
    if (delCount>0){
        long insertIndex=0;
        for (long i=0;i<spriteCount;++i){
            ISpriteTreeBase* sprite=m_list[i];
            if (sprite!=0){
                m_list[insertIndex]=sprite;
                ++insertIndex;
            }
        }
        m_list.resize(insertIndex);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

static void setPosLeftValue(TSpriteTree* pSpriteNode,long nValue)
{
	VSprite* pSprite = pSpriteNode->getSprite();
	if(!pSprite)
		return;

	TSpriteTree* pParent = pSpriteNode->getParent();
	if(!pParent)
	{
		pSpriteNode->setLeft(nValue);
		pSprite->setLeft(nValue);
		return;
	}

	long nLeft = 0;
	switch(pSpriteNode->getAlignX())
	{
		case alignX_left:
			nLeft = 0;//pParent->getLeft();
			break;
		case alignX_center:
			nLeft = (pParent->getSprite()->getWidth() - pSprite->getWidth())/ 2;
			break;
		case alignX_right:
			nLeft = pParent->getSprite()->getWidth()-pSprite->getWidth();
			break;
		default:
			nLeft = nValue;
			break;		
	}

	pSpriteNode->setLeft(nLeft);
	pSprite->setLeft(nLeft);
}
   
static void setPosTopValue(TSpriteTree* pSpriteNode,long nValue)
{
	VSprite* pSprite = pSpriteNode->getSprite();
	if(!pSprite)
		return;

	VSprite* pParentSprite = pSpriteNode->getParent()->getSprite();
	if(!pParentSprite)
	{
		pSpriteNode->setTop(nValue);
		pSprite->setTop(nValue);
		return;
	}

	long nTop = 0;
	switch(pSpriteNode->getAlignY())
	{
		case alignY_top:
			nTop = 0;
			break;
		case alignY_center:
			nTop = (pParentSprite->getHeight() - pSprite->getHeight()) / 2;
			break;
		case alignY_bottom:
			nTop = pParentSprite->getHeight() - pSprite->getHeight();
			break;
		default:
			nTop = nValue;
			break;
	}
	pSpriteNode->setTop(nTop);
	pSprite->setTop(nTop);
}

void TSpriteTree::setSpriteOffset(long nOffsetLeft,long nOffsetTop)
{
	long nLeft = getLeft() + nOffsetLeft;
	long nTop = getTop() + nOffsetTop;
   
	if(nOffsetLeft != 0)
		setAlignX(-1);

	if(nOffsetTop != 0)
		setAlignY(-1);

	setSpritePos(nLeft,nTop);
}

void TSpriteTree::setSpritePos(long nLeft,long nTop) 
{
	setPosLeftValue(this,nLeft);
	setPosTopValue(this,nTop);
}

void TSpriteTree::setSpriteLeft(long nLeft)
{
	setPosLeftValue(this,nLeft);
}
void TSpriteTree::setSpriteTop(long nTop)
{
	setPosTopValue(this,nTop);
}

void TSpriteTree::setSpriteAlignX(long align) {
	long nLeft = getLeft();

	_alignX = (TAlignX)align;

	setPosLeftValue(this,nLeft);
}

void TSpriteTree::setSpriteAlignY(long align) {
	long nTop = getTop();

	_alignY = (TAlignY)align;

	setPosTopValue(this,nTop);
}

void TSpriteTree::setParent(TSpriteTree* pParent)
{
	m_parent = pParent;
	setParentName(pParent->getName());

	//坐标和对齐方式
	setSpriteLeft(0);
	setSpriteTop(0);
	setSpriteAlignX(-1);
	setSpriteAlignY(-1);

	VSprite* pParentSprite = m_parent->getSprite();
	VSprite* pSprite = this->getSprite();
	if(pParentSprite && pSprite)
	{
		pParentSprite->addASprite(pSprite);
	}
}

void TSpriteTree::setSpriteZ(long z)
{
	setZ(z);
	VSprite* pSprite = this->getSprite();
	if(pSprite)
	{
		pSprite->setZ(z);
	}
}
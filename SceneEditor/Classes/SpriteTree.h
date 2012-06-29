#pragma once

#include "sysImport.h"
#include "VSprite.h"

//¶ÔÆë·½Ê½
enum TAlignX
{
	alignX_none = -1,
	alignX_left=0,
	alignX_center,
	alignX_right
};
enum TAlignY
{
	alignY_none = -1,
	alignY_top=0,
	alignY_center,
	alignY_bottom
};

typedef String SpriteClass;
class ISpriteTreeBase
{
public:
	explicit ISpriteTreeBase(){}
protected:
	String _name;
	String _path;
	String _parentName;
	String _posName;
	String _type;
	long _left;
	long _top;
	long _z;
	long _width;
	long _height;
	long _col;
	long _row;
	TAlignX _alignX;
	TAlignY _alignY;
public:
	inline String getName() {
		return _name;
	}
	inline void setName(const String& name){
		_name = name;
	}
	inline String getPosName() {
		return _posName;
	}
	inline void setPosName(const String& posName) {
		_posName = posName;
	}
	inline String getPath() {
		return _path;
	}
	inline void setPath(const String& path) {
		_path = path;
	}
	inline String getParentName(){
		return _parentName;
	}
	inline void setParentName(const String& parentName){
		_parentName = parentName;
	}
	inline void setLeft(long left){
		_left = left;
	}
	inline void setTop(long top){
		_top = top;
	}
	inline long getLeft() {
		return _left;
	}
	inline long getTop() {
		return _top;
	}
	inline long getZ() {
		return _z;
	}
	inline void setZ(long z) {
		_z = z;
	}
	inline long getCol() {
		return _col;
	}
	inline void setCol(long col) {
		_col = col;
	}
	inline long getRow() {
		return _row;
	}
	inline void setRow(long row) {
		_row = row;
	}
	inline void setAlignX(long align){
		_alignX = (TAlignX)align;
	}
	inline void setAlignY(long align){
		_alignY = (TAlignY)align;
	}
	inline TAlignX getAlignX() {
		return _alignX;
	}
	inline TAlignY getAlignY() {
		return _alignY;
	}	
	inline String getType() {
		return _type;
	}
	inline void setType(const String& type){
		_type = type;
	}
	inline long getWidth() {
		return _width;
	}
	inline void setWidth(long width) {
		_width = width;
	} 
	inline long getHeight() {
		return _height;
	}
	void setHeight(long height) {
		_height = height;
	}

	void clear()
	{
		_name = "";
		_posName = "";
		_parentName = "";
		_path = "";
		_type = "";
		_left = 0;
		_top = 0;
		_width = 0;
		_z = 0;
		_height = 0;
		_col = 1;
		_row = 1;
		_alignX = alignX_none;
		_alignY = alignY_none;
	}
};

class VChildSpriteList{
private:
    typedef Vector<ISpriteTreeBase*> TList;
    TList m_list;
    inline void freeSprite(ISpriteTreeBase*& sprite){
        if (sprite!=0){
            delete sprite;
            sprite = 0;		
        }
    }
public:
    VChildSpriteList():m_list(){}
	~VChildSpriteList(){  clear(); }
    inline long getSpriteCount()const{ return (long)m_list.size(); }
    inline void addASprite(ISpriteTreeBase* sprite){ m_list.push_back(sprite); }
	void clear();
	must_inline ISpriteTreeBase* getSprite(const String& name);
    must_inline ISpriteTreeBase* getSprite(long index) { return (m_list[index]); }
    void outASprite(ISpriteTreeBase* sprite);
    long findASprite(ISpriteTreeBase* sprite) const;
	void delKilledSprite();
};

class TSpriteTree:public ISpriteTreeBase{
public:
	TSpriteTree():ISpriteTreeBase(){
		init();
		m_parent = 0;
	}

	void setValue(TSpriteTree spriteTree){
		_name = spriteTree.getName();
		_posName = spriteTree.getPosName();
		_path = spriteTree.getPath();
		m_sprite = spriteTree.getSprite();
		_col = spriteTree.getCol();
		_row = spriteTree.getRow();
		_parentName = spriteTree.getParentName();
		_left = spriteTree.getLeft();
		_top = spriteTree.getTop();
		_z = spriteTree.getZ();
		_width = spriteTree.getWidth();
		_height = spriteTree.getHeight();
		_alignX = spriteTree.getAlignX();
		_alignY = spriteTree.getAlignY();
		_type = spriteTree.getType();
	}
	~TSpriteTree(){ }

public:
	inline void setSprite(VSprite* sprite) {
		m_sprite = sprite;
	}
	inline VSprite* getSprite() {
		return m_sprite;
	}
	inline TSpriteTree* getParent() const{
        return m_parent;
    }
	void setParent(TSpriteTree* pParent);
  
	void setSpriteOffset(long nLeft,long nTop);

	void setSpritePos(long nLeft,long nTop);
	void setSpriteLeft(long nLeft);
	void setSpriteTop(long nTop);

	void setSpriteAlignX(long align);
	void setSpriteAlignY(long align);

	void setSpriteZ(long z);
	void init()
	{
		m_parent = NULL;
		m_sprite = NULL;
		clear();
	}
public:
	inline VChildSpriteList& getSpriteList(){
        return m_spriteList;
    }
	inline void addASprite(TSpriteTree* aSprite){
        aSprite->outFromParent();
        aSprite->setParent(this);		
        m_spriteList.addASprite(aSprite);
    }
	
	inline void addASprite(const TSpriteTree& aSprite){
		if(this->_name == "")
		{
			this->setValue(aSprite);
		}
		else
		{
			TSpriteTree* aNewSprite = new TSpriteTree();
			aNewSprite->setValue(aSprite);//
			addASprite(aNewSprite);
		}
    }
	must_inline long getSpriteCount() {
		return m_spriteList.getSpriteCount();
	}

	must_inline bool findSpriteName(const String& name)
	{
		if(this->getName() == name)
			return true;
		return false;
	}

	must_inline TSpriteTree* getSprite(const String& name) {
		if(this->findSpriteName(name))
			return this;
	
		TSpriteTree* pChildNode = NULL;
		TSpriteTree* pMatchNode = NULL;
		for(long i = 0;i < getSpriteCount();i++)
		{
			pChildNode = (TSpriteTree*)m_spriteList.getSprite(i);
			if(pChildNode)
			{
				if((pMatchNode = pChildNode->getSprite(name)))
				{
					return pMatchNode;
				}
			}	
		}

		return NULL;
	}
	must_inline TSpriteTree* getSprite(long index)
	{ 
		return (TSpriteTree*)(m_spriteList.getSprite(index)); 
	}
    void outASprite(ISpriteTreeBase* sprite)
	{
		m_spriteList.outASprite(sprite);
	}

	inline void outFromParent() {
        if (m_parent!=0){
            m_parent->m_spriteList.outASprite(this);
			//test add
			m_sprite->outFromParent();
            m_parent=0;
        }
    }
	inline void freeSpriteTree()
	{
		m_spriteList.clear();
		if (m_sprite)
		{
			m_sprite->getSpriteList().clear();
		}

		_name = "";
	}

protected:
    VChildSpriteList m_spriteList;
private:
    TSpriteTree* m_parent;
	VSprite* m_sprite;
};


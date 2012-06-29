#pragma once
#include "sysImport.h"
#include "VSprite.h"
#include "SpriteTree.h"
#include "engine\GameRes.h"
#include "engine\SpritePosCfg.h"
#include "base\Xml.h"

typedef Map<String, String> XmlMap;
#include <utility>
typedef std::pair<String, String> Per;
typedef Vector<Per> AttributeMap;
class XmlKeyValueMap
{
public:
	explicit XmlKeyValueMap(){}
	~XmlKeyValueMap(){ m_map.clear();}
	void init() {}
	void setParentName(String& name) {
		if(name != "")
		m_map.push_back(std::make_pair("parentname", name));
	}
	void setSurfaceName(String& name) {
		m_map.push_back(std::make_pair("surfacename", name));
	}
	void setPosName(String& name) {
		if(name != "")
		m_map.push_back(std::make_pair("posname", name));
	}
	void setType(String& type) {
		m_map.push_back(std::make_pair("type", type));
	}
	void setLeft(long left) {
		if(left != 0)
		m_map.push_back(std::make_pair("left", intToStr(left)));
	}
	void setTop(long top) {
		if(top != 0)
		m_map.push_back(std::make_pair("top", intToStr(top)));
	}
	void setRows(long rows) {
		if(rows != 1)
		m_map.push_back(std::make_pair("rows", intToStr(rows)));
	}
	void setCols(long cols) {
		if(cols != 1)
		m_map.push_back(std::make_pair("cols", intToStr(cols)));
	}
	void setWidth(long width) {
		if(width != 0)
		m_map.push_back(std::make_pair("width", intToStr(width)));
	}
	void setHeight(long height) {
		if(height != 0)
		m_map.push_back(std::make_pair("height", intToStr(height)));
	}
	void setZ(long z) {
		if(z != 0)
		m_map.push_back(std::make_pair("z", intToStr(z)));
	}
	void setAlignX(long align) {
		if(align != -1)
		m_map.push_back(std::make_pair("alignX", intToStr(align)));
	}
	void setAlignY(long align) {
		if(align != -1)
		m_map.push_back(std::make_pair("alignY", intToStr(align)));
	}
	void setFilename(String& filename) {
		if(filename != "")
		m_map.push_back(std::make_pair("filename", filename));
	}
public:
	AttributeMap& getMap() {
		return m_map;
	}
private:
	AttributeMap m_map;
	//AttributeMap m_pMap;
};

class SaveXml
{
public:
	SaveXml(TSpriteTree* rootTree = 0):m_pRootTree(rootTree){}
	~SaveXml(){}
public:
	void setSaveXmlSpriteTree(TSpriteTree* rootTree)
	{
		if (rootTree != NULL) m_pRootTree = rootTree;
		else m_pRootTree = 0;
	}
	void setXmlFileName(String name);//
	void setXmlFileSavePath(String path);
	bool saveAll();
	bool saveConfig();
private:
	void initConfig(XmlDocument& xmlDoc, TSpriteTree* tree);
	void initPosConfig(XmlDocument& xmlDoc, TSpriteTree* tree);
	void setConfig(XmlNode* surfaceList, TSpriteTree* tree);
	void setBitmapClipConfig(XmlNode* bitmapcliplist, TSpriteTree* tree);
	void setPosConfig(XmlNode* SpritePos, TSpriteTree* tree);
	void setXmlMap();
	void addAttributeData(XmlNode* node, XmlKeyValueMap& map);
private:
	TSpriteTree* m_pRootTree;
	String m_name;
	String m_path;
};


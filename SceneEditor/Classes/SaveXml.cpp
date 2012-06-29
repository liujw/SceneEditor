
#include "SaveXml.h"


void SaveXml::setXmlFileName(String name)
{
	m_name = name;
}
void SaveXml::setXmlFileSavePath(String path)
{
	m_path = path;
}

bool SaveXml::saveConfig()
{
	//ÅäÖÃ
	XmlDocument rootXmlConfig((""));
	initConfig(rootXmlConfig, m_pRootTree);

	//ÊôÐÔ
	XmlDocument rootXmlPosConfig((""));
	initPosConfig(rootXmlPosConfig, m_pRootTree);
	/////-----------------------------
	rootXmlConfig.saveFile_((m_path).c_str());
	String subStr = "";
	size_t found = m_path.find(m_name);
	if (found != String::npos)
	{
		subStr = m_path.substr(0, found);
	}
	String title = m_name;
	title = title + "Pos.xml";
	rootXmlPosConfig.saveFile_((subStr + title).c_str());
	return true;
}

void SaveXml::initConfig(XmlDocument& xmlDoc, TSpriteTree* tree)
{
	xmlDoc.setNodeType(XmlNode::NodeType::ntDocument);
	xmlDoc.setXmlHeader("version=\"1.0\" encoding=\"gb2312\"");

	XmlNode *GameNode;
	GameNode = xmlDoc.addChild();
	GameNode->setParams(XmlNode::NodeType::ntElement, "", "game");

	XmlNode* SurfaceList = GameNode->addChild();
	SurfaceList->setParams(XmlNode::NodeType::ntElement, "", "surfacelist");
	setConfig(SurfaceList, tree);
	/////////////////////////////////////////////////////////////////////////////

	XmlNode* Bitmapcliplist = GameNode->addChild();
	Bitmapcliplist->setParams(XmlNode::NodeType::ntElement, "", "bitmapcliplist");
	setBitmapClipConfig(Bitmapcliplist, tree);
}

void SaveXml::setConfig(XmlNode* surfaceList, TSpriteTree* tree)
{
	typedef Vector<TSpriteTree*> TTreeList;
	TTreeList treeList;
	if (tree)
	{
		treeList.push_back(tree);
		long cur = 0;
		long last = 1;
		while (cur < treeList.size())
		{
			last = treeList.size();
			if (treeList[cur]->getPath() != "")
			{
				XmlNode* spriteNode = surfaceList->addChild();
				spriteNode->setParams(XmlNode::NodeType::ntElement, "", "surface");
				long k = 0;
				XmlAttribute nameAttr("name", treeList[cur]->getName());
				spriteNode->m_Attributes.push_back(nameAttr);

				XmlAttribute fileNameAttr("filename", treeList[cur]->getPath());
				spriteNode->m_Attributes.push_back(fileNameAttr);
			}
			while (cur < last)
			{
				for (long i = 0; i < (treeList[cur]->getSpriteList()).getSpriteCount(); ++i)
				{
					treeList.push_back(treeList[cur]->getSprite(i));
				}
				++cur;
				break;
			}
		}
	}
}

void SaveXml::setBitmapClipConfig(XmlNode* bitmapcliplist, TSpriteTree* tree)
{
	typedef Vector<TSpriteTree*> TTreeList;
	TTreeList treeList;
	if (tree)
	{
		treeList.push_back(tree);
		long cur = 0;
		long last = 1;
		while (cur < treeList.size())
		{
			last = treeList.size();
			if (treeList[cur]->getCol() > 1 || treeList[cur]->getRow() > 1)
			{
				XmlNode* clipSpriteNode = bitmapcliplist->addChild();
				clipSpriteNode->setParams(XmlNode::NodeType::ntElement, "", "bitmapclip");

				XmlAttribute nameAttr("name", treeList[cur]->getName());
				clipSpriteNode->m_Attributes.push_back(nameAttr);

				XmlAttribute surfaceNameAttr("surface", treeList[cur]->getName());
				clipSpriteNode->m_Attributes.push_back(surfaceNameAttr);

				XmlAttribute colsAttr("cols", intToStr(treeList[cur]->getCol()));
				clipSpriteNode->m_Attributes.push_back(colsAttr);

				XmlAttribute rowsAttr("rows", intToStr(treeList[cur]->getRow()));
				clipSpriteNode->m_Attributes.push_back(rowsAttr);
			}
			while (cur < last)
			{
				for (long i = 0; i < (treeList[cur]->getSpriteList()).getSpriteCount(); ++i)
				{
					treeList.push_back(treeList[cur]->getSprite(i));
				}
				++cur;
				break;
			}
		}
	}
}

void SaveXml::setPosConfig(XmlNode* SpritePos, TSpriteTree* tree)
{
	typedef Vector<TSpriteTree*> TTreeList;
	TTreeList treeList;
	if (tree)
	{
		treeList.push_back(tree);
		long cur = 0;
		long last = 1;
		while (cur < treeList.size())
		{
			last = treeList.size();
			if (treeList[cur]->getLeft() > 0 || treeList[cur]->getWidth() > 0)
			{
				XmlNode* spritePosNode = SpritePos->addChild();
				spritePosNode->setParams(XmlNode::NodeType::ntElement, "", "SpritePos");

				XmlAttribute nameAttr("name", treeList[cur]->getPosName());
				spritePosNode->m_Attributes.push_back(nameAttr);

				if (treeList[cur]->getLeft() > 0) {
					XmlAttribute leftAttr("left", intToStr(treeList[cur]->getLeft()));
					spritePosNode->m_Attributes.push_back(leftAttr);
				}

				if (treeList[cur]->getTop() > 0) {
					XmlAttribute topAttr("top", intToStr(treeList[cur]->getTop()));
					spritePosNode->m_Attributes.push_back(topAttr);
				}

				if (treeList[cur]->getWidth() > 0) {
					XmlAttribute widthAttr("width", intToStr(treeList[cur]->getWidth()));
					spritePosNode->m_Attributes.push_back(widthAttr);
				}

				if (treeList[cur]->getHeight() > 0) {
					XmlAttribute heightAttr("height", intToStr(treeList[cur]->getHeight()));
					spritePosNode->m_Attributes.push_back(heightAttr);
				}
			}
			while (cur < last)
			{
				for (long i = 0; i < (treeList[cur]->getSpriteList()).getSpriteCount(); ++i)
				{
					treeList.push_back(treeList[cur]->getSprite(i));
				}
				++cur;
				break;
			}
		}
	}
}

void SaveXml::initPosConfig(XmlDocument& xmlDoc, TSpriteTree* tree)
{
	xmlDoc.setNodeType(XmlNode::NodeType::ntDocument);
	xmlDoc.setXmlHeader("version=\"1.0\" encoding=\"gb2312\"");

	XmlNode *GameNode;
	GameNode = xmlDoc.addChild();
	GameNode->setParams(XmlNode::NodeType::ntElement, "", "game");

	XmlNode* positions = GameNode->addChild();
	positions->setParams(XmlNode::NodeType::ntElement, "", "positions");
	setPosConfig(positions, tree);
}

static bool isStrAllDigit(String& str)
{
	long i = 0;
	for (;i < str.length(); ++i)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}


void SaveXml::addAttributeData(XmlNode* node, XmlKeyValueMap& map)
{
	if (node == NULL ) return;
	AttributeMap::iterator it = map.getMap().begin();
	while (it != map.getMap().end())
	{
		if (/*it->second != "1" && it->second != "0" && */it->second != "")
		{
			XmlAttribute attr(it->first, it->second);
			node->m_Attributes.push_back(attr);
		}
		++it;
	}
}

bool SaveXml::saveAll()
{
	XmlDocument rootXml((""));
	rootXml.setNodeType(XmlNode::NodeType::ntDocument);
	rootXml.setXmlHeader("version=\"1.0\" encoding=\"gb2312\"");

	XmlNode *GameNode;
	GameNode = rootXml.addChild();
	GameNode->setParams(XmlNode::NodeType::ntElement, "", "game");

	XmlNode* SurfaceList = GameNode->addChild();
	SurfaceList->setParams(XmlNode::NodeType::ntElement, "", "surfacelist");

	TSpriteTree* root = m_pRootTree;
	typedef Vector<TSpriteTree*> TRootList;
	TRootList rootList;
	if (root)
	{
		rootList.push_back(root);
		long cur = 0;
		long last = 1;
		while (cur < rootList.size())
		{
			last = rootList.size();
			XmlNode* list = SurfaceList->addChild();
			list->setParams(XmlNode::NodeType::ntElement, "", "surface");
			while(cur < last)
			{
				XmlKeyValueMap map;
				TSpriteTree* tree = rootList[cur];
				map.setSurfaceName(tree->getName());
				map.setType(tree->getType());
				map.setParentName(tree->getParentName());				
				map.setPosName(tree->getPosName());				
				map.setRows(tree->getRow());
				map.setCols(tree->getCol());
				map.setWidth(tree->getWidth());
				map.setHeight(tree->getHeight());
				map.setTop(tree->getTop());
				map.setLeft(tree->getLeft());
				map.setZ(tree->getZ());
				map.setAlignX(tree->getAlignX());
				map.setAlignY(tree->getAlignX());
				map.setFilename(tree->getPath());
				addAttributeData(list, map);
				for (long i = 0; i < (tree->getSpriteList()).getSpriteCount(); ++i)
				{
					rootList.push_back(tree->getSprite(i));
				}
				++cur;
				break;
			}
		}
	}
	String subStr = "";
	size_t found = m_path.find(m_name);
	if (found != String::npos)
	{
		subStr = m_path.substr(0, found);
	}
	String title = m_name;
	title = title + "Sence.xml";
	rootXml.saveFile_((subStr + title).c_str());
	return false;
}
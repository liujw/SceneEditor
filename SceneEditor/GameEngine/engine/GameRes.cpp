#include "GameRes.h"
#include "VSprite.h"
#include "SpriteTree.h"

#define FAST_GAME_XML

bool GameResMgr::loadFromXml(const char* csFileName)
{
#ifndef FAST_GAME_XML
    static const String csSurfaceNodeName	("surfacelist");
    static const String csSurfaceFlag		("surface");
    static const String csClipNodeName		("bitmapcliplist");
    static const String csBitmapClipFlag	("bitmapclip");
    
    CsXmlDocument xmlDoc(csFileName);
    m_isReadSurface=true;
    xmlDoc.xmlNodeTree.findNodes(csSurfaceNodeName,csSurfaceFlag,this);
    m_isReadSurface=false;
    xmlDoc.xmlNodeTree.findNodes(csClipNodeName,csBitmapClipFlag,this);
#else
    static const String csSurfaceFlag		("surface");
    static const String csBitmapClipFlag	("bitmapclip");
	//test add
	//static const String csTypeFlag          ("type");
    
    m_isReadSurface=true;
    findGameNodes_fast(csFileName,csSurfaceFlag,this);
    m_isReadSurface=false;
    findGameNodes_fast(csFileName,csBitmapClipFlag,this);
#endif
    return true;
}


void GameResMgr::disposeNodeValues(const CsXmlValues& values){
    if (m_isReadSurface)
        readSurfaceNode(values);
    else
        readBitmapClipNode(values);
}

void GameResMgr::doLoadAllRes()
{
    for (TMapType::iterator it(m_SurfaceMap.begin()); it != m_SurfaceMap.end(); ++it)
    {
        it->second->notCache();
    }
    
#if (TOOL_SAVE_SCALED_PNG==0)
    for (TMapType8::iterator it(m_Surface8Map.begin()); it != m_Surface8Map.end(); ++it)
    {
        it->second->notCache();
    }
#endif
}

void GameResMgr::clear()
{
    for (TMapType::iterator it(m_SurfaceMap.begin()); it != m_SurfaceMap.end(); ++it)
        delete it->second;
    m_SurfaceMap.clear();	
    for (TMapType8::iterator it(m_Surface8Map.begin()); it != m_Surface8Map.end(); ++it)
        delete it->second;
    m_Surface8Map.clear();
}

extern TSpriteTree g_spriteTree;

void GameResMgr::readSurfaceNode(const CsXmlValues& values)
{

	TSpriteTree spriteNode;
	
    static const String csSurfaceName	("surfacename");
    static const String csFileNameFlag	("filename");
    static const String csIsGraySurface	("isgraysurface");
    static const String csRowsFlag	    ("rows");
    static const String csColsFlag	    ("cols");
    static const String csTrueFlag	    ("true");
	static const String csTypeFlag      ("type");
	static const String csPosName		("posname");
    static const String csLeft		("left");
    static const String csTop		("top");
    static const String csCenter	("center");
    static const String csWidth		("width");
    static const String csHeight	("height");
	static const String csParentName("parentname");
	static const String csZ         ("z");
	static const String csAlignX     ("alignX");
    static const String csAlignY     ("alignY");

    const String* strSurfaceName = getMapPValue(values,csSurfaceName);
    assert((strSurfaceName!=0)&&(!strSurfaceName->empty()));
    //assert(m_SurfaceMap.count(*strSurfaceName)==0);
    //assert(m_Surface8Map.count(*strSurfaceName)==0);
	spriteNode.setName(*strSurfaceName);

    const String* _strFileName = getMapPValue(values,csFileNameFlag);
	String strFileName = "";
	if(_strFileName != 0 && !_strFileName->empty())
    {
		//assert((_strFileName!=0)&&(!_strFileName->empty()));
		strFileName = transformPicFileName(*_strFileName);
		assert(file_exists(strFileName.c_str()));
		spriteNode.setPath(*_strFileName);
	}

	//test add
	const String* _strType = getMapPValue(values,csTypeFlag);
	assert((_strType!=0)&&(!_strType->empty()));
	spriteNode.setType(*_strType);

    const String* strRows = getMapPValue(values,csRowsFlag);
    long nRow = 1;
    if ((strRows!=0)&&(!atoi(*strRows, nRow))) nRow = 1;
    const String* strCols = getMapPValue(values,csColsFlag);
    long nCol = 1;
    if ((strCols!=0)&&(!atoi(*strCols, nCol))) nCol = 1;
    
	spriteNode.setRow(nRow);
	spriteNode.setCol(nCol);

    const String* strIsGraySurface =getMapPValue(values,csIsGraySurface);
    bool isGraySurface=(strIsGraySurface!=0)&&(strIsEqualIgnoreCase(csTrueFlag,*strIsGraySurface));
    //test add 
	if(strFileName != "" && !getIsExistClipCanvas(*strSurfaceName))
	{
		VSurface* pSurfaceData = new VSurface();
		pSurfaceData->loadFrom(strFileName.c_str(), nRow, nCol,true);	
		//lowerCase(*strSurfaceName);
		m_SurfaceMap[*strSurfaceName] = pSurfaceData;	
	}

	const String* strSurfacePosName = getMapPValue(values,csPosName);
    if(strSurfacePosName!=0&&(!strSurfacePosName->empty()))
	{
		spriteNode.setPosName(*strSurfacePosName);
	}

	const String* strLeft=getMapPValue(values,csLeft);
    if ((strLeft!=0)&&(!strLeft->empty()))
    {
        spriteNode.setLeft(strToInt(*strLeft));
    }
    
    const String* strTop=getMapPValue(values,csTop);
    if ((strTop != 0)&&(!strTop->empty()))
    {
        spriteNode.setTop(strToInt(*strTop));
    }
    
    const String* strWidth=getMapPValue(values,csWidth);
    if ((strWidth != 0)&&(!strWidth->empty()))
    {
		spriteNode.setWidth(strToInt(*strWidth));
    }
 
    const String* strHeight=getMapPValue(values,csHeight);
    if ((strHeight != 0)&&(!strHeight->empty()))
    {
        spriteNode.setHeight(strToInt(*strHeight));
    }
	
	const String* strZ = getMapPValue(values, csZ);
	if ((strZ != 0) && (!strZ->empty()))
	{
		spriteNode.setZ(strToInt(*strZ));
	}
	const String* strParentName = getMapPValue(values, csParentName);
	if ((strParentName != 0) && (!strParentName->empty()))
	{
		spriteNode.setParentName(*strParentName);
	}
	const String* strAlignX = getMapPValue(values, csAlignX);
	if ((strAlignX != 0) && (!strAlignX->empty()))
	{
		spriteNode.setAlignX(strToInt(*strAlignX));
	}

	const String* strAlignY = getMapPValue(values, csAlignY);
	if ((strAlignY != 0) && (!strAlignY->empty()))
	{
		spriteNode.setAlignY(strToInt(*strAlignY));
	}
	
	TSpriteTree* pParent = NULL;
	if(spriteNode.getParentName() != "")
		pParent = g_spriteTree.getSprite(spriteNode.getParentName());

	if(pParent == NULL)
		pParent = &g_spriteTree;

	pParent->addASprite(spriteNode);
}

void GameResMgr::readBitmapClipNode(const CsXmlValues& values)
{
    static const String csSurfaceFlag	("surface");
    //static const String csNameFlag	    ("name");
	static const String csNameFlag	    ("surfacename");
    static const String csRowsFlag	    ("rows");
    static const String csColsFlag	    ("cols");
    
    const String* _strSurfaceName=getMapPValue(values,csSurfaceFlag);
    assert((_strSurfaceName!=0)&&(!_strSurfaceName->empty()));
    String strSurfaceName(*_strSurfaceName);
    lowerCase(strSurfaceName);
    
    //const String* strBitmapClipName=getMapPValue(values,csNameFlag);
    //assert((strBitmapClipName!=0)||(!strBitmapClipName->empty()));
    
    const String* strRows = getMapPValue(values,csRowsFlag);
    long nRow = 1;
    if ((strRows!=0)&&(!atoi(*strRows, nRow))) nRow = 1;
    const String* strCols = getMapPValue(values,csColsFlag);
    long nCol = 1;
    if ((strCols!=0)&&(!atoi(*strCols, nCol))) nCol = 1;
    
    VSurface** pSurface =getMapPValue(m_SurfaceMap,strSurfaceName);
    if (pSurface!=0)
        (*pSurface)->setCachePicClip(nRow, nCol);
    else{
        VSurfaceGray8** pSurface8 =getMapPValue(m_Surface8Map,strSurfaceName);
        if (pSurface8!=0)
            (*pSurface8)->setCachePicClip(nRow, nCol);
        else
            assert(false);
    }
}

bool GameResMgr::getIsExistClipCanvas(const String& strSurfaceName)
{
    String strTmp(strSurfaceName);
    //lowerCase(strTmp);
    VSurface** result=getMapPValue(m_SurfaceMap,strTmp);
    return (result!= 0);
}

const VClipCanvas GameResMgr::getClipCanvas(const String& strSurfaceName)
{
    String strTmp(strSurfaceName);
    //lowerCase(strTmp);
    VSurface** result=getMapPValue(m_SurfaceMap,strTmp);
    assert(result!=0);
    return (*result)->getCacheClipCanvas<VClipCanvas>();
}


const VClipCanvasGray8 GameResMgr::getClipCanvasGray8(const String& strSurfaceName)
{
    String strTmp(strSurfaceName);
    //lowerCase(strTmp);
    VSurfaceGray8** result=getMapPValue(m_Surface8Map,strTmp);
    assert(result!=0);
    return (*result)->getCacheClipCanvas<VClipCanvasGray8>();
}

bool GameResMgr::loadSurface(TSpriteTree* pSpriteNode) //liujw
{
	String strFileName = pSpriteNode->getPath();
	if(strFileName != "" && file_exists(strFileName.c_str()))
	{
		VSurface* pSurfaceData = new VSurface();
		pSurfaceData->loadFrom(strFileName.c_str(), pSpriteNode->getRow(), pSpriteNode->getCol(),true);	
		m_SurfaceMap[pSpriteNode->getName()] = pSurfaceData;	
		return true;
	}

	return false;
}
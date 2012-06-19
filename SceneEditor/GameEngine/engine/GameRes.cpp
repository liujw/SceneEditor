#include "GameRes.h"

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


void GameResMgr::readSurfaceNode(const CsXmlValues& values)
{
    static const String csNameFlag	    ("name");
    static const String csFileNameFlag	("filename");
    static const String csIsGraySurface	("isgraysurface");
    static const String csRowsFlag	    ("rows");
    static const String csColsFlag	    ("cols");
    static const String csTrueFlag	    ("true");
    
    const String* strSurfaceName = getMapPValue(values,csNameFlag);
    assert((strSurfaceName!=0)&&(!strSurfaceName->empty()));
    assert(m_SurfaceMap.count(*strSurfaceName)==0);
    assert(m_Surface8Map.count(*strSurfaceName)==0);
    
    const String* _strFileName = getMapPValue(values,csFileNameFlag);
    assert((_strFileName!=0)&&(!_strFileName->empty()));
    String strFileName(transformPicFileName(*_strFileName));
    assert(file_exists(strFileName.c_str()));
    
    
    const String* strRows = getMapPValue(values,csRowsFlag);
    long nRow = 1;
    if ((strRows!=0)&&(!atoi(*strRows, nRow))) nRow = 1;
    const String* strCols = getMapPValue(values,csColsFlag);
    long nCol = 1;
    if ((strCols!=0)&&(!atoi(*strCols, nCol))) nCol = 1;
    
    const String* strIsGraySurface =getMapPValue(values,csIsGraySurface);
    bool isGraySurface=(strIsGraySurface!=0)&&(strIsEqualIgnoreCase(csTrueFlag,*strIsGraySurface));
    
    if (isGraySurface){
        VSurfaceGray8* pSurfaceData = new VSurfaceGray8();
        pSurfaceData->loadFrom(strFileName.c_str(), nRow, nCol,true);		
        m_Surface8Map[*strSurfaceName] = pSurfaceData;
    }else{
        VSurface* pSurfaceData = new VSurface();		
        pSurfaceData->loadFrom(strFileName.c_str(), nRow, nCol,true);		
        m_SurfaceMap[*strSurfaceName] = pSurfaceData;	
    }
}

void GameResMgr::readBitmapClipNode(const CsXmlValues& values)
{
    static const String csSurfaceFlag	("surface");
    static const String csNameFlag	    ("name");
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
    lowerCase(strTmp);
    VSurface** result=getMapPValue(m_SurfaceMap,strTmp);
    return (result!= 0);
}

const VClipCanvas GameResMgr::getClipCanvas(const String& strSurfaceName)
{
    String strTmp(strSurfaceName);
    lowerCase(strTmp);
    VSurface** result=getMapPValue(m_SurfaceMap,strTmp);
    assert(result!=0);
    return (*result)->getCacheClipCanvas<VClipCanvas>();
}


const VClipCanvasGray8 GameResMgr::getClipCanvasGray8(const String& strSurfaceName)
{
    String strTmp(strSurfaceName);
    lowerCase(strTmp);
    VSurfaceGray8** result=getMapPValue(m_Surface8Map,strTmp);
    assert(result!=0);
    return (*result)->getCacheClipCanvas<VClipCanvasGray8>();
}

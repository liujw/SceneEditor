#ifndef _GameRes_h_
#define _GameRes_h_

#include "../base/Xml.h"
#include "VSurface.h"

class TSpriteTree;

class GameResMgr:private IDisposeNodeValues
{
public:
    typedef HashMap<String, VSurface*> TMapType;
    typedef HashMap<String, VSurfaceGray8*> TMapType8;
    GameResMgr() {}
    virtual ~GameResMgr() {clear();}
	bool loadSurface(TSpriteTree* pSpriteNode);
    bool loadFromXml(const char* csFileName);
    void clear();
    void doLoadAllRes();
    bool getIsExistClipCanvas(const String& strSurfaceName);
    const VClipCanvas getClipCanvas(const String& strSurfaceName);
    const VClipCanvasGray8 getClipCanvasGray8(const String& strSurfaceName);

private:
    TMapType  m_SurfaceMap;
    TMapType8 m_Surface8Map;
    bool	m_isReadSurface;
    void readSurfaceNode(const CsXmlValues& values);
    void readBitmapClipNode(const CsXmlValues& values);
public:
    virtual void disposeNodeValues(const CsXmlValues& values);
};


#endif
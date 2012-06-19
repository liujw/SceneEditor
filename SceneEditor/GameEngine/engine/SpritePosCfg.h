#ifndef _SpritePosCfg_h_
#define _SpritePosCfg_h_

#include "VSprite.h"
#include "../base/Xml.h"

struct TSpritePos 
{
    float m_Left;
    float m_Top;
    float m_Width;
    float m_Height;	
    bool m_IsTopCenter;
    bool m_IsLeftCenter;
    void reset()
    {
        m_Left = 0;
        m_Top = 0;
        m_Width = 0;
        m_Height = 0;
        m_IsTopCenter = false;
        m_IsLeftCenter = false;
    }
};

class SpritePosCfg:private IDisposeNodeValues
{
public:
    typedef HashMap<String, TSpritePos> TMapSpritePos;
    SpritePosCfg() {}
    void setSpriteSize(VSprite* pSprite, const String& strSpriteName);
    void setSpritePos(VSprite* pSprite, const String& strSpriteName, long parentWidth, long parentHeight);
    void setSpritePos(VSprite* pSprite, const String& strSpriteName);
    virtual ~SpritePosCfg() {clear();}
    bool loadFromXml(const char* csFileName);
    void clear();
    inline TSpritePos* getPos(const String& _strSpriteName){
        String strSpriteName(_strSpriteName);
        lowerCase(strSpriteName);
        return getMapPValue(m_MapSpritePos,strSpriteName);
    }
private:
    TMapSpritePos  m_MapSpritePos;
    void readSpritePos(const CsXmlValues& values);
public:
    virtual void disposeNodeValues(const CsXmlValues& values){
        readSpritePos(values);
    }
};

#endif
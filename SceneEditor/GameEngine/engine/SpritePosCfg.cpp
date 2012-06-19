#include "SpritePosCfg.h"

#define FAST_GAME_XML

bool SpritePosCfg::loadFromXml(const char* csFileName)
{
#ifndef FAST_GAME_XML
    static const String csPosNodeName	("positions");
    static const String csSpritePos	    ("spritepos");
    
    CsXmlDocument xmlDoc(csFileName);
    xmlDoc.xmlNodeTree.findNodes(csPosNodeName,csSpritePos,this);
#else
    static const String csSpritePos		("SpritePos");
    
    findGameNodes_fast(csFileName,csSpritePos,this);
#endif
    return true;
}

void SpritePosCfg::clear()
{
    m_MapSpritePos.clear();
}

void SpritePosCfg::readSpritePos(const CsXmlValues& values)
{
    static const String csName		("name");
    static const String csLeft		("left");
    static const String csTop		("top");
    static const String csCenter	("center");
    static const String csWidth		("width");
    static const String csHeight	("height");
    
    const String* strSpriteName=getMapPValue(values,csName);
    assert((strSpriteName!=0)&&(!strSpriteName->empty()));
    
    TSpritePos& pData(m_MapSpritePos[*strSpriteName]);
    pData.reset();
    
    const String* strLeft=getMapPValue(values,csLeft);
    if ((strLeft!=0)&&(!strLeft->empty()))
    {
        if (strIsEqualIgnoreCase(*strLeft, csCenter))
        {
            pData.m_IsLeftCenter = true;
        }
        else 
        {
            double tmp = 0;
            atof(*strLeft, tmp);
            pData.m_Left =(float) mapGamePos(tmp);	
        }
    }
    
    const String* strTop=getMapPValue(values,csTop);
    if ((strTop != 0)&&(!strTop->empty()))
    {
        if (strIsEqualIgnoreCase(*strTop, csCenter))
        {
            pData.m_IsTopCenter = true;
        }
        else 
        {
            double tmp = 0;
            atof(*strTop, tmp);
            pData.m_Top =(float) mapGamePos(tmp);	
        }	
    }
    
    const String* strWidth=getMapPValue(values,csWidth);
    if ((strWidth != 0)&&(!strWidth->empty()))
    {
        double tmp = 0;
        atof(*strWidth, tmp);
        pData.m_Width =(float) mapGamePos(tmp);
    }
    
    const String* strHeight=getMapPValue(values,csHeight);
    if ((strHeight != 0)&&(!strHeight->empty()))
    {
        double tmp = 0;
        atof(*strHeight, tmp);
        pData.m_Height =(float) mapGamePos(tmp);
    }
}

void SpritePosCfg::setSpriteSize(VSprite* pSprite,const String& strSpriteName)
{
    if (pSprite != 0)
    {
        TSpritePos* pPosData = getPos(strSpriteName);
        if (pPosData != 0)
        {
            if (pPosData->m_Width > 0.0)
            {
                pSprite->setWidth((long)pPosData->m_Width);
            }	
            if (pPosData->m_Height > 0.0)
            {
                pSprite->setHeight((long)pPosData->m_Height);
            }			
        }
    }
}
void SpritePosCfg::setSpritePos(VSprite* pSprite,const String& strSpriteName)
{
    if (pSprite != 0)
    {
        VSprite* pParent = pSprite->getParent();
        if (pParent != 0)
        {
            setSpritePos(pSprite, strSpriteName, pParent->getWidth(), pParent->getHeight());
        }
        else 
        {
            setSpritePos(pSprite, strSpriteName, 0, 0);
        }
        
    }
}

void SpritePosCfg::setSpritePos(VSprite* pSprite,const String& strSpriteName, long parentWidth, long parentHeight)
{
    if (pSprite != 0)
    {
        TSpritePos* pPosData = getPos(strSpriteName);
        
        if (pPosData != 0)
        {
            if (pPosData->m_IsLeftCenter)
            {
                pSprite->setLeft((parentWidth - pSprite->getWidth()) / 2);
            }
            else 
            {
                pSprite->setLeft((long)pPosData->m_Left);
            }
            
            if (pPosData->m_IsTopCenter)
            {
                pSprite->setTop((parentHeight - pSprite->getHeight()) / 2);
            }
            else 
            {
                pSprite->setTop((long)pPosData->m_Top);
            }			
        }
    }
}


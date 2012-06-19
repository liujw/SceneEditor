#include "Xml.h"

/*
 const String XMLHEADER_START			= "<?xml";
 const String XMLHEADER_END				= "?>";
 const String XMLCOMMENT_START			= "<!--";
 const String XMLCOMMENT_END				= "-->";
 const String XMLNODE_START				= "<";
 const String XMLNODE_END				= "/>";
 const String XMLNOD_END_START			= "</";
 const String XMLSTARTTAG				= "<";
 const String XMLENDTAG					= ">";
 const String XMLATTRIBUTE_SPACE			= "=";
 const String XMLATTRIBUTEVALUE_START	= "\"";
 const String XMLATTRIBUTEVALUE_END		= "\"";
 
 
 
 int XmlDataParse::isAlpha(unsigned char anyByte)
 {
 if (anyByte <=asciiCharMaxValue)
 return isalpha(anyByte); //a..z, A..Z
 else return 1;
 }
 
 int XmlDataParse::isAlphaNum(unsigned char anByte)
 {
 if (anByte <=asciiCharMaxValue)
 return isalnum(anByte); //0..9
 else return 1;
 }
 
 bool XmlDataParse::readNodeName(const char* _p, String& name, const char** csRtn)
 {
 UInt8* p=(UInt8*)_p;
 *csRtn = 0;
 name = "";
 //Name start with letters or underscores.
 //After that, they can be letters, undersocres, numbers.
 if (isalpha(*p))
 {
 name += *p;
 ++p;
 while (p && *p && (isAlpha(*p) || isAlphaNum(*p) || *p == '_' || *p == '-' || *p == '.' || *p == ':'))
 {
 name += *p;
 ++p;
 }		
 if (name.size() > 0)
 {
 *csRtn =(char*)p;
 return true;
 }	
 else
 {
 --p;
 *csRtn =(char*)p;
 name = "";
 return false;
 }
 
 }
 else
 {
 *csRtn =(char*)p;
 return false;
 }
 }
 
 const char* XmlDataParse::findStrNoCase(const char* csSrc, const char* csSub, int srcLen, int subLen)
 {
 long idx = findStrIgnoreCase(csSrc, srcLen, csSub, subLen);
 if (idx != -1)
 return &csSrc[idx];
 return 0;
 }
 
 bool XmlDataParse::getComment(const String& strData, String& strBeforeRemain, String& strAfterRemain, String& strComment)
 {
 strBeforeRemain = "";
 strAfterRemain = "";
 strComment = "";
 const char* pTmp = XmlDataParse::findStrNoCase(strData.c_str(), XMLCOMMENT_START.c_str(), (long)strData.size(), (long)XMLCOMMENT_START.size());
 if (pTmp)
 {
 int nStart =(int)( pTmp + (int)XMLCOMMENT_START.size() - strData.c_str());
 const char* pEnd = XmlDataParse::findStrNoCase(pTmp + XMLCOMMENT_START.size(),  XMLCOMMENT_END.c_str(), (long)strData.size() - nStart,(long) XMLCOMMENT_END.size());
 if (pEnd)
 {
 int nEnd =(int)( pEnd - 1 - strData.c_str());
 strComment = trim(strData.substr(nStart, (nEnd - nStart + 1)));
 nStart =(long)( pTmp - 1 - strData.c_str());
 strBeforeRemain = trim(strData.substr(0, nStart + 1));
 nStart =(int)( pEnd + (int)XMLCOMMENT_END.size() - strData.c_str());
 strAfterRemain = trim(strData.substr(nStart, strData.size() - nStart));
 return true;
 }
 }
 return false;
 }
 
 int XmlDataParse::getIdxFromString(const String& str, const char* pSub)
 {
 if (pSub < str.c_str()) return 0;
 else if (pSub >= str.c_str() + str.size()) return (long)str.size() - 1;
 else return (long)(pSub - str.c_str());
 }
 
 void XmlDataParse::skipWhiteSpaces(const String& strTag, const SkipType skType, String& strSrc)
 {
 const char* pTmp = strSrc.c_str();
 while (pTmp < strSrc.c_str() + strSrc.size() && (pTmp = XmlDataParse::findStrNoCase(pTmp, strTag.c_str(),(long) strSrc.size() - (pTmp - strSrc.c_str()), (long)strTag.size())) != 0)
 {
 const char* pFlag = 0;
 if (skType == skipBefore) {
 pFlag = findNotSpace(pTmp - 1, skType, strSrc.c_str(),  strSrc.c_str() + strSrc.size() - 1);
 if (pFlag < pTmp - 1)
 {
 int nStart = getIdxFromString(strSrc, pFlag + 1);
 int nEnd = getIdxFromString(strSrc, pTmp - 1);
 strSrc.erase(nStart,  nEnd - nStart + 1);
 if (nStart + strTag.size() >= strSrc.size())
 break;
 else pTmp = &strSrc.c_str()[nStart + strTag.size()];
 }
 else pTmp += strTag.size();
 }
 else {
 pFlag = findNotSpace(pTmp + strTag.size(), skType, strSrc.c_str(), strSrc.c_str() + strSrc.size() - 1);
 if (pFlag > pTmp + strTag.size())
 {
 int nStart = getIdxFromString(strSrc, pTmp + strTag.size());
 int nEnd = getIdxFromString(strSrc, pFlag - 1);
 strSrc.erase(nStart,  nEnd - nStart + 1);
 pTmp = &strSrc.c_str()[nStart];
 }
 else pTmp += strTag.size();			
 }
 
 }
 }
 
 const char* XmlDataParse::findNotSpace(const char* csSrc, const SkipType skType, const char* csSrcMin, const char* csSrcMax)
 {
 if (skType == skipBefore)
 {
 while ((csSrc >= csSrcMin) && isspace(*(UInt8*)csSrc))
 --csSrc;
 }
 else
 {
 while (csSrc <= csSrcMax && isspace(*(UInt8*)csSrc))
 ++csSrc;
 }
 return csSrc;
 }
 
 void XmlDataParse::parseAttributes(const String& strData, Vector<XmlAttribute>* pAttributes)
 {
 String strTmpData(trim(strData));
 const char* pTmp;
 String strKey;
 String strValue;
 while ((pTmp = XmlDataParse::findStrNoCase(strTmpData.c_str(), XMLATTRIBUTE_SPACE.c_str(),(long) strTmpData.size(), (long)XMLATTRIBUTE_SPACE.size())) != 0)
 {
 int nStart =(int)( pTmp - 1 - strTmpData.c_str());
 strKey = trim(strTmpData.substr(0, nStart + 1));
 const char* pStart; 
 if ((pStart = XmlDataParse::findStrNoCase(pTmp + 1, XMLATTRIBUTEVALUE_START.c_str(), (long)strTmpData.size() - (pTmp + 1 - strTmpData.c_str()),(long) XMLATTRIBUTEVALUE_START.size())) != 0)
 {
 const char* pEnd = XmlDataParse::findStrNoCase(pStart + 1, XMLATTRIBUTEVALUE_END.c_str(), (long)strTmpData.size() - (pStart + 1 - strTmpData.c_str()), (long)XMLATTRIBUTEVALUE_END.size());
 if (pEnd)
 {
 nStart = (int)(pStart + 1 - strTmpData.c_str());
 int nEnd = (int)(pEnd - 1 - strTmpData.c_str());
 strValue = trim(strTmpData.substr(nStart, nEnd - nStart + 1));
 pAttributes->push_back(XmlAttribute(strKey, strValue));
 nStart = (int)(pEnd + 1 - strTmpData.c_str());
 strTmpData = trim(strTmpData.substr(nStart, strTmpData.size() - nStart));
 
 }else break;
 } else break;
 }	
 }
 
 bool XmlDataParse::readXmlHeader(const String& strData, String& strBeforeRemain, String& strAfterRemain, String& strHeader)
 {
 String strRtn;
 strBeforeRemain = "";
 strAfterRemain = "";
 strHeader = "";
 String strTmpData(trim(strData));
 const char* pStart = XmlDataParse::findStrNoCase(strTmpData.c_str(), XMLHEADER_START.c_str(), (long)strTmpData.size(),(long) XMLHEADER_START.size());
 if (pStart)
 {
 const char* pEnd = XmlDataParse::findStrNoCase(pStart, XMLHEADER_END.c_str(), (long)strTmpData.size() - (pStart - strTmpData.c_str()),(long) XMLHEADER_END.size());
 if (pEnd && isspace(*(UInt8*)(pStart + XMLHEADER_START.size())))
 {
 int nStart =(int)( pStart + (int)XMLHEADER_START.size() - strTmpData.c_str());
 int nEnd = (int)(pEnd - 1 - strTmpData.c_str());
 strRtn = trim(strTmpData.substr(nStart, nEnd - nStart + 1));
 if (pStart > strTmpData.c_str())
 {
 strBeforeRemain = trim(strTmpData.substr(0, pStart - strTmpData.c_str()));
 }
 int idx = (int)(pEnd + (int)XMLHEADER_END.size() - strTmpData.c_str());
 strAfterRemain = trim(strTmpData.substr(idx, strTmpData.size() - idx));
 strHeader = strRtn;		
 return true;
 }
 }
 return false;	
 }
 
 bool XmlDataParse::getElement(const String& strData, String& strBeforeRemain, String& strAfterRemain, String& strElementData, String& strElementName, String& strSubData)
 {
 strBeforeRemain = "";
 strAfterRemain = "";
 strElementData = "";
 strElementName = "";
 strSubData = "";
 const char* pTmp = strData.c_str();
 while ((pTmp = XmlDataParse::findStrNoCase(pTmp, XMLNODE_START.c_str(), (long)strData.size() - (pTmp - strData.c_str()),(long) XMLNODE_START.size())) != 0 && pTmp < strData.c_str() + strData.size())
 {
 const char* csRtn;
 String strTmpName;
 if (XmlDataParse::readNodeName(pTmp + 1, strTmpName, &csRtn))
 {
 String strTmp =  XMLNOD_END_START + strTmpName + XMLENDTAG; 
 const char* csFlag = 0;
 int nStart;
 int nEnd;
 if ((csFlag = XmlDataParse::findStrNoCase(csRtn, strTmp.c_str(),(long) strData.size() - (csRtn - strData.c_str()),(long) strTmp.size())) != 0)
 {
 const char* csNodeNameEnd = XmlDataParse::findStrNoCase(csRtn, XMLENDTAG.c_str(), (long)strData.size() - (csRtn - strData.c_str()),(long) XMLENDTAG.size());
 if (csNodeNameEnd && csNodeNameEnd < csFlag)
 {
 strBeforeRemain = strData.substr(0, pTmp - strData.c_str());
 nStart = (int)(csFlag + strTmp.size() - strData.c_str());
 nEnd = (int)strData.size() - 1;
 strAfterRemain = strData.substr(nStart, nEnd - nStart + 1);
 strElementName = strTmpName;
 nStart =(int)( pTmp + strTmpName.size() + 1 - strData.c_str());
 nEnd = (int)(csNodeNameEnd - 1 - strData.c_str());
 strElementData = strData.substr(nStart, nEnd - nStart + 1);	
 nStart = (int)(csNodeNameEnd + 1 - strData.c_str());
 nEnd =(int)( csFlag - 1 - strData.c_str());
 strSubData = strData.substr(nStart, nEnd - nStart + 1); 
 return true;
 }							
 }
 if ((csFlag = XmlDataParse::findStrNoCase(csRtn, XMLNODE_END.c_str(), (long)strData.size() - (csRtn - strData.c_str()),(long) XMLNODE_END.size())) != 0)
 {
 strBeforeRemain = strData.substr(0, pTmp - strData.c_str());
 nStart =(int)( csFlag + (int)XMLNODE_END.size() - strData.c_str());
 nEnd = (int)strData.size() - 1;
 strAfterRemain = strData.substr(nStart, nEnd - nStart + 1);
 strElementName = strTmpName;
 nStart =(int)( pTmp + (int)strTmpName.size() + 1 - strData.c_str());
 nEnd =(int)( csFlag - 1 - strData.c_str());
 strElementData = strData.substr(nStart, nEnd - nStart + 1);
 return true;
 }
 pTmp += XMLNODE_START.size();
 }
 else pTmp += XMLNODE_START.size();	
 }
 return false;
 }
 
 void XmlNode::setParams(const NodeType nodeType, const String& strData, const String& strNodeName)
 {
 clear();
 m_NodeType = nodeType;
 switch (nodeType) {
 case ntDocument:
 break;
 case ntElement:
 m_NodeName = strNodeName;
 XmlDataParse::parseAttributes(strData, &m_Attributes);
 break;
 default:
 m_TextData = strData;
 break;
 }
 }
 
 void XmlNode::clear()
 {
 m_Childs.clear();
 m_Attributes.clear();
 m_TextData = "";
 m_NodeName = "";
 m_NodeType = ntUnknown;
 }
 
 bool XmlNode::findKey(const String& strkey, String& strValue) const
 {
 String t_strkey(trim(strkey));
 for (unsigned int i = 0; i < m_Attributes.size(); ++i)
 {
 if (strIsEqualIgnoreCase(m_Attributes[i].getKey(), t_strkey))
 {
 strValue = m_Attributes[i].getValue();
 return true;
 }
 }
 strValue = "";
 return false;
 }
 
 XmlDocument::XmlDocument(const char* fileName): m_FileName(fileName)
 {
 setParams(ntDocument, "", "");
 setParent(0);
 loadFromFile(fileName);
 }
 
 void XmlDocument::saveFile_(const char* fileName)
 {
 MyMyTextFileOutputStream txtFile(fileName);
 writeXmlHeader(txtFile, m_XmlHeader);
 for (unsigned int i = 0; i < m_Childs.size(); ++i)
 {
 switch (m_Childs[i].getType())
 {
 case ntElement:
 writeElement(txtFile, m_Childs[i]);
 break;
 case ntComment:
 writeComment(txtFile, m_Childs[i].getStrData());
 break;
 default:
 writeUnknown(txtFile, m_Childs[i].getStrData());
 break;
 }
 }
 }
 
 void XmlDocument::loadFromFile(const char* fileName)
 {
 m_strData = "";
 FileInputStream inStream(fileName);
 
 unsigned long nCount=inStream.getDataSize();
 m_strData.resize(nCount);
 if (nCount>0){
 inStream.read((void*)&m_strData[0],nCount);
 }
 
 parseXml();
 }
 
 void XmlDocument::parseComment(const String& strData, XmlNode* parent)
 {
 String strTmpData = trim(strData);
 String strTmpBefore;
 String strTmpAfter;
 String strTmpVal;
 while (XmlDataParse::getComment(strTmpData, strTmpBefore, strTmpAfter, strTmpVal))
 {
 //text
 if (strTmpBefore != "") parent->addChild()->setParams(XmlNode::ntText, strTmpBefore, "");
 //comment
 if (strTmpVal != "") parent->addChild()->setParams(XmlNode::ntComment, strTmpVal, "");
 strTmpData = strTmpAfter;
 }
 //text
 if (strTmpData != "") parent->addChild()->setParams(XmlNode::ntText, strTmpData, "");	
 }
 
 void XmlDocument::getNodes(const String& strData, XmlNode* parent)
 {
 String strBeforeRemain = "";
 String strAfterRemain = "";
 String strElementData = "";
 String strElementName = "";
 String strSubData = "";
 if (XmlDataParse::getElement(strData, strBeforeRemain, strAfterRemain, strElementData, strElementName, strSubData))
 {
 parseComment(strBeforeRemain, parent);
 XmlNode* tmpParent;
 tmpParent = parent->addChild();
 tmpParent->setParams(XmlNode::ntElement, strElementData, strElementName);
 strSubData = trim(strSubData);
 if (strSubData != "") getNodes(strSubData, tmpParent);
 strAfterRemain = trim(strAfterRemain);
 if (strAfterRemain != "")
 getNodes(strAfterRemain, parent);
 }
 else
 {
 parseComment(strData, parent);
 }
 }
 
 void XmlDocument::doEnumNodes(const XmlNode* pNode, EnumXmlNodeProc lpEnumProc)
 {
 lpEnumProc(pNode);
 if ((pNode->getType() == ntElement) || (pNode->getType() == ntDocument))
 {
 for (unsigned int i = 0; i < pNode->m_Childs.size(); i++)
 doEnumNodes(&(pNode->m_Childs[i]), lpEnumProc);
 }
 }
 
 void XmlDocument::enumNodes(EnumXmlNodeProc lpEnumProc)
 {
 if (lpEnumProc != 0)
 doEnumNodes(this, lpEnumProc);
 }
 
 void XmlDocument::parseXml()
 {
 clear();
 m_XmlHeader = "";
 setNodeType(ntDocument);
 setParent(0);
 if (m_strData == "") return;
 String strData = m_strData;
 String strTmp;
 XmlDataParse::skipWhiteSpaces(XMLSTARTTAG, XmlDataParse::skipAfter, strData);
 XmlDataParse::skipWhiteSpaces(XMLENDTAG, XmlDataParse::skipBefore, strData);
 String strAfterRemain;
 String strBeforeRemain;
 if (XmlDataParse::readXmlHeader(strData, strBeforeRemain, strAfterRemain, m_XmlHeader))
 {
 parseComment(strBeforeRemain, this);
 strData = strAfterRemain;
 }
 getNodes(strData, this);
 }
 
 void XmlDocument::writeXmlHeader(MyTextFileOutputStream &txtFile, const String& strHeader)
 {
 txtFile.writeLine(XMLHEADER_START + " " + strHeader + XMLHEADER_END);
 }
 
 void XmlDocument::writeElement(MyTextFileOutputStream &txtFile, const XmlNode& node)
 {
 String strAttributes = "";
 for (unsigned int i = 0; i < node.m_Attributes.size(); ++i)
 {
 strAttributes += node.m_Attributes[i].getKey() + XMLATTRIBUTE_SPACE + XMLATTRIBUTEVALUE_START + node.m_Attributes[i].getValue() + XMLATTRIBUTEVALUE_END + " ";
 }
 if (node.m_Childs.size() > 0)
 {
 String strNode = XMLSTARTTAG + node.getNodeName() + " " + strAttributes + XMLENDTAG;
 txtFile.writeLine(strNode);
 for (unsigned int i = 0; i < node.m_Childs.size(); ++i)
 {
 switch (node.getType())
 {
 case ntElement:
 writeElement(txtFile, node.m_Childs[i]);
 break;
 case ntComment:
 writeComment(txtFile, node.m_Childs[i].getStrData());
 break;
 default:
 writeUnknown(txtFile, node.m_Childs[i].getStrData());
 break;
 }
 }
 strNode = XMLNOD_END_START + node.getNodeName() + XMLENDTAG;
 txtFile.writeLine(strNode);		
 }
 else
 {
 String strNode = XMLNODE_START + node.getNodeName() + " " + strAttributes + XMLNODE_END;
 txtFile.writeLine(strNode);
 }
 }
 
 void XmlDocument::writeComment(MyTextFileOutputStream &txtFile, const String& strText)
 {
 String strNode = XMLCOMMENT_START + strText + XMLCOMMENT_END;
 txtFile.writeLine(strNode);
 }
 
 void XmlDocument::writeUnknown(MyTextFileOutputStream &txtFile, const String& strText)
 {
 txtFile.writeLine(strText);
 }
 */

//////////////////////////////////////////////////////////////////////////

class CsXmlParse{
public:
    must_inline static const char* skip_space(const char* cur,const char* end){
        while ((cur!=end)&&(charIsSpace(*cur)))
            ++cur;
        return cur;
    }
    must_inline static const char* skip_space_unsafe(const char* cur){//不考虑越界
        while (charIsSpace(*cur))
            ++cur;
        return cur;
    }
    must_inline static const char* findKeyStrEnd(const char* cur,const char* end){
        while ((cur!=end)&&(charIsKeyChar(*cur)))
            ++cur;
        return cur;
    }
    must_inline static const char* findKeyStrEnd_unsafe(const char* cur){//不考虑越界
        while (charIsKeyChar(*cur))
            ++cur;
        return cur;
    }
    
private:
    static void parseHead(const char*& cur,const char* end,CsXmlNode& head,const String& errorTree);
    static bool parseNode(const char*& cur,const char* end,CsXmlNode& node,const String& errorTree);
    static void parseValues(const char* cur,const char* end,CsXmlNode& out_node,const String& errorTree);
    
public:
    static void doParse(const char* cur,const char* end,CsXmlNode& head,CsXmlNode& nodeTree,const String& errorTree){
        parseHead(cur,end,head,errorTree);//head 
        while (parseNode(cur,end,nodeTree,errorTree)){}//循环解析node
    }
    static void throwErrorMsg(const String& errorTree,const String& errorCur){
        throw new CsXmlException("xmlFile parse error,"+errorTree+errorCur);
    }
};

void CsXmlParse::parseHead(const char*& cur,const char* end,CsXmlNode& head,const String& errorTree){
    long ipos =findStrIgnoreCase(cur,end-cur,"<?",2);
    if (ipos<0) throwErrorMsg(errorTree,"not find head start tag.");
    cur+=(ipos+2);
    ipos=findStr(cur,end-cur,"?>",2);
    if (ipos<0) throwErrorMsg(errorTree,"not find head end tag.");
    head.clear();
    parseValues(cur,cur+ipos,head,"parse head value list,"+errorTree);
    cur+=(ipos+2);
}

bool CsXmlParse::parseNode(const char*& cur,const char* end,CsXmlNode& node,const String& errorTree){
    //<
    cur=skip_space(cur,end);
    if (cur==end) return false;
    if ((*cur)!='<') throwErrorMsg(errorTree,"not find node start tag.");
    ++cur;
    
    //node name or type
    const char* keyStrEnd=findKeyStrEnd(cur,end);
    if (keyStrEnd==end)
        throwErrorMsg(errorTree,"node's name or type error.");
    else if ((keyStrEnd==cur)&&(*cur=='!')){ //is a remark start
        if ((end-cur<3)||(cur[1]!='-')||(cur[2]!='-')) throwErrorMsg(errorTree,"remark start tag error.");
        cur+=3;
        long ipos=findStr(cur,end-cur,"-->",3);
        if (ipos<0) throwErrorMsg(errorTree,"not find remark end tag.");
        //String remark(cur,cur+ipos);
        cur+=ipos+3;	
    }else if (keyStrEnd==cur) 
        throwErrorMsg(errorTree,"not find node's name or type.");
    else if (charIsSpace(*keyStrEnd)) { //value list 
        long ipos=findStr(cur,end-cur,"/>",2);
        if (ipos<0) throwErrorMsg(errorTree,"not find node's value list end tag.");
        CsXmlNode child_node;
        parseValues(cur,cur+ipos,child_node,errorTree);
        cur+=ipos+2;
        CsXmlNode& chNode(node.childs[child_node.node_name]);
        chNode.swap(child_node);
    }else if (*keyStrEnd=='>'){ //is a node start
        String node_name(cur,keyStrEnd);
        String cur_errorTree(errorTree+"node:<"+node_name+">-");
        String nodeEndTag; 
        nodeEndTag.reserve(2+(keyStrEnd-cur)+1+1);
        nodeEndTag+="</"; 
        nodeEndTag.insert(nodeEndTag.end(),node_name.begin(),node_name.end());
        nodeEndTag+=">";
        cur=keyStrEnd+1;
        long ipos=findStr(cur,end-cur,nodeEndTag.c_str(),nodeEndTag.size());
        if (ipos<0) throwErrorMsg(cur_errorTree,"not find node end tag.");
        
        lowerCase(node_name);
        CsXmlNode& child_node(node.childs[node_name]);
        child_node.node_name.swap(node_name);
        const char* child_cur=cur;
        const char* child_end=cur+ipos;
        cur+=ipos+nodeEndTag.size();
        while (parseNode(child_cur,child_end,child_node,cur_errorTree)){}//循环解析node
    }else
        throwErrorMsg(errorTree,"unknow node.");
    return (cur!=end);
}

void CsXmlParse::parseValues(const char* cur,const char* end,CsXmlNode& out_node,const String& errorTree){
    static const String xml_tag_name("name");
    
    //type
    cur=skip_space(cur,end);
    const char* keyStrEnd=findKeyStrEnd(cur,end);
    if (keyStrEnd==cur) throwErrorMsg(errorTree,"type error.");
    String type(cur,keyStrEnd);
    cur=keyStrEnd;
    lowerCase(type);
    out_node.type.swap(type);
    
    //values
    while (cur!=end){
        //key
        cur=skip_space(cur,end);
        if (cur==end) break;
        const char* keyStrEnd=findKeyStrEnd(cur,end);
        if (keyStrEnd==cur) throwErrorMsg(errorTree,"unknow char.");
        String key(cur,keyStrEnd);
        cur=keyStrEnd;
        
        //=
        cur=skip_space(cur,end);
        if ((cur==end) || ((*cur)!='=')) throwErrorMsg(errorTree,"not find key:"+key+"'s '=' tag.");
        ++cur;
        
        //value
        cur=skip_space(cur,end);
        if ((cur==end) || ((*cur)!='"')) throwErrorMsg(errorTree,"not find key:"+key+"'s value.");
        ++cur;
        long iPos=findStr(cur,end-cur,"\"",1);
        if (iPos<0) return throwErrorMsg(errorTree,"not find key:"+key+"'s value end tag.");
        String value(cur,cur+iPos);
        cur+=iPos+1;
        
        lowerCase(key);
        if (key==xml_tag_name){//name
            lowerCase(value);
            out_node.node_name=value;
        }
        String& cvalue(out_node.values[key]);
        cvalue.swap(value);
    }
}

////////

void CsXmlNode::findNodes(const CsXmlNodes& nodeList,const String& nodeName,const String& valueType,IDisposeNodeValues* callBackProc){
    const CsXmlNode* node=getMapPValue(nodeList,nodeName);
    if (node!=0){
        const CsXmlNodes& nodes(node->childs);
        for (CsXmlNodes::const_iterator it(nodes.begin()); it!=nodes.end(); ++it){
            const CsXmlNode& node(it->second);
            if (strIsEqualIgnoreCase(node.type,valueType))
                callBackProc->disposeNodeValues(node.values);
        }
    }else{
        for (CsXmlNodes::const_iterator it(nodeList.begin()); it!=nodeList.end(); ++it){
            const CsXmlNodes& childs((it->second).childs);
            if (!childs.empty())
                findNodes(childs,nodeName,valueType,callBackProc);
        }	
    }
}


void CsXmlDocument::loadXmlFile(FileInputStream* inStream,const String& errorTree){
    clear();
    
    unsigned long nCount=inStream->getDataSize();
    if (nCount>0){
        const char* pbuf=(const char*)(inStream->read(nCount));
        assert(nCount==inStream->getDataSize());
        CsXmlParse::doParse(pbuf,pbuf+nCount,this->xmlHead,this->xmlNodeTree,errorTree);
    }else
        CsXmlParse::throwErrorMsg(errorTree,"file size is 0.");
}

//////////////////

void findGameNodes_fast(const char* cur,const char*end,const String& valueType,IDisposeNodeValues* callBackProc){
    static const String xml_tag_name("name");
    
    const char* const cur_back=cur;
    const long valueType_size=valueType.size();
    
    CsXmlValues	values;
    while (true){
        cur=findStr_fast(cur,end,&valueType[0],valueType_size);
        if (end-cur<=valueType_size) break;
        if ((cur==cur_back)||(cur[-1]!='<')||(!charIsSpace(cur[valueType_size]))) { cur+=valueType_size; continue; }
        
        cur+=valueType_size;
        values.clear();
        while (true){
            //key
            cur=CsXmlParse::skip_space_unsafe(cur);
            if (*cur=='/'){
                assert(((cur+1)!=end)&&(cur[1]=='>'));
                break;
            }
            const char* keyStrEnd=CsXmlParse::findKeyStrEnd_unsafe(cur);
            String key(cur,keyStrEnd);
            cur=keyStrEnd;
            
            //=
            cur=CsXmlParse::skip_space_unsafe(cur);
            assert((cur!=end) && ((*cur)=='='));
            ++cur;
            
            //value
            cur=CsXmlParse::skip_space_unsafe(cur);
            assert((cur!=end) && ((*cur)=='"'));
            ++cur;
            const char* vend=findChar_fast(cur,end,'\"');
            assert(vend!=end);
            String value(cur,vend);
            cur=vend+1;
            
            lowerCase(key);
            if (key==xml_tag_name){//name
                lowerCase(value);
            }
            String& cvalue(values[key]);
            cvalue.swap(value);
        }
        callBackProc->disposeNodeValues(values);
    }
}
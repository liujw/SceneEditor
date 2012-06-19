#ifndef _Xml_h_
#define _Xml_h_

#include "hStream.h"

/*
 class XmlNode;
 typedef Vector<XmlNode> XmlNodeList;
 typedef void (*EnumXmlNodeProc)(const XmlNode* node);
 
 class XmlAttribute
 {
 private:
 String m_Key;
 String m_Value;
 public:
 explicit XmlAttribute(const String& strKey, const String& strValue): m_Key(strKey), m_Value(strValue) {}
 must_inline String& getKey(){ return m_Key; }
 must_inline const String& getKey()const { return m_Key; }
 must_inline String& getValue(){ return m_Value; }
 must_inline const String& getValue()const{ return m_Value; }
 };
 
 class XmlDataParse
 {
 public:
 enum SkipType{skipBefore, skipAfter};
 static int isAlpha(unsigned char anyByte);
 static int isAlphaNum(unsigned char anByte);
 static const char* findStrNoCase(const char* csSrc, const char* csSub, int srcLen, int subLen);
 static bool readNodeName(const char* p, String& name, const char** csRtn);
 static int getIdxFromString(const String& str, const char* pSub);
 static void skipWhiteSpaces(const String& strTag, const SkipType skType, String& strSrc);
 static const char* findNotSpace(const char* csSrc, const SkipType skType, const char* csSrcMin, const char* csSrcMax);
 static void parseAttributes(const String& strData, Vector<XmlAttribute>* pAttributes);
 static bool readXmlHeader(const String& strData, String& strBeforeRemain, String& strAfterRemain, String& strHeader);
 static bool getElement(const String& strData, String& strBeforeRemain, String& strAfterRemain, String& strElementData, String& strElementName, String& strSubData);
 static bool getComment(const String& strData, String& strBeforeRemain, String& strAfterRemain, String& strComment);
 };
 
 class XmlNode
 {
 public:
 enum NodeType {ntUnknown, ntDocument, ntElement, ntComment, ntText};
 XmlNodeList m_Childs;						//obtains child Nodes
 Vector<XmlAttribute> m_Attributes;		//obtains attributes
 public:
 XmlNode() {clear();}
 virtual ~XmlNode(){}
 void setParams(const NodeType nodeType, const String& strData, const String& strNodeName);	
 int hashChild() {return (int)m_Childs.size();}
 XmlNode* addChild()
 {
 m_Childs.push_back(XmlNode());
 m_Childs.back().m_Parent = this;
 return &m_Childs.back();		
 }
 String operator[](const String& strKey) const {
 String strValue;
 findKey(strKey, strValue);
 return strValue;
 }
 void clear();
 bool findKey(const String& strkey, String& strValue) const;
 String getStrData() const {return m_TextData;}	//return may be note, value, unknown
 NodeType getType() const {return m_NodeType;}	//return node type, may be unknown, document, element, comment, text
 String getNodeName() const {return m_NodeName;} //return element node name
 XmlNode* getParent() {return m_Parent;}
 void setParent(XmlNode* parent) {m_Parent = parent;}
 void setNodeType(const NodeType ntType) {m_NodeType =  ntType;}
 protected:
 NodeType m_NodeType;
 String m_TextData;							//May be note, value, unknown
 String m_NodeName;                          //node name
 XmlNode* m_Parent;							//parent node
 };
 
 class XmlDocument: public XmlNode
 {
 public:
 explicit XmlDocument(const char* fileName);
 void loadFromFile(const char* fileName);
 void saveFile_(const char* fileName);
 void enumNodes(EnumXmlNodeProc lpEnumProc);
 void setXmlHeader(const String& strHeader) {m_XmlHeader = strHeader;}
 String getXmlHeader() const {return m_XmlHeader;}
 protected:
 void writeXmlHeader(MyTextFileOutputStream &txtFile, const String& strHeader);
 void writeElement(MyTextFileOutputStream &txtFile, const XmlNode& node);
 void writeComment(MyTextFileOutputStream &txtFile, const String& strText);
 void writeUnknown(MyTextFileOutputStream &txtFile, const String& strText);
 private:
 void getNodes(const String& strData, XmlNode* parent);
 void parseXml();
 void parseComment(const String& strData, XmlNode* parent);
 void doEnumNodes(const XmlNode* pNode, EnumXmlNodeProc lpEnumProc);
 String m_XmlHeader;
 String m_FileName;
 String m_strData;
 };
 */

//////////////////
//只读Xml 

class CsXmlNode;
typedef Map<String,CsXmlNode> CsXmlNodes;
typedef HashMap<String,String> CsXmlValues;

class CsXmlException:public InfoException{
public:
    inline explicit CsXmlException():InfoException("class InfoException Error."){}
    inline explicit CsXmlException(const char* error):InfoException(error){}
    inline explicit CsXmlException(const String& error):InfoException(error){}
};

class IDisposeNodeValues{
public:
    virtual void disposeNodeValues(const CsXmlValues& values)=0;
    virtual ~IDisposeNodeValues(){}
};


class CsXmlNode{
public:
    String			type;
    CsXmlValues		values;//property
    
    String			node_name;
    CsXmlNodes		childs;
    
    must_inline void clear(){
        type.clear();
        node_name.clear();
        values.clear();
        childs.clear();
    }
    must_inline void swap(CsXmlNode& node){
        type.swap(node.type);
        node_name.swap(node.node_name);
        values.swap(node.values);
        childs.swap(node.childs);
    }
    inline void findNodes(const String& nodeName,const String& valueType,IDisposeNodeValues* callBackProc){
        CsXmlNode::findNodes(this->childs,nodeName,valueType,callBackProc);
    }
    static void findNodes(const CsXmlNodes& nodeList,const String& nodeName,const String& valueType,IDisposeNodeValues* callBackProc);
};

//使用不太安全的解析手段:直接定位结束标记 (""\注释\双字节编码等都可能造成错误!!!)
class CsXmlDocument{
private:
    void loadXmlFile(FileInputStream* inStream,const String& errorTree);
public:
    CsXmlNode		xmlHead;
    CsXmlNode		xmlNodeTree;
public:
    inline explicit CsXmlDocument() {}
    inline explicit CsXmlDocument(FileInputStream* inStream) {  loadXmlFile(inStream,"");  }	
    inline explicit CsXmlDocument(const char* fileName) { TextFileInputStream inStream(fileName); loadXmlFile(&inStream,String("xmlFile name:")+fileName+","); }
    
    void loadXmlFile(FileInputStream* inStream) {  loadXmlFile(inStream,"");  }
    inline void loadXmlFile(const char* fileName) { TextFileInputStream inStream(fileName); loadXmlFile(&inStream,String("xmlFile name:")+fileName+","); }
    inline void clear() { xmlNodeTree.clear(); xmlHead.clear(); }
};


//只考虑速度! 不处理字符串\注释等可能造成的错误  所以对xml文件有一定的要求
void findGameNodes_fast(const char* xmlStrBegin,const char*xmlStrEnd,const String& valueType,IDisposeNodeValues* callBackProc);
static inline void findGameNodes_fast(FileInputStream* xmlInStream,const String& valueType,IDisposeNodeValues* callBackProc){
    unsigned long nCount=xmlInStream->getDataSize();
    assert(nCount>0);
    const char* pbuf=(const char*)(xmlInStream->read(nCount));
    assert(nCount==xmlInStream->getDataSize());
    findGameNodes_fast(pbuf,pbuf+nCount,valueType,callBackProc);
}
static inline void findGameNodes_fast(const char* xmlFileName,const String& valueType,IDisposeNodeValues* callBackProc){
    TextFileInputStream inStream(xmlFileName);
    findGameNodes_fast(&inStream,valueType,callBackProc);
}


#endif
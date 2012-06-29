#ifndef _IniFile_h_
#define _IniFile_h_

#include "string.h"
#include "SysImportType.h"
#include "hStream.h"
#include "MyCrypt.h"

class IniValue{
public:
    bool   isNote;
    String notes;
    String key;
    String value;
    inline explicit IniValue(const String& aNotes): isNote(true), notes(aNotes){	
    }
    explicit IniValue(const String& aKey, const String& aValue);
};

class Section
{
public:
    String name;
    Vector<IniValue> list;
    IniValue* findKey(const String& strKey);
public:
    inline explicit Section(const String& aName): name(aName){
    }
    
    inline ~Section() {
    }
    
    bool readString(const String& strKey, String& strVal);
    
    void writeString(const String& strKey,const String& strVal);
};

typedef Vector<Section> SectionList;

class IniFile
{
protected:
    String      m_txtFileName;
    bool        m_isChanged;
    bool		m_isDecrypt;
    TDesKey		m_desKey;
    SectionList m_sctList;
    Section* getBackSection();
    void saveFile();
    void addItem(const String& strSection, const String& strKey, const String& strValue);	
    void parseText(BufInputStream& txtFile);
public:
    explicit IniFile(const char* strFileName);
    explicit IniFile(const char* strFileName, const bool isDocumentFile, const TDesKey& desKey, bool isDocDecrypt = true);
    virtual ~IniFile();
    void clearList();
    long getSectionCount() const {return m_sctList.size();}
    Section* getSectionItem(long idx);
    Section* findSection(const String& strSection);
    bool readString(const String& strSection, const String& strKey, const String& strDefault, String& strVal);
    void writeString(const String& strSection, const String& strKey, const String& strVal);
    void flush(){
        if (m_isChanged)
            saveFile();
    }
};

#endif
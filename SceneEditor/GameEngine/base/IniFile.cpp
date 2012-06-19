#include "IniFile.h"

const char sectionStart = '[';
const char sectionEnd = ']';
const String valSpace = "=";

IniValue::IniValue(const String& aKey, const String& aValue): isNote(false), key(aKey), value(aValue){
    
}

IniValue* Section::findKey(const String& strKey)
{
    for (unsigned int i = 0; i < list.size(); i++)
    {
        if (!list[i].isNote && strIsEqualIgnoreCase(trim(list[i].key), trim(strKey)))
        {
            return &list[i];
        }
    }
    return NULL;
}

IniFile::IniFile(const char* strFileName): m_txtFileName(strFileName), m_isChanged(false), m_isDecrypt(false)
{
    TextFileInputStream txtFile(strFileName);
    parseText(txtFile);
}

IniFile::IniFile(const char* strFileName, const bool isDocumentFile,const TDesKey& desKey, bool isDocDecrypt): m_txtFileName(strFileName), m_isChanged(false), m_isDecrypt(isDocDecrypt),m_desKey(desKey)
{
    BufInputStream* txtFile=0;
    if (isDocumentFile)
    {
        if (m_isDecrypt)
            txtFile=new MyTextFileInputStream(strFileName, m_desKey);
        else
            txtFile=new MyTextFileInputStream(strFileName);
    }
    else{
        txtFile=new MyTextFileInputStream(strFileName);
    }
    parseText(*txtFile);
    delete txtFile;
}

Section* IniFile::getSectionItem(long idx)
{
    if ((idx >= 0) && (idx < (long)m_sctList.size()))
    {
        return &m_sctList[idx];
    }
    return 0;
}

IniFile::~IniFile()
{
    if (m_isChanged)
        saveFile();
}

Section* IniFile::findSection(const String& strSection)
{
    for (unsigned int i = 0; i < m_sctList.size(); i++)
    {
        if (strIsEqualIgnoreCase(trim(m_sctList[i].name), trim(strSection)))
            return &m_sctList[i];
    }
    return NULL;
}

void IniFile::saveFile()
{
    MyTextFileOutputStream* pTxtFile;
    if (m_isDecrypt)
    {
        pTxtFile = new MyTextFileOutputStream(m_txtFileName.c_str(), m_desKey);
    }
    else pTxtFile = new MyTextFileOutputStream(m_txtFileName.c_str());
    for (unsigned int i = 0; i < m_sctList.size(); i++)
    {
        String strTmp(sectionStart + m_sctList[i].name + sectionEnd);
        pTxtFile->writeLine(strTmp.c_str());
        for (unsigned int j = 0; j < m_sctList[i].list.size(); j++)
        {
            if (m_sctList[i].list[j].isNote)
            {
                pTxtFile->writeLine(m_sctList[i].list[j].notes);
            }
            else
            {
                strTmp = m_sctList[i].list[j].key + valSpace + m_sctList[i].list[j].value;
                pTxtFile->writeLine(strTmp);
            }
        }
    }
    delete pTxtFile;
    m_isChanged=false;
}

Section* IniFile::getBackSection()
{
    if (m_sctList.size() == 0) m_sctList.push_back(Section(""));
    return &m_sctList.back();
}

void IniFile::parseText(BufInputStream& txtFile)
{
    //txtFile.seek(0, skBegin);
    clearList();
    const char notesFlag = ';';
    String strLine;
    while (txtFile.readLine(strLine))
    {
        trimTo(strLine);
        String& strTmp=strLine;
        long strTmp_size=(long)strTmp.size();
        //Notes
        if ((strTmp_size > 0) && (notesFlag != strTmp[0]))		
        {
            //Section
            if (strTmp_size >= 2)
            {
                if ((sectionStart == strTmp[0]) && (sectionEnd == strTmp[strTmp_size - 1]))
                {
                    String strSection;
                    trim(strTmp.c_str()+1,strTmp_size-2,strSection);
                    m_sctList.push_back(Section(strSection));
                }
                else
                {
                    long nIdxSpace = findStr(strTmp, valSpace);
                    if (nIdxSpace >= 0) {
                        String strKey;
                        trim(strTmp.c_str(),nIdxSpace,strKey);
                        
                        String strVal;
                        if ((nIdxSpace + 1) < strTmp_size) 
                            trim(strTmp.c_str()+nIdxSpace + 1,strTmp_size - nIdxSpace - 1,strVal);
                        getBackSection()->list.push_back(IniValue(strKey, strVal));
                    }
                    //else getBackSection()->list.push_back(IniValue(strLine));					
                }				
            }
            //else getBackSection()->list.push_back(IniValue(strLine));
        }
        else 
        {
            getBackSection()->list.push_back(IniValue(strLine));
        }
    }	
}


void IniFile::clearList()
{
    m_sctList.clear();
}


/////
bool Section::readString(const String& strKey, String& strVal){
    IniValue* value = findKey(strKey);
    if (value == 0) return false;
    strVal = value->value;
    return true;
}

void Section::writeString(const String& strKey,const String& strVal)
{
    IniValue* value = findKey(strKey);
    if (value == 0) 
    {
        list.push_back(IniValue(strKey, strVal));
    }else
    {
        value->value = strVal;
    }
}


///////


bool IniFile::readString(const String& strSection, const String& strKey, const String& strDefault, String& strVal){
    bool bTmp = false;
    Section* section = findSection(strSection);
    if (section != 0)
        bTmp = section->readString(strKey, strVal);
    if (!bTmp) strVal = strDefault;
    return bTmp;
}

void IniFile::writeString(const String& strSection, const String& strKey, const String& strVal)
{
    m_isChanged=true;
    Section* section=findSection(strSection);
    if (section == 0)
    {
        m_sctList.push_back(Section(strSection));
        m_sctList.back().writeString(strKey, strVal);
    }
    else
    {
        section->writeString(strKey, strVal);
    }
}



//
//  VFont.h
//
//  Created by housisong on 08-4-7.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//
#ifndef _VFont_h_
#define _VFont_h_

#include "../base/SysImportType.h"

struct VFont{
protected:
    String       m_name;
    long         m_size;
    bool		 m_isBold; //是否粗体
    bool		 m_isChanged;
    static const char* getDefaultFontName();
    void init(){
        m_name=getDefaultFontName();
        m_size=12;
        m_isBold=false;
        m_isChanged=true;
    }
    void changed() {
        m_isChanged=true;
    }
public:	
    explicit VFont(){
        init();
    }
    explicit VFont(long fontSize){
        init();
        setSize(fontSize);
    }
    explicit VFont(const VFont& srcFont){
        init();
        assign(srcFont);
    }
    explicit VFont(const String& fontName,long fontSize,bool fontIsBlod){
        init();
        setName(fontName);
        setSize(fontSize);
        setIsBold(fontIsBlod);
    }
    void assign(const VFont& srcFont){
        setName(srcFont.m_name);
        setSize(srcFont.m_size);
        setIsBold(srcFont.m_isBold);
    }
    VFont& operator=(const VFont& srcFont){
        assign(srcFont);
        return *this;
    }
    must_inline bool operator==(const VFont& srcFont)const {
        return (m_size==srcFont.m_size)&&(m_isBold==srcFont.m_isBold)&&(m_name==srcFont.m_name);		
    }
    must_inline bool operator!=(const VFont& srcFont)const {
        return !(*this==srcFont);		
    }
    
    must_inline bool operator <(const VFont& srcFont)const{
        if (m_size!=srcFont.m_size)
            return m_size<srcFont.m_size;
        else if (m_isBold!=srcFont.m_isBold)
            return (int)m_isBold < (int)srcFont.m_isBold;
        else
            return m_name < srcFont.m_name;	
    }
    
    inline unsigned long hashCode()const{
        int result=m_size*2;
        if (m_isBold) ++result;
        long strLength=(long)m_name.size();
        if (strLength>0){
            result+=::hashCode(m_name.c_str(),strLength)*31;
        }
        return result;
    }
    must_inline unsigned long hashCode(unsigned long src)const{
        return hashCode()*31+src;
    }
    void setName(const String& fontName){
        if (m_name!=fontName){
            m_name=fontName;
            changed();
        }
    }
    const String& getName()const{
        return m_name;
    }
    
    void setSize(long fontSize){
        if (m_size!=fontSize){
            m_size=fontSize;
            changed();
        }
    }
    long getSize()const{
        return m_size;
    }
    void setIsBold(bool fontIsBold){
        if (m_isBold!=fontIsBold){
            m_isBold=fontIsBold;
            changed();
        }
    }
    bool getIsBold()const{
        return m_isBold;
    }
    
    bool getIsChanged()const{
        return m_isChanged;
    }
    void updateChange(){
        m_isChanged=false;
    }
};



#endif //_VFont_h_
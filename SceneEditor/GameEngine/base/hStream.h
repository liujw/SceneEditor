//hStream.h

#ifndef _hStream_h_
#define _hStream_h_

#include "SysImportType.h"
#include "MyCrypt.h"

struct ErrorStream:public Exception{
public:
    virtual const char* what() const throw() { return "class ErrorStream"; }
};
struct ErrorReadStream:public ErrorStream{
public:
    virtual const char* what() const throw() { return "class ErrorReadStream"; }
};
struct ErrorWriteStream:public ErrorStream{
public:
    virtual const char* what() const throw() { return "class ErrorWriteStream"; }
};

struct IInputStream
{
protected:
    void operator =(const IInputStream& tmp);//error
public:
    virtual void* read(unsigned long& bCount)=0;
    virtual void* test_read(unsigned long& bCount)const=0; //尝试读一小块数据，但不移动读指针
    virtual ~IInputStream(){}
    
    inline void* read_trust(const unsigned long bCount) {
        unsigned long tmpReadCount=bCount; 
        void* result=read(tmpReadCount);
        if (tmpReadCount!=bCount)
            throw new ErrorReadStream();
        return result;
    }
    inline  void read(void* Dst,unsigned long& bCount) { void* data=read(bCount); memcpy(Dst,data,bCount); }
    inline  void read_trust(void* Dst,unsigned long bCount) {
        while (bCount>0){
            unsigned long readCount=bCount;
            read(Dst,readCount);
            bCount-=readCount;
            (UInt8*&)Dst+=readCount;
        }
    }
    virtual void skip(unsigned long& bCount) { read(bCount);  }
    virtual void skip_trust(const unsigned long bCount) { read_trust(bCount); }
    
    inline  bool test_read(void* Dst,unsigned long bCount)const{ 
        unsigned long readedCount=bCount;
        void* data=test_read(readedCount); 
        memcpy(Dst,data,readedCount); 
        return (readedCount==bCount);
    }
};

struct IOutputStream
{
protected:
    void operator =(const IOutputStream& tmp);//error
public:
    virtual void write(const void* SrcBuf,const unsigned long bCount)=0;
    virtual ~IOutputStream(){}
    
    void skip_fill(unsigned long bCount,UInt8 fillValue=0);
};


class  IRandInputStream:public IInputStream
{
protected:
public:
    virtual unsigned long getPos()const=0;
    virtual bool setPos(unsigned long pos)=0;
    virtual unsigned long getDataSize()const=0;
};

const UInt8 csTagLineEnd = 13;
const UInt8 csTagLineEndSkip = 10;

class  BufInputStream:public IRandInputStream
{
protected:
    long		m_readPosIndex;
    const UInt8*	m_BufBegin;
    const UInt8*	m_BufEnd;
    //解密数据 一般在开始read数据前调用一次
    void decrypt(const TDesKey& key);
public:
    explicit BufInputStream(const UInt8* aBufBegin,const UInt8* aBufEnd):m_readPosIndex(0),m_BufBegin(aBufBegin),m_BufEnd(aBufEnd){}
    virtual void* read(unsigned long& bCount);
    virtual void* test_read(unsigned long& bCount)const;
    inline void read(void* Dst,unsigned long& bCount) { void* pbuf=read(bCount); memcpy(Dst,pbuf,bCount); }
    
    virtual unsigned long getPos()const{
        return m_readPosIndex;
    }
    virtual bool setPos(unsigned long pos);
    virtual unsigned long getDataSize()const{
        return (unsigned long)(m_BufEnd-m_BufBegin);
    }
    bool readLine(String& out_result);
};


class  FileInputStream:public BufInputStream
{
protected:
    UInt8*	m_BufBck;
    void*	m_hFile;
    void*	m_MapHandle;
    bool    m_isTextFile;
public:
    explicit FileInputStream(const char* aFileName,bool isTextFile=false);
    virtual ~FileInputStream();
    const UInt8* getData(){
        return m_BufBck;
    }
};

class  TextFileInputStream:public FileInputStream
{
public:
    explicit TextFileInputStream(const char* aFileName):FileInputStream(aFileName,true){}
};


class  BufOutputStream:public IOutputStream
{
protected:
    long		m_WritePosIndex;
    UInt8*		m_BufBack;
    UInt8*		m_BufBegin;
    UInt8*		m_BufEnd;
    virtual void  new_capacity(long aNewCapacity);
    //加密数据 一般在write完数据后调用一次
    void encrypt(const TDesKey& key);
public:
    BufOutputStream(const long Capacity=0):m_WritePosIndex(0),m_BufBack(0),m_BufBegin(0),m_BufEnd(0){ if (Capacity>0) new_capacity(Capacity); }
    virtual ~BufOutputStream(){ delete[] (UInt8*)m_BufBack; }
    virtual void write(const void* SrcBuf,const unsigned long bCount);
    
    bool writeLine(const char* strLine,long strLength);
    must_inline bool writeLine(const char* strLine){
        return writeLine(strLine,strlen(strLine));
    }
    bool writeLine(const String& strLine){
        return writeLine(strLine.c_str(), strLine.size());
    }
};


class  LockVectorOutputStream:public IOutputStream
{
protected:
    Vector<UInt8>&	m_lockVector;
public:
    LockVectorOutputStream(Vector<UInt8>& lockVector):m_lockVector(lockVector){ }
    //virtual ~VectorOutputStream(){ }
    virtual void write(const void* SrcBuf,const unsigned long bCount){
        const UInt8* srcBegin=(const UInt8*)SrcBuf;
        m_lockVector.insert(m_lockVector.end(),srcBegin,srcBegin+bCount);
    }
};

class  FileOutputStream:public BufOutputStream
{
protected:
    void*	m_hFile;
    bool    m_isTextFile;
public:
    FileOutputStream(const char* aFileName,bool isTextFile=false):m_hFile((void*)csNullFileHandle),m_isTextFile(isTextFile)
    { m_hFile=file_create(aFileName); assert(m_hFile!=csNullFileHandle);  }
    virtual ~FileOutputStream();
};

class  TextFileOutputStream:public FileOutputStream
{
public:
    TextFileOutputStream(const char* aFileName):FileOutputStream(aFileName,true) {}
};


class MyDataFileInputStream:public BufInputStream{
protected:
    UInt8*  m_BufBck;
    bool    m_isTextFile;
    void init(const char* aFileName,bool  isDecrypt,const TDesKey& key,bool isTextFile);
public:
    MyDataFileInputStream(const char* aFileName,bool isTextFile=false):BufInputStream(0,0) { TDesKey key; init(aFileName,false,key,isTextFile); }
    MyDataFileInputStream(const char* aFileName,const TDesKey& key,bool isTextFile=false):BufInputStream(0,0) { init(aFileName,true,key,isTextFile);  }	
    virtual ~MyDataFileInputStream();
    const UInt8* getData(){
        return m_BufBck;
    }
};

class MyTextFileInputStream:public MyDataFileInputStream{
public:
    MyTextFileInputStream(const char* aFileName):MyDataFileInputStream(aFileName,true){}
    MyTextFileInputStream(const char* aFileName,const TDesKey& key):MyDataFileInputStream(aFileName,key,true) {}	
};

class MyDataFileOutputStream:public BufOutputStream{
protected:
    String  m_fileName;
    bool    m_isEncrypt;
    TDesKey m_key;
    bool    m_isTextFile;
    void init(const char* aFileName,bool  isEncrypt,const TDesKey& key,bool isTextFile);
    virtual void  new_capacity(long aNewCapacity);
public:
    MyDataFileOutputStream(const char* aFileName,bool isTextFile=false) { TDesKey key; init(aFileName,false,key,isTextFile); }	
    MyDataFileOutputStream(const char* aFileName,const TDesKey& key,bool isTextFile=false) { init(aFileName,true,key,isTextFile); }	
    virtual ~MyDataFileOutputStream();
};

class MyTextFileOutputStream:public MyDataFileOutputStream{
public:
    MyTextFileOutputStream(const char* aFileName):MyDataFileOutputStream(aFileName,true){}	
    MyTextFileOutputStream(const char* aFileName,const TDesKey& key):MyDataFileOutputStream(aFileName,key,true){}
};

#endif //_hStream_h_
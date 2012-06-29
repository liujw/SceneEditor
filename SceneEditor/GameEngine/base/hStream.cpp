//hStream.cpp
#include "hStream.h"
#include "CRC32.H"

/////////////
//utf8 文本标记处理
const char* utf8TextFileTag="\xEF\xBB\xBF"; //utf8 BOM: EF BB BF
const int utf8TextFileTagLength=3;
static void loadedTextFile(const UInt8*& bufBegin,const UInt8* bufEnd,String* out_gbk){
    assert(bufBegin!=bufEnd);
    bool isUtf8;
    if (bufEnd-bufBegin<utf8TextFileTagLength){
        isUtf8=false;
    }else{
        //check BOM
        isUtf8=true;
        for (int i=0;i<utf8TextFileTagLength;++i){
            if (bufBegin[i]!=(UInt8)utf8TextFileTag[i]){
                isUtf8=false;
                break;
            }
        }
    }
    
    out_gbk->clear();
    if (isUtf8){
        bufBegin+=utf8TextFileTagLength;
    }else{//兼容以前的gbk文本文件
        if (!isAsciiChars((const char*)bufBegin,bufEnd-bufBegin)){
            gbkToUtf8((const char*)bufBegin, (const char*)bufEnd,out_gbk);
        }
    }
}

static void beforeSaveTextFile(UInt8*& bufBegin,const UInt8* bufEnd){
    assert(bufEnd-bufBegin>=utf8TextFileTagLength);
    for (int i=0;i<utf8TextFileTagLength;++i)
        bufBegin[i]=utf8TextFileTag[i];
    bufBegin+=utf8TextFileTagLength;
}
static void beforeSaveTextFile(void* hFileHandle){
    file_write(hFileHandle,(const UInt8*)utf8TextFileTag,utf8TextFileTagLength);
}

/////////////

void IOutputStream::skip_fill(unsigned long bCount,UInt8 fillValue) { 
    if (bCount>=4){
        unsigned long nodeCount;
        if (bCount>32*4){
            if (bCount>1024*4){
                if (bCount>1024*64*4){
                    nodeCount=1024*64;
                }else
                    nodeCount=1024;
            }else
                nodeCount=32;
            
            Vector<UInt8> fillBuf(nodeCount,fillValue);
            for (;bCount>=nodeCount;bCount-=nodeCount)
                write(&fillBuf[0],nodeCount);
        }
        
        UInt32 fillData=fillValue|(fillValue<<8);
        fillData=fillData|(fillData<<16);
        for (;bCount>=4;bCount-=4)
            write(&fillData,4);
    }
    
    for (;bCount>=1;bCount-=1)
        write(&fillValue,1);
}

/////////


void  BufOutputStream::new_capacity(long aNewCapacity)
{
    long oldCapacity=(long)(m_BufEnd-m_BufBegin);
    if (aNewCapacity<=oldCapacity) return;
    if (aNewCapacity<oldCapacity*2) aNewCapacity=oldCapacity*2;
    const int csMinCapacity=16;
    if (aNewCapacity<csMinCapacity) aNewCapacity=csMinCapacity;
    assert(utf8TextFileTagLength<=csMinCapacity);
    
    const int headSize=(m_BufBegin-m_BufBack);
    UInt8* NewBuf=new UInt8[aNewCapacity+headSize];
    if (m_BufBack!=0){
        memcpy(NewBuf,m_BufBack,headSize+m_WritePosIndex);
        delete[] (UInt8*)m_BufBack;
    }
    m_BufBack=NewBuf;
    m_BufBegin=m_BufBack+headSize;
    m_BufEnd=&m_BufBegin[aNewCapacity];
    //m_WritePosIndex=m_WritePosIndex;
}


/////


bool BufInputStream::setPos(unsigned long pos){
    //if (((long)pos)<0) return false;
    if (m_BufBegin+pos>m_BufEnd) return false;
    m_readPosIndex=pos;
    return true;
}

void* BufInputStream::read(unsigned long& bCount){ 
    const UInt8* result=&m_BufBegin[m_readPosIndex]; 
    if (&result[bCount]>m_BufEnd)
        bCount=(unsigned long)(m_BufEnd-result); 
    m_readPosIndex+=bCount; 
    return (void*)result; 
}

void* BufInputStream::test_read(unsigned long& bCount)const{
    const UInt8* result=&m_BufBegin[m_readPosIndex]; 
    if (&result[bCount]>m_BufEnd)
        bCount=(unsigned long)(m_BufEnd-result); 
    //m_readPosIndex+=bCount; 
    return (void*)result; 
}

bool BufInputStream::readLine(String& out_result){
    out_result = "";
    if (m_BufBegin + m_readPosIndex >= m_BufEnd) return false; 
    long itEnd_R = posEx((const char*)m_BufBegin,(long)( m_BufEnd - m_BufBegin), m_readPosIndex, csTagLineEnd);
    long itEnd_N = posEx((const char*)m_BufBegin,(long)(m_BufEnd - m_BufBegin), m_readPosIndex, csTagLineEndSkip);
    
    long nOffset = MyBase::abs(itEnd_N - itEnd_R);
    long itEnd = -1;
    if (nOffset > 1)
    {
        if ((itEnd_R >= 0) && (itEnd_N >= 0))
        {
            if (itEnd_R > itEnd_N) itEnd = itEnd_N;
            else itEnd = itEnd_R;			
        }
        else
        {
            if (itEnd_R >= 0) itEnd = itEnd_R;
            else itEnd = itEnd_N;			
        }
    }
    else if (nOffset == 1)
    {
        if (itEnd_R > itEnd_N) itEnd = itEnd_R;
        else itEnd = itEnd_N;		
    }
    long new_readPosIndex;
    if (itEnd < 0){
        itEnd = (long)(m_BufEnd - m_BufBegin);
        new_readPosIndex =(long)(m_BufEnd - m_BufBegin);
    }
    else
    {		
        new_readPosIndex = itEnd + 1;
        if (nOffset == 1) --itEnd;
    }
    if ((itEnd - m_readPosIndex) > 0)
    {
        out_result.resize(itEnd - m_readPosIndex);
        memcpy(&out_result[0], &m_BufBegin[m_readPosIndex], out_result.size());
    }
    m_readPosIndex = new_readPosIndex;
    return true;
}

void BufInputStream::decrypt(const TDesKey& key){
    desDecrypt(m_BufBegin,(long)(m_BufEnd-m_BufBegin),key,(UInt8*)m_BufBegin);
}


void BufOutputStream::write(const void* SrcBuf,const unsigned long bCount)
{ 
    long newPos=m_WritePosIndex+bCount;
    
    if (newPos>(m_BufEnd-m_BufBegin))
    {
        new_capacity(newPos);
    }
    memcpy(&m_BufBegin[m_WritePosIndex],SrcBuf,bCount);
    m_WritePosIndex=newPos;
}


bool BufOutputStream::writeLine(const char* strLine, long strLength){
    if (strLine == NULL) return false;
    write(strLine,strLength);
    write(&csTagLineEnd, 1);
    write(&csTagLineEndSkip, 1);
    return true;
}

void BufOutputStream::encrypt(const TDesKey& key){
    desEncrypt(m_BufBack,(m_BufBegin-m_BufBack)+m_WritePosIndex,key,m_BufBack);
}

///////

FileInputStream::FileInputStream(const char* aFileName,bool isTextFile)
:BufInputStream(0,0),
m_BufBck(0),
m_hFile(0),
m_MapHandle(0),
m_isTextFile(isTextFile)
{
    m_hFile=file_open_read(aFileName);
    assert(m_hFile!=csNullFileHandle); 
    long int fSize=get_file_size(m_hFile);
    //m_MapHandle=file_map(m_hFile,(void**)&m_BufBegin);
    if (m_MapHandle==0 )//map failing
    {
        //then copy
        m_BufBck=new UInt8[fSize];
        m_BufBegin=m_BufBck;
        file_read(m_hFile,m_BufBck,fSize);
        file_close(m_hFile);
        m_hFile=(void*)csNullFileHandle;
    }
    m_BufEnd   =&m_BufBegin[fSize];
    
    if (m_isTextFile&&(m_BufEnd!=m_BufBegin)){
        String strBuf;
        loadedTextFile(m_BufBegin,m_BufEnd,&strBuf);
        if (!strBuf.empty()){
            delete m_BufBck;
            int fSize=strBuf.size();
            m_BufBck=new UInt8[fSize];
            memcpy(m_BufBck,&strBuf[0],fSize);
            m_BufBegin=m_BufBck;
            m_BufEnd  =&m_BufBegin[fSize];
        }
    }
}

FileInputStream::~FileInputStream()
{
    //if (m_MapHandle!=0 )
    //	file_map_free(m_MapHandle,(void**)&m_BufBegin);
    if (m_hFile!=csNullFileHandle ){
        file_close(m_hFile);
        m_hFile=(void*)csNullFileHandle;
    }
    if (m_BufBck!=0){
        delete [](UInt8*)m_BufBck;
        m_BufBck=0;
    }
}

FileOutputStream::~FileOutputStream() {
    if (m_isTextFile&&(m_WritePosIndex>0)) 
        beforeSaveTextFile(m_hFile);
    file_write(m_hFile,m_BufBegin,m_WritePosIndex);
    if (m_hFile!=csNullFileHandle) 
        file_close(m_hFile); 
}

/////////



void MyDataFileInputStream::init(const char* aFileName,bool  isDecrypt,const TDesKey& key,bool isTextFile){
    m_isTextFile=isTextFile;
    m_BufBck=0;
    
    long fSize=file_readMyData(aFileName,0,0);
    m_BufBck=new UInt8[fSize];
    file_readMyData(aFileName,m_BufBck,fSize);
    m_BufBegin=m_BufBck;
    m_BufEnd  =&m_BufBegin[fSize];
    
    /*if (isDecrypt){
     if (fSize>=4) {
     UInt32 crc_old=read4Byte(*(UInt32*)&m_BufEnd[-4]);
     UInt32 crc_new=CRC32_GetCRC(m_BufBegin,fSize-4);
     if (crc_old==crc_new){ //CRC32校验
     m_BufEnd-=4;
     decrypt(key);
     return;
     }
     }
     
     //文件转换成加密数据  自动转换!!!
     {
     MyDataFileOutputStream outFile(aFileName,key);
     outFile.write(m_BufBegin,(long)(m_BufEnd-m_BufBegin));
     }
     }*/
    
    if (isDecrypt){
        if (fSize>=4) {
            UInt32 crc_old=read4Byte(*(UInt32*)&m_BufEnd[-4]);
            UInt32 crc_new=0;//CRC32_GetCRC(m_BufBegin,fSize-4);
            if (crc_old==crc_new){ //CRC32校验
                m_BufEnd-=4;
                decrypt(key);
            }
        }
    }
    if (m_isTextFile&&(m_BufEnd!=m_BufBegin)){
        String strBuf;
        loadedTextFile(m_BufBegin,m_BufEnd,&strBuf);
        if (!strBuf.empty()){
            delete m_BufBck;
            int fSize=strBuf.size();
            m_BufBck=new UInt8[fSize];
            memcpy(m_BufBck,&strBuf[0],fSize);
            m_BufBegin=m_BufBck;
            m_BufEnd  =&m_BufBegin[fSize];
        }
    }
}

MyDataFileInputStream::~MyDataFileInputStream(){
    if (m_BufBck!=0)
        delete[]m_BufBck;
}



void MyDataFileOutputStream::init(const char* aFileName,bool  isEncrypt,const TDesKey& key,bool isTextFile){
    m_fileName=aFileName;
    m_isEncrypt=isEncrypt;
    m_key=key;
    m_isTextFile=isTextFile;
}

MyDataFileOutputStream::~MyDataFileOutputStream(){
    const int headSize=(m_BufBegin-m_BufBack);
    if ((m_isEncrypt&&(headSize+m_WritePosIndex>0))){
        encrypt(m_key);
        UInt32 crc_new=0;//CRC32_GetCRC(m_BufBack,headSize+m_WritePosIndex);
        write(&crc_new,4);
    }
    file_writeMyData(m_fileName.c_str(),m_BufBack,headSize+m_WritePosIndex);
}

void  MyDataFileOutputStream::new_capacity(long aNewCapacity)
{
    const bool isNeedAddTextHead=(m_isTextFile)&&(m_BufBegin==m_BufBack);
    if (isNeedAddTextHead)
        aNewCapacity+=utf8TextFileTagLength;
    BufOutputStream::new_capacity(aNewCapacity);
    if (isNeedAddTextHead){
        assert(m_WritePosIndex==0);
        beforeSaveTextFile(m_BufBegin,m_BufEnd);
    }
}





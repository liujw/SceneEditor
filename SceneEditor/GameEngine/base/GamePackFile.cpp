//GamePackFile.cpp

#include "GamePackFile.h"
#include "../../import/importInclude/FileImport.h"

inline String getSafePackFileName(const char* fileName){
    String str(fileName);
    //处理 双斜杠  
    //处理 '/'  to '\\'
    bool isInDirTag=false;
    long inserIndex=0;
    for (long i=0;i<(long)str.size();++i){
        char c=str[i];
        switch (c) {
            case '/':
            case '\\':
                if (!isInDirTag){
                    isInDirTag=true;
                    str[inserIndex]='\\';
                    ++inserIndex;
                }
                break;
            default:{
                isInDirTag=false;
                str[inserIndex]=lowerCaseChar(c);
                ++inserIndex;
            }
                break;
        }
    }
    str.resize(inserIndex);
    return str;
}

must_inline static bool readFile(void* packFileHandle,void* dst,long readSize){
    long readed=sys_file_read(packFileHandle,(UInt8*)dst,readSize);
    return (readed==readSize);
}
must_inline static UInt32 readInt(UInt8*& buf){
    UInt32 r= read4Byte(*(UInt32*)buf);
    buf+=4;
    return r;
}
must_inline static UInt32 readByte(UInt8*& buf){
    UInt32 r= buf[0];
    buf+=1;
    return r;
}
must_inline static void readDataPushStr(UInt8*& buf,String& dst_str,long readSize){
    dst_str.insert(dst_str.end(),(const char*)buf,(const char*)buf+readSize);
    buf+=readSize;
}


bool GamePackFile::loadPackData(void* packFileHandle,VLock* lock,const char* prefix){
    //tag
    const char* csPackTag="PakH";
    String tag(4,' ');
    if (!readFile(packFileHandle,&tag[0],4)) return false;
    if (tag!=csPackTag) return false;
    
    Int32 packFileSize=0;
    if (!readFile(packFileHandle,&packFileSize,4)) return false;
    Int32 headSize=0;
    if (!readFile(packFileHandle,&headSize,4)) return false;
    long packFileHeadSize=headSize;
    UInt32 crc32=0;
    if (!readFile(packFileHandle,&crc32,4)) return false; //为了速度不检查crc32
    
    long dirAndFileBufSize=headSize-4*4;
    Vector<UInt8> dirAndFileBuf;
    dirAndFileBuf.resize(dirAndFileBufSize);
    if (!readFile(packFileHandle,&dirAndFileBuf[0],dirAndFileBufSize)) return false;
    
    String tmpStr;
    tmpStr.reserve(1024);
    Vector<String> dirList;
    const char*  prefix_end=prefix;
    if (prefix!=0)
        prefix_end=prefix+strlen(prefix);
    
    UInt8* buf=&dirAndFileBuf[0];
    long dirCount=readInt(buf);
    for (long i=0;i<dirCount;++i){
        tmpStr.assign(prefix,prefix_end);
        long L=readByte(buf);
        readDataPushStr(buf,tmpStr,L);
        dirList.push_back(tmpStr);
    }
    
    //long fileCount=readInt(buf);
    readInt(buf); //fileCount
    for (long i=0;i<dirCount;++i){
        long dirFileCount=readInt(buf);
        for (long j=0;j<dirFileCount;++j){
            tmpStr=dirList[i];
            if (!tmpStr.empty()) tmpStr.push_back('\\');
            long L=readByte(buf);
            readDataPushStr(buf,tmpStr,L);
            
            TPackFileInfo* r=new TPackFileInfo();
            r->lock=lock;
            r->packFileHandle=packFileHandle;
            r->fileOffset=packFileHeadSize + readInt(buf);
            r->fileSize=readInt(buf);
            
            //TFileMap::iterator it=m_fileMap.find(tmpStr);
            //if (it!=m_fileMap.end()){
            //	delete it->second;
            //	//m_fileMap.erase(it);
            //}
            m_fileMap[tmpStr]=r;
        }
    }
    
    for (long i=0;i<dirCount;++i){
        TPackFileInfo* r=new TPackFileInfo();
        r->packFileHandle=packFileHandle;
        
        //TFileMap::iterator it=m_dirSet.find(dirList[i]);
        //if (it!=m_dirSet.end()){
        //	delete it->second;
        //	//m_dirSet.erase(it);
        //}
        m_dirSet[dirList[i]]=r;
    }
    return true;
}

bool GamePackFile::loadPackFile(const char* packFileName,const char* prefix){
    //closePackFile();
    void* packFileHandle=sys_file_open_read(packFileName,0);
    if (packFileHandle==csNullFileHandle) return false;
    
    VLock* lock=new VLock();
    if (!loadPackData(packFileHandle,lock,prefix)){
        sys_file_close(packFileHandle);
        delete lock;
        return false;
    }else{
        m_fileHandleList.push_back(packFileHandle);
        m_fileLockList.push_back(lock);
        m_fileNameList.push_back(packFileName);
        return true;
    }
}


void GamePackFile::delFileByHandle(TFileMap& fmap,void* packFileHandle){
    Vector<TPackFileInfo*> fileList;
    TFileNameList	fileNameList;
    for (TFileMap::iterator it=fmap.begin();it!=fmap.end();++it){
        if (it->second->packFileHandle==packFileHandle){
            delete it->second;
        }else{
            fileNameList.push_back(it->first);
            fileList.push_back(it->second);
        }
    }
    fmap.clear();
    for (long i=0;i<(long)fileNameList.size();++i){
        fmap.insert(TFileMap::value_type(fileNameList[i],fileList[i]));
    }
    fileList.clear();
    fileNameList.clear();
}

void GamePackFile::closePackFile(const char* packFileName){
    long del_i=-1;
    for (long i=0;i<(long)m_fileNameList.size();++i){
        if (m_fileNameList[i]==packFileName){
            del_i=i;
            break;
        }
    }
    if (del_i<0) return;
    
    void* packFileHandle=m_fileHandleList[del_i];
    sys_file_close(packFileHandle);
    delete m_fileLockList[del_i];
    m_fileNameList.erase(m_fileNameList.begin()+del_i);
    m_fileHandleList.erase(m_fileHandleList.begin()+del_i);
    m_fileLockList.erase(m_fileLockList.begin()+del_i);
    
    delFileByHandle(m_fileMap,packFileHandle);
    delFileByHandle(m_dirSet,packFileHandle);
}

void GamePackFile::closeAllPackFile(){
    for (long i=0;i<(long)m_fileHandleList.size();++i){
        sys_file_close(m_fileHandleList[i]);
        delete m_fileLockList[i];
    }
    m_fileHandleList.clear();
    m_fileLockList.clear();
    m_fileNameList.clear();
    
    for (TFileMap::iterator it=m_dirSet.begin();it!=m_dirSet.end();++it)
        delete it->second;
    m_dirSet.clear();
    for (TFileMap::iterator it=m_fileMap.begin();it!=m_fileMap.end();++it)
        delete it->second;
    m_fileMap.clear();
}

GamePackFile::GamePackFile(){
}

GamePackFile::~GamePackFile(void){
    closeAllPackFile();
}

void* GamePackFile::file_open_read(const char* fileName){
    TFileMap::iterator it=m_fileMap.find(getSafePackFileName(fileName));
    if (it==m_fileMap.end()) return csNullFileHandle;
    return it->second;
}

bool  GamePackFile::file_exists(const char* fileName){
    TFileMap::iterator it=m_fileMap.find(getSafePackFileName(fileName));
    return (it!=m_fileMap.end());
}

bool  GamePackFile::path_exists(const char* pathName){
    TFileMap::iterator it=m_dirSet.find(getSafePackFileName(pathName));
    return (it!=m_dirSet.end());
}

long GamePackFile::get_file_size(void* fileHandle){
    if (fileHandle==csNullFileHandle) return 0;
    TPackFileInfo* pfi=(TPackFileInfo*)fileHandle;
    return pfi->fileSize;
}

long GamePackFile::file_read(void* fileHandle,long pos,UInt8* dst,unsigned long readSize){
    if (pos<0) return 0;
    if (fileHandle==csNullFileHandle) return 0;
    TPackFileInfo* pfi=(TPackFileInfo*)fileHandle;
    if (pos+(long)readSize > pfi->fileSize)
        readSize=pfi->fileSize-pos;
    if (readSize<=0) return 0;
    
    VAutoLock _AutoLock(*(pfi->lock));
    {
        /*static volatile long lc=0;
         while (lc>0) {
         VThread::sleep_s(0.001);
         }
         ++lc;*/
        long r= sys_file_read_from(pfi->packFileHandle,pfi->fileOffset+pos,dst,readSize);
        //--lc;
        return r;
    }
}

void GamePackFile::file_close(void* fileHandle){
    //do nothing
}


//GamePackFile.h

#ifndef _GamePackFile_h_
#define _GamePackFile_h_
#include "SysImportType.h"
#include "VThread.h"

class GamePackFile;

struct TPackHandleData{
    GamePackFile*	packHandle;
    void*			fileHandle;
    long			fileReadPos;
    TPackHandleData()
    :fileHandle(0),packHandle(0),fileReadPos(0){
    }
};

struct TPackFileInfo{
    void*	packFileHandle;
    long	fileOffset;
    long	fileSize;
    VLock*	lock;
    TPackFileInfo():packFileHandle(0),fileOffset(0),fileSize(0),lock(0){
    }
};

class GamePackFile{
private:
    typedef Vector<void*> TFileHandleList;
    typedef Vector<VLock*> TFileLockList;
    typedef Vector<String> TFileNameList;
    //typedef HashMultiMap<String,TPackFileInfo*> TFileMap;
    typedef HashMap<String,TPackFileInfo*> TFileMap;
    
    TFileHandleList m_fileHandleList;
    TFileLockList   m_fileLockList;
    TFileNameList	m_fileNameList;
    TFileMap		m_dirSet;
    TFileMap		m_fileMap;
    bool loadPackData(void* packFileHandle,VLock* lock,const char* prefix);
    void delFileByHandle(TFileMap& fmap,void* packFileHandle);
public:
    GamePackFile();
    bool loadPackFile(const char* packFileName,const char* prefix=0);
    void closePackFile(const char* packFileName);
    
    void closeAllPackFile();
    virtual ~GamePackFile(void);
    
    void* file_open_read(const char* fileName);
    bool  file_exists(const char* fileName);
    bool  path_exists(const char* pathName);
    long get_file_size(void* fileHandle);
    long file_read(void* fileHandle,long pos,UInt8* dst,unsigned long readSize);
    void file_close(void* fileHandle);
};


#endif
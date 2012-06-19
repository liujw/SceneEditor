//
//  INetLink.h
//
//  INetLink 代表一个底层连接通道
//
#ifndef _INetLink_h_
#define _INetLink_h_

//link interface
#ifdef __cplusplus 
extern "C" {
#endif
    
    typedef enum  TNetLinkEventType{ net_Linked=1, net_Error=2, net_UnLinked=3 }TNetLinkEventType;
    typedef enum  TNetLinkErrorEvnetData { net_Error_Sockt=1,net_Error_Host=2,net_Error_Connect=3,net_Error_Send=4,net_Error_Read=5,net_Error_Read_UnLink=6 ,net_Error_Bind=7,net_Error_Listen=8 }TNetLinkErrorEvnetData;
    typedef void* TNetLinkImportHandle;
    
    typedef void (*TNetLinkEventCallBackProc)(TNetLinkImportHandle sender,void* userData,TNetLinkEventType netLinkEventType,long eventData);
    
#ifdef __cplusplus 
}
#endif

class INetLink{
public:
    virtual bool connect(const char* hostName,long port)=0;
    virtual void setOnEventCallBackProc(TNetLinkEventCallBackProc callBackProc,void* userData)=0;
    virtual bool sendData(const unsigned char* data,long length)=0;
    virtual bool getIsReceivedData()=0;
    virtual bool readData(unsigned char* out_data,long read_length)=0;
    virtual void close()=0;
    virtual ~INetLink(){}
};

//class INetHostLink

#endif //_INetIO_h_

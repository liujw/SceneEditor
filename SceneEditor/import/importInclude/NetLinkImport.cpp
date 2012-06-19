//NetLinkImport.cpp


#include "../importInclude/NetLinkImport.h"
#include "SysImport.h"
#ifdef Set
#undef Set
#endif
#ifdef WINDOWS
#ifndef INITGUID
#define INITGUID
#endif
#endif
#include "ThreadImport.h"

#include <stdlib.h>
#include <string.h>

#ifdef _MZPhone
#include <mzfc_inc.h>
#include <CallNotifyApi.h>
#include<wininet.h>
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"CallInterface.lib") 
//#pragma comment(lib,"PhoneAdapter.lib") 
#include "VThread.h"
extern const void* getMainFormHandle();
#endif

#ifdef WINDOWS
#ifdef WINCE
#include <Winsock2.h>
#pragma comment (lib, "ws2.lib")//winsock.lib "Ws2.lib"
const WORD wVersionRequested = MAKEWORD(2,2); //0x0202
#ifndef _MZPhone
#include <connmgr.h>

//#pragma comment (lib, "Cellcore.lib")
typedef HRESULT (WINAPI *T_ConnMgrConnectionStatus)(HANDLE hConnection,DWORD *pdwStatus);
typedef HRESULT (WINAPI *T_ConnMgrEstablishConnectionSync)(CONNMGR_CONNECTIONINFO *pConnInfo,HANDLE *phConnection,DWORD dwTimeout,DWORD *pdwStatus);
typedef HRESULT (WINAPI *T_ConnMgrEnumDestinations)(int nIndex, CONNMGR_DESTINATION_INFO *pDestInfo);
typedef HRESULT (WINAPI *T_ConnMgrReleaseConnection)(HANDLE hConnection,LONG lCache);
typedef HRESULT (WINAPI *T_ConnMgrMapURL)(LPCTSTR pwszURL, GUID *pguid, DWORD *pdwIndex );
typedef HANDLE  (WINAPI *T_ConnMgrApiReadyEvent)();
T_ConnMgrConnectionStatus			ConnMgrConnectionStatus_load		;
T_ConnMgrEstablishConnectionSync	ConnMgrEstablishConnectionSync_load	;
T_ConnMgrEnumDestinations			ConnMgrEnumDestinations_load		;
T_ConnMgrReleaseConnection			ConnMgrReleaseConnection_load		;
T_ConnMgrMapURL						ConnMgrMapURL_load					;
T_ConnMgrApiReadyEvent				ConnMgrApiReadyEvent_load			;
bool LoadConnMgrProc(){
    static bool isLoaded=false;
    static bool isCanUse=false;
    if (isLoaded)
        return isCanUse;
    
    isLoaded=true;
    HINSTANCE mhdll=LoadLibrary(TEXT("Cellcore.dll"));
    isCanUse=(mhdll!=0);
    if (isCanUse) {
        ConnMgrConnectionStatus_load		=(T_ConnMgrConnectionStatus			)GetProcAddress(mhdll,TEXT("ConnMgrConnectionStatus"));
        ConnMgrEstablishConnectionSync_load	=(T_ConnMgrEstablishConnectionSync	)GetProcAddress(mhdll,TEXT("ConnMgrEstablishConnectionSync"));
        ConnMgrEnumDestinations_load		=(T_ConnMgrEnumDestinations			)GetProcAddress(mhdll,TEXT("ConnMgrEnumDestinations"));
        ConnMgrReleaseConnection_load		=(T_ConnMgrReleaseConnection		)GetProcAddress(mhdll,TEXT("ConnMgrReleaseConnection"));
        ConnMgrMapURL_load					=(T_ConnMgrMapURL					)GetProcAddress(mhdll,TEXT("ConnMgrMapURL"));
        ConnMgrApiReadyEvent_load			=(T_ConnMgrApiReadyEvent			)GetProcAddress(mhdll,TEXT("ConnMgrApiReadyEvent"));
        isCanUse=isCanUse&&(ConnMgrConnectionStatus_load!=0);
        isCanUse=isCanUse&&(ConnMgrEstablishConnectionSync_load!=0);
        isCanUse=isCanUse&&(ConnMgrEnumDestinations_load!=0);
        isCanUse=isCanUse&&(ConnMgrReleaseConnection_load!=0);
        isCanUse=isCanUse&&(ConnMgrMapURL_load!=0);
        isCanUse=isCanUse&&(ConnMgrApiReadyEvent_load!=0);
    }
    return isCanUse;
}

#endif
#else   
#include <Winsock.h>
#pragma comment (lib, "Ws2_32.lib")
const WORD wVersionRequested = MAKEWORD(1,1); //0x0101
#endif
extern void VGame_outDebugInfo(const char* text,bool isClearOld);

inline static void debug_out_NetLastErrorCode(const char* tag){
    /*int errCode=WSAGetLastError();
     char text[512];
     ::sprintf(text,"%s: %d",tag,errCode);
     VGame_outDebugInfo(text,false);*/
}

class CAutoNetRes{
private:
    bool m_isInited;
    bool m_isStartGprs;
    bool m_isWSAStartup;
    void* m_hConnection;
    void doInit(){
        m_isStartGprs=false;
        m_isWSAStartup=false;
        m_hConnection=0;
        
#ifdef WINCE 
#ifdef _MZPhone 
        DWORD dwNetWorkStatus=::QueryNetWorkStatus();
        if(dwNetWorkStatus == NETWORK_NONE){
            //连接EDGE网络...;
            if(RESULT_OK == Dial_StartGprsConnect2((HWND)getMainFormHandle(),GPRS_FORCE_APP_TYPE)) {
                m_isStartGprs = true;
            } else {
                //"连接失败！请检查EDGE网络";
                //return;
            }
        }
#else
        if (LoadConnMgrProc())
            toActiveNet();
#endif
#endif
        
        //版本协商     
        WSADATA wsaData;     
        int err = WSAStartup(wVersionRequested,&wsaData);   
        if(err!=0)   
            return ; 
        //if(wVersionRequested!=MAKEWORD(LOBYTE(wsaData.wVersion),HIBYTE(wsaData.wVersion))) {   
        //	WSACleanup();
        //	return ;   
        //}
        m_isWSAStartup=true;
        
        //ok
    }
    void doClear(){
        if (m_isWSAStartup){
            WSACleanup();
            m_isWSAStartup=false;
        }
        if (m_isStartGprs){
#ifdef WINCE 
#ifdef _MZPhone 
            Dial_StopGprsConnect2((HWND)getMainFormHandle());
#else
            if (m_hConnection){
                //关于如何关闭，好像接口是有点问题。我是这样做的
                //通过 RasEnumConnections查询当前的链接，找到匹配的链接然后调用RasHangUp就可以断开了
                if (LoadConnMgrProc())
                    ConnMgrReleaseConnection_load(m_hConnection, FALSE);
                m_hConnection=0;
            }
#endif
#endif
            m_isStartGprs=false;
        }
    }
    
#ifdef WINCE 
#ifndef _MZPhone 
    bool toActiveNet(){
        if (!getConnMgrAvailable()) return false;//检查连接管理是否可用
        
        //用远程URL映射的方式来完成，这样可以让系统自动选取一个最好的连接
        GUID atNetworkDest_GUID;
        memset ( &atNetworkDest_GUID, 0, sizeof(GUID) );
        DWORD nIndex = 0;
#define csHostURL TEXT("http://80166.com")
        HRESULT hResult =ConnMgrMapURL_load(csHostURL,&atNetworkDest_GUID,&nIndex);
        if ( FAILED(hResult) ) {
            //GetLastError ();
            return false;
        }
        
        //启用指定编号的连接
        CONNMGR_CONNECTIONINFO ConnInfo;
        memset(&ConnInfo,0,sizeof(ConnInfo));
        ConnInfo.cbSize = sizeof(ConnInfo);
        ConnInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
        ConnInfo.dwFlags = 0;//CONNMGR_FLAG_PROXY_HTTP | CONNMGR_FLAG_PROXY_WAP | CONNMGR_FLAG_PROXY_SOCKS4 | CONNMGR_FLAG_PROXY_SOCKS5;
        ConnInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
        ConnInfo.guidDestNet = atNetworkDest_GUID;
        ConnInfo.bExclusive  = FALSE; //是否独占  FALSE,与其他连接共享一个线路
        ConnInfo.bDisabled = FALSE;
        
        const long waitAllTime= 10*1000; //10秒
        DWORD dwStatus = 0;
        hResult = ConnMgrEstablishConnectionSync_load(&ConnInfo, &m_hConnection,waitAllTime, &dwStatus );
        if(FAILED(hResult)) {
            ConnInfo.guidDestNet ==IID_DestNetInternet;	 //另外尝试			
            hResult = ConnMgrEstablishConnectionSync_load(&ConnInfo, &m_hConnection,waitAllTime, &dwStatus );
            if(FAILED(hResult)) {
                m_hConnection=0;
                return false;
            }
        }
        
        //检查连接状态
        DWORD dwStartTime = GetTickCount();
        BOOL bRet = FALSE;
        while ( GetTickCount ()-dwStartTime <=waitAllTime) {
            DWORD dwStatus = 0;
            HRESULT hr = ConnMgrConnectionStatus_load ( m_hConnection, &dwStatus );
            if ( SUCCEEDED(hr) ) {
                if ( dwStatus == CONNMGR_STATUS_CONNECTED ) {
                    bRet = TRUE;
                    break;
                }
            }
            Sleep(100);
        }
        return bRet!=0;
    }
    
    bool getConnMgrAvailable(){ 
        HANDLE hConnMgr = ConnMgrApiReadyEvent_load();
        BOOL bAvailbale = FALSE;
        DWORD dwResult = ::WaitForSingleObject ( hConnMgr,2000);
        if ( dwResult == WAIT_OBJECT_0 ){
            bAvailbale = TRUE;
        }
        if ( hConnMgr ) CloseHandle( hConnMgr );// 关闭
        return bAvailbale!=0;
    }
#endif
#endif
public:
    CAutoNetRes():m_isInited(false){}
    ~CAutoNetRes(){ tryClear(); }
    inline void tryInit(){
        if (m_isInited) return;
        doInit();
        m_isInited=true;
    }
    void tryClear(){
        if (!m_isInited) return;
        doClear();
        m_isInited=false;
    }
};

CAutoNetRes _autoNetRes;
void g_netLinkClear(){
    _autoNetRes.tryClear();
}
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#ifdef _WIN64
typedef Int64 TSOCKET;
#else
typedef int TSOCKET;
#endif

const TSOCKET csNullTcp=-1;

typedef struct TCPConnection{
    TSOCKET						sockFD;
    struct sockaddr_in			addr;
    struct hostent *			he; 
    TNetLinkEventCallBackProc	callBack;
    void*						userData;
    
    UInt8						testReadedBuf;
    bool						isTestReadedBuf;
    bool isTestReaded()const{
        return isTestReadedBuf;
    }
    void pushTestReaded(UInt8 data){
        //assert(!isTestReadedBuf);
        isTestReadedBuf=true;
        testReadedBuf=data;
    }
    long popTestReaded(UInt8* out_data,long read_length){
        if (read_length<=0) return 0;
        if (isTestReadedBuf){
            out_data[0]=testReadedBuf;
            isTestReadedBuf=false;
            return 1;
        }else{
            return 0;
        }
    }
} TCPConnection;

static TSOCKET _getNewTCPSockt(){
#ifdef WINDOWS
    _autoNetRes.tryInit();
#endif
    
    TSOCKET sockHandle=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    return sockHandle;
}

static void _closeTCPSockt(TSOCKET sockFD){
#if defined(_IOS) || defined(_MACOSX)
    close(sockFD);
#else
    closesocket(sockFD);
#endif
}


#ifdef _AutoLockTCP_
//m8上 关闭连接后第二次不能打开  
//  所以特殊处理  始终保有一个句柄    !无效

class VAutoLockTCP{
private:
    TSOCKET  m_oldTcp;
public:
    VAutoLockTCP():m_oldTcp(csNullTcp){
    }
    ~VAutoLockTCP(){
        if (m_oldTcp!=csNullTcp){
            lockTcp(csNullTcp);
        }
    }
    void lockTcp(TSOCKET  newTcp){
        TSOCKET  oldTcp=m_oldTcp;
        m_oldTcp=newTcp;
        if (csNullTcp!=oldTcp)
            _closeTCPSockt(oldTcp);
    }
};

static VAutoLockTCP autoLockTCP;

TSOCKET getNewTCPSockt(){
    return _getNewTCPSockt();
}

void closeTCPSockt(TSOCKET sockFD){
    autoLockTCP.lockTcp(sockFD);
}

#else
static TSOCKET getNewTCPSockt(){
    return _getNewTCPSockt();
}

static void closeTCPSockt(TSOCKET sockFD){
    _closeTCPSockt(sockFD);
}
#endif


static TCPConnection* TCPConnection_create(){
    //int sockFD=getNewTCPSockt();
    //if (sockFD ==csNullTcp) return 0;
    
    TCPConnection* tcp=(TCPConnection*)malloc(sizeof(TCPConnection));
    memset(tcp,0,sizeof(TCPConnection));
    tcp->sockFD=csNullTcp;
    return tcp;
}

static void TCPConnection_close(TCPConnection* tcp){
    if (tcp==0) return;
    //if (tcp->he!=0)
    //TODO:?	freehostent(tcp->he);
    if (tcp->sockFD!=csNullTcp)
        closeTCPSockt (tcp->sockFD);
    memset(tcp,0,sizeof(TCPConnection));
    tcp->sockFD=csNullTcp;
}

static void TCPConnection_delete(TCPConnection* tcp){
    TCPConnection_close(tcp);
    free(tcp);
}


static bool TCPConnection_initSockt(TCPConnection* tcp,const char* hostName,long port){
    if (tcp->sockFD==csNullTcp){
        tcp->sockFD=getNewTCPSockt();
        if (tcp->sockFD==csNullTcp) {
            if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Sockt);
            return false;
        }
    }
    
    tcp->addr.sin_family = AF_INET;
    tcp->addr.sin_port = htons ((unsigned short)port);
    UInt32& t_s_addr=*(UInt32*)&(tcp->addr.sin_addr);
#if defined(_IOS) || defined(_MACOSX) 
    t_s_addr=INADDR_NONE;
#else
    t_s_addr = inet_addr(hostName); 
#endif
    if (t_s_addr==INADDR_NONE){
        tcp->he = gethostbyname(hostName);
        if (!tcp->he) {
            if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Host);
            return false;
        }
        memcpy(&t_s_addr, tcp->he->h_addr,4);
    }
    
    //char text[512];
    //::sprintf(text,"ip:%d.%d.%d.%d.",tcp->addr.sin_addr.S_un.S_un_b.s_b1,tcp->addr.sin_addr.S_un.S_un_b.s_b2,tcp->addr.sin_addr.S_un.S_un_b.s_b3,tcp->addr.sin_addr.S_un.S_un_b.s_b4);
    //VGame_outDebugInfo(text,false);
    
    return true;
}

static bool TCPConnection_connect(TCPConnection* tcp,const char* hostName,long port){
    if (!TCPConnection_initSockt(tcp,hostName,port)) return false;
    
    //connect
    if (connect(tcp->sockFD, (struct sockaddr *)&tcp->addr, sizeof(tcp->addr)) < 0) {
        //debug_out_NetLastErrorCode("connect:");
        if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Connect);
        return false;
    }
    
    if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Linked,0);	
    return true;
}


static bool TCPConnection_Host_listen(TCPConnection* tcp,const char* myHostName,long hostPort,long queueLinkPeerCount){
    if (!TCPConnection_initSockt(tcp,myHostName,hostPort)) return false;
    
    //bind
    if (bind(tcp->sockFD, (struct sockaddr *)&tcp->addr, sizeof(tcp->addr)) < 0) {
        if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Bind);
        return false;
    }
    
    //listen
    if (listen(tcp->sockFD,queueLinkPeerCount) < 0) {
        if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Listen);
        return false;
    }
    
    return true;
}

static TCPConnection* TCPConnection_Host_checkNewPeerLink(TCPConnection* tcp,const char*& out_peerName,long& peerPort){
    
#ifdef WIN32 
    typedef int TAcceptSizeType;
#else
    typedef unsigned int TAcceptSizeType;
#endif
    
    struct sockaddr	peer_addr;
    TAcceptSizeType peer_addrLen=(TAcceptSizeType)sizeof(peer_addr);
    memset(&peer_addr,0,peer_addrLen);
    
    TSOCKET peer_sockFD=accept(tcp->sockFD,&peer_addr,&peer_addrLen);
    if (peer_sockFD<0)
        return 0;
    else {
        TCPConnection* peerTCP=TCPConnection_create();
        peerTCP->sockFD=peer_sockFD;
        peerTCP->addr=*(sockaddr_in*)&peer_addr;
        peerTCP->he=gethostbyaddr((const char*)&(peerTCP->addr.sin_addr),sizeof(peerTCP->addr.sin_addr),AF_INET);
        out_peerName=peerTCP->he->h_name;
        //const char* peerName=inet_ntoa(peerTCP->addr.sin_addr);
        //out_peerName=peerName;
        peerPort=peerTCP->addr.sin_port;
        
        return peerTCP;
    }
}


static bool TCPConnection_send(TCPConnection* tcp,const unsigned char* data,long length){
    while (length>0){
        long slength=send(tcp->sockFD,(const char*)data,length,0);
        if (slength<0){
            //debug_out_NetLastErrorCode("send:");
            if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Send);
            if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_UnLinked,0);	
            return false;
        }else if (slength==0)
            curThread_sleep_s(0.001);
        else{
            data+=slength;
            length-=slength;
            curThread_sleep_s(0);
        }
    }
    return true;
}

static void TCPConnection_doReadError(TCPConnection* tcp,long rlength){
    if (rlength<=0){
        if (rlength<0)
            if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Read);
            else
                if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_Error,(long)net_Error_Read_UnLink);				
        if (tcp->callBack!=0) tcp->callBack(tcp,tcp->userData,net_UnLinked,0);	
    }			
}
static bool TCPConnection_is_can_read(TCPConnection* tcp){
    unsigned char data;
    if (tcp->isTestReaded())
        return true;
#ifdef WINCE
    int rlength=recv(tcp->sockFD,(char*)&data,1,0);
    if (rlength>0)
        tcp->pushTestReaded(data);
#else 
    int rlength=recv(tcp->sockFD,(char*)&data,1,MSG_PEEK);
#endif
    if (rlength<0){
        //debug_out_NetLastErrorCode("can read:");
        TCPConnection_doReadError(tcp,rlength);
    }
    return (rlength>0);
}


static bool TCPConnection_read(TCPConnection* tcp,unsigned char* out_data,long read_length){	
    if (tcp->isTestReaded()){
        long rlength=tcp->popTestReaded(out_data,read_length);
        out_data+=rlength;
        read_length-=rlength;
    }
    
    while (read_length>0){
        long rlength=recv(tcp->sockFD,(char*)out_data,read_length,0);
        if (rlength<0){
            //debug_out_NetLastErrorCode("connect:");
            TCPConnection_doReadError(tcp,rlength);
            return false;
        }else if (rlength==0)
            curThread_sleep_s(0.001);
        else{
            out_data+=rlength;
            read_length-=rlength;
            curThread_sleep_s(0);
        }
    } 
    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////


TNetLinkImportHandle netLink_create(){
    TCPConnection* tcp=TCPConnection_create();
    return tcp;
}

void netLink_delete(TNetLinkImportHandle netLinkImportHandle){
    TCPConnection_delete((TCPConnection*)netLinkImportHandle);
}

void netLink_close(TNetLinkImportHandle netLinkImportHandle){
    TCPConnection_close((TCPConnection*)netLinkImportHandle);
}

void netLink_setOnEventCallBackProc(TNetLinkImportHandle netLinkImportHandle,TNetLinkEventCallBackProc callBackProc,void* userData){
    TCPConnection* tcp=(TCPConnection*)netLinkImportHandle;
    if (tcp==0) return;
    tcp->callBack=callBackProc;
    tcp->userData=userData;
}

bool netLink_connect(TNetLinkImportHandle netLinkImportHandle,const char* hostName,long port){
    TCPConnection* tcp=(TCPConnection*)netLinkImportHandle;
    if (tcp==0) return false;
    return TCPConnection_connect(tcp,hostName,port);
}

bool netLink_sendData(TNetLinkImportHandle netLinkImportHandle,const unsigned char* data,long length){
    TCPConnection* tcp=(TCPConnection*)netLinkImportHandle;
    if (tcp==0) return false;
    return TCPConnection_send(tcp,data,length);
}

bool netLink_getIsReceivedData(TNetLinkImportHandle netLinkImportHandle){
    TCPConnection* tcp=(TCPConnection*)netLinkImportHandle;
    if (tcp==0) return false;
    return TCPConnection_is_can_read(tcp);
}

bool netLink_readData(TNetLinkImportHandle netLinkImportHandle,unsigned char* out_data,long read_length){
    TCPConnection* tcp=(TCPConnection*)netLinkImportHandle;
    if (tcp==0) return false;
    return TCPConnection_read(tcp,out_data,read_length);
}

////////

TNetHostImportHandle netHost_create(){
    return (TNetHostImportHandle*)netLink_create();
}
void netHost_setOnEventCallBackProc(TNetHostImportHandle netHostImportHandle,TNetLinkEventCallBackProc callBackProc,void* userData){
    netLink_setOnEventCallBackProc((TNetHostImportHandle*)netHostImportHandle,callBackProc,userData);
}
void netHost_close(TNetHostImportHandle netHostImportHandle){
    netLink_close((TNetLinkImportHandle*)netHostImportHandle);
}
void netHost_delete(TNetHostImportHandle netHostImportHandle){
    netLink_delete((TNetLinkImportHandle*)netHostImportHandle);
}

const char* netHost_getDefaultMyHostName(){
    const long  hostNameLength=256;
    static bool is_hostName_init=false;
    static char hostName[hostNameLength+1];
    if (!is_hostName_init){
        bool result=gethostname(hostName,hostNameLength)<0;
        if (result)
            is_hostName_init=true;
    }
    
    if (is_hostName_init)
        return &hostName[0];
    else
        return 0;
}

bool netHost_listen(TNetHostImportHandle netHostImportHandle,const char* myHostName,long hostPort,long queueLinkPeerCount){
    TCPConnection* tcp=(TCPConnection*)netHostImportHandle;
    if (tcp==0) return false;
    return TCPConnection_Host_listen(tcp,myHostName,hostPort,queueLinkPeerCount);
}

TNetLinkImportHandle netHost_checkNewPeerLink(TNetHostImportHandle netHostImportHandle,const char*& out_peerName,long& peerPort){
    TCPConnection* tcp=(TCPConnection*)netHostImportHandle;
    if (tcp==0) return 0;
    return TCPConnection_Host_checkNewPeerLink(tcp,out_peerName,peerPort);
}

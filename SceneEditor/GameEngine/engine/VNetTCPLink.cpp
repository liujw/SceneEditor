//VNetTCPLink.cpp

#include "VNetTCPLink.h"
//#include "../../import/importInclude/NetLinkImport.h"


VNetTCPLink::VNetTCPLink():m_Import(0){
    //m_Import=netLink_create();
}

VNetTCPLink::~VNetTCPLink(){
    //if (m_Import!=0){
    //    netLink_delete(m_Import);
    //}
}

bool VNetTCPLink::connect(const char* hostName,long port){
    //return netLink_connect(m_Import,hostName,port);
	return false;
}

void VNetTCPLink::setOnEventCallBackProc(TNetLinkEventCallBackProc callBackProc,void* userData){
    //netLink_setOnEventCallBackProc(m_Import,callBackProc,userData);
}

bool VNetTCPLink::sendData(const unsigned char* data,long length){
    //return netLink_sendData(m_Import,data,length);
	return false;
}

bool VNetTCPLink::getIsReceivedData(){
    //return netLink_getIsReceivedData(m_Import);
	return false;
}

bool VNetTCPLink::readData(unsigned char* out_data,long read_length){
    //return netLink_readData(m_Import,out_data,read_length);
	return false;
}

void VNetTCPLink::close(){
    //netLink_close(m_Import);
}


//
//  VComDataRec.h
//  ddz
//
//  Created by  on 12-2-23.
//  Copyright (c) 2012年 liujw. All rights reserved.
//

#ifndef _VComDataRec_h
#define _VComDataRec_h

//#define DEBUG_PACKDATA

#ifdef DEBUG_PACKDATA

#include "hStream.h"

class VComDataRec
{
public:
    static void recPackData(const unsigned char* data,int size)
    {
        const char* g_PackDataRecFileName	= "4avol_IPadDdzPackData.rec";
        
        String strData;
        
        {
            MyTextFileInputStream packDataFile(g_PackDataRecFileName);
            unsigned long nCount = packDataFile.getDataSize();
            
            strData.resize(nCount);
            if (nCount > 0) 
            {
                packDataFile.read((void*)&strData[0], nCount);
            }
        }
        
        strData.insert(strData.end(),data,data + size);
        strData.append("\n");
        
        MyTextFileOutputStream packDataFile(g_PackDataRecFileName);
        packDataFile.write(strData.data(), strData.size());
    }
    
    static void recConnectData(const char* hostName,long port)
    {
        String strConncet = "connect: ";
        strConncet.append(hostName);
        strConncet.append(" ");
        strConncet.append(intToStr(port));
        
        recPackData((unsigned char*)strConncet.data(),strConncet.size());
    }
};

#endif

#endif

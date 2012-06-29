//Rand.cpp

#include "Rand.h"
#include "../../import/importInclude/SysImport.h"
#include "../engine/VThread.h"
#include "mmTimer.h"

static long getRandomSeed(){
    static VRandLong rand;
    static bool isInit=false;
    static VLock lock;
    
    VAutoLock autoLock(lock);
    if (!isInit){
        double t=getMMTimerCount()+getNowTime_s()*1000;
        rand.setSeed((long)(UInt64)t);
        isInit=true;
    }
    return rand.next();
    
}

void VRand::setSeedByNowTime(){
    setSeed(getRandomSeed());
}

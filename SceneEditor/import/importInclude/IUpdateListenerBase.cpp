//IUpdateListenerBase.cpp

#include "IUpdateListenerBase.h"
#include <string.h>

void initUpdateListener(IUpdateListenerBase* pIUpdateListenerBase)  {
    memset(pIUpdateListenerBase,0,sizeof(IUpdateListenerBase));
}


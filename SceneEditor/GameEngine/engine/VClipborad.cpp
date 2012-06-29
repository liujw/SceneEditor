//VClipborad.cpp

#include "VClipborad.h"
#include "../../import/importInclude/clipboradImport.h"

bool VClipborad::setText(const String& text){
    return clipborad_setText(text.c_str());
}
const String VClipborad::getText(){
    return clipborad_getText();
}
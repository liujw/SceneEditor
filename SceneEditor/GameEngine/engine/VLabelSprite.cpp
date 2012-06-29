//VLabelSprite.cpp

#include "VLabelSprite.h"
#include "../base/Rand.h"
#include "../base/IntSet.h"

void VLabelSprite::updateChange(){
    if (m_font.getIsChanged()){
        textChanged();
        m_font.updateChange();
    }
    VSprite::updateChange();
}
void VLabelSprite::getCharCanvasList(const String& text,const VFont& font,VCharCanvasEffect* charCanvasEffect,TTextCanvasList& out_TextCanvasList){
    const char* ptext=text.c_str();
    UInt32 aChar=getNextChar(ptext);
    while (aChar!=0){
        out_TextCanvasList.push_back(VPosCharCanvas(font,aChar,charCanvasEffect));
        aChar=getNextChar(ptext);
    }
}
void VLabelSprite::doUpdateChange(){
    TTextCanvasList newTextCanvasList; 
    getCharCanvasList(m_text,m_font,&m_charCanvasEffect,newTextCanvasList);
    
    m_textlineStartIndexList.clear();
    resetPos(newTextCanvasList,m_textlineStartIndexList);
    
    m_TextCanvasList.swap(newTextCanvasList);
} 

void VLabelSprite::cutFixedLenFirstLine(String& text, const long cutWidth)
{
    String str = text;
    TTextCanvasList outStrCanvasList;
    const char* pStr=str.c_str();
    UInt32 aChar=getNextChar(pStr);
    while (aChar!=0){
        outStrCanvasList.push_back(VPosCharCanvas(m_font,aChar,&m_charCanvasEffect));
        aChar=getNextChar(pStr);
    }
    long wChar = 0;
    long nChar = 0;
    bool isOver = false;
    for (long i = 0; i < (long)outStrCanvasList.size(); ++i ) {
        wChar += outStrCanvasList[i].getWidth();
        if (wChar <= cutWidth) {
            ++nChar;
        }
        else {
            nChar = nChar - 2;//如果超出长度，把最后两个去掉，加上".."
            isOver = true;
            break;
        }
    }
    cutUtf8SpecifiedLenStr(str, nChar);
    if (isOver) str += "..";
    text = str;
}

long VLabelSprite::setLinePos_returnHeight(VPosCharCanvas* textCanvas,long count,long posY){
    long lineWidth=0;
    long maxHeight=0;
    for (long i=0;i<count;++i){
        lineWidth+=textCanvas[i].getWidth();
        if (maxHeight<textCanvas[i].getHeight())
            maxHeight=textCanvas[i].getHeight();
    }
    if (maxHeight==0)
        maxHeight=m_font.getSize();
    if (count>1)
        lineWidth+=(count-1)*m_spaceWidth;
    
    long posX;
    if (m_align==align_center)
        posX=(this->getWidth()-lineWidth)/2;
    else if (m_align==align_right)
        posX=this->getWidth()-lineWidth;
    else
        posX=0;
    
    for (long i=0;i<count;++i){
        textCanvas[i].posX=posX;
        posX+=textCanvas[i].getWidth()+m_spaceWidth;
        textCanvas[i].posY=posY+maxHeight-textCanvas[i].getHeight();
    }
    
    return maxHeight;
}

UInt8 _EnWordCharSet_table[256];
const UInt8* EnWordCharSet_table=&_EnWordCharSet_table[0];

class _CAuto_inti_EnWordCharSet_table
{
private:
public:
    _CAuto_inti_EnWordCharSet_table() {
        for (int i=0;i<256;++i){
            if ( ((i>='a')&&(i<='z')) || ((i>='A')&&(i<='Z')) || ((i>='0')&&(i<='9')) || (i=='_') )
                _EnWordCharSet_table[i]=1;
            else
                _EnWordCharSet_table[i]=0;
        }
    }
};
static _CAuto_inti_EnWordCharSet_table _Auto_inti_EnWordCharSet_table;

must_inline static bool isEnWordChar(int aChar){
    return ( 0 != EnWordCharSet_table[(UInt8)aChar] );
}

void VLabelSprite::resetPos(TTextCanvasList& textCanvasList,TTextlineStartIndexList& out_textlineStartIndexList){
    long textCount=(long)textCanvasList.size();
    
    long lineBegin=0;
    long lineCount=0;
    long posY=0;
    long posX=0;
    for (long i=0;i<textCount;){
        VPosCharCanvas& textCanvas=textCanvasList[i];
        long lineWidth=posX;
        if (i>lineBegin) 
            lineWidth+=m_spaceWidth;
        lineWidth+=textCanvas.getWidth();
        if (m_isAllowMultiLine&&(textCanvas.getChar()==csTag_Line)){ //换行符
            long lineHeight=setLinePos_returnHeight(&textCanvasList[lineBegin],i-lineBegin,posY);
            posY+=lineHeight+m_spaceHeight;
            ++lineCount;
            setLinePos_returnHeight(&textCanvas,1,posY);//csTag_Line字符放到一边不管
            out_textlineStartIndexList.push_back(lineBegin);
            lineBegin=i+1;
            posX=0;
            ++i;
        } else if (m_isAllowMultiLine&&m_isAutoLine&&(i>lineBegin)&&(lineWidth>this->getWidth())) {
            long newBegin=i;
            
            if (m_isAutoLineNotClipEnWord){
                //该行最后一个字符和下一行第一个字符是字母或数字或下划线
                if( (i>=1)&&(isEnWordChar(textCanvasList[i-1].getChar()))
                   &&(isEnWordChar(textCanvasList[i].getChar()))  ){
                    //并且这个单词的首字符不是该行的首字符 则 从该单词首字符分行
                    long wordBegin=i-1;
                    for (long w=i-2;w>=lineBegin;--w){
                        if (isEnWordChar(textCanvasList[w].getChar()))
                            wordBegin=w;
                        else
                            break;
                    }
                    if (wordBegin!=lineBegin)
                        newBegin=wordBegin;
                }
            }
            
            out_textlineStartIndexList.push_back(lineBegin);
            long lineHeight=setLinePos_returnHeight(&textCanvasList[lineBegin],newBegin-lineBegin,posY);
            posY+=lineHeight+m_spaceHeight;
            ++lineCount;
            if (getIsAutoAlignment()){//是否允许自动两端对齐
                adjustOffetPos(textCanvasList,lineBegin,newBegin);
            }
            
            lineBegin=newBegin;
            posX=0;
            for (long w=lineBegin;w<i;++w){
                posX+=textCanvasList[w].getWidth();
                if (w!=i-1)
                    posX+=m_spaceWidth;
            }
        }else if (i==(textCount-1)){
            long lineHeight=setLinePos_returnHeight(&textCanvasList[lineBegin],textCount-lineBegin,posY);
            posY+=lineHeight+m_spaceHeight;
            ++lineCount;
            out_textlineStartIndexList.push_back(lineBegin);
            posX=0;
            ++i;
        }else{
            posX=lineWidth;
            ++i;
        }
    }
    if (lineCount>0)
        posY-=m_spaceHeight;
    m_txtPixelHeight = posY;
}
void VLabelSprite::adjustOffetPos(TTextCanvasList &textCanvasList, long lineBeginWordIndex, long lineEndWordIndex)
{
    long lineLastWordIndex=lineEndWordIndex-1;
    long lineBalance = this->getWidth() - textCanvasList[lineLastWordIndex].posX - textCanvasList[lineLastWordIndex].getWidth();
    if (lineBalance<=0) return;
    long gapNum = lineEndWordIndex - lineBeginWordIndex-1;
    if (gapNum<=1) return;
    long charWidth=(textCanvasList[lineLastWordIndex].getPosXRight()-textCanvasList[lineBeginWordIndex].posX)/(gapNum+1);
    if ((lineBalance>(this->getWidth()/3))||(lineBalance>charWidth*2)) return;
    
    Vector<int> vecOffet(gapNum);//有gapNum个空隙
    long multiple = lineBalance/gapNum;
    IntFloat_16 averageOffet = (lineBalance - multiple*gapNum)*(1<<16)/gapNum;
    IntFloat_16 oneStepOffet = averageOffet;
    for (long j = 0; j < gapNum; ++j){
        if (oneStepOffet > ((1<<16)>>1)){
            vecOffet[j] = multiple + 1;
            oneStepOffet -= (1<<16);
        }else{
            vecOffet[j] = multiple;
        }
        oneStepOffet += averageOffet;
    }
    long sumOff=0;
    for (long i=0;i<gapNum;++i){
        sumOff+=vecOffet[i];
        vecOffet[i]=sumOff;
    }
    for (long i=0;i<gapNum;++i){
        textCanvasList[lineBeginWordIndex+i+1].posX +=vecOffet[i];
    }
}

void VLabelSprite::doDrawAChar(const VCanvas& dst,long x0,long y0,const VCharCanvas& charCanvas,VCharCanvasEffect* charCanvasEffect){
    dst.blendLight(x0,y0,charCanvas.getCanvas(),charCanvasEffect->fontColor);
}

void VLabelSprite::doDraw(const VCanvas& dst,long x0,long y0){
    long textCount=(long)m_TextCanvasList.size(); 
    for (long i=0;i<textCount;++i){	
        const VPosCharCanvas& textCanvas=m_TextCanvasList[i];
        doDrawAChar(dst,x0+textCanvas.posX,y0+textCanvas.posY,textCanvas,textCanvas.charCanvasEffect);
    }
    
    //static VRandLong rnd;
    //dst.rect(x0,y0,x0+getWidth(),y0+this->getAllTextLinePixelsHeight()+1,Color32(rnd.next(),rnd.next(),rnd.next()));
}

void VLabelSprite::getWantDrawRect(long x0,long y0,TRect& out_rect){
    updateChange();
    
    long textCount=(long)m_TextCanvasList.size();
    if (textCount<=0){
        out_rect.setEmpty();
        return;
    }
    if (m_isTextViewsChanged){
        const VPosCharCanvas& gtextCanvas=m_TextCanvasList[0];
        TRect result(gtextCanvas.posX,gtextCanvas.posY,gtextCanvas.posX+gtextCanvas.getWidth(),gtextCanvas.posY+gtextCanvas.getHeight());
        for (long i=1;i<textCount;++i){	
            const VPosCharCanvas& textCanvas=m_TextCanvasList[i];
            result.max(textCanvas.posX,textCanvas.posY,textCanvas.posX+textCanvas.getWidth(),textCanvas.posY+textCanvas.getHeight());
        }
        m_isTextViewsChanged=false;
        m_textRectInfo=result;
    }
    TRect drawRect=m_textRectInfo;
    drawRect.x0+=x0;
    drawRect.y0+=y0;
    drawRect.x1+=x0;
    drawRect.y1+=y0;
    out_rect=drawRect;
}


const VLabelSprite::VPosCharCanvas* VLabelSprite::testHitCharCanvas(long x,long y){
    updateChange();
    
    for (long i=0;i<(long)m_TextCanvasList.size();++i){
        const VPosCharCanvas& textCanvas=m_TextCanvasList[i];
        if (textCanvas.isInRect(x,y,m_spaceWidth,m_spaceHeight))
            return &textCanvas;
    }
    return 0;
}

bool VLabelSprite::getIsHit(long clientX0, long clientY0){
    return (0!=testHitCharCanvas(clientX0,clientY0));
}

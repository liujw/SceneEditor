/*
 *  VMemoSprite.cpp
 *  testEngine
 *
 *  Created by housisong on 08-6-19.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "VMemoSprite.h"



const char* csHotBeginTag="{\\\\"; //is "{\\"     
const long csHotBeginTagLength=3;
const char* csHotEndTag="{//}";
const long csHotEndTagLength=4;

class TMemoTextParse{
protected:
    must_inline String subString(const String& text,long strBegin,long strEnd){
        return String(text.begin()+strBegin,text.begin()+strEnd);
    }
protected:
    typedef VLabelSprite::TTextCanvasList    TTextCanvasList;
    typedef VMemoSprite::THotspotInfo        THotspotInfo;
    typedef VMemoSprite::VExCharCanvasEffect TExCharCanvasEffect;
    
    
    bool parseLinkHot(const String& text,long hotBegin,long hotHeadEnd,String& hotspotName,String& hotspotText){
        //超链接的表达方式:
        //{\\anchor=url;www.80166.com}主页{//} 其中分号前为超链接方式, 分号后为超链接地址, 两个标志之间为实际显示的内容
        //{\\anchor=playername;玩家1}玩家1{//}
        
        //text as "anchor=%;%"
        const char* csLinkTag="anchor=";
        const long csLinkTagLength=7;
        const char csNodeTag=';';
        
        if (0!=findStrIgnoreCase(&text[hotBegin],hotHeadEnd-hotBegin,csLinkTag,csLinkTagLength))
            return false;
        hotBegin+=csLinkTagLength;
        long nodeIndex=posEx(&text[0],hotHeadEnd,hotBegin,csNodeTag);
        if (nodeIndex<hotBegin)
            return false;
        hotspotName=subString(text,hotBegin,nodeIndex);
        hotspotText=subString(text,nodeIndex+1,hotHeadEnd);
        return true;
    }
    bool parseFontHot(const String& text,long hotBegin,long hotHeadEnd,VFont& out_font,Color32& out_fontColor,bool& out_isUnderline){
        //格式如下 {\\style=\b\u\i\c$ffffff}content{//}
        //	\b 代表粗体字 \u 代表下划线 \i 代表斜体字 \s 代表外框 \c代表颜色后面跟十六进制或十进制表示的颜色值
        //	以上方式可以组合,也可以省略, 省略代表缺少方式
        
        const char* csFontTag="style=";
        const long csFontTagLength=6;
        const char* csBoldTag="\\b"; //is "\b" 
        const long csBoldTagLength=2;
        const char* csUnderlineTag="\\u"; //is "\u" 
        const long csUnderlineTagLength=2;
        const char* csColorTag="\\c"; //is "\c" 
        const long csColorTagLength=2;
        
        if (0!=findStrIgnoreCase(&text[hotBegin],hotHeadEnd-hotBegin,csFontTag,csFontTagLength))
            return false;
        hotBegin+=csFontTagLength;
        
        if (0<=findStrIgnoreCase(&text[hotBegin],hotHeadEnd-hotBegin,csBoldTag,csBoldTagLength))
            out_font.setIsBold(true);
        
        if (0<=findStrIgnoreCase(&text[hotBegin],hotHeadEnd-hotBegin,csUnderlineTag,csUnderlineTagLength))
            out_isUnderline=true;
        
        long colorIndex=hotBegin+findStrIgnoreCase(&text[hotBegin],hotHeadEnd-hotBegin,csColorTag,csColorTagLength);
        if (colorIndex>=hotBegin){
            colorIndex+=csColorTagLength;
            Color32 newColor(out_fontColor);
            
            if (newColor.fromColorText_try(&text[colorIndex],hotHeadEnd-colorIndex)){
                out_fontColor=newColor;
            }
        }
        return true;
    }
    
protected:
    void pushText(const String& text,long strBegin,long strEnd,const VFont& font,const Color32& fontColor,bool isUnderline,
                  const THotspotInfo& hotspotInfo,TTextCanvasList& out_TextCanvasList){
        TExCharCanvasEffect* charCanvasEffect=new TExCharCanvasEffect();
        charCanvasEffect->fontColor=fontColor;
        charCanvasEffect->isUnderline=isUnderline;
        charCanvasEffect->hotspotInfo=hotspotInfo;
        const long csBackCvCount=(long)out_TextCanvasList.size();
        VLabelSprite::getCharCanvasList(subString(text,strBegin,strEnd),font,charCanvasEffect,out_TextCanvasList);
        charCanvasEffect->usedCount=(long)out_TextCanvasList.size()-csBackCvCount;
        if (charCanvasEffect->usedCount<=0)
            delete charCanvasEffect;
    }
    void parseHot(const String& text,long hotBegin,long hotEnd,const VFont& font,const Color32& fontColor,bool isUnderline,
                  const THotspotInfo& hotspotInfo,TTextCanvasList& out_TextCanvasList){
        
        const char csHotHeadEndTag='}'; 
        long hotHeadEnd=posEx(&text[0],hotEnd,hotBegin,csHotHeadEndTag);
        if (hotHeadEnd<hotBegin){ // 头标记有错误,不完整
            parseText(text,hotBegin,hotEnd,font,fontColor,isUnderline,hotspotInfo,out_TextCanvasList);
            return;
        }
        long textBegin=hotHeadEnd+1;
        
        VFont new_font(font);
        Color32  new_fontColor(fontColor);
        bool new_isUnderline=isUnderline;
        if (parseFontHot(text,hotBegin,hotHeadEnd,new_font,new_fontColor,new_isUnderline)){
            parseText(text,textBegin,hotEnd,new_font,new_fontColor,new_isUnderline,hotspotInfo,out_TextCanvasList);
        } else {
            String hotspotName; 
            String hotspotText;
            if (parseLinkHot(text,hotBegin,hotHeadEnd,hotspotName,hotspotText)){
                THotspotInfo new_hotspotInfo;
                new_hotspotInfo.hotspotName=hotspotName;
                new_hotspotInfo.hotspotText=hotspotText;
                parseText(text,textBegin,hotEnd,font,fontColor,isUnderline,new_hotspotInfo,out_TextCanvasList);
            }else  //不知道怎么处理的hot
                parseText(text,textBegin,hotEnd,font,fontColor,isUnderline,hotspotInfo,out_TextCanvasList);
        }
    }
    
    
    long findHotEnd(const String& text,long findBegin,long strEnd,long findEndCount=1){
        //查找配对的结束标记
        long pos0=findBegin;
        long hotFindEnd=pos0+findStr(&text[pos0],strEnd-pos0,csHotEndTag,csHotEndTagLength);
        if (hotFindEnd<pos0) 
            return -1; //没有找到
        else{
            //查找这之间是否还有其它开始标记
            long otherHotBegin=pos0+findStr(&text[pos0],hotFindEnd-pos0,csHotBeginTag,csHotBeginTagLength);
            if (otherHotBegin<pos0){ //中间没有开始标记  成功找到结束标记
                if (findEndCount<=1)
                    return hotFindEnd; //成功
                else
                    return findHotEnd(text,hotFindEnd+csHotEndTagLength,strEnd,findEndCount-1);//继续找下一个结束标记;
            }
            else
                return findHotEnd(text,otherHotBegin+csHotBeginTagLength,strEnd,findEndCount+1);//继续找结束标记;
        }	
    }
public:
    TMemoTextParse(){}
    
    //从text中抽出Hotspot的信息 
    void parseText(const String& text,long strBegin,long strEnd,const VFont& font,const Color32& fontColor,bool isUnderline,
                   const THotspotInfo& hotspotInfo,TTextCanvasList& out_TextCanvasList){
        if (strBegin>=strEnd) return;
        
        long hotBegin=strBegin+findStr(&text[strBegin],strEnd-strBegin,csHotBeginTag,csHotBeginTagLength);
        if (hotBegin<strBegin){ //没有找到开始标记
            pushText(text,strBegin,strEnd,font,fontColor,isUnderline,hotspotInfo,out_TextCanvasList);
            return;
        }
        if (hotBegin>strBegin){ //前面有一段普通字符
            pushText(text,strBegin,hotBegin,font,fontColor,isUnderline,hotspotInfo,out_TextCanvasList);
        }
        
        long hotEnd=findHotEnd(text,hotBegin+csHotBeginTagLength,strEnd);		
        if (hotEnd<hotBegin){//没有找到结束标记
            strBegin=strEnd;
            hotEnd=strEnd;
        }else{
            strBegin=hotEnd+csHotEndTagLength;
            //hotEnd;
        }			
        
        parseHot(text,hotBegin+csHotBeginTagLength,hotEnd,font,fontColor,isUnderline,hotspotInfo,out_TextCanvasList);
        parseText(text,strBegin,strEnd,font,fontColor,isUnderline,hotspotInfo,out_TextCanvasList);
    }
};

/////////////////////////////////////////////

void VMemoSprite::delAllTextCanvasListByIndexs(long delBeginIndex,long delEndIndex){
    for (long i=delBeginIndex;i<delEndIndex;++i){
        VExCharCanvasEffect* ef=(VExCharCanvasEffect*)(m_allTextCanvasList[i].charCanvasEffect);
        --(ef->usedCount);
        if (ef->usedCount==0)
            delete ef;		
    }
    m_allTextCanvasList.erase(m_allTextCanvasList.begin()+delBeginIndex,m_allTextCanvasList.begin()+delEndIndex);
}

void VMemoSprite::doUpdateChange(){
    //如果文本没有改变就直接返回
    if (!m_isTextViewsChanged) return;
    m_isTextViewsChanged=false;
    
    if (m_viewLineCount<0) m_viewLineCount=0;
    if (m_topLine>getLineCount()-1) m_topLine=getLineCount()-1;
    if (m_topLine<0) m_topLine=0;
    if (m_maxLineCount<0) m_maxLineCount=1;
    
    if (getLineCount()>m_maxLineCount){
        long delLine=getLineCount()-m_maxLineCount;		
        m_lineList.erase(m_lineList.begin(),m_lineList.begin()+delLine);
        long delCount;
        long newLineCount=getLineCount();
        if (newLineCount>0)
            delCount=m_lineList[0].textCanvasIndexBegin;
        else
            delCount=(long)m_allTextCanvasList.size();
        delAllTextCanvasListByIndexs(0,delCount);
        for (long i=0;i<newLineCount;++i){
            m_lineList[i].textCanvasIndexBegin-=delCount;
            m_lineList[i].textCanvasIndexEnd-=delCount;
        }
    }
    
    m_TextCanvasList.clear(); 
    long endLineIndex=m_topLine+m_viewLineCount;
    if (endLineIndex>getLineCount()) endLineIndex=getLineCount();
    long posY=0;
    for (long l=m_topLine;l<endLineIndex;++l){
        long lineHeight=0;
        for (long i=m_lineList[l].textCanvasIndexBegin;i<m_lineList[l].textCanvasIndexEnd;++i){
            m_allTextCanvasList[i].posY=posY;
            if (lineHeight<m_allTextCanvasList[i].getHeight()) 
                lineHeight=m_allTextCanvasList[i].getHeight();
            m_TextCanvasList.push_back(m_allTextCanvasList[i]);
        }
        //if (lineHeight==0) lineHeight=m_font.getSize()*zoomScale?;//! error
        posY+=lineHeight+m_spaceHeight;
    }
    this->m_txtPixelHeight=posY;
}

void VMemoSprite::addLine(const String& text){
    VLabelSprite::addLine(text);
    
    TMemoTextParse memoTextParse;
    TTextCanvasList textCanvasList;
    memoTextParse.parseText(text,0,(long)text.size(),m_font,m_charCanvasEffect.fontColor,false,THotspotInfo(),textCanvasList);
    //设置字符显示位置和得到换行位置 
    TTextlineStartIndexList textlineStartIndexList;
    VLabelSprite::resetPos(textCanvasList,textlineStartIndexList);
    
    //添加新行
    long newLineCount=(long)textlineStartIndexList.size();
    long curTextCanvasIndex=(long)m_allTextCanvasList.size();
    for (long i=0;i<newLineCount;++i){
        TLineText lineText;
        lineText.textCanvasIndexBegin=curTextCanvasIndex+textlineStartIndexList[i];
        if (i==newLineCount-1)
            lineText.textCanvasIndexEnd=curTextCanvasIndex+(long)textCanvasList.size();
        else
            lineText.textCanvasIndexEnd=curTextCanvasIndex+textlineStartIndexList[i+1];			
        m_lineList.push_back(lineText);
    }
    m_allTextCanvasList.insert(m_allTextCanvasList.end(),textCanvasList.begin(),textCanvasList.end());		
    
    toLastLine();
    textChanged();
}


void VMemoSprite::clear(){
    if (m_allTextCanvasList.size()>0){
        delAllTextCanvasListByIndexs(0,m_allTextCanvasList.size());
        m_lineList.clear();
        m_topLine=0;
        VLabelSprite::clear();
        textChanged();
    }
}

void VMemoSprite::doDrawAChar(const VCanvas& dst,long x0,long y0,const VCharCanvas& charCanvas,VCharCanvasEffect* charCanvasEffect){
    VLabelSprite::doDrawAChar(dst,x0,y0,charCanvas,charCanvasEffect);
    VExCharCanvasEffect* ef=(VExCharCanvasEffect*)charCanvasEffect;
    if (ef->isUnderline){
        Color32 underlineColor(ef->fontColor);
        underlineColor.a>>=1;
        dst.lineH(y0+charCanvas.getHeight()-1,x0,x0+charCanvas.getWidth(),underlineColor);
    }
}

void VMemoSprite::disposeEventMouseClick(const TSpriteMouseEventInfo& mouseDownEventInfo,const TSpriteMouseEventInfo& mouseUpEventInfo) { 
    VLabelSprite::disposeEventMouseClick(mouseDownEventInfo,mouseUpEventInfo);
    
    if (m_hotspotListener==0) return;
    const VPosCharCanvas* cv=testHitCharCanvas(mouseDownEventInfo.clientX0,mouseDownEventInfo.clientY0);
    if (cv==0) return;
    VExCharCanvasEffect* ef=(VExCharCanvasEffect*)cv->charCanvasEffect;
    if ((ef==0)||(ef->hotspotInfo.empty())) return;
    cv=testHitCharCanvas(mouseUpEventInfo.clientX0,mouseUpEventInfo.clientY0);
    if (ef!=cv->charCanvasEffect) return;
    m_hotspotListener->doHotspotLink(this,ef->hotspotInfo.hotspotName,ef->hotspotInfo.hotspotText);
}

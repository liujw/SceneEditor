#include "SpriteDraw.h"

#include "MyGame.h"
#include "VGameCtrl.h"
#include "../importInclude/hPixels32ImportBase.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TCanvasBaseData:public _TCanvasBaseData{
	TContextDataBase* m_contextDataBase;
	long	m_colorBit;
#ifdef CANVAS_COLOR_RGB16
TCanvasBaseData(long colorBit=16){
#else
TCanvasBaseData(long colorBit=32){
#endif
        pdata=0;
        byte_width=0;
        width=0;
        height=0;
		m_contextDataBase=0;
		m_colorBit=colorBit;
    } 
	must_inline bool isEmpty()const{
		return (pdata==0);
	}
    void resizeFast(long aWidth,long aHeight){
        if ((width!=aWidth)||(height!=aHeight)){
            clear();
			getNewPixels_Import(aWidth,aHeight,&pdata,&byte_width,(void **)&m_contextDataBase);
			 if (pdata!=0)
			 {
                width=aWidth;
                height=aHeight;

				(BYTE*&)pdata+=(height-1)*byte_width;
				byte_width*=-1;
            }
        }
    }
    void clear(){
        if ((m_contextDataBase!=0)||(pdata!=0)){
            if (pdata!=0)
				(BYTE*&)pdata+=(height-1)*byte_width; 
            deletePixels_Import(m_contextDataBase,pdata);
			m_contextDataBase=0;
            pdata=0;
        }
        byte_width=0;
        width=0;
        height=0;
    }
	must_inline HDC getDC()const{
		if (m_contextDataBase==0)
			return 0;
		else
			return (HDC)m_contextDataBase->context;
	}
    ~TCanvasBaseData(){ clear(); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
CSpriteDraw::CSpriteDraw(CWnd* pWnd,MyGame* pGame)
{
	m_pMainWnd = pWnd;
	m_bReDraw = FALSE;

	m_pCanvasData = new TCanvasBaseData();

	m_pGame = pGame;
	m_pGameCtrl = new VGameCtrl(m_pGame,NULL); 
	
	Initial();
}

CSpriteDraw::~CSpriteDraw()
{
	if(m_pGameCtrl)
		delete m_pGameCtrl;

	if(m_pCanvasData)
		delete m_pCanvasData;
}
void CSpriteDraw::Initial()
{
	m_bReDraw = TRUE;

	m_pMainWnd->GetClientRect(&m_clientRect);

	m_pCanvasData->resizeFast(m_clientRect.Width(),m_clientRect.Height());

	m_pGame->loadSprite();
}

HDC CSpriteDraw::GetSourceDC()
{
	TRect* pRect=0;
	long rectCount=0;

	Pixels32Ref* dst_ref=(Pixels32Ref*)m_pCanvasData;
    VCanvas  dst = VCanvas(*dst_ref);
	m_pGameCtrl->draw(dst,false,&pRect,&rectCount);
	return m_pCanvasData->getDC();
}
void CSpriteDraw::Draw(CDC* pDC)
{
	//if(!m_bReDraw)	return;

	//m_bReDraw = FALSE;
	
	HDC srcDC = GetSourceDC();
	if (srcDC) 
	{
		BitBlt(pDC->m_hDC,m_clientRect.left,m_clientRect.top,m_clientRect.Width(),m_clientRect.Height(), srcDC,m_clientRect.left,m_clientRect.top,SRCCOPY);
	}
}


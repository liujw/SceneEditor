#include "StdAfx.h"
#include "GameSprite.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CGameSprite::CGameSprite(CWnd* pWnd, HDC hDC)
{
	m_pMainWnd = pWnd;
	m_hDC = hDC;
	m_pCoBmp = NULL;

	m_bReDraw = FALSE;
	m_bInitialed = FALSE;


	m_gBitmap = NULL;
}

CGameSprite::~CGameSprite()
{
	if(m_pCoBmp) delete m_pCoBmp;

	if(m_gBitmap) delete m_gBitmap;

	
}
void CGameSprite::Initial()
{
	if(!m_bInitialed)
	{
		m_pMainWnd->GetClientRect(&m_clientRect);

		m_nClientWidth = m_clientRect.Width();
		m_nClientHeight = m_clientRect.Height();
	}	
}


BOOL CGdiDraw::CreateMemBitmap()
{
	if(m_gDC.GetSafeHdc())
		m_gDC.Detach();
	
	m_gDC.Attach(m_hDC);

	if(m_memBitmap.GetSafeHandle())
		m_memBitmap.Detach();

	if(!m_memBitmap.CreateCompatibleBitmap(&m_gDC,m_clientRect.Width(),m_clientRect.Height()))
		return FALSE;

	if(m_memDC.GetSafeHdc())
		m_memDC.Detach();

	if(!m_memDC.CreateCompatibleDC(NULL))
		return FALSE;

	CBitmap * m_pOldBitmap = m_memDC.SelectObject(&m_memBitmap);
	if(!m_pOldBitmap)
		return FALSE;

	if(m_gBitmap)
		delete m_gBitmap;

	m_gBitmap = Graphics::FromHDC(m_memDC.m_hDC);

	
	return TRUE;
}

void CGdiDraw::Draw(int nSelectHistogram)
{
	if(!m_bInitialed)          //初始化时或窗口size changed
	{
		Initial();
	 	if(!CreateMemBitmap())
			return;

		m_bInitialed = TRUE;
	}
	

	DrawMemBitmap(m_gBitmap);

	m_gDC.BitBlt(0,0,m_clientRect.Width(),m_clientRect.Height(),&m_memDC,0,0,SRCCOPY);
}

BOOL CreateDib(int w, int h, HBITMAP& hBmpSection, BYTE** bmpData)    
{    
	BITMAPINFO info = {0};    
	info.bmiHeader.biSize = sizeof(info.bmiHeader);    
	info.bmiHeader.biWidth = w;    
	info.bmiHeader.biHeight = -h;    
	info.bmiHeader.biPlanes = 1;    
	info.bmiHeader.biBitCount = 32;    
	info.bmiHeader.biCompression = BI_RGB;    
	info.bmiHeader.biSizeImage = w * h * 32 / 8;    
	HDC hdc = ::GetDC(NULL);    
	hBmpSection = ::CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)bmpData, NULL, 0);    
	::ReleaseDC(NULL, hdc);    
	return hBmpSection != NULL;    
}    

// m_hBmpSection是用CreateDib创建出来的     
LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)    
{    
	CPaintDC dc(m_hWnd);    
	HDC hdcMen = ::CreateCompatibleDC(dc.m_hDC);    
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hdcMen, m_hBmpSection);    
	// 宽度和高度自己取下     
	::BitBlt(dc.m_hDC, 0, 0, m_width, m_height, hdcMen, 0, 0, SRCCOPY);    
	::SelectObject(hdcMen, hOldBmp);    
	::DeleteDC(hdcMen);    
	return TRUE;    
}    

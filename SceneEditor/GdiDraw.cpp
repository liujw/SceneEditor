#include "StdAfx.h"
#include "GdiDraw.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CGdiDraw::CGdiDraw(CWnd* pWnd, CDC* pDC)
{
	m_pMainWnd = pWnd;
	m_pDC = pDC;
}

CGdiDraw::~CGdiDraw()
{
	
}
void CGdiDraw::Initial()
{
	m_pMainWnd->GetClientRect(&m_clientRect);

}


BOOL CGdiDraw::CreateMemBitmap()
{
	if(m_memBitmap.GetSafeHandle())
		m_memBitmap.Detach();

	if(!m_memBitmap.CreateCompatibleBitmap(m_pDC,m_clientRect.Width(),m_clientRect.Height()))
		return FALSE;

	if(m_memDC.GetSafeHdc())
		m_memDC.Detach();

	if(!m_memDC.CreateCompatibleDC(NULL))
		return FALSE;

	m_memDC.SelectObject(&m_memBitmap);
		
	return TRUE;
}


void CGdiDraw::Draw()
{
	//m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->BitBlt(0,0,m_clientRect.Width(),m_clientRect.Height(),&m_memDC,0,0,SRCCOPY);
	// 
	//m_memDC.SelectObject(m_pOldBitmap);

}


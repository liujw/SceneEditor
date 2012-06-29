#pragma once

#include "stdafx.h"
#include <vector>
#include <deque>

class CGdiDraw
{
public:
	CGdiDraw(CWnd* pWnd, CDC* pDC);
	virtual ~CGdiDraw();

public:
	
	void Draw();// draw to device 
	
	//zoom
public:
	
protected:
	BOOL CreateMemBitmap();
	void Initial();

private:
	CWnd* m_pMainWnd; 
	CDC m_memDC;
	CDC* m_pDC;
	CBitmap m_memBitmap;

	CRect m_clientRect;
};


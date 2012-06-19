#pragma once

#include "stdafx.h"
#include <vector>
#include <deque>

class VGameCtrl;

struct TCanvasBaseData;

class CSpriteDraw
{
public:
	CSpriteDraw(CWnd* pWnd);
	virtual ~CSpriteDraw();

public:
	
	void Draw(CDC* pDC);// draw to device 
	
	//zoom
public:
	
protected:
	void Initial();

private:
	CWnd* m_pMainWnd; 
	CRect m_clientRect;

	VGameCtrl* m_pGameCtrl; 
	TCanvasBaseData* m_pCanvasData;
};


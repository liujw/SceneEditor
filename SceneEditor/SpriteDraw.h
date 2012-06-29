#pragma once

#include "stdafx.h"

#include "sysimport.h"

class VGameCtrl;
class MyGame;
struct TCanvasBaseData;

class CSpriteDraw
{
public:
	CSpriteDraw(CWnd* pWnd,	MyGame* pGame);
	virtual ~CSpriteDraw();

public:
	void Draw(CDC* pDC);
		
	void Initial();

private:

	void LoadGame();
	HDC GetSourceDC();

private:
	CWnd* m_pMainWnd; 
	CRect m_clientRect;

	MyGame* m_pGame;
	VGameCtrl* m_pGameCtrl; 
	TCanvasBaseData* m_pCanvasData;

	BOOL m_bReDraw;

	CRect m_dragRect;
};


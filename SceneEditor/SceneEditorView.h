
// SceneEditorView.h : CSceneEditorView ��Ľӿ�
//

#pragma once

#include "sysimport.h"

class CSpriteDraw;
class VSprite;
class MyGame;
class CMainFrame;
class CSceneEditorDoc;
class TSpriteTree;

class CSceneEditorView : public CView
{
protected: // �������л�����
	CSceneEditorView();
	DECLARE_DYNCREATE(CSceneEditorView)

// ����
public:
	CSceneEditorDoc* GetDocument() const;

	inline void UpdateView(BOOL bReCreate = FALSE);
	VSprite* GetCurrentVSprite() { return m_pSprite; }
	TSpriteTree* GetCurrentTSpriteTree() { return m_pSpriteTree; }

	void SetSelectSprite(const String& spriteName);

	MyGame* m_pGame;
	CSpriteDraw* m_pSpriteDraw;

	CEvent m_ePaint;
	CWinThread *m_pThread;
	bool m_bIsThreadQuit;

	BOOL m_bDraging;
	CPoint m_ptMouse;

	VSprite* m_pSprite;
	TSpriteTree* m_pSpriteTree;
	String m_strSpriteName;
	CMainFrame *m_pMainFrm;
// ����
public:
	static UINT PaintThread(LPVOID param);
	void DrawSprite();

	void SetNewPosition(CPoint point);
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CSceneEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnOpenXml();
	afx_msg void OnSaveXml();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

#ifndef _DEBUG  // SceneEditorView.cpp �еĵ��԰汾
inline CSceneEditorDoc* CSceneEditorView::GetDocument() const
   { return reinterpret_cast<CSceneEditorDoc*>(m_pDocument); }
#endif


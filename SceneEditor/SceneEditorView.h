
// SceneEditorView.h : CSceneEditorView 类的接口
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
protected: // 仅从序列化创建
	CSceneEditorView();
	DECLARE_DYNCREATE(CSceneEditorView)

// 特性
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
// 操作
public:
	static UINT PaintThread(LPVOID param);
	void DrawSprite();

	void SetNewPosition(CPoint point);
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CSceneEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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

#ifndef _DEBUG  // SceneEditorView.cpp 中的调试版本
inline CSceneEditorDoc* CSceneEditorView::GetDocument() const
   { return reinterpret_cast<CSceneEditorDoc*>(m_pDocument); }
#endif


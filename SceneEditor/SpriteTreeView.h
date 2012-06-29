
#pragma once

#include "ViewTree.h"
class TSpriteTree;
class CMainFrame;
class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CSpriteTreeView : public CDockablePane
{
public:
	CSpriteTreeView();
	virtual ~CSpriteTreeView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	void BuildSpriteTree();
protected:
	CClassToolBar m_wndToolBar;
	CViewTree m_wndSceneView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;
	//test add
	HTREEITEM m_selectTreeItem;
	CMainFrame *m_pMainFrm;

	void BuildTree(TSpriteTree* rootSprite,HTREEITEM& hParentItem);

	void DeleteTreeItem(long nChoice,CTreeCtrl* pTree,TSpriteTree* spriteTree);
	
// ÖØÐ´
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	afx_msg LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDragItem(WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnSceneDelete();
};


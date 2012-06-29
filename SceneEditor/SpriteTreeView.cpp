
#include "stdafx.h"
#include "MainFrm.h"
#include "SpriteTreeView.h"
#include "Resource.h"
#include "SceneEditor.h"
#include "SceneEditorView.h"
#include "MyGame.h"

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 构造/析构
//////////////////////////////////////////////////////////////////////

CSpriteTreeView::CSpriteTreeView()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
	m_pMainFrm = NULL;
}

CSpriteTreeView::~CSpriteTreeView()
{
}

BEGIN_MESSAGE_MAP(CSpriteTreeView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)

	ON_MESSAGE(UM_LBUTTONDOWN, OnLButtonDown) 
	ON_MESSAGE(UM_DRAGITEM, OnDragItem) 
	
	ON_COMMAND(ID_SCENE_DELETE, &CSpriteTreeView::OnSceneDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView 消息处理程序

int CSpriteTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndSceneView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("未能创建类视图\n");
		return -1;      // 未能创建
	}

	// 加载图像:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	return 0;
}

void CSpriteTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CSpriteTreeView::BuildSpriteTree()
{
	HTREEITEM hRoot = m_wndSceneView.GetRootItem();
	if(hRoot != NULL)
	{
		m_wndSceneView.DeleteAllItems();
		hRoot = NULL;
	}

	BuildTree(&g_spriteTree,hRoot);
	
	m_wndSceneView.Expand(hRoot, TVE_EXPAND);
}

void CSpriteTreeView::BuildTree(TSpriteTree* pSprite,HTREEITEM& hParentItem)
{
	if(!pSprite || pSprite->getName() == "")
		return;

	HTREEITEM hItem = m_wndSceneView.InsertItem(pSprite->getName().c_str(), hParentItem);
	m_wndSceneView.SetItemData(hItem,(DWORD_PTR)pSprite);

	for(long i=0;i<pSprite->getSpriteCount();i++)
	{
		BuildTree(pSprite->getSprite(i),hItem);
	}

	m_wndSceneView.Expand(hParentItem, TVE_EXPAND);
}

void CSpriteTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndSceneView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		AfxMessageBox(_T("添加"));
		return;
	}
	
	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			m_selectTreeItem = hTreeItem;
			CMenu menu;
			menu.LoadMenuA(IDR_POPUP_SCENE);
			//CMFCPopupMenu::SetForceShadow(true);
			HMENU hMenu = menu.GetSubMenu(0)->Detach();
			pWndTree->SelectItem(hTreeItem);
			theApp.GetContextMenuManager()->ShowPopupMenu(hMenu, point.x, point.y, this, TRUE);
		}

		pWndTree->SetFocus();
	}	
}

void CSpriteTreeView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndSceneView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CSpriteTreeView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CSpriteTreeView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CSpriteTreeView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CSpriteTreeView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("添加成员函数..."));
}

void CSpriteTreeView::OnClassAddMemberVariable()
{
	// TODO: 在此处添加命令处理程序代码
}

void CSpriteTreeView::OnClassDefinition()
{
	// TODO: 在此处添加命令处理程序代码
}

void CSpriteTreeView::OnClassProperties()
{
	// TODO: 在此处添加命令处理程序代码
}

void CSpriteTreeView::OnNewFolder()
{
	//AfxMessageBox(_T("新建文件夹..."));
}

void CSpriteTreeView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndSceneView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CSpriteTreeView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	//m_wndClassView.SetFocus();
}

void CSpriteTreeView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndSceneView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 锁定*/);
}

LRESULT CSpriteTreeView::OnLButtonDown(WPARAM wParam, LPARAM lParam) 
{ 
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndSceneView;
	ASSERT_VALID(pWndTree);

	HTREEITEM hTreeItem = pWndTree->GetSelectedItem();
	if(hTreeItem)
	{
		CString strItem = pWndTree->GetItemText(hTreeItem);
		
		m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;

		CSceneEditorView* pView=(CSceneEditorView*)(m_pMainFrm->GetActiveView());
		if(pView)
			pView->SetSelectSprite(strItem.GetBuffer());
	}

	return 0;
} 
LRESULT CSpriteTreeView::OnDragItem(WPARAM wParam, LPARAM lParam) 
{ 
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndSceneView;
	ASSERT_VALID(pWndTree);
	
	HTREEITEM hItemDragS = (HTREEITEM)wParam;
	HTREEITEM hItemDragD = (HTREEITEM)lParam;

	TSpriteTree* pSrcNode = (TSpriteTree*)pWndTree->GetItemData(hItemDragS);
	TSpriteTree* pDstNode = (TSpriteTree*)pWndTree->GetItemData(hItemDragD);

	pDstNode->addASprite(pSrcNode);
	
	m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;
	if(m_pMainFrm)
	{
		m_pMainFrm->UpdateSceneView();	
	}

	return 0;
} 

void CSpriteTreeView::DeleteTreeItem(long nRet,CTreeCtrl* treeCtrl, TSpriteTree* delSprite)
{
	if (nRet == IDYES || nRet == IDOK)
	{
		treeCtrl->DeleteItem(m_selectTreeItem);
		delSprite->freeSpriteTree();
		delSprite->outFromParent();
		delete delSprite;
	}
	else if (nRet == IDNO)
	{
		//no del child
		TSpriteTree* parent = delSprite->getParent();
		for (long i = 0; i < delSprite->getSpriteCount(); ++i)
		{
			TSpriteTree* child = delSprite->getSprite(i);
			child->outFromParent();
			parent->addASprite(child);
		}
		delSprite->outFromParent();
		delete delSprite;
		BuildSpriteTree();
	}
			
	m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;
	if(m_pMainFrm)
	{
		m_pMainFrm->UpdateSceneView();	
	}
}


void CSpriteTreeView::OnSceneDelete()
{
	// TODO: 在此添加命令处理程序代码

	CTreeCtrl* pTree = (CTreeCtrl*)&m_wndSceneView;
	TSpriteTree* spriteTree = (TSpriteTree*)(pTree->GetItemData(m_selectTreeItem));

	BOOL bHaveChildItem = spriteTree->getSpriteCount();
	
	CString strInfo = bHaveChildItem ? _T("该节点包含子节点，是否确认一起删除？") : _T("确认删除该节点？");
	long nMessageBoxType = bHaveChildItem ? MB_YESNOCANCEL : MB_OKCANCEL;
	
	int nRet = MessageBox(strInfo, _T("删除精灵"),nMessageBoxType);
	if (nRet == IDCANCEL)
		return;

	DeleteTreeItem(nRet,pTree, spriteTree);
}

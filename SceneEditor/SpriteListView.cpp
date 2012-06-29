
#include "stdafx.h"
#include "mainfrm.h"
#include "SpriteListView.h"
#include "Resource.h"
#include "SceneEditor.h"
#include "SceneEditorView.h"
#include "SpriteSet.h"
#include "MyGame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CSpriteListView::CSpriteListView()
{
}

CSpriteListView::~CSpriteListView()
{
}

BEGIN_MESSAGE_MAP(CSpriteListView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SPRITE_PROPERTIES, OnProperties)
	ON_COMMAND(ID_SPRITE_ADD, OnAddSprite)

	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CSpriteListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | /*TVS_HASLINES | TVS_LINESATROOT | */TVS_HASBUTTONS;

	if (!m_wndSpriteView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	m_wndSpriteView.setEnableDrag(FALSE);
	// 加载视图图像:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndSpriteView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	InitialSpriteList();
	AdjustLayout();

	return 0;
}

void CSpriteListView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CSpriteListView::InitialSpriteList()
{
	HTREEITEM hRoot = m_wndSpriteView.InsertItem(_T("VBitmapSprite"), 0, 0);
	m_wndSpriteView.InsertItem(_T("VBitmapBaseSprite"), 0, 0);
	m_wndSpriteView.InsertItem(_T("VButtonSprite"), 0, 0);
	m_wndSpriteView.InsertItem(_T("VEditSprite"), 0, 0);
	m_wndSpriteView.InsertItem(_T("VLabelSprite"), 0, 0);
	m_wndSpriteView.InsertItem(_T("VMemoSprite"), 0, 0);
	
}

void CSpriteListView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndSpriteView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	/*if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}*/

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CSpriteListView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndSpriteView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CSpriteListView::OnProperties()
{
	AfxMessageBox(_T("精灵属性...."));

}

void CSpriteListView::OnAddSprite()
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndSpriteView;
	ASSERT_VALID(pWndTree);

	CString strSpriteType = "";
	HTREEITEM hTreeItem = pWndTree->GetSelectedItem();
	if(hTreeItem)
	{
		strSpriteType = pWndTree->GetItemText(hTreeItem);		
	}

	CSpriteSet SpriteSetDlg(strSpriteType);
	if(SpriteSetDlg.DoModal() == IDOK)
	{
		TSpriteTree treeNode;
				
		treeNode.setName(SpriteSetDlg.m_strSurfaceName);
		treeNode.setType(SpriteSetDlg.m_strSpriteType);

		if(SpriteSetDlg.m_strFileName != "")
		{
			treeNode.setPath(SpriteSetDlg.m_strFileName);
			treeNode.setRow(SpriteSetDlg.m_nRow);
			treeNode.setCol(SpriteSetDlg.m_nCol);
		}

		g_spriteTree.addASprite(treeNode);
		m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;
		if(m_pMainFrm)
		{
			m_pMainFrm->BuildSceneTree();
			m_pMainFrm->UpdateSceneView(TRUE);
		}
	}
}

void CSpriteListView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndSpriteView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CSpriteListView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndSpriteView.SetFocus();
}

void CSpriteListView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

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

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndSpriteView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}
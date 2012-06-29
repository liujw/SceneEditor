
// SceneEditorView.cpp : CSceneEditorView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SceneEditor.h"
#endif

#include "SpriteDraw.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "MainFrm.h"
#include "MyGame.h"
#include "DragSprite.h"
#include "SaveXml.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSceneEditorView

IMPLEMENT_DYNCREATE(CSceneEditorView, CView)

BEGIN_MESSAGE_MAP(CSceneEditorView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSceneEditorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_OPEN_XML, &CSceneEditorView::OnOpenXml)
	ON_COMMAND(ID_SAVE_XML, &CSceneEditorView::OnSaveXml)
	ON_WM_KEYDOWN()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CSceneEditorView 构造/析构

CSceneEditorView::CSceneEditorView()
{
	// TODO: 在此处添加构造代码
	m_pSpriteDraw = NULL;
	m_bIsThreadQuit = FALSE;
	m_pThread = NULL;
	m_bDraging = FALSE;
	m_strSpriteName = "";
	m_pSprite = NULL;
}

CSceneEditorView::~CSceneEditorView()
{
	if(m_pSpriteDraw)
		delete m_pSpriteDraw;
}

BOOL CSceneEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CSceneEditorView 绘制

void CSceneEditorView::OnDraw(CDC* /*pDC*/)
{
	CSceneEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	m_ePaint.SetEvent(); //绘制
}


// CSceneEditorView 打印


void CSceneEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSceneEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CSceneEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CSceneEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CSceneEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSceneEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSceneEditorView 诊断

#ifdef _DEBUG
void CSceneEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSceneEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSceneEditorDoc* CSceneEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSceneEditorDoc)));
	return (CSceneEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CSceneEditorView 消息处理程序

void CSceneEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	// TODO:  在此添加您专用的创建代码
	m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;

	m_pGame = new MyGame(NULL);
	m_pSpriteDraw = new CSpriteDraw(this,m_pGame);

	m_pMainFrm->BuildSceneTree();

	m_pThread = AfxBeginThread(PaintThread,this,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if(m_pThread)
	{
		m_pThread->ResumeThread();
	}

	DragAcceptFiles(TRUE);
}

UINT CSceneEditorView::PaintThread(LPVOID param)
{
	CSceneEditorView *pThis	= static_cast<CSceneEditorView *>(param);
	pThis->DrawSprite();

	return TRUE ;
}


void  CSceneEditorView::DrawSprite()
{
	while(TRUE)
	{
		WaitForSingleObject(m_ePaint.m_hObject,INFINITE);
		if (m_bIsThreadQuit )
		{
			break;
		}
		if(GetSafeHwnd())
		{
			try
			{
				CDC* pDC = GetDC();
				if(pDC)
				{
					if(m_pSpriteDraw )
					{
						m_pSpriteDraw->Draw(pDC);
					}
					ReleaseDC(pDC);
				}
			}
			catch(...)
			{
				MessageBox((LPCTSTR)"绘图""异常",MB_OK);
			}
		}
		m_ePaint.ResetEvent();
	}
}

void CSceneEditorView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_bIsThreadQuit = true;
	SetEvent(m_ePaint.m_hObject);
	if(WAIT_TIMEOUT == WaitForSingleObject(m_pThread->m_hThread,3000))
	{
		DWORD dwExitCode;
		if(::GetExitCodeThread(m_pThread->m_hThread,&dwExitCode))
		{
			::TerminateThread(m_pThread->m_hThread,dwExitCode);
		}
	}
}


BOOL CSceneEditorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*CRect   rect;   
	GetClientRect(&rect);   
	CBrush   brush;  
	brush.CreateSolidBrush(RGB(255,255,255));   
	pDC->FillRect(&rect,&brush);   */
	//return CView::OnEraseBkgnd(pDC);

	return TRUE;
}


void CSceneEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	/*if(m_pSpriteDraw)
	{
		m_pSpriteDraw->Initial();
		Invalidate();
	}*/

	UpdateView();
	// TODO: 在此处添加消息处理程序代码
}

void CSceneEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bDraging)
	{
		m_pSprite = m_pGame->GetSprite(point.x,point.y);
		if(m_pSprite)
		{
			m_strSpriteName = m_pGame->GetSpriteName(m_pSprite);

			m_pSpriteTree = m_pGame->GetSpriteTree(m_strSpriteName);

			m_pSprite->setDrawSpritePos(true);

			m_pMainFrm->SetSpriteValue(m_pSpriteTree);

			m_bDraging = TRUE;
			m_ptMouse = point;

			UpdateView();
		}
	}
	else //
	{
		m_ptMouse = point;
	}
	CView::OnLButtonDown(nFlags, point);
}


void CSceneEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_pSprite && m_bDraging)
	{
		m_pSprite->setDrawSpritePos(false);
		
		UpdateView();
	}

	m_pSprite = NULL;
	m_strSpriteName = "";

	m_bDraging = FALSE;

	CView::OnLButtonUp(nFlags, point);
}


void CSceneEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_pSprite && m_bDraging && m_ptMouse != point)
	{
		SetNewPosition(point);
		Invalidate();

		m_ptMouse = point;
	}

	CView::OnMouseMove(nFlags, point);
}
void CSceneEditorView::SetSelectSprite(const String& spriteName)
{
	if(m_pSprite)
	{
		m_pSprite->setDrawSpritePos(false);
		m_bDraging = false;
	}

	m_strSpriteName = spriteName;
	m_pSprite = m_pGame->GetSprite(spriteName);
	if(m_pSprite)
	{
		m_pSpriteTree = m_pGame->GetSpriteTree(m_strSpriteName);

		m_pMainFrm->SetSpriteValue(m_pSpriteTree);

		m_pSprite->setDrawSpritePos(true);

		UpdateView();
	}
	else
	{
		AfxMessageBox("没有找到对应名字的精灵对象！");
	}
}

void CSceneEditorView::SetNewPosition(CPoint point)
{
	long offsetLeft = point.x - m_ptMouse.x;
	long offsetTop = point.y - m_ptMouse.y;

	if(m_pSprite)
	{
		m_pSpriteTree->setSpriteOffset(offsetLeft,offsetTop);
	
		m_pMainFrm->SetSpriteValue(m_pSpriteTree);
	}
	else
	{
		AfxMessageBox("选择拖拽的精灵！");
	}
}

void CSceneEditorView::UpdateView(BOOL bReCreate)
{
	if(bReCreate)
	{
		m_pGame->loadSprite();
	}
	Invalidate();
}

void CSceneEditorView::OnOpenXml()
{
	// TODO: 在此添加命令处理程序代码
	String strXmlFile;
	CFileDialog dlgFile(TRUE,NULL,NULL,OFN_HIDEREADONLY,"Describe Files (*.xml)|*.xml", NULL);
	if (dlgFile.DoModal())
	{
		strXmlFile = dlgFile.GetPathName();
		m_pGame->loadRes(strXmlFile);

		m_pMainFrm->BuildSceneTree();

		UpdateView(TRUE);
	}
}


void CSceneEditorView::OnSaveXml()
{
	// TODO: 在此添加命令处理程序代码
	String filePath = "";
	String fileTitle = "";
	CFileDialog dlgFile(FALSE,"xml",NULL,OFN_HIDEREADONLY,"Describe Files (*.xml)|*.xml", NULL);
	if (dlgFile.DoModal())
	{
		filePath = dlgFile.GetPathName();
		fileTitle = dlgFile.GetFileTitle();

		SaveXml test(&g_spriteTree);

		test.setXmlFileName(fileTitle);
		test.setXmlFileSavePath(filePath);
		test.saveConfig();
		test.saveAll();
	}
}


void CSceneEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_pSpriteTree)
	{
		long nLeftOffset = 0;
		long nTopOffset = 0;
		switch(nChar)
		{
		case VK_DELETE:
			MessageBox("你确定要删除该精灵吗？");
			break;
		case VK_LEFT:   
			nLeftOffset -= 1;
			break;
		case VK_UP:          
			nTopOffset -= 1;
			break;
		case VK_RIGHT:
			nLeftOffset += 1;
			break;
		case VK_DOWN:
			nTopOffset += 1;
			break;
		}

		if(nLeftOffset != 0 || nTopOffset != 0)
		{
			m_pSpriteTree->setSpriteOffset(nLeftOffset,nTopOffset);	
			m_pMainFrm->SetSpriteValue(m_pSpriteTree);
			UpdateView();
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CSceneEditorView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	char szFileName[512] = {0};
	//UINT nFiles = DragQueryFile(hDropInfo,0xFFFFFFFF, NULL, 0);
	
	String strFileName;
	DragQueryFile(hDropInfo, 0, szFileName, 512);
	strFileName = szFileName;

	if(strFileName != "")
	{
		CDragSprite DragSpriteDlg(szFileName);
		if(DragSpriteDlg.DoModal() == IDOK)
		{
			TSpriteTree treeNode;
				
			treeNode.setName(DragSpriteDlg.m_strSurfaceName);
			treeNode.setType(DragSpriteDlg.m_strSpriteType);
	
			treeNode.setPath(DragSpriteDlg.m_strFileName);
			treeNode.setRow(DragSpriteDlg.m_nRow);
			treeNode.setCol(DragSpriteDlg.m_nCol);
			
			g_spriteTree.addASprite(treeNode);
			m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;
			if(m_pMainFrm)
			{
				m_pMainFrm->BuildSceneTree();
				m_pMainFrm->UpdateSceneView(TRUE);
			}
		}
	}
	DragFinish(hDropInfo);
	CView::OnDropFiles(hDropInfo);

}

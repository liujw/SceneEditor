
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

#include "MyGameCreate.h"

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
//	ON_WM_ACTIVATE()
//	ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CSceneEditorView 构造/析构

CSceneEditorView::CSceneEditorView()
{
	// TODO: 在此处添加构造代码
	m_pSpriteDraw = NULL;
	m_bIsThreadQuit = FALSE;
	m_pThread = NULL;
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
	//cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,0,(HBRUSH)::GetStockObject(RGB(255,255,255)),0); 

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
	m_pSpriteDraw = new CSpriteDraw(this);

	m_pThread = AfxBeginThread(PaintThread,this,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if(m_pThread)
	{
		m_pThread->ResumeThread();
	}
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
	CRect   rect;   
	GetClientRect(&rect);   
	CBrush   brush;   

	brush.CreateSolidBrush(RGB(255,255,255));   
	pDC->FillRect(&rect,&brush);   
	return CView::OnEraseBkgnd(pDC);

	//return TRUE;
}

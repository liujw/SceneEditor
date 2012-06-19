
// SceneEditorView.cpp : CSceneEditorView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
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

// CSceneEditorView ����/����

CSceneEditorView::CSceneEditorView()
{
	// TODO: �ڴ˴���ӹ������
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	//cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,0,(HBRUSH)::GetStockObject(RGB(255,255,255)),0); 

	return CView::PreCreateWindow(cs);
}

// CSceneEditorView ����

void CSceneEditorView::OnDraw(CDC* /*pDC*/)
{
	CSceneEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	m_ePaint.SetEvent(); //����
}


// CSceneEditorView ��ӡ


void CSceneEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSceneEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CSceneEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CSceneEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
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


// CSceneEditorView ���

#ifdef _DEBUG
void CSceneEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSceneEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSceneEditorDoc* CSceneEditorView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSceneEditorDoc)));
	return (CSceneEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CSceneEditorView ��Ϣ�������

void CSceneEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	// TODO:  �ڴ������ר�õĴ�������
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
				MessageBox((LPCTSTR)"��ͼ""�쳣",MB_OK);
			}
		}
		m_ePaint.ResetEvent();
	}
}

void CSceneEditorView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect   rect;   
	GetClientRect(&rect);   
	CBrush   brush;   

	brush.CreateSolidBrush(RGB(255,255,255));   
	pDC->FillRect(&rect,&brush);   
	return CView::OnEraseBkgnd(pDC);

	//return TRUE;
}

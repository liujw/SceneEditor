
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
	// ��׼��ӡ����
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

// CSceneEditorView ����/����

CSceneEditorView::CSceneEditorView()
{
	// TODO: �ڴ˴���ӹ������
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

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
	// TODO: �ڴ˴������Ϣ����������
}

void CSceneEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
		AfxMessageBox("û���ҵ���Ӧ���ֵľ������");
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
		AfxMessageBox("ѡ����ק�ľ��飡");
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_pSpriteTree)
	{
		long nLeftOffset = 0;
		long nTopOffset = 0;
		switch(nChar)
		{
		case VK_DELETE:
			MessageBox("��ȷ��Ҫɾ���þ�����");
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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


#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "SceneEditor.h"

#include "SpriteTree.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
	
const char* szAlignX[] = {"none","Left","Center","Right"};
const char* szAlignY[] = {"none","Top","Center","Bottom"};

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_pTreeNode = NULL;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyCmb = 0;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 1))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pSurface = new CMFCPropertyGridProperty(_T("Surface"));
	
	m_pSurfaceName = new CMFCPropertyGridProperty(_T("SurfaceName"), (_variant_t)"", _T("Surface Name"));
	pSurface->AddSubItem(m_pSurfaceName);
	registerSettor("SurfaceName",SetSurfaceName);

	static const TCHAR szPngFilter[] = _T("图像文件(*.png)|*.png|所有文件(*.*)|*.*||");
	m_pFileName = new CMFCPropertyGridFileProperty(_T("FileName"), TRUE, _T(""), _T("ico"), 0, szPngFilter, _T("surface图像文件"));
	pSurface->AddSubItem(m_pFileName);
	registerSettor("FileName",SetFileName);	

	m_pPosName = new CMFCPropertyGridProperty(_T("PosName"), (_variant_t)"", _T("坐标名字"));
	pSurface->AddSubItem(m_pPosName);
	registerSettor("PosName",SetPosName);	
	m_wndPropList.AddProperty(pSurface);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Position"), 0, FALSE);

	m_pLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t) 0l, _T("精灵的Left值"));
	pSize->AddSubItem(m_pLeft);
	registerSettor("Left",SetLeft);

	m_pTop = new CMFCPropertyGridProperty( _T("Top"), (_variant_t) 0l, _T("精灵的Top值"));
	pSize->AddSubItem(m_pTop);
	registerSettor("Top",SetTop);

	m_pWidth = new CMFCPropertyGridProperty(_T("Width"), (_variant_t) 0l, _T("精灵的高度"));
	pSize->AddSubItem(m_pWidth);
	registerSettor("Width",SetWidth);

	m_pHeight = new CMFCPropertyGridProperty( _T("Height"), (_variant_t) 0l, _T("精灵的宽度"));
	pSize->AddSubItem(m_pHeight);
	registerSettor("Height",SetHeight);

	pSize->Expand(TRUE);
	m_wndPropList.AddProperty(pSize);

	//test add
	CMFCPropertyGridProperty* pAlign = new CMFCPropertyGridProperty(_T("Align"), 0, FALSE);

	m_pAlignX = new CMFCPropertyGridProperty(_T("AlignX"),_T(""),_T("X方向上对齐方式"));
	for(long i = 1; i<4;i++)
	{
		m_pAlignX->AddOption(szAlignX[i]);
	}
	m_pAlignX->AllowEdit(FALSE);
	pAlign->AddSubItem(m_pAlignX);
	registerSettor("AlignX",SetAlignX);

	m_pAlignY = new CMFCPropertyGridProperty(_T("AlignY"),_T(""),_T("Y方向上对齐方式"));
	for(long i = 1; i<4;i++)
	{
		m_pAlignY->AddOption(szAlignY[i]);
	}
	m_pAlignY->AllowEdit(FALSE);
	pAlign->AddSubItem(m_pAlignY);
	registerSettor("AlignY",SetAlignY);

	pAlign->Expand(TRUE);
	m_wndPropList.AddProperty(pAlign);

	CMFCPropertyGridProperty* pLevel = new CMFCPropertyGridProperty(_T("Level"), 0, FALSE);

	m_pLevel = new CMFCPropertyGridProperty( _T("Z"), (_variant_t) 0l, _T("精灵的层次"));
	pLevel->AddSubItem(m_pLevel);
	registerSettor("Z",SetZ);
	pLevel->Expand(TRUE);

	m_wndPropList.AddProperty(pLevel);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

void CPropertiesWnd::SetSpriteValue(TSpriteTree* pTreeNode)
{
	m_pTreeNode = pTreeNode;

	m_pSurfaceName->SetValue((_variant_t)(pTreeNode->getName().c_str()));
	m_pFileName->SetValue((_variant_t)(pTreeNode->getPath().c_str()));
	m_pLeft->SetValue((_variant_t)(pTreeNode->getLeft()));
	m_pTop->SetValue((_variant_t)(pTreeNode->getTop()));

	VSprite* pSprite = pTreeNode->getSprite();
	long nWidth = pTreeNode->getWidth();
	if(nWidth == 0)
		nWidth = pSprite->getWidth();

	m_pWidth->SetValue((_variant_t)nWidth);

	long nHeight = pTreeNode->getHeight();
	if(nHeight == 0)
		nHeight = pSprite->getHeight();
	m_pHeight->SetValue((_variant_t)nHeight);	

	m_pAlignX->SetValue((_variant_t)(szAlignX[long(pTreeNode->getAlignX()+1)]));
	m_pAlignY->SetValue((_variant_t)(szAlignY[long(pTreeNode->getAlignY()+1)]));

	m_pLevel->SetValue((_variant_t)(pTreeNode->getZ()));

	m_pPosName->SetValue((_variant_t)(pTreeNode->getPosName().c_str()));
}

void CPropertiesWnd::ResetSpritePos()
{
	if(m_pTreeNode)
	{
		m_pLeft->SetValue((_variant_t)(m_pTreeNode->getLeft()));
		m_pTop->SetValue((_variant_t)(m_pTreeNode->getTop()));
	}
}

LRESULT CPropertiesWnd::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	if(m_pTreeNode == NULL)
		return 0;

	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam;

    COleVariant vPropValue=pProp->GetValue();
	vPropValue.ChangeType(VT_BSTR);
	CString strValue = CString(vPropValue.bstrVal);

	vPropValue=pProp->GetOriginalValue();
	vPropValue.ChangeType(VT_BSTR);
	CString strOldValue = CString(vPropValue.bstrVal);

	CString strKey = pProp->GetName();

	funSpriteSet pSpriteSettor = m_spriteSetMap[strKey.GetBuffer()];
	if(pSpriteSettor)
	{
		if((*pSpriteSettor)(strOldValue.GetBuffer(),strValue.GetBuffer(),m_pTreeNode))
		{
			m_pMainFrm = (CMainFrame *)theApp.m_pMainWnd;
			m_pMainFrm->UpdateSceneView();
			ResetSpritePos();
		}
	}

	return 0;
}

 BOOL CPropertiesWnd::SetSurfaceName(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 //暂步支持
	 return FALSE;
 }
 BOOL CPropertiesWnd::SetFileName(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if(pTreeNode)
	 {
		 //pTreeNode->setPath(strValue);

		 return TRUE;
	 }

	 return FALSE;
 }
 BOOL CPropertiesWnd::SetPosName(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if(pTreeNode)
	 {
		 pTreeNode->setPosName(strValue);

		 return TRUE;
	 }

	 return FALSE;
 }
 BOOL CPropertiesWnd::SetLeft(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if(pTreeNode)
	 {
		 long nLeft = strToInt(strValue);
		 pTreeNode->setSpriteLeft(nLeft);

		 return TRUE;
	 }
	 return FALSE;
 }
 BOOL CPropertiesWnd::SetTop(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if(pTreeNode)
	 {
		 long nTop = strToInt(strValue);
		 pTreeNode->setSpriteTop(nTop);

		 return TRUE;
	 }
	 return FALSE;
 }
 BOOL CPropertiesWnd::SetWidth(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 /*if(pTreeNode && pSprite)
	 {
		 long nWidth = strToInt(strValue);
		 pTreeNode->setWidth(nWidth);

		 return TRUE;
	 }*/
	 return FALSE;
 }
 BOOL CPropertiesWnd::SetHeight(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 /*if(pTreeNode && pSprite)
	 {
		 long nHeight = strToInt(strValue);
		 pTreeNode->setHeight(nHeight);

		 return TRUE;
	 }*/
	 return FALSE;
 }
 long getAlign(const char* pAlign[],const char* value)
 {
	long align = -1;
	for(long i= 0 ;i < 4;i++)
	{
		 if(strcmp(pAlign[i], value) == 0)
		 {
			align = i-1;
			break;
		 }
	 }
	return align;
 }
  BOOL CPropertiesWnd::SetAlignX(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if(strValue == strOldValue)
		 return FALSE;

	 long align = getAlign(szAlignX, strValue.c_str());
	 pTreeNode->setSpriteAlignX(align);
	 
	 return TRUE;
 }

 BOOL CPropertiesWnd::SetAlignY(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if(strValue == strOldValue)
		 return FALSE;

	 long align = getAlign(szAlignY, strValue.c_str());
	 pTreeNode->setSpriteAlignY(align);
	 
	 return TRUE;
 }

 BOOL CPropertiesWnd::SetZ(String strOldValue,String strValue,TSpriteTree* pTreeNode)
 {
	 if (pTreeNode)
	 {
		 pTreeNode->setSpriteZ(strToInt(strValue));
	 }
	 return TRUE;
 }
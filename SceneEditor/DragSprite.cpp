// DragSprite.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "DragSprite.h"
#include "afxdialogex.h"


// CDragSprite 对话框

IMPLEMENT_DYNAMIC(CDragSprite, CDialogEx)

CDragSprite::CDragSprite(CString strFileName,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDragSprite::IDD, pParent)
{
	m_strFileName = strFileName;
}

CDragSprite::~CDragSprite()
{
}

void CDragSprite::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDragSprite, CDialogEx)
	ON_WM_CREATE()


END_MESSAGE_MAP()


// CSpriteSet 消息处理程序


int CDragSprite::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	GetClientRect(&rectDummy);
	rectDummy.bottom -= 60;

	//rectDummy.SetRectEmpty ();
	if (!m_wndPropList .Create (WS_VISIBLE | WS_CHILD, rectDummy, this, 1))
	{
	  TRACE0("Failed to create Properies Grid \n");
	  return -1;      // fail to create
	}

	m_wndPropList.EnableHeaderCtrl (FALSE);//标头
	m_wndPropList.EnableDescriptionArea ();//
	m_wndPropList.SetVSDotNetLook ();//
	
	m_pPropSurfaceName = new CMFCPropertyGridProperty(_T("SurfaceName"), (_variant_t)"", _T("Surface"));
	m_wndPropList.AddProperty(m_pPropSurfaceName);

	CMFCPropertyGridProperty* pType = new CMFCPropertyGridProperty(_T("Type"), 0, FALSE);

	m_pPropType = new CMFCPropertyGridProperty(_T("SpriteType"),_T("VBitmapSprite"),_T("VBitmapSprite"));
	m_pPropType->AddOption(_T("VBitmapSprite"));
	m_pPropType->AddOption(_T("VBitmapBaseSprite"));
	m_pPropType->AddOption(_T("VButtonSprite"));
	m_pPropType->AllowEdit(FALSE);
	pType->AddSubItem(m_pPropType);
	m_wndPropList.AddProperty(pType);
	
	//m_pPropFileName = new CMFCPropertyGridProperty(_T("SurfaceName"), (_variant_t)"", _T("Surface"));
	//m_wndPropList.AddProperty(m_pPropSurfaceName);

	m_pPropRow = new CMFCPropertyGridProperty( _T("Row"), (_variant_t) 1l, _T("图片的row"));
	m_wndPropList.AddProperty(m_pPropRow);

	m_pPropCol = new CMFCPropertyGridProperty(_T("Col"), (_variant_t) 1l, _T("图片的col"));
	m_wndPropList.AddProperty(m_pPropCol);

	return 0;
}

CString GetPropValue(CMFCPropertyGridProperty* pProp)
{
	COleVariant vPropValue = pProp->GetValue();
	vPropValue.ChangeType(VT_BSTR);
	CString strValue = CString(vPropValue.bstrVal);
	return strValue;
}
void CDragSprite::OnOK()
{
	m_strSurfaceName = GetPropValue(m_pPropSurfaceName).GetBuffer();
	if(m_strSurfaceName == "")
	{
		MessageBox("请设置SurfaceName!");
		return;
	}

	m_strSpriteType = GetPropValue(m_pPropType).GetBuffer();
	if(m_strSpriteType == "")
	{
		MessageBox("请设置SpriteType!");
		return;
	}

	CString strValue = GetPropValue(m_pPropRow).GetBuffer();
	m_nRow = atoi(strValue.GetBuffer());

	strValue = GetPropValue(m_pPropCol).GetBuffer();
	m_nCol = atoi(strValue.GetBuffer());
	
	if(m_nRow <= 0 || m_nCol <= 0 )
	{
		MessageBox("请设置准确的Row,Col值!");
		return;
	}
	
	CDialogEx::OnOK();
}

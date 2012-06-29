// SpriteSet.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "SpriteSet.h"
#include "afxdialogex.h"


// CSpriteSet 对话框

IMPLEMENT_DYNAMIC(CSpriteSet, CDialogEx)

CSpriteSet::CSpriteSet(CString strSpriteType,CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpriteSet::IDD, pParent)
{
	m_strSpriteType = strSpriteType.GetBuffer();
	m_strSurfaceName = "";
	m_strFileName = "";
	m_nRow = 1;
	m_nCol = 1;
}

CSpriteSet::~CSpriteSet()
{
}

void CSpriteSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpriteSet, CDialogEx)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)


END_MESSAGE_MAP()


// CSpriteSet 消息处理程序


int CSpriteSet::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	static const TCHAR szPngFilter[] = _T("图像文件(*.png)|*.png|所有文件(*.*)|*.*||");
	m_pPropFileName = new CMFCPropertyGridFileProperty(_T("FileName"), TRUE, _T(""), _T("png"), 0, szPngFilter, _T("surface图像文件"));
	m_wndPropList.AddProperty(m_pPropFileName);
	
	m_pPropRow = new CMFCPropertyGridProperty( _T("Row"), (_variant_t) 1l, _T("图片的row"));
	m_wndPropList.AddProperty(m_pPropRow);

	m_pPropCol = new CMFCPropertyGridProperty(_T("Col"), (_variant_t) 1l, _T("图片的col"));
	m_wndPropList.AddProperty(m_pPropCol);

	return 0;
}

LRESULT CSpriteSet::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam;

    const COleVariant& strValue=pProp->GetValue();//获得子项值

	CString strTmp = CString(strValue.bstrVal);
	strTmp = pProp->GetName();

	return 0;
}
CString CSpriteSet::GetPropValue(CMFCPropertyGridProperty* pProp)
{
	COleVariant vPropValue = pProp->GetValue();
	vPropValue.ChangeType(VT_BSTR);
	CString strValue = CString(vPropValue.bstrVal);
	return strValue;
}
void CSpriteSet::OnOK()
{
	m_strSurfaceName = GetPropValue(m_pPropSurfaceName).GetBuffer();
	if(m_strSurfaceName == "")
	{
		MessageBox("请设置SurfaceName!");
		return;
	}

	m_strFileName = GetPropValue(m_pPropFileName).GetBuffer();
	if(m_strSpriteType == "VBitmapSprite" || m_strSpriteType == "VBitmapBaseSprite" || m_strSpriteType == "VButtonSprite")
	{
		if(m_strFileName == "")
		{
			MessageBox("请设置FileName!");
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
	}
	else
	{
		m_strFileName = "";
		m_nCol = 1;
		m_nRow = 1;
	}

	CDialogEx::OnOK();
}

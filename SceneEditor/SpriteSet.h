#pragma once

#include "sysImport.h"

// CSpriteSet 对话框

class CSpriteSet : public CDialogEx
{
	DECLARE_DYNAMIC(CSpriteSet)

public:
	CSpriteSet(CString strSpriteType,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSpriteSet();

// 对话框数据
	enum { IDD = IDD_ADDSPRITE };

	CMFCPropertyGridCtrl m_wndPropList;
	CMFCPropertyGridProperty* m_pPropSurfaceName;
	CMFCPropertyGridProperty* m_pPropFileName;
	CMFCPropertyGridProperty* m_pPropRow;
	CMFCPropertyGridProperty* m_pPropCol;

	String m_strSpriteType;
	String m_strSurfaceName;
	String m_strFileName;
	long m_nRow;
	long m_nCol;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CString GetPropValue(CMFCPropertyGridProperty* pProp);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);


	virtual void OnOK();
};

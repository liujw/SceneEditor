#pragma once

#include "SysImport.h"

// CDragSprite �Ի���

class CDragSprite : public CDialogEx
{
	DECLARE_DYNAMIC(CDragSprite)

public:
	CDragSprite(CString strFileName,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDragSprite();

// �Ի�������
	enum { IDD = IDD_DRAGSPRITE };

	CMFCPropertyGridCtrl m_wndPropList;
	//test add
	String m_strSurfaceName;
	String m_strSpriteType;
	long m_nRow;
	long m_nCol;
	String m_strFileName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	CMFCPropertyGridProperty* m_pPropSurfaceName;
	CMFCPropertyGridProperty* m_pPropType;
	CMFCPropertyGridProperty* m_pPropRow;
	CMFCPropertyGridProperty* m_pPropCol;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	virtual void OnOK();
};

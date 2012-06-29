
#pragma once

#include "sysimport.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class TSpriteTree;
class CMainFrame;

typedef BOOL (*funSpriteSet)(String strOldValue,String strValue,TSpriteTree* pTreeNode); 

class CPropertiesWnd : public CDockablePane
{
// 构造
public:
	CPropertiesWnd();

	void AdjustLayout();

	void SetSpriteValue(TSpriteTree* pTreeNode);

	void ResetSpritePos();
// 特性
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

	CMFCPropertyGridProperty* m_pSurfaceName;
	CMFCPropertyGridProperty* m_pFileName;
	CMFCPropertyGridProperty* m_pLeft;
	CMFCPropertyGridProperty* m_pTop;
	CMFCPropertyGridProperty* m_pWidth;
	CMFCPropertyGridProperty* m_pHeight;
	//test add
	CMFCPropertyGridProperty* m_pAlignX;
	CMFCPropertyGridProperty* m_pAlignY;
	CMFCPropertyGridProperty* m_pLevel;
	CMFCPropertyGridProperty* m_pPosName;

	TSpriteTree* m_pTreeNode;

	CMainFrame *m_pMainFrm;
// 实现
public:
	virtual ~CPropertiesWnd();

	static BOOL SetSurfaceName(String strOldValue,String strValue,TSpriteTree* pTreeNode);
    static BOOL SetFileName(String strOldValue,String strValue,TSpriteTree* pTreeNode);
    static BOOL SetLeft(String strOldValue,String strValue,TSpriteTree* pTreeNode);
    static BOOL SetTop(String strOldValue,String strValue,TSpriteTree* pTreeNode);
	static BOOL SetWidth(String strOldValue,String strValue,TSpriteTree* pTreeNode);
    static BOOL SetHeight(String strOldValue,String strValue,TSpriteTree* pTreeNode);
	static BOOL SetAlignX(String strOldValue,String strValue,TSpriteTree* pTreeNode);
	static BOOL SetAlignY(String strOldValue,String strValue,TSpriteTree* pTreeNode);
	static BOOL SetZ(String strOldValue,String strValue,TSpriteTree* pTreeNode);
	static BOOL SetPosName(String strOldValue,String strValue,TSpriteTree* pTreeNode);

	typedef HashMap<String,funSpriteSet> TSpriteSetMap;
	TSpriteSetMap	m_spriteSetMap;

	bool registerSettor(const String& strValueType,funSpriteSet pSpriteCreator)
	{ 
		m_spriteSetMap[strValueType] = pSpriteCreator; 
		return true; 
	}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};


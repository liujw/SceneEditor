
#pragma once
#include "XTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CViewTree ����

class CViewTree : public CXTreeCtrl
{
// ����
public:
	CViewTree();

// ��д
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ʵ��
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};

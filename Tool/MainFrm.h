
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once
#include "Tool_Defines.h"

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	void Get_ClientRect(UINT iRow, UINT iCol, RECT* pRect);
	Tool::OBJECTID Get_ObjectID();
	D3DXVECTOR3 Get_Scale();
	FLOAT Get_Height();
	INT Get_UnitOccupation();
	INT Get_StateType();	// �ü�, ���� ���� ���� ��(false == �Ϲ�, true == Ư��)
	INT Get_NatureType();
	D3DXVECTOR3 Get_Axis();
	FLOAT Get_Radian();

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

private:
	CSplitterWnd	m_MainSplitter;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnDestroy();
};



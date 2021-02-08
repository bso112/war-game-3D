
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "Tool_Defines.h"

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	void Get_ClientRect(UINT iRow, UINT iCol, RECT* pRect);
	Tool::OBJECTID Get_ObjectID();
	D3DXVECTOR3 Get_Scale();
	FLOAT Get_Height();
	INT Get_UnitOccupation();
	INT Get_StateType();	// 궁수, 생성 지점 상태 값(false == 일반, true == 특별)
	INT Get_NatureType();
	D3DXVECTOR3 Get_Axis();
	FLOAT Get_Radian();

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

private:
	CSplitterWnd	m_MainSplitter;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnDestroy();
};



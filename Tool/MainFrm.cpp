
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "ControlView.h"

#include "MainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

HWND g_Main_hWnd;
HINSTANCE g_Main_hInst;

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

void CMainFrame::Get_ClientRect(UINT iRow, UINT iCol, RECT * pRect)
{
	CWnd* pView = m_MainSplitter.GetPane(iRow, iCol);
	if (nullptr == pView)
		return;

	pView->GetClientRect(pRect);
}

Tool::OBJECTID CMainFrame::Get_ObjectID()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return Tool::OBJECTID();

	return pControlView->Get_ObjectID();
}

D3DXVECTOR3 CMainFrame::Get_Scale()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return D3DXVECTOR3();

	return pControlView->Get_Scale();
}

FLOAT CMainFrame::Get_Height()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return 0;

	return pControlView->Get_Height();
}

INT CMainFrame::Get_UnitOccupation()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return -1;

	return pControlView->Get_UnitOccupation();
}

INT CMainFrame::Get_StateType()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return -1;

	return pControlView->Get_ArcherType();
}

INT CMainFrame::Get_NatureType()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return -1;

	return pControlView->Get_NatureType();
}

D3DXVECTOR3 CMainFrame::Get_Axis()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return D3DXVECTOR3();

	return pControlView->Get_Axis();
}

FLOAT CMainFrame::Get_Radian()
{
	CControlView* pControlView = dynamic_cast<CControlView*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pControlView)
		return 0.f;

	return pControlView->Get_Radian();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.hMenu = NULL;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	g_Main_hWnd = GetSafeHwnd();
	g_Main_hInst = AfxGetInstanceHandle();

	if (FALSE == m_MainSplitter.CreateStatic(this, 1, 2))
	{
		AfxMessageBox(L"Failed To Split CMainFrame");
		return FALSE;
	}

	if (FALSE == m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(g_iToolViewCX, g_iToolViewCY), pContext))
	{
		AfxMessageBox(L"Failed To Create CToolView");
		return FALSE;
	}

	if (FALSE == m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CControlView), CSize(g_iSideViewCX, g_iSideViewCY), pContext))
	{
		AfxMessageBox(L"Failed To Create CControlView");
		return FALSE;
	}

	return TRUE;
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_MainSplitter.DestroyWindow();
}

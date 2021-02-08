
// MainFrm.cpp : CMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	cs.hMenu = NULL;

	return TRUE;
}

// CMainFrame ����

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


// CMainFrame �޽��� ó����

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_MainSplitter.DestroyWindow();
}

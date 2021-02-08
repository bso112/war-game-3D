// ToolView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolView.h"
#include "MainFrm.h"

// CToolView
IMPLEMENT_DYNCREATE(CToolView, CView)

#include "MainApp.h"
#include "Infantry.h"
#include "Cube.h"
#include "Castle.h"
#include "BasePlane.h"
#include "PickingMgr.h"

HWND g_hWnd;

CToolView::CToolView()
{

}

CToolView::~CToolView()
{
}

BEGIN_MESSAGE_MAP(CToolView, CView)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CToolView 그리기입니다.

void CToolView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CToolView 진단입니다.

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

HRESULT CToolView::Ready_Window_Size()
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	RECT rcMainFrame, rcToolVeiw, rcSideView;

	pMainFrame->GetWindowRect(&rcMainFrame);
	GetClientRect(&rcToolVeiw);
	pMainFrame->Get_ClientRect(0, 1, &rcSideView);

	UINT iFrameGapWidth = (rcMainFrame.right - rcMainFrame.left) - (rcToolVeiw.right + rcSideView.right);
	UINT iFrameGapHeight = (rcMainFrame.bottom - rcMainFrame.top) - rcToolVeiw.bottom;

	if (FALSE == pMainFrame->SetWindowPos(nullptr, 0, 0, g_iToolViewCX + g_iSideViewCX + iFrameGapWidth, g_iToolViewCY + iFrameGapHeight, SWP_NOMOVE))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::Ready_Layer_Infantry(const wchar_t * pLayerTag)
{
	// Control Info
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	_float3 vScale = pMainFrame->Get_Scale();
	_float fHeight = pMainFrame->Get_Height();
	_int iOcc = pMainFrame->Get_UnitOccupation();
	_bool isSpecial = 1 == pMainFrame->Get_StateType() ? true : false;

	// Cursor Info
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	_float3 vPickingPos;
	if (false == CPickingMgr::Get_Instance()->Get_TerrainPos(ptCursor, &vPickingPos))
		return E_FAIL;

	vPickingPos = _float3(vPickingPos.x, vPickingPos.y + fHeight, vPickingPos.z);

	if (-1 != iOcc &&
		0 != vScale.x && 0 != vScale.y && 0 != vScale.z)
	{
		// Create Unit
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;

		Safe_AddRef(pManagement);

		CInfantry::INFANTRYDESC tInfantryDesc;
		tInfantryDesc.tBasedesc = BASEDESC(vPickingPos, vScale);
		tInfantryDesc.bFriendly = false;	// flase == 적군
		tInfantryDesc.tOccupation = (CInfantry::OCCUPATION)iOcc;
		tInfantryDesc.isSpecial = isSpecial;
		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STATIC, pLayerTag, &tInfantryDesc))
		{
			Safe_Release(pManagement);
			return E_FAIL;
		}

		Safe_Release(pManagement);
	}

	return S_OK;
}

HRESULT CToolView::Ready_Layer_Cube(const wchar_t * pLayerTag)
{
	// Control Info
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	_float3 vScale = pMainFrame->Get_Scale();
	_float fHeight = pMainFrame->Get_Height();
	_float3 vAxis = pMainFrame->Get_Axis();
	_float fAngle = pMainFrame->Get_Radian();

	// Cursor Info
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	_float3 vPickingPos;
	if (false == CPickingMgr::Get_Instance()->Get_TerrainPos(ptCursor, &vPickingPos))
		return E_FAIL;

	vPickingPos = _float3(vPickingPos.x, vPickingPos.y + fHeight, vPickingPos.z);

	if (0 != vScale.x && 0 != vScale.y && 0 != vScale.z)
	{
		// Create Cube
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;

		Safe_AddRef(pManagement);

		CCube::STATEDESC StateDesc;
		StateDesc.tBasedesc = BASEDESC(vPickingPos, vScale);
		StateDesc.vAxis = vAxis;
		StateDesc.fAngle = fAngle;
		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Cube", SCENE_STATIC, pLayerTag, &StateDesc))
		{
			Safe_Release(pManagement);
			return E_FAIL;
		}

		Safe_Release(pManagement);
	}

	return S_OK;
}

HRESULT CToolView::Ready_Layer_Castle(const wchar_t * pLayerTag)
{
	// Control Info
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	_float3 vScale = pMainFrame->Get_Scale();
	_float3 vAxis = pMainFrame->Get_Axis();
	_float fRadian = pMainFrame->Get_Radian();
	_float fHeight = pMainFrame->Get_Height();

	// Cursor Info
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	_float3 vPickingPos;
	if (false == CPickingMgr::Get_Instance()->Get_TerrainPos(ptCursor, &vPickingPos))
		return E_FAIL;

	vPickingPos = _float3(vPickingPos.x, vPickingPos.y + fHeight, vPickingPos.z);

	if (0 != vScale.x && 0 != vScale.y && 0 != vScale.z)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;

		Safe_AddRef(pManagement);

		CCastle::STATEDESC StateDesc;
		StateDesc.vScale = vScale;
		StateDesc.vAxis = vAxis;
		StateDesc.vPos = vPickingPos;
		StateDesc.fRadian = fRadian;
		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Castle", SCENE_STATIC, pLayerTag, &StateDesc))
		{
			Safe_Release(pManagement);
			return E_FAIL;
		}

		Safe_Release(pManagement);
	}

	return S_OK;
}

HRESULT CToolView::Ready_Layer_Waypoint(const wchar_t * pLayerTag)
{
	// Control Info
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	_float3 vScale = pMainFrame->Get_Scale();
	_float fHeight = pMainFrame->Get_Height();
	_bool isSpecial = 1 == pMainFrame->Get_StateType() ? true : false;

	// Cursor Info
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	_float3 vPickingPos;
	if (false == CPickingMgr::Get_Instance()->Get_TerrainPos(ptCursor, &vPickingPos))
		return E_FAIL;

	vPickingPos = _float3(vPickingPos.x, vPickingPos.y + fHeight, vPickingPos.z);

	if (0 != vScale.x && 0 != vScale.y && 0 != vScale.z)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;

		Safe_AddRef(pManagement);

		CBasePlane::STATEDESC StateDesc;
		StateDesc.isSpecial = isSpecial;
		StateDesc.vScale = vScale;
		StateDesc.vPos = vPickingPos;
		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BasePlane", SCENE_STATIC, pLayerTag, &StateDesc))
		{
			Safe_Release(pManagement);
			return E_FAIL;
		}

		Safe_Release(pManagement);
	}

	return S_OK;
}

HRESULT CToolView::Ready_Layer_Nature(const wchar_t * pLayerTag)
{
	// Control Info
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	_int iNatureType = pMainFrame->Get_NatureType();
	_float3 vScale = pMainFrame->Get_Scale();
	_float fHeight = pMainFrame->Get_Height();

	// Cursor Info
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	_float3 vPickingPos;
	if (false == CPickingMgr::Get_Instance()->Get_TerrainPos(ptCursor, &vPickingPos))
		return E_FAIL;

	vPickingPos = _float3(vPickingPos.x, vPickingPos.y + fHeight, vPickingPos.z);

	if (0 <= iNatureType &&
		0 != vScale.x && 0 != vScale.y && 0 != vScale.z)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;

		Safe_AddRef(pManagement);

		CBasePlane::STATEDESC StateDesc;
		StateDesc.iNatureID = iNatureType;
		StateDesc.vScale = vScale;
		StateDesc.vPos = vPickingPos;
		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BasePlane", SCENE_STATIC, pLayerTag, &StateDesc))
		{
			Safe_Release(pManagement);
			return E_FAIL;
		}

		Safe_Release(pManagement);
	}

	return S_OK;
}

HRESULT CToolView::Erase_CurInfantry()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Infantry");
	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	auto iter = --plistClone->end();
	Safe_Release(*iter);

	if (nullptr != m_pInfantry)
		Safe_Release(m_pInfantry);

	plistClone->erase(iter);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CToolView::Erase_CurCube()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Cube");
	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	auto iter = --plistClone->end();
	Safe_Release(*iter);

	if (nullptr != m_pInfantry)
		Safe_Release(m_pCube);

	plistClone->erase(iter);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CToolView::Erase_PickedInfantry()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (nullptr == m_pInfantry)
		return E_FAIL;

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Infantry");
	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	for (auto iter = plistClone->begin(); iter != plistClone->end(); iter++)
	{
		if (m_pInfantry == *iter)
		{
			Safe_Release(*iter);
			Safe_Release(m_pInfantry);

			plistClone->erase(iter);

			break;
		}
	}

	Safe_Release(pManagement);

	return S_OK;
}
HRESULT CToolView::Erase_PickedCube()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (nullptr == m_pCube)
		return E_FAIL;

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Cube");
	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	for (auto iter = plistClone->begin(); iter != plistClone->end(); iter++)
	{
		if (m_pCube == *iter)
		{
			Safe_Release(*iter);
			Safe_Release(m_pCube);

			plistClone->erase(iter);
			break;
		}
	}

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CToolView::Erase_PickedCastle()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (nullptr == m_pCastle)
		return E_FAIL;

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Castle");
	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	for (auto iter = plistClone->begin(); iter != plistClone->end(); iter++)
	{
		if (m_pCastle == *iter)
		{
			Safe_Release(*iter);
			Safe_Release(m_pCastle);

			plistClone->erase(iter);

			break;
		}
	}

	Safe_Release(pManagement);

	return S_OK;
}
HRESULT CToolView::Erase_PickedBasePlane()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return E_FAIL;

	if (nullptr == m_pBasePlane)
		return E_FAIL;

	Tool::OBJECTID eObjectID = pMainFrame->Get_ObjectID();
	list<CGameObject*>* plistClone = nullptr;

	switch (eObjectID)
	{
	case Tool::OBJ_WAYPOINT:
		plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Waypoint");
		break;
	case Tool::OBJ_NATURE:
		plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Nature");
		break;
	}

	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	for (auto iter = plistClone->begin(); iter != plistClone->end(); iter++)
	{
		if (m_pBasePlane == *iter)
		{
			Safe_Release(*iter);
			Safe_Release(m_pBasePlane);

			plistClone->erase(iter);
			break;
		}
	}

	Safe_Release(pManagement);

	return S_OK;
}
#endif
#endif //_DEBUG


// CToolView 메시지 처리기입니다.

void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	g_hWnd = GetSafeHwnd();

	CMainApp::Get_Instance()->Ready_MainApp();

	if (FAILED(Ready_Window_Size()))
		return;
}

void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (nullptr == pMainFrame)
		return;

	Tool::OBJECTID eObjectID = pMainFrame->Get_ObjectID();

	switch (eObjectID)
	{
	case Tool::OBJ_INFANTRY:
		if (FAILED(Ready_Layer_Infantry(L"Layer_Infantry")))
			return;
		break;
	case Tool::OBJ_CUBE:
		if (FAILED(Ready_Layer_Cube(L"Layer_Cube")))
			return;
		break;
	case Tool::OBJ_CASTLE:
		if (FAILED(Ready_Layer_Castle(L"Layer_Castle")))
			return;
		break;
	case Tool::OBJ_WAYPOINT:
		if (FAILED(Ready_Layer_Waypoint(L"Layer_Waypoint")))
			return;
		break;
	case Tool::OBJ_NATURE:
		if (FAILED(Ready_Layer_Nature(L"Layer_Nature")))
			return;
		break;
	}

	CView::OnLButtonDown(nFlags, point);
}

void CToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 근접 객체 선택
	CPickingMgr* pPickingMng = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMng)
		return;

	// Cursor Info
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);


	m_pInfantry = nullptr;
	m_pInfantry = dynamic_cast<CInfantry*>(pPickingMng->Picking(ptCursor));

	m_pCube = nullptr; 
	m_pCube = dynamic_cast<CCube*>(pPickingMng->Picking(ptCursor));

	m_pCastle = nullptr;
	m_pCastle = dynamic_cast<CCastle*>(pPickingMng->Picking(ptCursor));

	m_pBasePlane = nullptr;
	m_pBasePlane = dynamic_cast<CBasePlane*>(pPickingMng->Picking(ptCursor));

	if (nullptr == m_pInfantry &&
		nullptr == m_pCube &&
		nullptr == m_pCastle &&
		nullptr == m_pBasePlane)
		return;

	CView::OnRButtonDown(nFlags, point);
}

void CToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// Z : 최근 유닛 삭제
	if ('Z' == nChar)
		if (FAILED(Erase_CurInfantry()))
			return;

	// C : 최근 충돌 삭제
	if ('C' == nChar)
		if (FAILED(Erase_CurCube()))
			return;

	// X : 픽킹 객체 삭제
	if ('X' == nChar)
	{
		if (nullptr != m_pInfantry)
		{
			if (FAILED(Erase_PickedInfantry()))
				return;
		}
		else if (nullptr != m_pCube)
		{
			if (FAILED(Erase_PickedCube()))
				return;
		}
		else if (nullptr != m_pCastle)
		{
			if (FAILED(Erase_PickedCastle()))
				return;
		}
		else if (nullptr != m_pBasePlane)
		{
			if (FAILED(Erase_PickedBasePlane()))
				return;
		}
	}

	// 방향키 이동
	if (VK_UP == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Go_Straight(0.1f);
		else if (nullptr != m_pCube)
			m_pCube->Go_Straight(0.1f);
	}

	if (VK_DOWN == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Go_Backward(-0.1f);
		else if (nullptr != m_pCube)
			m_pCube->Go_Backward(-0.1f);
	}

	if (VK_RIGHT == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Go_Right(0.1f);
		else if (nullptr != m_pCube)
			m_pCube->Go_Right(0.1f);
	}

	if (VK_LEFT == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Go_Left(-0.1f);
		else if (nullptr != m_pCube)
			m_pCube->Go_Left(-0.1f);
	}

	if (VK_ADD == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Go_Up(0.1f);
		else if (nullptr != m_pCube)
			m_pCube->Go_Up(0.1f);
	}

	if (VK_SUBTRACT == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Go_Down(-0.1f);
		else if (nullptr != m_pCube)
			m_pCube->Go_Down(-0.1f);
	}

	if (VK_MULTIPLY == nChar)
	{
		if (nullptr != m_pInfantry)
			m_pInfantry->Turn_Right(0.1);
		else if (nullptr != m_pCube)
			m_pCube->Turn_Right(0.1f);
	}

	if (VK_DIVIDE == nChar)
	{

		if (nullptr != m_pInfantry)
			m_pInfantry->Turn_Left(-0.1);
		else if (nullptr != m_pCube)
			m_pCube->Turn_Left(-0.1f);
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CToolView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (0 != CMainApp::Destroy_Instance())
		AfxMessageBox(L"Failed To Destory CMainApp");
}

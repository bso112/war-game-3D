#include "stdafx.h"
#include "..\Headers\InputManager.h"
#include "KeyMgr.h"
#include "PickingMgr.h"
#include "Management.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Unit.h"
#include "GameManager.h"
#include "Waypoint.h"

IMPLEMENT_SINGLETON(CInputManager)
CInputManager::CInputManager()
{
	CKeyMgr* pKeyMgr = CKeyMgr::Get_Instance();
	if (nullptr == pKeyMgr)
		MSG_BOX("Fail to Ready InputMgr");

	pKeyMgr->RegisterObserver(this);
}

HRESULT CInputManager::OnKeyDown(_int KeyCode)
{
	CCamera_Manager* pCameraManager = CCamera_Manager::Get_Instance();

	if (KeyCode == 'Z')
	{
		CGameManager::Get_Instance()->Alter_Gold(100);
	}

	if (KeyCode == VK_LBUTTON)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		//피킹
		Pick_Object(pt);

	}
	else if (pCameraManager->Get_CamState() == CCamera_Manager::STATE_COMMAND)
	{

		_bool Use = false;

		if (KeyCode == 'E')
		{
			if (Can_Spawn_Object())
				Use = CGameManager::Get_Instance()->Use_Gold(10);
			if (Use)
				Spawn_Object(CInfantry::OCC_WARRIOR);
		}
		if (KeyCode == 'R')
		{
			if (Can_Spawn_Object())
				Use = CGameManager::Get_Instance()->Use_Gold(20);
			if (Use)
				Spawn_Object(CInfantry::OCC_ARCHER);
		}
		if (KeyCode == 'T')
		{
			if (Can_Spawn_Object())
				Use = CGameManager::Get_Instance()->Use_Gold(30);
			if (Use)
				Spawn_Object(CInfantry::OCC_MAGICIAN);
		}

	}
	if (KeyCode == VK_TAB)
	{
		if (CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Map")->Get_Active())
			CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Map")->Set_Active(false);
		else
			CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Map")->Set_Active(true);

	}




	//if (KeyCode == VK_LBUTTON)
	//{
	//	CCamera_Manager* pCameraManager = CCamera_Manager::Get_Instance();
	//	//Safe_AddRef(pCameraManager);

	//	POINT pt;
	//	GetCursorPos(&pt);
	//	ScreenToClient(g_hWnd, &pt);



	//	case CCamera_Manager::STATE_COMMAND:
	//	{
	//		_bool Use = false;
	//		CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	//		if (!pUIManager->Check_In_Mouse_Button() && Can_Spawn_Object())
	//		{
	//			CInfantry::OCCUPATION Occupation = CGameManager::Get_Instance()->Get_CurrentOccupation();
	//			switch (Occupation)
	//			{
	//			case Client::CInfantry::OCC_WARRIOR:
	//				Use = CGameManager::Get_Instance()->Use_Gold(10);
	//				break;
	//			case Client::CInfantry::OCC_ARCHER:
	//				Use = CGameManager::Get_Instance()->Use_Gold(20);
	//				break;
	//			case Client::CInfantry::OCC_MAGICIAN:
	//				Use = CGameManager::Get_Instance()->Use_Gold(30);
	//				break;
	//			case Client::CInfantry::OCC_END:
	//			default:
	//				break;
	//			}

	//			if (Use)
	//				Spawn_Object(Occupation);
	//			else
	//				// 돈 없다.
	//				int i = 0;
	//		}
	//		break;
	//	}
	//	default:
	//		break;
	//	}

	//}

	if (KeyCode == '6')
	{
		CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
		if (nullptr == pPickingMgr)
			return E_FAIL;
		WAYPOINT waypoint = pPickingMgr->Get_Waypoint(6);

		_int sign = rand() % 2 ? -1 : 1;
		_float3 fMargin = _float3((rand() % (_int)30.f) * sign, 0.f, (rand() % (_int)30.f) * sign);
		pPickingMgr->Spawn_Ally(waypoint.iNumber, waypoint.vPosition + fMargin, CInfantry::OCC_MAGICIAN);
	}

	if (KeyCode == '9')
	{
		CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
		if (nullptr == pPickingMgr)
			return E_FAIL;
		WAYPOINT waypoint = pPickingMgr->Get_Waypoint(9);

		_int sign = rand() % 2 ? -1 : 1;
		_float3 fMargin = _float3((rand() % (_int)30.f) * sign, 0.f, (rand() % (_int)30.f) * sign);
		pPickingMgr->Spawn_Ally(waypoint.iNumber, waypoint.vPosition + fMargin, CInfantry::OCC_MAGICIAN);

	}


	return S_OK;
}

HRESULT CInputManager::OnKeyPressing(_int KeyCode)
{
	return S_OK;
}

HRESULT CInputManager::OnKeyUp(_int KeyCode)
{
	return S_OK;
}

void CInputManager::Clear_DeadObject()
{
	if (m_pPicked != nullptr && m_pPicked->Get_Dead())
	{
		Safe_Release(m_pPicked);
		m_pPicked = nullptr;
	}
}

HRESULT CInputManager::Pick_Object(POINT& pt)
{
	//피킹된 인펜트리가 피킹되어있으면 피킹안함.
	CInfantry* pOldPick = dynamic_cast<CInfantry*>(m_pPicked);
	if (pOldPick != nullptr)
	{
		if (pOldPick->Get_ControlMode())
			return NO_ERROR;
	}

	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMgr)
		return E_FAIL;


	//피킹한 오브젝트를 저장
	CGameObject* pPicked = pPickingMgr->Picking(pt);
	if (nullptr == pPicked)
		return E_FAIL;

	//피킹된 게임오브젝트로 할일 하기.
	CUnit* pUnit = dynamic_cast<CUnit*>(pPicked);
	if (nullptr == pUnit)
		return E_FAIL;

	//전에 피킹된 오브젝트의 컨트롤을 해제한다.
	if (m_pPicked != nullptr)
		m_pPicked->Set_ControllMode(false);

	Safe_Release(m_pPicked);

	//새로운 오브젝트를 현재 오브젝트로 한다.
	m_pPicked = pUnit;

	Safe_AddRef(m_pPicked);


	//새로운 오브젝트를 컨트롤한다. (적군은 무시)
	m_pPicked->Set_ControllMode(true);

	//피킹된 오브젝트가 컨트롤 가능한 상태하면, 그 오브젝트로 카메라를 이동한다.
	if (m_pPicked->Get_ControlMode())
	{
		CCamera_Manager::Get_Instance()->Change_Camera_Third_Person(pUnit, 0.5f);

		CUI_Button_Skill* pSkill = nullptr;
		pSkill = dynamic_cast<CUI_Button_Skill*>(CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Battle"));

		switch (dynamic_cast<CInfantry*>(m_pPicked)->Get_OCC())
		{
		case CInfantry::OCC_WARRIOR:
			pSkill->Set_Skill(SKILL_HEAL);
			break;

		case CInfantry::OCC_ARCHER:
			pSkill->Set_Skill(SKILL_SNIPE);
			break;

		case CInfantry::OCC_MAGICIAN:
			pSkill->Set_Skill(SKILL_METEOR);
			break;

		default:
			break;
		}
	}

	return S_OK;
}

HRESULT CInputManager::Spawn_Object(_uint _eType)
{
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMgr)
		return E_FAIL;

	vector<WAYPOINT> waypoints = pPickingMgr->Get_Waypoints();

	//아군이 아무도 없을 경우 현재 정복된 웨이포인트 중 가장 숫자가 큰 웨이포인트에 생성
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	list<CGameObject*>* pAllyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");

	if (nullptr == pAllyList || pAllyList->empty())
	{
		list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Waypoint");
		if (nullptr == plistClone)
			return E_FAIL;

		CWaypoint* pTarget = nullptr;
		_uint iBiggerNum = 0;

		for (auto& pWaypoint : *plistClone)
		{
			WAYPOINT Waypoint = dynamic_cast<CWaypoint*>(pWaypoint)->Get_Waypoint();

			if (false == Waypoint.bHide && true == Waypoint.bOccupied)
			{
				if (iBiggerNum <= Waypoint.iNumber)
				{
					iBiggerNum = Waypoint.iNumber;
					pTarget = dynamic_cast<CWaypoint*>(pWaypoint);
				}
			}
		}
		if (pTarget != nullptr)
			pPickingMgr->Spawn_Ally(iBiggerNum, pTarget->Get_BasecampPos(), _eType);
	}
	//아군이 있으면
	else
	{
		_uint iBiggerNum = 0;
		CWaypoint* pTarget = nullptr;

		//웨이포인트를 순회해서 지휘관이 있는 정복된 웨이포인트에서 아군 생성
		for (auto& point : waypoints)
		{
			if (nullptr == m_pPicked)
				continue;

			//숨겨진 웨이포인트는 제외
			if (point.bHide)
				continue;

			list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Waypoint");
			if (nullptr == plistClone)
				return E_FAIL;


			for (auto& pWaypoint : *plistClone)
			{
				if (nullptr != pWaypoint)
				{
					WAYPOINT Waypoint = dynamic_cast<CWaypoint*>(pWaypoint)->Get_Waypoint();
					//현재 지휘관이 있는 웨이포인트 찾기
					CTransform* pPickedTransform = (CTransform*)m_pPicked->Find_Component(L"Com_Transform");
					if (nullptr == pPickedTransform) return E_FAIL;
					_float fDist = D3DXVec3Length(&(Waypoint.vPosition - pPickedTransform->Get_Position()));
				
					//현재 지휘관이 있는 웨이포인트가
					if (fDist < Waypoint.fRadius)
					{
						//안숨겨져있고 정복되어있으면
						if (false == Waypoint.bHide && true == Waypoint.bOccupied)
						{
							if (iBiggerNum <= Waypoint.iNumber)
							{
								iBiggerNum = Waypoint.iNumber;
								pTarget = dynamic_cast<CWaypoint*>(pWaypoint);
							}
						}

					}
				}
			}
		}

		if (pTarget != nullptr)
			pPickingMgr->Spawn_Ally(iBiggerNum, pTarget->Get_BasecampPos(), _eType);
	}


	return S_OK;
}

_bool CInputManager::Can_Spawn_Object()
{
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMgr)
		return E_FAIL;

	vector<WAYPOINT> waypoints = pPickingMgr->Get_Waypoints();

	CManagement* pManagement = CManagement::Get_Instance();

	if (nullptr == pManagement)
		return E_FAIL;
	list<CGameObject*>* pAllyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");

	//아군이 아무도 없을 경우
	if (nullptr == pAllyList || pAllyList->empty())
	{
		return true;
	}
	//아군이 있을경우
	else
	{
		for (auto& point : waypoints)
		{
			//지휘관이 없으면 생성못함
			if (nullptr == m_pPicked)
				return NO_ERROR;
			//숨겨진 웨이포인트는 제외
			if (point.bHide)
				continue;
			//현재 지휘관이 있는 웨이포인트 찾기
			CTransform* pPickedTransform = (CTransform*)m_pPicked->Find_Component(L"Com_Transform");
			if (nullptr == pPickedTransform) return E_FAIL;
			_float fDist = D3DXVec3Length(&(point.vPosition - pPickedTransform->Get_Position()));
			//찾았으면
			if (fDist < point.fRadius)
			{
				//정복된 웨이포인트가 있으면 유닛생성가능
				if (point.bOccupied)
				{
					return true;
				}
			}
		}

	}

	return false;
}

void CInputManager::Free()
{
	Safe_Release(m_pPicked);

	CKeyMgr* pKeyMgr = CKeyMgr::Get_Instance();
	if (nullptr == pKeyMgr)
		MSG_BOX("Fail to Release InputMgr");

	pKeyMgr->UnRegisterObserver(this);
}

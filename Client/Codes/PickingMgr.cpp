#include "stdafx.h"
#include "..\Headers\PickingMgr.h"
#include "Management.h"
#include "Terrain.h"
#include "Waypoint.h"
#include "GameManager.h"
#include "Unit.h"
#include "PickingMgr.h"
#include "Camera_Manager.h"

IMPLEMENT_SINGLETON(CPickingMgr)
CPickingMgr::CPickingMgr()
{
}

HRESULT CPickingMgr::Ready_PickingMgr(PDIRECT3DDEVICE9 pGraphic_Device)
{
	//터레인 저장
	CManagement* pManagement = CManagement::Get_Instance();
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_Terrain"));
	if (nullptr == pTerrain) return -1;
	m_pTerrain = pTerrain;


#pragma region LOADFILE
	HANDLE hFile = 0;
	_ulong dwByte = 0;

	WAYPOINT WaypointDesc;
	_uint iNumber = 0;

	hFile = CreateFile(L"../Bin/ToolFiles/Waypoint.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &WaypointDesc, sizeof(WAYPOINT), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		WaypointDesc.iNumber = iNumber++;
		WaypointDesc.fRadius = WaypointDesc.fRadius * 2.f;
		if (FAILED(Create_Waypoint(pGraphic_Device, WaypointDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);
#pragma endregion


	//////웨이포인트 생성
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(30.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(70.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(130.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(200.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;


	return S_OK;
}

void CPickingMgr::Register_Observer(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return;
	m_listObserver.push_back(pGameObject);
}

void CPickingMgr::UnRegister_Observer(CGameObject * pGameObject)
{
	auto& iter = m_listObserver.begin();
	while (iter != m_listObserver.end())
	{
		if (*iter == pGameObject)
			iter = m_listObserver.erase(iter);
		else
			++iter;
	}
}

CGameObject * CPickingMgr::Picking(POINT & pt)
{
	vector<CGameObject*> vecPicked;

	for (auto& obj : m_listObserver)
	{
		if (obj->Picking(pt))
		{
			vecPicked.push_back(obj);
		}

	}

	if (!vecPicked.empty())
	{
		//가장 z값이 작은 것을 구한다.
		sort(vecPicked.begin(), vecPicked.end(), [](CGameObject* pA, CGameObject* pB) {
			CTransform* pTransformA = (CTransform*)pA->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pB->Find_Component(L"Com_Transform");
			if (nullptr == pTransformA || nullptr == pTransformB)
				return false;
			return pTransformA->Get_State(CTransform::STATE_POSITION).z > pTransformB->Get_State(CTransform::STATE_POSITION).z;
		});

	}

	if (vecPicked.empty())
		return nullptr;

	return vecPicked.front();
}

CGameObject * CPickingMgr::Get_Closest_Object(_float3 vOrigin, CGameObject* pSelf)
{
	if (nullptr == m_pTerrain)
		return nullptr;


	_float fMinDist = FLT_MAX;
	CGameObject* pClosest = nullptr;
	//해당 위치에 가장 가까운 오브젝트를 찾는다.
	for (auto& pObj : m_listObserver)
	{
		if (pObj == pSelf)
			continue;

		CTransform* pTransform = (CTransform*)pObj->Find_Component(L"Com_Transform");
		if (nullptr == pTransform) return nullptr;
		_float fDist = D3DXVec3Length(&(vOrigin - pTransform->Get_Position()));
		if (fMinDist > fDist)
		{
			fMinDist = fDist;
			pClosest = pObj;
		}

		return pClosest;
	}

	return nullptr;
}

vector<CGameObject*> CPickingMgr::OverlapShpere(_float3 vOrigin, _float fRadius, CGameObject* pSelf)
{
	vector<CGameObject*> Collided;
	for (auto& pObserver : m_listObserver)
	{
		if (pObserver == pSelf)
			continue;

		CTransform* pObserverTransform = (CTransform*)pObserver->Find_Component(L"Com_Transform");
		if (nullptr == pObserverTransform)
			continue;
		_float fDist = D3DXVec3Length(&(vOrigin - pObserverTransform->Get_Position()));
		if (fDist < fRadius)
			Collided.push_back(pObserver);
	}
	return Collided;
}

vector<CGameObject*> CPickingMgr::OverlapShpere(_float3 vOrigin, _float fRadius, SCENEID pLayerSceneID, const _tchar * pLayerTag, CGameObject * pSelf)
{
	vector<CGameObject*> Collided;
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return vector<CGameObject*>();
	list<CGameObject*>* pObjList = pManagement->Get_ObjectList(pLayerSceneID, pLayerTag);
	if (nullptr == pObjList)
		return vector<CGameObject*>();

	for (auto& pObj : *pObjList)
	{
		if (pObj == pSelf)
			continue;

		CTransform* pObjTransform = (CTransform*)pObj->Find_Component(L"Com_Transform");
		if (nullptr == pObjTransform)
			continue;
		_float fDist = D3DXVec3Length(&(vOrigin - pObjTransform->Get_Position()));
		if (fDist < fRadius)
			Collided.push_back(pObj);
	}
	return Collided;
}

_bool CPickingMgr::Get_TerrainPos(POINT & pt, _float3 * _vTerrainPos)
{
	if (nullptr == m_pTerrain)
		return false;
	return m_pTerrain->Get_TerrainPos(pt, _vTerrainPos);
}

_bool CPickingMgr::Get_TerrainPos(_float3 vWorldPos, _float3 & _vTerrainPos)
{
	if (nullptr == m_pTerrain)
		return false;
	return m_pTerrain->Get_TerrainPos(vWorldPos, _vTerrainPos);
}

_bool CPickingMgr::Get_TerrainNormal(_float3 vWorldPos, _float3 & _vTerrainNormal)
{
	if (nullptr == m_pTerrain)
		return false;
	return m_pTerrain->Get_TerrainNormal(vWorldPos, _vTerrainNormal);
}

_float CPickingMgr::Update_Blur_Dist()
{
	CCamera_Manager* pCameraMgr = CCamera_Manager::Get_Instance();
	m_fFOV = pCameraMgr->Get_CurrentCamera()->Get_fFOV();
	return _float();
}

_float3 CPickingMgr::Get_Blur_Center()
{
	////_bool bMouseMove = 0;
	////if (CInput_Device::Get_Instance()->Get_DIMouseMoveState(CInput_Device::DIMM_X) || CInput_Device::Get_Instance()->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
	////{
	////	bMouseMove = 1;
	////}
	//_float3 vBlurCenter = CManagement::Get_Instance()->Get_CamPosition();
	////_float3 vTemp;

	////if(bMouseMove == 0)

	//if (CGameManager::Get_Instance()->Get_CurrSkill() == SKILL_SNIPE)
	//{
	//	CGameObject* pTarget = CPickingMgr::Get_Instance()->Picking(POINT{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 });
	//	if (pTarget != nullptr)
	//		vBlurCenter = ((CUnit*)pTarget)->Get_Position();
	//	//else if (CPickingMgr::Get_Instance()->Get_TerrainPos(POINT{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 },vTemp))
	//	else
	//		CPickingMgr::Get_Instance()->Get_TerrainPos(POINT{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 }, &vBlurCenter);
	//}

	return m_vCurFocus;
}

void CPickingMgr::Update_Blur_Center()
{
	CCamera_Manager* pCameraMgr = CCamera_Manager::Get_Instance();

	m_fFOV = pCameraMgr->Get_CurrentCamera()->Get_fFOV();

	_float3 vBlurCenter = CManagement::Get_Instance()->Get_CamPosition();
	//_float3 vTemp;

	//if(bMouseMove == 0)
	m_bIsSniping = 0;

	if (CGameManager::Get_Instance()->Get_CurrSkill() == SKILL_SNIPE)
	{
		m_bIsSniping = 1;
		CGameObject* pTarget = CPickingMgr::Get_Instance()->Picking(POINT{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 });
		if (pTarget != nullptr)
			vBlurCenter = ((CUnit*)pTarget)->Get_Position();
		//else if (CPickingMgr::Get_Instance()->Get_TerrainPos(POINT{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 },vTemp))
		else
			CPickingMgr::Get_Instance()->Get_TerrainPos(POINT{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 }, &vBlurCenter);
	}
	m_vCurFocus = vBlurCenter;
}


vector<WAYPOINT> CPickingMgr::Get_Waypoints()
{
	vector<WAYPOINT> waypoints;
	for (auto& pWaypoint : m_Waypoints)
		waypoints.push_back(pWaypoint->Get_Waypoint());

	return waypoints;
}

WAYPOINT CPickingMgr::Get_Waypoint(size_t iIndex)
{
	if (m_Waypoints.size() <= iIndex)
		return WAYPOINT();

	return m_Waypoints[iIndex]->Get_Waypoint();
}

WAYPOINT CPickingMgr::Get_CurrentWaypoint()
{
	_int maxNum = 0;
	WAYPOINT result;
	for (auto& pWaypoint : m_Waypoints)
	{
		WAYPOINT tWaypont = pWaypoint->Get_Waypoint();
		if (!tWaypont.bHide && tWaypont.bOccupied)
		{
			if (maxNum < tWaypont.iNumber)
			{
				maxNum = tWaypont.iNumber;
				result = tWaypont;
			}
		}
	}
	return result;
}

HRESULT CPickingMgr::Occupy_Waypoint(size_t _iIndex, _double dOccupyRate)
{
	if (m_Waypoints.size() <= _iIndex)
		return E_FAIL;

	m_Waypoints[_iIndex]->Occupy(dOccupyRate);

	return S_OK;
}

HRESULT CPickingMgr::Spawn_Ally(size_t iWaypointIndex, _float3 vPos, _uint eUnitType)
{
	if (m_Waypoints.size() <= iWaypointIndex)
		return E_FAIL;

	return 	m_Waypoints[iWaypointIndex]->Spawn_Ally(vPos, eUnitType);
}

void CPickingMgr::Clear_PickingMgr()
{
	for (auto& pWaypoint : m_Waypoints)
	{
		Safe_Release(pWaypoint);
	}

	m_Waypoints.clear();
}

HRESULT CPickingMgr::Create_Waypoint(PDIRECT3DDEVICE9 pGraphic_Device, WAYPOINT WaypointDesc)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return E_FAIL;

	//지면 살짝 위로
	_float offSetY = 1.f;
	WAYPOINT tWaypoint;
	CWaypoint* pWaypoint = nullptr;
	CWaypoint::STATEDESC tWaypointDesc;

	memcpy(&tWaypoint, &WaypointDesc, sizeof(WAYPOINT));
	Get_TerrainPos(tWaypoint.vPosition, tWaypoint.vPosition);
	tWaypoint.vPosition.y += offSetY;
	tWaypointDesc.tWaypoint = tWaypoint;

	pWaypoint = (CWaypoint*)pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Waypoint", SCENE_STAGE, L"Layer_Waypoint", &tWaypoint);
	if (nullptr == pWaypoint) return E_FAIL;
	Safe_AddRef(pWaypoint);
	m_Waypoints.push_back(pWaypoint);

	return S_OK;
}


void CPickingMgr::Free()
{
	//프리할때 멤버변수인 웨이포인트 지우면 터짐. 
	//왜냐하면 오브젝트 매니저 지우기 전에 Clear_PickingMgr로 이미 웨이포인트 클리어했고
	//오브젝트매니저에서 지웠으니 댕글링포인터가 됨.
}

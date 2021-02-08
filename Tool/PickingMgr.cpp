#include "stdafx.h"
#include "PickingMgr.h"
#include "Management.h"
#include "Terrain.h"

IMPLEMENT_SINGLETON(CPickingMgr)

CPickingMgr::CPickingMgr()
{
}

HRESULT CPickingMgr::Ready_PickingMgr()
{
	CManagement* pManagement = CManagement::Get_Instance();
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pManagement->Get_ObjectPointer(SCENE_STATIC, L"Layer_Terrain"));
	if (nullptr == pTerrain) return -1;
	m_pTerrain = pTerrain;

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
	return vector<CGameObject*>();
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

void CPickingMgr::Free()
{
}

#include "stdafx.h"
#include "..\Headers\Unit.h"
#include "PickingMgr.h"

CUnit::CUnit(PDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CUnit::CUnit(const CUnit & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUnit::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CUnit::Ready_GameObject(void * pArg)
{

	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMgr) return E_FAIL;
	vector<WAYPOINT> waypoints = pPickingMgr->Get_Waypoints();

	for (size_t i = 0; i < waypoints.size(); ++i)
	{
		////정렴된 웨이포인트는 무시
		//if (waypoints[i].bOccupied)
		//	continue;

		m_WayPoints.push_back(waypoints[i]);
	}

	return S_OK;
}

_int CUnit::Update_GameObject(_double TimeDelta)
{

	return _int();
}

_int CUnit::Late_Update_GameObject(_double TimeDelt)
{
	return _int();
}

HRESULT CUnit::Render_GameObject()
{
	return S_OK;
}

void CUnit::Render_Debug()
{
}

_matrix CUnit::Get_WorldMatrix()
{
	return _matrix();
}



_float3 CUnit::Get_Position()
{
	return _float3();
}

_bool CUnit::IsInWaypoint()
{
	//웨이포인트들과 거리를 비교해서 지휘관이 웨이포인트 안에 있으면 일정시간 후 점령
	for (auto& waypoint : m_WayPoints)
	{
		_float fDist = D3DXVec3Length(&(Get_Position() - waypoint.vPosition));
		//웨이포인트 진입시 점령표시
		if (fDist < m_WayPoints.front().fRadius)
		{
			return true;
		}
	}
	return false;
}

void CUnit::OnControlModeEnter()
{

}

void CUnit::OnControlModeExit()
{
}

_bool CUnit::TakeDamage(_int _iDamage)
{

	m_tStat.iCurrHp = max(m_tStat.iCurrHp - _iDamage, 0);
	if (m_tStat.iCurrHp <= 0)
	{
		m_bDead = true;
		OnDead();
		return false;
	}

	m_bInvincible = true;

	OnTakeDamage(_iDamage);

	return true;
}

_bool CUnit::TakeDamage(_double _dDamage)
{
	m_AccDamage += _dDamage;
	if (1.0 <= m_AccDamage)
	{
		m_AccDamage = 1.0;
		m_tStat.iCurrHp = max(m_tStat.iCurrHp - _dDamage, 0);
	}

	if (m_tStat.iCurrHp <= 0)
	{
		m_bDead = true;
		OnDead();
		return true;
	}

	m_bInvincible = true;

	OnTakeDamage(_dDamage);

	return true;
}

void CUnit::OnTakeDamage(_int _iDamage)
{
}

void CUnit::Free()
{
	CGameObject::Free();
}

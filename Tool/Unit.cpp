#include "stdafx.h"
#include "Unit.h"

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
	return S_OK;
}

_int CUnit::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CUnit::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CUnit::Render_GameObject()
{
	return S_OK;
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
	}

	m_bInvincible = true;

	return true;
}

void CUnit::Free()
{
	CGameObject::Free();
}

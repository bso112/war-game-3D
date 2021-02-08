#include "stdafx.h"
#include "..\Headers\Weapon.h"

CWeapon::CWeapon(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Ready_GameObject(void * pArg)
{
	return S_OK;
}

_int CWeapon::Update_GameObject(_double TimeDelta)
{
	return _int(0);
}

_int CWeapon::Late_Update_GameObject(_double TimeDelta)
{
	return _int(0);
}

HRESULT CWeapon::Render_GameObject()
{
	return S_OK;
}

HRESULT CWeapon::Attack(_double TimeDelta)
{
	return S_OK;
}

void CWeapon::Free()
{
	CGameObject::Free();
}

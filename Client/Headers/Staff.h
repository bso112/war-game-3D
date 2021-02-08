#pragma once
#include "Weapon.h"

BEGIN(Client)

class CEnergyBall;
class CStaff final : public CWeapon
{
private:
	explicit CStaff(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CStaff(const CStaff& rhs);
	virtual ~CStaff() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Attack(_double TimeDelta);

private:
	_double		m_AccTimer = 0.0;
	_bool		m_isAttack = false;
	_bool		m_hasEnergyBall = false;
	_bool		m_isCharged = false;

private:	// EnergyBall
	CEnergyBall*	m_pEnergyBall = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Layer_EnergyBall(const _tchar* pLayerTag);
	_int Update_EnergyBall_ParentsMatrix();
	_int Update_Attack(_double TimeDelta);

public:
	static CStaff* Create_Staff_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
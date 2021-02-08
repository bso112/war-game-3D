#pragma once
#include "Weapon.h"

BEGIN(Client)

class CEnergyBall final : public CWeapon
{
private:
	explicit CEnergyBall(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEnergyBall(const CEnergyBall& rhs);
	virtual ~CEnergyBall() = default;

public:
	const _bool Get_HasAttack() const { return m_hasAttack; }

public:
	void Set_IsDead(_bool bCheck) { m_isDead = bCheck; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void OnCollisionStay(CGameObject* _pOther);

private:
	_double		m_TimeDelta = 0.0;
	_double		m_AccTimer = 0.0;
	_double		m_AttackCnt = 5.0;
	_double		m_TextureCnt = 10.0;
	_uint		m_TextureIndex = 0;
	_bool		m_hasAttack = true;
	_bool		m_isDead = false;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	_int Update_Attack(_double TimeDelta);
	_int Face_Camera();

public:
	static CEnergyBall* Create_EnergyBall_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
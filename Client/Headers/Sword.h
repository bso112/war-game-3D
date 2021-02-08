#pragma once
#include "Weapon.h"

BEGIN(Client)

class CSword final : public CWeapon
{
public:
	enum ATTACKID { ATTACK_DOWN, ATTACK_UP, ATTACK_END };

private:
	explicit CSword(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSword(const CSword& rhs);
	virtual ~CSword() = default;

public:
	const _bool	Get_IsBoss() const { return m_isBoss; }

public:
	void Set_IsAttack(_bool bCheck) { m_isAttack = bCheck; }
	void Set_IsBoss(_bool bCheck) { m_isBoss = bCheck; }
	void Set_HasBoss(_bool bCheck) { m_hasBoss = bCheck; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Attack(_double TimeDelta);

public:
	virtual void OnCollisionEnter(CGameObject* _pOther);

private:
	CCollider_Box*	m_pCollider = nullptr;
	ATTACKID		m_eAttackID = ATTACK_DOWN;
	_float3			m_vAxis;
	_double			m_AccTimer = 0.0;
	_bool			m_isAttack = false;

private:	// Boss
	_bool			m_isBoss = false;	// 보스가 사용 하다
	_bool			m_hasBoss = false;	// 보스가 장착 하다
	_double			m_BossTimer = 0.0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	_int Update_Attack(_double TimeDelta);

public:
	static CSword* Create_Sword_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
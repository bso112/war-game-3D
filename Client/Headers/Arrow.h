#pragma once
#include "Weapon.h"

BEGIN(Client)

class CArrow final : public CWeapon
{
private:
	explicit CArrow(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CArrow(const CArrow& rhs);
	virtual ~CArrow() = default;

public:
	void Set_ParentsMatrix(_matrix ParentsMatrix) { m_pTransform->Set_ParentsMatrix(ParentsMatrix); }
	void Set_Power(_double dPower);/* { m_dPower = dPower; }*/
	void Set_WorldMatrix(_matrix matMatrix) { m_pTransform->Set_WorldMatrix(matMatrix); }
	void Set_Scale(_float3 vScale) { m_pTransform->SetUp_Scale(vScale); }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void OnCollisionEnter(CGameObject* _pOther);

private:
	_bool	IsCollidedTerrain();
	void Face_Camera();
	void Stuck(CGameObject* pParent = nullptr);


private:
	_double		m_AccTimer = 0.0;
	_double		m_dPower = 1.0;
	_int		m_iSkin = 0;
	CGameObject*	m_pParent = nullptr;
	CTransform*	m_pParentTransform = nullptr;
	_bool		m_bNoCollide = 0;
	_bool		m_bIsStuck = 0;
	_matrix		m_matParentalLocal;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	//_int Update_Movement(_double TimeDelta);

public:
	static CArrow* Create_Arrow_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
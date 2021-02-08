#pragma once

// 1. 객체의 상태를 가진다. (게임 세계 == 월드 스페이스) 월드변환 행렬.

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigid_Body final : public CComponent
{
public:
	typedef struct tagStateDesc
	{
		_float	fMass;
		_float	fBounce;
		_float	fFriction_Static;
		_float	fFriction_Dynamic;
		_float	fMaxFallSpeed;
		_float	fGravity_Multiply;
		_float	fMaxSpeed = 0;
		_float3	m_vSlideNormal = _float3(0.f, 0.f, 0.f);
		_float3 m_vTempRight = _float3(0.f, 0.f, 0.f);
		_float3 m_vForce = _float3(0.f, 0.f, 0.f);
		_float3 m_vLocalPos = _float3(0.f, 0.f, 0.f);
		_float3 m_vInitForce = _float3(0.f, 0.f, 0.f);
		_float3 m_vSlideForce = _float3(0.f, 0.f, 0.f);
		_float3 m_vDownForce = _float3(0.f, 0.f, 0.f);
		_float3 m_vMoveDir = _float3(0.f, 0.f, 0.f);
		_float3 m_vInputForce = _float3(0.f, 0.f, 0.f);

	}STATEDESC;

private:
	explicit CRigid_Body(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CRigid_Body(const CRigid_Body& rhs);
	virtual ~CRigid_Body() = default;

public: // Getter
	const STATEDESC&	GetStateDesc() { return m_tStateDesc; }
	STATEDESC&			GetStateDesc_Direct() { return m_tStateDesc; }
	_float3				Get_Velocity() { return m_vVelocity; }
	_float3				Get_ExternForce() { return m_vExternForce; }

public: // Setter
	void	Set_Velocity(_float3 vVelocity) { m_vVelocity = vVelocity; }
	void	Reset_ExternForce() { m_vExternForce = _float3(0.f, 0.f, 0.f); }
	void	Set_SlideNormal(_float3 vSNor) { m_tStateDesc.m_vSlideNormal = vSNor; }
	void	Set_NormalRight(_float3 vSNor) { m_tStateDesc.m_vTempRight = vSNor; }

public: // Function
	void	Add_Force(_float3 vForce) { m_vExternForce += vForce; }
	void	Add_Velocity(_float3 vVelocity) { m_vVelocity += vVelocity; }

	virtual HRESULT Ready_Component_Prototype();
public:
	virtual HRESULT Ready_Component(void* pArg);

private:
	STATEDESC		m_tStateDesc;
	_float3			m_vVelocity = _float3(0.f, 0.f, 0.f);
	_float3			m_vExternForce = _float3(0.f,0.f,0.f);

public:
	static CRigid_Body* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
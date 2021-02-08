#pragma once

#include "Collider.h"

BEGIN(Engine)
class CShader;
class ENGINE_DLL CCollider_Ellipse final : public CCollider
{
public:
	typedef struct tagStateDesc : public CCollider::STATEDESC
	{
		_float	fRadius = 0.f;
		_float	fOffSetX = 0.f, fOffSetY = 0.f, fOffSetZ = 0.f;
	}STATEDESC_DERIVED;
private:
	explicit CCollider_Ellipse(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCollider_Ellipse(const CCollider_Ellipse& rhs);
	virtual ~CCollider_Ellipse() = default;
public:
	virtual HRESULT Ready_Component_Prototype(const _tchar* _pTexturePath);
	virtual HRESULT Ready_Component(void* pArg);
	virtual _int Update_Collider(_matrix StateMatrix);
	virtual HRESULT Render_Collider();
public:
	_bool	Check_Collision(CCollider* pTarget, _float& fCollisionLength, _float3& vPushDir);

public:
	virtual	vector<_float3>	Get_Axis() override;
	virtual CProjection Project(_float3 axis) override;

private:
	HRESULT SetUp_ConstantTable();
private:
	STATEDESC_DERIVED		m_StateDesc;

private:
	CShader*		m_pShaderCom = nullptr;
	LPD3DXMESH		m_pMesh = nullptr;
public:
	static CCollider_Ellipse* Create(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar* _pTexturePath);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
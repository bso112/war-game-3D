#pragma once
#include "Collider.h"
BEGIN(Engine)
class ENGINE_DLL CCollider_Box : public CCollider
{


public:
	typedef struct tagStateDesc : public CCollider::STATEDESC
	{
		_float	fWidth = 0.f, fHeight = 0.f;
		_float	fOffSetX = 0.f, fOffSetY = 0.f;
	}STATEDESC_DERIVED;
protected:
	explicit CCollider_Box(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCollider_Box(const CCollider_Box& rhs);
	virtual ~CCollider_Box() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
	virtual _int Update_Collider(_matrix StateMatrix);
	virtual HRESULT Render_Collider();

public:
	virtual	_bool	Check_Collision_AABB(CCollider* pTarget, _float& fCollisionLength);
	virtual	_bool	Check_Collision(CCollider* pTarget, _float& fCollisionLength, _float3& vPushDir);

public:
	vector<_float3>	Get_Axis() override;
	CProjection Project(_float3 axis) override;

private:
	STATEDESC_DERIVED	m_tDesc;
	_float3				m_vOriginalPoint[8];
	_float3				m_vCurrPoint[8];

private:
	LPD3DXLINE	m_pLine = nullptr;

public:
	static CCollider_Box* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};
END

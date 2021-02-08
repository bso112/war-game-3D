#pragma once

#include "Collider.h"

BEGIN(Engine)

class CPipeLine;
class ENGINE_DLL CCollider_Rect final : public CCollider
{
public:
	typedef struct tagStateDesc : public CCollider::STATEDESC
	{
		_float	fWidth = 0.f, fHeight = 0.f;
		_float	fOffSetX = 0.f, fOffSetY = 0.f;
	}STATEDESC_DERIVED;
private:
	explicit CCollider_Rect(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCollider_Rect(const CCollider_Rect& rhs);
	virtual ~CCollider_Rect() = default;


public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
	virtual _int Update_Collider(_matrix StateMatrix);
	virtual HRESULT Render_Collider();
public:
	_bool	Check_Collision(CCollider* pTarget, _float& fCollisionLength, _float3& vPushDir) override;
	RECT	Get_Rect();
private:
	LPD3DXLINE	m_pLine = nullptr;
private:
	STATEDESC_DERIVED	m_StateDesc;
	_float3				m_vOriginalPoint[4];
	_float3				m_vPoint[4];


public:
	static CCollider_Rect* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
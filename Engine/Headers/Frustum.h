#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase 
{
	DECLARE_SINGLETON(CFrustum)

private:
	_float3				m_vOriginPoint[8];
	_float3				m_vPoint[8];

private:
	D3DXPLANE		m_Plane[6];

private:
	explicit CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Ready_Frustum();
	_int Update_Frustum();
	_bool Is_InFrustum(_float3 _vWorldPos, _float _fRadius);

public:
	virtual void Free() override;
};

END
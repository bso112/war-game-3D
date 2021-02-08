#include "..\Headers\Frustum.h"
#include "Management.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Ready_Frustum()
{
	m_vOriginPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginPoint[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginPoint[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginPoint[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginPoint[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginPoint[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginPoint[7] = _float3(-1.f, -1.f, 1.f);
	return S_OK;
}

_int CFrustum::Update_Frustum()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return false;

	Safe_AddRef(pManagement);

	_matrix ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_matrix ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vOriginPoint[i], &ProjMatrix);
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &ViewMatrix);
	}

	// x
	D3DXPlaneFromPoints(&m_Plane[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
	D3DXPlaneFromPoints(&m_Plane[1], &m_vPoint[4], &m_vPoint[0], &m_vPoint[3]);

	// y
	D3DXPlaneFromPoints(&m_Plane[2], &m_vPoint[4], &m_vPoint[5], &m_vPoint[1]);
	D3DXPlaneFromPoints(&m_Plane[3], &m_vPoint[3], &m_vPoint[2], &m_vPoint[6]);

	// z
	D3DXPlaneFromPoints(&m_Plane[4], &m_vPoint[5], &m_vPoint[4], &m_vPoint[7]);
	D3DXPlaneFromPoints(&m_Plane[5], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);

	Safe_Release(pManagement);
	return NOERROR;
}

_bool CFrustum::Is_InFrustum(_float3 _vWorldPos, _float _fRadius)
{
	for (int i = 0; i < 6; ++i)
	{
		if (_fRadius < D3DXPlaneDotCoord(&m_Plane[i], &_vWorldPos))
		{
			return false;
		}
	}
	return true;
}

void CFrustum::Free()
{
}

#include "..\Headers\Collider.h"
#include "VIBuffer.h"
#include "Shader.h"


USING(Engine)

CCollider::CCollider(PDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pPipeLine(CPipeLine::Get_Instance())
{
	Safe_AddRef(m_pPipeLine);
}


CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCollider::Ready_Component_Prototype()
{


	return S_OK;
}

HRESULT CCollider::Ready_Component(void * pArg)
{


	return S_OK;
}

_int CCollider::Update_Collider(_matrix StateMatrix)
{


	return _int();
}

HRESULT CCollider::Render_Collider()
{
	return S_OK;
}

vector<_float3> CCollider::Get_Axis()
{
	return vector<_float3>();
}

CCollider::CProjection CCollider::Project(_float3 axis)
{
	return CProjection(0.f,0.f);
}

_float3 CCollider::Get_Size()
{
	return _float3(D3DXVec3Length(((_float3*)&m_StateMatrix.m[0][0])),
		D3DXVec3Length(((_float3*)&m_StateMatrix.m[1][0])),
		D3DXVec3Length(((_float3*)&m_StateMatrix.m[2][0])));
}

_float3 CCollider::Get_Pos()
{
	return _float3(m_StateMatrix.m[3][0], m_StateMatrix.m[3][1], m_StateMatrix.m[3][2]);
}

void CCollider::Free()
{
	Safe_Release(m_pPipeLine);

	CComponent::Free();
}

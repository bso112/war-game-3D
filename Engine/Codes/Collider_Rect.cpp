#include "..\Headers\Collider_Rect.h"
#include "PipeLine.h"

USING(Engine)

CCollider_Rect::CCollider_Rect(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCollider(pGraphic_Device)
{

}


CCollider_Rect::CCollider_Rect(const CCollider_Rect & rhs)
	: CCollider(rhs)
	, m_pLine(rhs.m_pLine)	
{
	
	memcpy(m_vOriginalPoint, rhs.m_vOriginalPoint, sizeof(_float3) * 4);
	memcpy(m_vPoint, rhs.m_vOriginalPoint, sizeof(_float3) * 4);

	Safe_AddRef(m_pLine);
}

HRESULT CCollider_Rect::Ready_Component_Prototype()
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);

	if (FAILED(D3DXCreateLine(m_pGraphic_Device, &m_pLine)))
		return E_FAIL;

	m_vOriginalPoint[0] = _float3(0.5f, -0.5f, 0.f);
	m_vOriginalPoint[1] = _float3(0.5f, 0.5f, 0.f);
	m_vOriginalPoint[2] = _float3(-0.5f, 0.5f, 0.f);
	m_vOriginalPoint[3] = _float3(-0.5f, -0.5f, 0.f);

	memcpy(m_vPoint, m_vOriginalPoint, sizeof(_float3) * 4);

	return S_OK;
}

HRESULT CCollider_Rect::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC_DERIVED));

	

	return S_OK;
}

_int CCollider_Rect::Update_Collider(_matrix StateMatrix)
{
	if (!m_bEnable)
		return false;

	D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[0][0], (_float3*)&StateMatrix.m[0][0]);
	D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[1][0], (_float3*)&StateMatrix.m[1][0]);	
	D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[2][0], (_float3*)&StateMatrix.m[2][0]);

	StateMatrix.m[3][0] = StateMatrix.m[3][0] + m_StateDesc.fOffSetX;
	StateMatrix.m[3][1] = StateMatrix.m[3][1] + m_StateDesc.fOffSetY;

	memcpy(&m_StateDesc.StateMatrix.m[3][0], &StateMatrix.m[3][0], sizeof(_float3));	

	memcpy(&m_StateDesc.StateMatrix.m[0][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[0][0] * m_StateDesc.fWidth), sizeof(_float3));
	memcpy(&m_StateDesc.StateMatrix.m[1][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[1][0] * m_StateDesc.fHeight), sizeof(_float3));




	for (size_t i = 0; i < 4; ++i)
	{
		// 1x4 * 4x4
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vOriginalPoint[i], &m_StateDesc.StateMatrix);
	}		

	return _int();
}

HRESULT CCollider_Rect::Render_Collider()
{
	if (!m_bEnable)
		return false;

	if (nullptr == m_pLine || 
		nullptr == m_pPipeLine)
		return E_FAIL;

	_float2		vPoint[5] = {
		{ m_vPoint[0].x, m_vPoint[0].y },
		{ m_vPoint[1].x, m_vPoint[1].y },
		{ m_vPoint[2].x, m_vPoint[2].y },
		{ m_vPoint[3].x, m_vPoint[3].y },
		{ m_vPoint[0].x, m_vPoint[0].y },
	};

	for (size_t i = 0; i < 5; ++i)
	{
		D3DXVec2TransformCoord(&vPoint[i], &vPoint[i], &m_pPipeLine->Get_Transform(D3DTS_VIEW));
	}
	


	m_pLine->SetWidth(2.f);
	m_pLine->Begin();
	m_pLine->Draw(vPoint, 5, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
	m_pLine->End();

	return S_OK;
}

_bool CCollider_Rect::Check_Collision(CCollider* pTarget, _float& fCollisionLength, _float3& vPushDir)
{
	if (!m_bEnable)
		return false;

	CCollider_Rect* pOther = dynamic_cast<CCollider_Rect*>(pTarget);
	if (nullptr == pOther)
	{
		MSG_BOX("허용되는 콜라이더가 아닙니다.");
		return false;
	}
	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	vSourMin = m_vPoint[3];
	vSourMax = m_vPoint[1];

	vDestMin = pOther->m_vPoint[3];
	vDestMax = pOther->m_vPoint[1];

	// x축선상에서 안겹쳤니?
	//더 작은 놈의 Max < 더 큰 놈의 Min 이면 false
	if (min(vSourMax.x, vDestMax.x) < max(vSourMin.x, vDestMin.x))	
		return false;

	// y축선상에서 안겹쳤니?
	//더 작은놈의 Max < 더 큰놈의 Min 이면 false
	if (min(vSourMax.y, vDestMax.y) < max(vSourMin.y, vDestMin.y))
		return false;

	return true;	
}

RECT CCollider_Rect::Get_Rect()
{
	RECT rc = { (LONG)m_vPoint[3].x, (LONG)m_vPoint[3].y, (LONG)m_vPoint[1].x, (LONG)m_vPoint[1].y };
	
	return rc;
}

CCollider_Rect * CCollider_Rect::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MessageBox(0, L"Failed To Creating CCollider_Rect", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Rect::Clone_Component(void * pArg)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"Failed To Creating CCollider_Rect", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Rect::Free()
{

	Safe_Release(m_pLine);

	CCollider::Free();
}

#include "..\Headers\Collider_Box.h"
#include "Shader.h"
CCollider_Box::CCollider_Box(PDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

CCollider_Box::CCollider_Box(const CCollider_Box & rhs)
	: CCollider(rhs)
	, m_pLine(rhs.m_pLine)
{
	memcpy(&m_vOriginalPoint, &rhs.m_vOriginalPoint, sizeof(_float3) * 8);
	memcpy(&m_vCurrPoint, &rhs.m_vCurrPoint, sizeof(_float3) * 8);

	Safe_AddRef(m_pLine);
}

HRESULT CCollider_Box::Ready_Component_Prototype()
{
	m_vOriginalPoint[0] = _float3(-0.5f, 0.5f, -0.5f);
	m_vOriginalPoint[1] = _float3(0.5f, 0.5f, -0.5f);
	m_vOriginalPoint[2] = _float3(0.5f, -0.5f, -0.5f);
	m_vOriginalPoint[3] = _float3(-0.5f, -0.5f, -0.5f);
	m_vOriginalPoint[4] = _float3(-0.5f, 0.5f, 0.5f);
	m_vOriginalPoint[5] = _float3(0.5f, 0.5f, 0.5f);
	m_vOriginalPoint[6] = _float3(0.5f, -0.5f, 0.5f);
	m_vOriginalPoint[7] = _float3(-0.5f, -0.5f, 0.5f);

	memcpy(&m_vCurrPoint, &m_vOriginalPoint, sizeof(_float3) * 8);

	if (FAILED(D3DXCreateLine(m_pGraphic_Device, &m_pLine)))
		return E_FAIL;

	D3DXMatrixIdentity(&m_StateMatrix);



	return S_OK;
}

HRESULT CCollider_Box::Ready_Component(void * pArg)
{
	D3DXMatrixIdentity(&m_StateMatrix);
	D3DXMatrixIdentity(&m_tDesc.StateMatrix);


	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(STATEDESC_DERIVED));

	m_eType = TYPE_BOX;

	return S_OK;
}

_int CCollider_Box::Update_Collider(_matrix StateMatrix)
{
	if (!m_bEnable)
		return false;

	m_tDesc.StateMatrix = StateMatrix;
	m_StateMatrix = StateMatrix;

	for (size_t i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vCurrPoint[i], &m_vOriginalPoint[i], &StateMatrix);
	}
	return 0;
}

HRESULT CCollider_Box::Render_Collider()
{
	if (nullptr == m_pLine ||
		nullptr == m_pPipeLine)
		return E_FAIL;

	CPipeLine* pPipeline = CPipeLine::Get_Instance();
	if (nullptr == pPipeline)return E_FAIL;

	_float3		vPoint[17] = {
		//앞면
		{ m_vOriginalPoint[0].x, m_vOriginalPoint[0].y, m_vOriginalPoint[0].z },
		{ m_vOriginalPoint[1].x, m_vOriginalPoint[1].y, m_vOriginalPoint[1].z },
		{ m_vOriginalPoint[2].x, m_vOriginalPoint[2].y, m_vOriginalPoint[2].z },
		{ m_vOriginalPoint[3].x, m_vOriginalPoint[3].y, m_vOriginalPoint[3].z },
		{ m_vOriginalPoint[0].x, m_vOriginalPoint[0].y, m_vOriginalPoint[0].z },
		//뒷면
		{ m_vOriginalPoint[4].x, m_vOriginalPoint[4].x, m_vOriginalPoint[4].z },
		{ m_vOriginalPoint[5].x, m_vOriginalPoint[5].x, m_vOriginalPoint[5].z },
		{ m_vOriginalPoint[6].x, m_vOriginalPoint[6].x, m_vOriginalPoint[6].z },
		{ m_vOriginalPoint[7].x, m_vOriginalPoint[7].x, m_vOriginalPoint[7].z },
		{ m_vOriginalPoint[4].x, m_vOriginalPoint[4].x, m_vOriginalPoint[4].z },
		//중간
		{ m_vOriginalPoint[5].x, m_vOriginalPoint[5].x, m_vOriginalPoint[5].z },
		{ m_vOriginalPoint[1].x, m_vOriginalPoint[1].x, m_vOriginalPoint[1].z },
		{ m_vOriginalPoint[6].x, m_vOriginalPoint[6].x, m_vOriginalPoint[6].z },
		{ m_vOriginalPoint[2].x, m_vOriginalPoint[2].x, m_vOriginalPoint[2].z },
		{ m_vOriginalPoint[6].x, m_vOriginalPoint[6].x, m_vOriginalPoint[6].z },
		{ m_vOriginalPoint[3].x, m_vOriginalPoint[3].x, m_vOriginalPoint[3].z },
		{ m_vOriginalPoint[7].x, m_vOriginalPoint[7].x, m_vOriginalPoint[7].z },
	};


	m_pLine->SetWidth(2.f);
	m_pLine->Begin();
	m_pLine->DrawTransform(vPoint, 17, &(m_tDesc.StateMatrix * pPipeline->Get_Transform(D3DTS_VIEW) * pPipeline->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
	m_pLine->End();
	return S_OK;
}

_bool CCollider_Box::Check_Collision_AABB(CCollider* pTarget, _float& fCollisionLength)
{
	if (!m_bEnable)
		return false;

	CCollider_Box* pOther = dynamic_cast<CCollider_Box*>(pTarget);
	if (nullptr == pOther)
	{
		MSG_BOX("허용되는 콜라이더가 아닙니다.");
		return false;
	}

	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	vSourMin = m_vCurrPoint[3];
	vSourMax = m_vCurrPoint[5];

	vDestMin = pOther->m_vCurrPoint[3];
	vDestMax = pOther->m_vCurrPoint[5];

	// x축선상에서 안겹쳤니?
	//더 작은 놈의 Max < 더 큰 놈의 Min 이면 false
	if (min(vSourMax.x, vDestMax.x) < max(vSourMin.x, vDestMin.x))
		return false;

	// y축선상에서 안겹쳤니?
	//더 작은놈의 Max < 더 큰놈의 Min 이면 false
	if (min(vSourMax.y, vDestMax.y) < max(vSourMin.y, vDestMin.y))
		return false;

	// z축선상에서 안겹쳤니?
	if (min(vSourMax.z, vDestMax.z) < max(vSourMin.z, vDestMin.z))
		return false;


	//겹친 길이 구하기
	_float3 Collision;
	Collision.x = min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x);
	Collision.y = min(vSourMax.y, vDestMax.y) - max(vSourMin.y, vDestMin.y);
	Collision.z = min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z);

	fCollisionLength = D3DXVec3Length(&Collision);

	return true;
}

_bool CCollider_Box::Check_Collision(CCollider * pTarget, _float & fCollisionLength, _float3& vPushDir)
{
	/*
	CCollider_Box*	pOther_Box = nullptr;
		pOther_Box = dynamic_cast<CCollider_Box*>(pTarget);
		if (nullptr != pOther_Box)
		{
			_float	fMinOverlap = FLT_MAX;
			vector<_float3> axes1 = Get_Axis();
			vector<_float3> axes2 = pTarget->Get_Axis();

			for (size_t i = 0; i < axes1.size(); ++i)
			{
				_float3 axis = axes1[i];
				CCollider::CProjection projection1 = Project(axis);
				CCollider::CProjection projection2 = pTarget->Project(axis);
				if (!projection1.Overlap(projection2))
					return false;
				else
				{
					_float fOverlap = projection1.Get_Overlap(projection2);
					if (fOverlap < fMinOverlap)
					{
						fMinOverlap = fOverlap;
						vPushAxis = axis;
					}
				}
			}

			for (size_t i = 0; i < axes2.size(); ++i)
			{
				_float3 axis = axes2[i];
				CCollider::CProjection projection1 = Project(axis);
				CCollider::CProjection projection2 = pTarget->Project(axis);
				if (!projection1.Overlap(projection2))
					return false;
				else
				{
					_float fOverlap = projection1.Get_Overlap(projection2);
					if (fOverlap < fMinOverlap)
					{
						fMinOverlap = fOverlap;
						vPushAxis = axis;

					}
				}

			}

			fCollisionLength = fMinOverlap;
			D3DXVec3Normalize(&vPushDir, &vPushAxis);

			return true;

		}
	*/
	if (!m_bEnable)
		return false;

	TYPE eColliderType = pTarget->Get_Type();
	if (eColliderType != TYPE_BOX && eColliderType != TYPE_SPHERE)
		return false;

	_float	fMinOverlap = FLT_MAX;
	vector<_float3> axes1 = Get_Axis();
	vector<_float3> axes2 = pTarget->Get_Axis();
	_float3	vPushAxis;


	for (size_t i = 0; i < axes1.size(); ++i)
	{
		_float3 axis = axes1[i];
		CCollider::CProjection projection1 = Project(axis);
		CCollider::CProjection projection2 = pTarget->Project(axis);
		if (!projection1.Overlap(projection2))
			return false;
		else
		{
			_float fOverlap = projection1.Get_Overlap(projection2);
			if (fOverlap < fMinOverlap)
			{
				fMinOverlap = fOverlap;
				vPushAxis = axis;
			}
		}
	}

	for (size_t i = 0; i < axes2.size(); ++i)
	{
		_float3 axis = axes2[i];
		CCollider::CProjection projection1 = Project(axis);
		CCollider::CProjection projection2 = pTarget->Project(axis);
		if (!projection1.Overlap(projection2))
			return false;
		else
		{
			_float fOverlap = projection1.Get_Overlap(projection2);
			if (fOverlap < fMinOverlap)
			{
				fMinOverlap = fOverlap;
				vPushAxis = axis;
			}
		}

	}

	fCollisionLength = fMinOverlap;
	D3DXVec3Normalize(&vPushDir, &vPushAxis);
	return true;
}

vector<_float3> CCollider_Box::Get_Axis()
{
	vector<_float3> axis;

	_float3 vRight, vUp, vLook;
	vRight = _float3(m_StateMatrix.m[0][0], m_StateMatrix.m[0][1], m_StateMatrix.m[0][2]);
	vUp = _float3(m_StateMatrix.m[1][0], m_StateMatrix.m[1][1], m_StateMatrix.m[1][2]);
	vLook = _float3(m_StateMatrix.m[2][0], m_StateMatrix.m[2][1], m_StateMatrix.m[2][2]);

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	axis.push_back(vRight);
	axis.push_back(vUp);
	axis.push_back(vLook);
	return axis;
}

CCollider::CProjection CCollider_Box::Project(_float3 axis)
{
	//각 정점과 축의 내적
	_float min = D3DXVec3Dot(&axis, &m_vCurrPoint[0]);
	_float max = min;
	for (int i = 1; i < 8; ++i)
	{
		_float result = D3DXVec3Dot(&axis, &m_vCurrPoint[i]);

		if (result < min)
			min = result;
		else if (result > max)
			max = result;

	}
	return CProjection(min, max);
}


CCollider_Box * CCollider_Box::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Box*	pInstance = new CCollider_Box(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MessageBox(0, L"Failed To Creating CCollider_Box", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider_Box::Clone_Component(void * pArg)
{
	CCollider_Box*	pInstance = new CCollider_Box(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"Failed To Creating CCollider_Box", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCollider_Box::Free()
{
	Safe_Release(m_pLine);
	CCollider::Free();
}

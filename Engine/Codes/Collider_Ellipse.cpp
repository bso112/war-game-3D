#include "..\Headers\Collider_Ellipse.h"
#include "Collider_Box.h"
#include "Shader.h"
USING(Engine)

CCollider_Ellipse::CCollider_Ellipse(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCollider(pGraphic_Device)
{
}

CCollider_Ellipse::CCollider_Ellipse(const CCollider_Ellipse & rhs)
	: CCollider(rhs),
	m_pMesh(rhs.m_pMesh),
	m_pShaderCom(rhs.m_pShaderCom)

{
	Safe_AddRef(m_pMesh);
	Safe_AddRef(m_pShaderCom);
}

HRESULT CCollider_Ellipse::Ready_Component_Prototype(const _tchar* _pTexturePath)
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);
	D3DXMatrixIdentity(&m_StateMatrix);

	m_StateDesc.fRadius = 0.5f;
	m_StateDesc.fOffSetX = 0.f;
	m_StateDesc.fOffSetY = 0.f;

	//트랜스폼으로 크기를 지정하므로 지름이 1 되도록 만들자.
	D3DXCreateSphere(m_pGraphic_Device, 0.5f, 10, 5, &m_pMesh, NULL);

	m_pShaderCom = CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Collider.fx");

	return S_OK;
}

HRESULT CCollider_Ellipse::Ready_Component(void * pArg)
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);
	D3DXMatrixIdentity(&m_StateMatrix);


	// 복제할때 원의 사이즈와 오프셋을 받아온다. 위치도 받아오긴하지만, 업데이트에서 갱신될 정보.
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC_DERIVED));
	
	m_eType = TYPE_SPHERE;

	return S_OK;
}

_int CCollider_Ellipse::Update_Collider(_matrix StateMatrix)
{
	if (!m_bEnable)
		return false;


	D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[0][0], (_float3*)&StateMatrix.m[0][0]);
	D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[1][0], (_float3*)&StateMatrix.m[1][0]);
	D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[2][0], (_float3*)&StateMatrix.m[2][0]);

	memcpy(&m_StateDesc.StateMatrix.m[0][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[0][0] * m_StateDesc.fRadius * 2.f), sizeof(_float3));
	memcpy(&m_StateDesc.StateMatrix.m[1][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[1][0] * m_StateDesc.fRadius * 2.f), sizeof(_float3));
	memcpy(&m_StateDesc.StateMatrix.m[2][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[2][0] * m_StateDesc.fRadius * 2.f), sizeof(_float3));

	_float3 vRight, vUp, vLook;
	D3DXVec3Normalize(&vRight, (_float3*)&StateMatrix.m[0][0]);
	D3DXVec3Normalize(&vUp, (_float3*)&StateMatrix.m[1][0]);
	D3DXVec3Normalize(&vLook, (_float3*)&StateMatrix.m[2][0]);
	
	_float3 fOffset;
	fOffset = vRight * m_StateDesc.fOffSetX + vUp * m_StateDesc.fOffSetY + vLook * m_StateDesc.fOffSetZ;
	_float3 vNewPos = *(_float3*)&StateMatrix.m[3][0] + fOffset;

	memcpy(&m_StateDesc.StateMatrix.m[3][0], &vNewPos, sizeof(_float3));
	memcpy(&m_StateMatrix, &m_StateDesc.StateMatrix, sizeof(_matrix));


	return _int();
}

HRESULT CCollider_Ellipse::Render_Collider()
{
	if (!m_bEnable)
		return false;

	if (nullptr == m_pMesh	||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	m_pMesh->DrawSubset(0);

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();



	return S_OK;
}

_bool CCollider_Ellipse::Check_Collision(CCollider* pTarget, _float& fCollisionLength, _float3& vPushDir)
{
	if (!m_bEnable)
		return false;

	CCollider_Ellipse* pOther_Ellipse = nullptr;
	pOther_Ellipse = dynamic_cast<CCollider_Ellipse*>(pTarget);

	TYPE eColliderType = pTarget->Get_Type();

	_float3	vPushAxis;
	
	//구 충돌
	if (TYPE_SPHERE == eColliderType)
	{
		_float3		vDirection = *(_float3*)&pOther_Ellipse->m_StateDesc.StateMatrix.m[3][0] - *(_float3*)&m_StateDesc.StateMatrix.m[3][0];

		_float		fDistance = D3DXVec3Length(&vDirection);


		if (m_StateDesc.fRadius + pOther_Ellipse->m_StateDesc.fRadius < fDistance)
		{
			m_isColl = false;
			pOther_Ellipse->m_isColl = false;
			return false;
		}

		m_isColl = true;
		pOther_Ellipse->m_isColl = true;


		fCollisionLength = m_StateDesc.fRadius + pOther_Ellipse->m_StateDesc.fRadius - fDistance;

		return true;
	}
	//박스충돌
	else if(TYPE_BOX == eColliderType)
	{
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
	}

	return false;
}



vector<_float3> CCollider_Ellipse::Get_Axis()
{
	vector<_float3> axis;

	_float3 vRight, vUp, vLook;
	vRight = _float3(m_StateDesc.StateMatrix.m[0][0], m_StateDesc.StateMatrix.m[0][1], m_StateDesc.StateMatrix.m[0][2]);
	vUp = _float3(m_StateDesc.StateMatrix.m[1][0], m_StateDesc.StateMatrix.m[1][1], m_StateDesc.StateMatrix.m[1][2]);
	vLook = _float3(m_StateDesc.StateMatrix.m[2][0], m_StateDesc.StateMatrix.m[2][1], m_StateDesc.StateMatrix.m[2][2]);

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	axis.push_back(vRight);
	axis.push_back(vUp);
	axis.push_back(vLook);
	return axis;

}

CCollider::CProjection CCollider_Ellipse::Project(_float3 axis)
{
	CUBE cube = Make_Cube(Get_Pos(), _float3(m_StateDesc.fRadius * 2.f, m_StateDesc.fRadius * 2.f, m_StateDesc.fRadius * 2.f));

	//각 정점과 축의 내적
	_float min = D3DXVec3Dot(&axis, &cube.vVertices[0]);
	_float max = min;
	for (int i = 1; i < 8; ++i)
	{
		_float result = D3DXVec3Dot(&axis, &cube.vVertices[i]);

		if (result < min)
			min = result;
		else if (result > max)
			max = result;

	}
	return CProjection(min, max);
}

HRESULT CCollider_Ellipse::SetUp_ConstantTable()
{
	CPipeLine*	pPipeline = CPipeLine::Get_Instance();
	if (nullptr == pPipeline)
		return E_FAIL;

	Safe_AddRef(pPipeline);

	m_pShaderCom->Set_Value("g_matWorld", &m_StateDesc.StateMatrix, sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pPipeline->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pPipeline->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_vColor", &_float4(0.f,1.f,0.f,1.f), sizeof(_float4));

	
	Safe_Release(pPipeline);

	return S_OK;
}

CCollider_Ellipse * CCollider_Ellipse::Create(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar* _pTexturePat)
{
	CCollider_Ellipse*	pInstance = new CCollider_Ellipse(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(_pTexturePat)))
	{
		MessageBox(0, L"Failed To Creating CCollider_Ellipse", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Ellipse::Clone_Component(void * pArg)
{
	CCollider_Ellipse*	pInstance = new CCollider_Ellipse(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"Failed To Creating CCollider_Ellipse", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Ellipse::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pShaderCom);
	CCollider::Free();
}

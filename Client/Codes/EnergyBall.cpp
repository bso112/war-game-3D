#include "stdafx.h"
#include "..\Headers\EnergyBall.h"
#include "Unit.h"

CEnergyBall::CEnergyBall(PDIRECT3DDEVICE9 pGraphic_Device)
	: CWeapon(pGraphic_Device)
{
}

CEnergyBall::CEnergyBall(const CEnergyBall & rhs)
	: CWeapon(rhs)
{
}

HRESULT CEnergyBall::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CEnergyBall::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->SetUp_Scale(m_StateDesc.vScale);
	m_pTransform->SetUp_Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	m_pTransform->SetUp_Position(m_StateDesc.vPos);

	return S_OK;
}

_int CEnergyBall::Update_GameObject(_double TimeDelta)
{
	if (true == m_isDead)
	{
		m_bDead = true;
		return -1;
	}

	if (0x80000000 & Update_Attack(TimeDelta))
		return -1;

	if (0x80000000 & Face_Camera())
		return -1;

	return _int(0);
}

_int CEnergyBall::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer || nullptr == m_pCollider)
		return -1;

	if (false == m_hasAttack)
		return 0;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return -1;

	if (pManagement->Is_InFrustum(m_pTransform->Get_Position()))
	{
		if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
			return -1;

		if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_ARROW, this)))
			return -1;
	}

	m_pCollider->Update_Collider(m_pTransform->Get_WorldMatrix());

	return _int(0);
}

HRESULT CEnergyBall::Render_GameObject()
{
	if (nullptr == m_pVIBuffer ||
		nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();
	m_pShader->Begin_Pass(0);

	m_pVIBuffer->Render_VIBuffer();

	m_pShader->End_Pass();
	m_pShader->End_Shader();

#ifdef _DEBUG
	if (FAILED(m_pCollider->Render_Collider()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CEnergyBall::OnCollisionStay(CGameObject * _pOther)
{
	if (nullptr == _pOther)
		return;

	//무기 주인이 아니라면
	if (m_tOwner.iOwnerID != _pOther->Get_InstanceID())
	{
		CUnit* pUnit = dynamic_cast<CUnit*>(_pOther);
		if (nullptr != pUnit)
		{
			//적군이면
			if (m_tOwner.bFriendly != pUnit->Is_Friendly())
			{
				pUnit->TakeDamage(10.f * (_float)m_TimeDelta);
			}
		}
	}
}

HRESULT CEnergyBall::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	// For.Com_Transform
	CTransform::STATEDESC StateDesc;
	StateDesc.SpeedPerSec = 16.0;
	StateDesc.RotatePerSec = 0.0;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform, &StateDesc)))
		return E_FAIL;

	// For.Component_Texture_EnergyBall
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_EnergyBall", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Collider
	CCollider_Ellipse::STATEDESC_DERIVED tColliderDesc;
	tColliderDesc.fRadius = m_StateDesc.vScale.x * 0.25f;
	tColliderDesc.fOffSetY = m_StateDesc.vScale.y * -0.1f;
	D3DXMatrixIdentity(&tColliderDesc.StateMatrix);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Ellipse", L"Com_Collider", (CComponent**)&m_pCollider, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnergyBall::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix));
	m_pShader->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShader->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	m_pTexture->Set_TextureOnShader(m_pShader, "g_DiffuseTexture", m_TextureIndex);

	Safe_Release(pManagement);

	return S_OK;
}

_int CEnergyBall::Update_Attack(_double TimeDelta)
{
	m_AccTimer += TimeDelta * m_TextureCnt;
	m_TimeDelta = TimeDelta;

	if (m_TextureCnt * m_AttackCnt <= m_AccTimer)
		m_hasAttack = false;
	else
		m_TextureIndex = (_uint)m_AccTimer % 5;

	return _int(0);
}

_int CEnergyBall::Face_Camera()
{
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	_float3 vNewRight, vNewUp, vNewLook;

	vNewRight = (_float3)WorldMatrix.m[0];
	vNewUp = (_float3)WorldMatrix.m[1];
	vNewLook = (_float3)WorldMatrix.m[3] - CManagement::Get_Instance()->Get_CamPosition();

	D3DXVec3Cross(&vNewRight, &vNewUp, &vNewLook);

	D3DXVec3Normalize(&vNewRight, &vNewRight);
	D3DXVec3Normalize(&vNewLook, &vNewLook);

	memcpy(WorldMatrix.m[0], &(vNewRight * m_StateDesc.vScale.x), sizeof(_float3));
	memcpy(WorldMatrix.m[2], &vNewLook, sizeof(_float3));

	// 부모 행렬, 공전 행렬 제거 후 월드 행렬 대입
	WorldMatrix *= m_pTransform->Get_ParentsMatrixInverse() * m_pTransform->Get_RevolutionMatrixInverse();
	m_pTransform->Set_WorldMatrix(WorldMatrix);

	return _int(0);
}

CEnergyBall * CEnergyBall::Create_EnergyBall_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CEnergyBall* pInstance = new CEnergyBall(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CEnergyBall Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnergyBall::Clone_GameObject(void * pArg)
{
	CEnergyBall* pInstance = new CEnergyBall(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CEnergyBall Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnergyBall::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pCollider);

	CWeapon::Free();
}

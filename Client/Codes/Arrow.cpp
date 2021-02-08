#include "stdafx.h"
#include "..\Headers\Arrow.h"
#include "Unit.h"
#include "PickingMgr.h"
CArrow::CArrow(PDIRECT3DDEVICE9 pGraphic_Device)
	: CWeapon(pGraphic_Device)
{
}

CArrow::CArrow(const CArrow & rhs)
	: CWeapon(rhs)
{
	m_iSkin = rand() % 4;
}

void CArrow::Set_Power(_double dPower)
{
	_float3 vDir = m_pTransform->Get_State(CTransform::STATE_UP);
	D3DXVec3Normalize(&vDir, &vDir);
	m_pRigidBody->Add_Velocity(vDir * dPower);
}

HRESULT CArrow::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CArrow::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	//m_pTransform->SetUp_Scale(_float3());
	//m_pTransform->SetUp_Position(m_StateDesc.vPos);

	return S_OK;
}

_int CArrow::Update_GameObject(_double TimeDelta)
{
	if (m_bDead == 1)
		return _int(0);

	//if (0x80000000 & Update_Movement(TimeDelta))
	//	return -1;

	m_AccTimer += TimeDelta;
	if (10.0 <= m_AccTimer)
	{
		m_bDead = true;
		return -1;
	}

	//if (m_bIsStuck)
	//{
	//	//유닛인지 판별(널포인터), 부모행렬 변환시(초기 로컬 포함) 크기값 없애주기
	//	if (m_pParent != nullptr)
	//	{
	//		_matrix matParent = m_pParentTransform->Get_WorldMatrix();
	//		m_pTransform->Set_WorldMatrix(m_matParentalLocal * matParent);
	//	}

	//}

	Face_Camera();

	//m_pTransform->ApplyPhysics(m_pRigidBody, TimeDelta);
	return _int(0);
}

_int CArrow::Late_Update_GameObject(_double TimeDelta)
{
	if (m_bDead == 1)
		return _int(0);

	if (m_pParent != nullptr && m_pParent->Is_Dead())
		m_bDead = 1;
	if (nullptr == m_pRenderer || nullptr == m_pTransform || nullptr == m_pCollider)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();

	Safe_AddRef(pManagement);
	if (nullptr == pManagement) return -1;

	if (pManagement->Is_InFrustum(m_pTransform->Get_Position()))
	{
		if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;

		if (m_bNoCollide == 0)
		{
			if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_ARROW, this)))
				return -1;
		}
	}

	if (m_bIsStuck == 0)
	{
		if (FAILED(CPhysicsMgr::Get_Instance()->Add_PhysGroup(CPhysicsMgr::PHYS_NORMAL, m_pTransform, m_pCollider, m_pRigidBody)))
			return -1;
	}

	Safe_Release(pManagement);

	if (m_bNoCollide == 0)
	{
		m_pCollider->Update_Collider(m_pTransform->Get_WorldMatrix());
		IsCollidedTerrain();
	}

	return _int(0);
}

HRESULT CArrow::Render_GameObject()
{
	if (nullptr == m_pVIBuffer ||
		nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();
	m_pShader->Begin_Pass(6);

	m_pVIBuffer->Render_VIBuffer();

	m_pShader->End_Pass();
	m_pShader->End_Shader();

	m_pCollider->Render_Collider();



	return S_OK;
}

void CArrow::OnCollisionEnter(CGameObject * _pOther)
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
				pUnit->TakeDamage(10);
				m_pTransform->Set_Parent((CTransform*)_pOther->Find_Component(L"Com_Transform"));
				m_pTransform->SetUp_Position(_float3(0.f, 0.f, 0.f));
				m_pTransform->SetUp_Scale(_float3(30.f, 30.f, 30.f));

				m_bDead = true;
				//Stuck(/*_pOther*/);
			}
		}
	}
}


_bool CArrow::IsCollidedTerrain()
{
	if (nullptr == m_pTransform)
		return false;
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float3 vTerrainPos;
	if (pPickingMgr->Get_TerrainPos(m_pTransform->Get_Position(), vTerrainPos))
	{
		if (vTerrainPos.y > m_pTransform->Get_Position().y)
		{
			Stuck();
			return true;
		}
	}

	return false;
}

void CArrow::Face_Camera()
{

	_float3 vCamToArrow = m_pTransform->Get_WorldPosition() - CManagement::Get_Instance()->Get_CamPosition();
	_float3 vNewLook, vNewRight, vNewUp;

	vNewUp = m_pTransform->Get_WorldState(Engine::CTransform::STATE_UP);
	vNewLook = m_pTransform->Get_WorldState(Engine::CTransform::STATE_LOOK);
	vNewRight = m_pTransform->Get_WorldState(Engine::CTransform::STATE_RIGHT);

	_float fCUp = D3DXVec3Length(&vNewUp);
	_float fCLook = D3DXVec3Length(&vNewLook);
	_float fCRight = D3DXVec3Length(&vNewRight);

	vNewUp = m_pRigidBody->Get_Velocity();

	D3DXVec3Normalize(&vNewUp, &vNewUp);
	D3DXVec3Cross(&vNewRight, &vNewUp, &vCamToArrow);
	D3DXVec3Normalize(&vNewRight, &vNewRight);
	D3DXVec3Cross(&vNewLook, &vNewRight, &vNewUp);
	D3DXVec3Normalize(&vNewLook, &vNewLook);

	m_pTransform->Set_State(Engine::CTransform::STATE_LOOK, vNewLook * fCLook);
	m_pTransform->Set_State(Engine::CTransform::STATE_RIGHT, vNewRight * fCRight);
	m_pTransform->Set_State(Engine::CTransform::STATE_UP, vNewUp * fCUp);

}

void CArrow::Stuck(CGameObject * pParent)
{
	m_pParent = pParent;
	if (m_pParent != nullptr)
	{
		////_float3 vVel = m_pRigidBody->Get_Velocity();
		////_float	fVel = D3DXVec3Length(&vVel);
		////D3DXVec3Normalize(&vVel, &vVel);
		////m_pTransform->SetUp_Position(m_pCollider->Get_Pos());
		////if (fVel > 1.f)
		////	m_pTransform->SetUp_Position(m_pTransform->Get_WorldPosition() - vVel * powf(fVel, 3.f) *0.1f);
		////else
		////	m_pTransform->SetUp_Position(m_pTransform->Get_WorldPosition() - vVel * 1.f);

		////m_pTransform->SetUp_Position(m_pTransform->Get_WorldPosition() - vVel * fVel * 1.f);


		m_pParentTransform = (CTransform*)(pParent->Find_Component(L"Com_Transform"));
		//_matrix matTemp = ((CUnit*)m_pParent)->Get_WorldMatrix();
		////D3DXMatrixInverse(&matTemp, nullptr, &matTemp);
		////_matrix matTemp = ((CUnit*)m_pParent)->Get_Wor();
		////D3DXMatrixInverse(&matTemp, nullptr, &matTemp);
		_matrix matTemp = m_pParentTransform->Get_WorldMatrixInverse();
		m_matParentalLocal = m_pTransform->Get_WorldMatrix() * matTemp;


		//_matrix dddddd;
		//D3DXMatrixIdentity(&dddddd);
		//dddddd *= 10.f;

		//m_pTransform->Set_WorldMatrix(m_matParentalLocal);

		//m_pTransform->Set_Parent((CTransform*)pParent->Find_Component(L"Com_Transform"));

	}
	else
		m_matParentalLocal = m_pTransform->Get_WorldMatrix();

	m_bIsStuck = 1;
	m_bNoCollide = 1;
}

HRESULT CArrow::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Arrow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Transform
	CTransform::STATEDESC StateDesc;
	StateDesc.SpeedPerSec = 10.0;
	StateDesc.RotatePerSec = 0.0;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform, &StateDesc)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Collider
	CCollider_Ellipse::STATEDESC_DERIVED tColliderDesc;
	tColliderDesc.fRadius = 0.2f;
	tColliderDesc.fOffSetZ = 0.f;
	D3DXMatrixIdentity(&tColliderDesc.StateMatrix);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Ellipse", L"Com_Collider", (CComponent**)&m_pCollider, &tColliderDesc)))
		return E_FAIL;

	// For.Com_RigidBody
	Engine::CRigid_Body::STATEDESC RigideDesc;
	RigideDesc.fBounce = 0.f;
	RigideDesc.fFriction_Dynamic = 0.5f;
	RigideDesc.fFriction_Static = 0.5f;
	RigideDesc.fGravity_Multiply = 1.f;
	RigideDesc.fMass = 0.5f;
	RigideDesc.fMaxFallSpeed = 0.f;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_RigidBody", L"Com_RigidBody", (CComponent**)&m_pRigidBody, &RigideDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CArrow::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pTexture->Set_TextureOnShader(m_pShader, "g_DiffuseTexture", m_iSkin);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	_float fmDist = CPickingMgr::Get_Instance()->Get_Blur_mDist();
	_float fMDist = CPickingMgr::Get_Instance()->Get_Blur_MDist();
	m_pShader->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShader->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShader->Set_Value("g_BlurCenter", &_float4(pManagement->Get_CamPosition(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix));
	m_pShader->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShader->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	Safe_Release(pManagement);

	return S_OK;
}

//_int CArrow::Update_Movement(_double TimeDelta)
//{
//	m_AccTimer += TimeDelta;
//	if (1.0 <= m_AccTimer)
//	{
//		m_bDead = true;
//		return -1;
//	}
//
//	m_pTransform->Go_Straight(TimeDelta * m_dPower);
//
//	return _int(0);
//}

CArrow * CArrow::Create_Arrow_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CArrow* pInstance = new CArrow(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CArrow Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CArrow::Clone_GameObject(void * pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CArrow Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArrow::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRigidBody);

	CWeapon::Free();
}

#include "stdafx.h"
#include "..\Headers\Meteor.h"
#include "Management.h"
#include "PickingMgr.h"
#include "CollisionMgr.h"
#include "Effect.h"
#include "Unit.h"
#include "Cube.h"
#include "SoundMgr.h"
CMeteor::CMeteor(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMeteor::CMeteor(const CMeteor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeteor::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CMeteor::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPosition);

	return S_OK;
}

_int CMeteor::Update_GameObject(_double TimeDelta)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement ||
		nullptr == m_pTransformCom)
		return -1;
	Safe_AddRef(pManagement);
	
	_matrix ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * m_StateDesc.vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * m_StateDesc.vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
	
	m_pTransformCom->Go_Dst(m_StateDesc.vDstPosition, TimeDelta);

	_float3 floor = m_StateDesc.vDstPosition;

	if (30 > abs(m_pTransformCom->Get_Position().y - floor.y))
	{

		CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
		if (nullptr == pSoundMgr) return -1;

		pSoundMgr->StopSound(CSoundMgr::EFFECT);
		pSoundMgr->PlaySound(L"meteor_burst.mp3", CSoundMgr::EFFECT);


		CEffect::STATEDESC EffectDesc;
		ZeroMemory(&EffectDesc, sizeof(CEffect::STATEDESC));
		EffectDesc.eTextureScene = SCENE_STAGE;
		EffectDesc.pTextureKey = L"Component_Texture_Effect_Explosion";
		EffectDesc.iTextureNum = 18;
		EffectDesc.iShaderPass = 11;
		EffectDesc.fFrameTime = 0.1f;

		EffectDesc.vPosition = floor;
		EffectDesc.vScale = m_StateDesc.vScale;
		pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect", &EffectDesc);

		list<CGameObject*>* pLayer = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");
		if (nullptr == pLayer)
		{
			m_bDead = true;
			Safe_Release(pManagement);
			return E_FAIL;
		}

		list<CGameObject*>* pBossLayer = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Boss");
		if (nullptr == pBossLayer)
		{
			m_bDead = true;
			Safe_Release(pManagement);
			return E_FAIL;
		}

		for (CGameObject* pGameObject : *pLayer)
		{
			if (pGameObject->Get_Active())
			{
				CTransform* pTransform = (CTransform*)pGameObject->Find_Component(L"Com_Transform");
				_float3 DirVec = floor - pTransform->Get_Position();
				_float Distance = D3DXVec3Length(&DirVec);
				if (m_StateDesc.fRadius > Distance)
				{
					CUnit* pUnit = (CUnit*)pGameObject;
					pUnit->TakeDamage((_int)m_StateDesc.fDamage);
				}
			}

			for (CGameObject* pGameObject : *pBossLayer)
			{
				if (pGameObject->Get_Active())
				{
					CTransform* pTransform = (CTransform*)pGameObject->Find_Component(L"Com_Transform");
					_float3 DirVec = floor - pTransform->Get_Position();
					_float Distance = D3DXVec3Length(&DirVec);
					if (m_StateDesc.fRadius > Distance)
					{
						CUnit* pUnit = (CUnit*)pGameObject;
						pUnit->TakeDamage((_int)m_StateDesc.fDamage);
					}
				}
			}
		}
		// 捞棋飘 积己
		m_bDead = true;
	}
	Safe_Release(pManagement);
	return NOERROR;
}

_int CMeteor::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	Compute_ViewZ(m_pTransformCom->Get_Position());

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return -1;

	if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COLLISION_GROUP::COL_SKILL,  this)))
		return -1;

	return _int();
}

HRESULT CMeteor::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(0);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	m_pColliderCom->Render_Collider();
	return S_OK;
}

HRESULT CMeteor::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	CTransform::STATEDESC StateDesc;
	StateDesc.SpeedPerSec = m_StateDesc.fSpeed;
	StateDesc.RotatePerSec = D3DXToRadian(90.0f);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &StateDesc)))
		return E_FAIL;

	// For.Com_Collider
	CCollider_Ellipse::STATEDESC_DERIVED tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider_Ellipse::STATEDESC_DERIVED));
	tColliderDesc.fRadius = m_StateDesc.vScale.x * 0.1f;
	D3DXMatrixIdentity(&tColliderDesc.StateMatrix);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Ellipse", L"Com_Collider", (CComponent**)&m_pColliderCom, &tColliderDesc)))
		return E_FAIL;

	m_pColliderCom->Set_Trigger(true);

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Meteor", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeteor::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

void CMeteor::OnCollisionEnter(CGameObject * _pOther)
{
	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	if (nullptr == pSoundMgr) return;

	pSoundMgr->StopSound(CSoundMgr::EFFECT);
	pSoundMgr->PlaySound(L"meteor_burst.mp3", CSoundMgr::EFFECT);


	CCube* pCube = dynamic_cast<CCube*>(_pOther);
	if (nullptr == pCube)
		return;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	CEffect::STATEDESC EffectDesc;
	ZeroMemory(&EffectDesc, sizeof(CEffect::STATEDESC));
	EffectDesc.eTextureScene = SCENE_STAGE;
	EffectDesc.pTextureKey = L"Component_Texture_Effect_Explosion";
	EffectDesc.iTextureNum = 18;
	EffectDesc.iShaderPass = 4;
	EffectDesc.fFrameTime = 0.1f;

	EffectDesc.vPosition = m_pTransformCom->Get_Position();
	EffectDesc.vScale = m_StateDesc.vScale;
	pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect", &EffectDesc);

	list<CGameObject*>* pLayer = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");
	if (nullptr == pLayer)
	{
		m_bDead = true;
		Safe_Release(pManagement);
		return;
	}

	for (CGameObject* pGameObject : *pLayer)
	{
		if (pGameObject->Get_Active())
		{
			CTransform* pTransform = (CTransform*)pGameObject->Find_Component(L"Com_Transform");
			_float3 DirVec = m_pTransformCom->Get_Position() - pTransform->Get_Position();
			_float Distance = D3DXVec3Length(&DirVec);
			if (m_StateDesc.fRadius > Distance)
			{
				CUnit* pUnit = (CUnit*)pGameObject;
				pUnit->TakeDamage((_int)m_StateDesc.fDamage);
			}
		}
		else
			int i = 0;
	}
	// 捞棋飘 积己
	m_bDead = true;


	
	Safe_Release(pManagement);
}


CMeteor * CMeteor::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CMeteor*	pInstance = new CMeteor(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CMeteor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMeteor::Clone_GameObject(void * pArg)
{
	CMeteor*	pInstance = new CMeteor(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CMeteor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeteor::Free()
{
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

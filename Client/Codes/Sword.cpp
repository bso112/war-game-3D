#include "stdafx.h"
#include "..\Headers\Sword.h"
#include "Unit.h"
#include "PickingMgr.h"
#include "SoundMgr.h"

CSword::CSword(PDIRECT3DDEVICE9 pGraphic_Device)
	: CWeapon(pGraphic_Device)
{
}

CSword::CSword(const CSword & rhs)
	: CWeapon(rhs)
{
}

HRESULT CSword::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CSword::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	m_StateDesc.iAttack = 5;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->SetUp_Scale(m_StateDesc.vScale);
	m_pTransform->SetUp_Position(m_StateDesc.vPos);

	m_vAxis = _float3(1.f, -1.f, -1.f);

	m_pCollider->Set_Enable(false);

	_float3 vLook, vRight;
	vRight = m_pTransform->Get_WorldState(Engine::CTransform::STATE_LOOK);
	vLook = -m_pTransform->Get_WorldState(Engine::CTransform::STATE_RIGHT);

	m_pTransform->Set_State(Engine::CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(Engine::CTransform::STATE_LOOK, vLook);

	return S_OK;
}

_int CSword::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
		return -1;

	if (0x8000 & GetKeyState(VK_LBUTTON) && false == m_isAttack && CONTROL_USER == m_eControlID)
		m_isAttack = true;

	if (CONTROL_USER == m_eControlID)
		int i = 10;

	if (0x80000000 & Update_Attack(TimeDelta))
		return -1;

#pragma region BOSS
	if (true == m_isBoss)
	{
		m_BossTimer += TimeDelta;
		if (1.5 <= m_BossTimer)
		{
			m_isBoss = false;

			CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
			if (nullptr == pSoundMgr)
				return -1;

			_tchar* pSoundTags = { L"Boss_Sound.wav" };

			pSoundMgr->PlaySound_Overwrite(pSoundTags, CSoundMgr::BOSS);
		}

		_matrix RotationMatrix;
		D3DXMatrixRotationAxis(&RotationMatrix, &_float3(1.f, 0.f, 0.f), D3DXToRadian(180.f) * -(_float)m_BossTimer);
		m_pTransform->Set_RevolutionMatrix(RotationMatrix);
	}
#pragma endregion

	if (nullptr == m_pCollider ||
		nullptr == m_pTransform)
		return -1;

	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	_float3 vLook, vRight;

	vRight = m_pTransform->Get_WorldState(Engine::CTransform::STATE_LOOK) * -1.f;
	vLook = m_pTransform->Get_WorldState(Engine::CTransform::STATE_RIGHT);

	memcpy(&WorldMatrix.m[CTransform::STATE_RIGHT], &vRight, sizeof(_float3));
	memcpy(&WorldMatrix.m[CTransform::STATE_LOOK], &vLook, sizeof(_float3));

	m_pCollider->Update_Collider(WorldMatrix);

	return _int(0);
}

_int CSword::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer || nullptr == m_pRenderer)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return -1;

	if (pManagement->Is_InFrustum(m_pTransform->Get_Position()))
	{
		if (true == m_hasBoss)
		{
			if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
				return -1;
		}
		else
		{
			if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
				return -1;
		}
	}

	return _int(0);
}

HRESULT CSword::Render_GameObject()
{
	if (nullptr == m_pVIBuffer ||
		nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();
	if (true == m_hasBoss)
		m_pShader->Begin_Pass(4);
	else
		m_pShader->Begin_Pass(6);

	m_pVIBuffer->Render_VIBuffer();

	m_pShader->End_Pass();
	m_pShader->End_Shader();

#ifdef _DEBUG
	if (FAILED(m_pCollider->Render_Collider()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CSword::Attack(_double TimeDelta)
{
	if (false == m_isAttack)
		m_isAttack = true;

	return S_OK;
}

void CSword::OnCollisionEnter(CGameObject * _pOther)
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
			}
		}
	}
}


HRESULT CSword::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Sword", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Box", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShader->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShader->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShader->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix));
	m_pShader->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShader->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	m_pTexture->Set_TextureOnShader(m_pShader, "g_DiffuseTexture", m_hasBoss);

	Safe_Release(pManagement);

	return S_OK;
}

_int CSword::Update_Attack(_double TimeDelta)
{
	if (false == m_isAttack)
		return 0;

	
	_matrix RotationMatrix;
	D3DXMatrixIdentity(&RotationMatrix);

	if (ATTACK_DOWN == m_eAttackID)		// 내려치기
	{
		m_AccTimer += TimeDelta * 8.0;

		D3DXMatrixRotationAxis(&RotationMatrix, &m_vAxis, D3DXToRadian(180.f) * (_float)m_AccTimer);

		m_pTransform->Set_RevolutionMatrix(RotationMatrix);

		if (1.0 <= m_AccTimer)
		{
			m_isAttack = false;
			m_eAttackID = ATTACK_UP;
		}
	}
	else if (ATTACK_UP == m_eAttackID)	// 올려치기
	{
		m_AccTimer -= TimeDelta * 8.0;

		D3DXMatrixRotationAxis(&RotationMatrix, &m_vAxis, D3DXToRadian(180.f) * (_float)m_AccTimer);

		m_pTransform->Set_RevolutionMatrix(RotationMatrix);

		if (0.0 >= m_AccTimer)
		{
			m_isAttack = false;
			m_eAttackID = ATTACK_DOWN;
		}
	}

	return _int(0);
}

CSword * CSword::Create_Sword_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CSword* pInstance = new CSword(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CSword Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSword::Clone_GameObject(void * pArg)
{
	CSword* pInstance = new CSword(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CSword Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
	if (true == m_hasBoss)
	{
		int i = 10;
	}

	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);

	CWeapon::Free();
}

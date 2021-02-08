#include "stdafx.h"
#include "..\Headers\Staff.h"
#include "EnergyBall.h"

CStaff::CStaff(PDIRECT3DDEVICE9 pGraphic_Device)
	: CWeapon(pGraphic_Device)
{
}

CStaff::CStaff(const CStaff & rhs)
	: CWeapon(rhs)
{
}

HRESULT CStaff::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CStaff::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	m_StateDesc.iAttack = 10;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->SetUp_Scale(m_StateDesc.vScale);
	m_pTransform->SetUp_Position(m_StateDesc.vPos);

	_float3 vLook, vRight;
	vLook = -m_pTransform->Get_WorldState(Engine::CTransform::STATE_RIGHT);
	vRight = m_pTransform->Get_WorldState(Engine::CTransform::STATE_LOOK);

	m_pTransform->Set_State(Engine::CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(Engine::CTransform::STATE_LOOK, vLook);

	return S_OK;
}

_int CStaff::Update_GameObject(_double TimeDelta)
{
	if (0x8000 & GetKeyState(VK_LBUTTON) && false == m_isAttack && CONTROL_USER == m_eControlID)
		m_isAttack = true;

	if (0x80000000 & Update_Attack(TimeDelta))
		return -1;

	if (0x80000000 & Update_EnergyBall_ParentsMatrix())
		return -1;

	return _int(0);
}

_int CStaff::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer || nullptr == m_pRenderer)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return -1;

	if (pManagement->Is_InFrustum(m_pTransform->Get_Position()))
	{
		if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;
	}
	return _int(0);
}

HRESULT CStaff::Render_GameObject()
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

	return S_OK;
}

HRESULT CStaff::Attack(_double TimeDelta)
{
	if (false == m_isAttack)
		m_isAttack = true;

	return S_OK;
}

HRESULT CStaff::Add_Component()
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

	// For.Component_Texture_Staff
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Staff", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaff::Ready_Layer_EnergyBall(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CEnergyBall::STATEDESC StateDesc;
	StateDesc.vScale = _float3(g_fUnitCX * 5.f, g_fUnitCY * 5.f, g_fUnitCZ * 5.f);
	StateDesc.vPos = _float3(0.f, StateDesc.vScale.y * 0.1f, StateDesc.vScale.z * 0.5f);
	StateDesc.iAttack = m_StateDesc.iAttack;
	if (FAILED(m_pEnergyBall = dynamic_cast<CEnergyBall*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_EnergyBall", SCENE_STAGE, pLayerTag, &StateDesc))))
		return E_FAIL;

	m_pEnergyBall->Set_Owner(m_tOwner);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CStaff::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix));
	m_pShader->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShader->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	m_pTexture->Set_TextureOnShader(m_pShader, "g_DiffuseTexture");

	Safe_Release(pManagement);

	return S_OK;
}

_int CStaff::Update_EnergyBall_ParentsMatrix()
{
	if (nullptr == m_pEnergyBall)
		return 0;

	// 부모 행렬
	_matrix ParentsMatrix = m_pTransform->Get_WorldMatrix_Scale1();
	_float3 vRight, vUp, vLook;

	memcpy(&vRight, &ParentsMatrix.m[2], sizeof(_float3));
	memcpy(&vLook, &ParentsMatrix.m[0], sizeof(_float3));
	vRight *= -1.f;

	memcpy(ParentsMatrix.m[0], &vRight, sizeof(_float3));
	memcpy(ParentsMatrix.m[2], &vLook, sizeof(_float3));

	m_pEnergyBall->Set_ParentsMatrix(ParentsMatrix);

	return _int(0);
}

_int CStaff::Update_Attack(_double TimeDelta)
{
	if (false == m_isAttack)
		return 0;

	if (false == m_isCharged)
	{
		m_AccTimer += TimeDelta;

		if (1.0 <= m_AccTimer)	// 충전 시간
		{
			m_AccTimer = 0.0;
			m_isCharged = true;
		}
	}

	if (false == m_hasEnergyBall && true == m_isCharged)
	{
		m_AccTimer += TimeDelta;

		if (0.5 <= m_AccTimer)	// 마법 생성
		{
			m_hasEnergyBall = true;

			m_pTransform->SetUp_Position(_float3(m_StateDesc.vPos.x, m_StateDesc.vPos.y, m_StateDesc.vPos.z + m_StateDesc.vScale.z * 0.5f));

			if (FAILED(Ready_Layer_EnergyBall(L"Layer_EnergyBall")))
				return -1;
		}
		else
		{
			m_pTransform->AddForce(_float3(0.f, 0.f, (_float)TimeDelta * m_StateDesc.vScale.z));
		}
	}
	else if(true == m_hasEnergyBall)
	{
		if (false == m_pEnergyBall->Get_HasAttack())	// 마법 종료 확인
		{
			m_AccTimer -= TimeDelta;

			if (0.0 >= m_AccTimer)
			{
				m_isAttack = false;
				m_hasEnergyBall = false;

				m_AccTimer = 0.0;
				m_pTransform->SetUp_Position(_float3(m_StateDesc.vPos.x, m_StateDesc.vPos.y, m_StateDesc.vPos.z));

				m_pEnergyBall->Set_IsDead(true);
				m_pEnergyBall = nullptr;
			}
			else
			{
				m_pTransform->AddForce(_float3(0.f, 0.f, (_float)TimeDelta * -m_StateDesc.vScale.z));
			}
		}
	}

	return _int(0);
}

CStaff * CStaff::Create_Staff_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CStaff* pInstance = new CStaff(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CStaff Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaff::Clone_GameObject(void * pArg)
{
	CStaff* pInstance = new CStaff(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CStaff Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaff::Free()
{
	if (nullptr != m_pEnergyBall)
		m_pEnergyBall->Set_IsDead(true);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);

	CWeapon::Free();
}

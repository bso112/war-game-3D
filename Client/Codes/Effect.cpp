#include "stdafx.h"
#include "..\Headers\Effect.h"
#include "Management.h"
#include "PickingMgr.h"

CEffect::CEffect(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPosition);

	return S_OK;
}

_int CEffect::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
		return -1;

	if (!m_bActive)
		return NOERROR;
	
	if (m_StateDesc.bAlpha)
	{
		_float a = m_StateDesc.fFrameTime * m_StateDesc.iTextureNum;
		_float b = 0.f;
		b =(_float)TimeDelta / a;
		m_fCurrentAlpha -= b;
	}

	m_fCurrentFrameTime += (_float)TimeDelta;
	if (m_StateDesc.fFrameTime <= m_fCurrentFrameTime)
	{
		m_fCurrentFrameTime = 0.f;
		++m_iCurrentTexture;
		if (m_StateDesc.iTextureNum - 1 <= m_iCurrentTexture)
		{
			m_bDead = true;
			return -1;
		}
	}

	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	_matrix ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, (_float3)ViewMatrix.m[0] * m_StateDesc.vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, (_float3)ViewMatrix.m[1] * m_StateDesc.vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, (_float3)ViewMatrix.m[2]);

	Safe_Release(pManagement);

	return NOERROR;
}

_int CEffect::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	Compute_ViewZ(m_pTransformCom->Get_Position());

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return -1;

	return NOERROR;
}

HRESULT CEffect::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(m_StateDesc.iShaderPass);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CEffect::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(m_StateDesc.eTextureScene, m_StateDesc.pTextureKey, L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShaderCom->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_Alpha", &m_fCurrentAlpha, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_iCurrentTexture)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}


CEffect * CEffect::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect*	pInstance = new CEffect(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect::Clone_GameObject(void * pArg)
{
	CEffect*	pInstance = new CEffect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

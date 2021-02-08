#include "stdafx.h"
#include "Particle.h"
#include "Management.h"
#include "PickingMgr.h"

USING(Client)

CParticle::CParticle(PDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CParticle::CParticle(const CParticle & rhs)
	: CGameObject(rhs)
{
}



HRESULT CParticle::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CParticle::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CParticle::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	return _int();
}

_int CParticle::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (nullptr == m_pRendererCom)
		return -1;

	Compute_ViewZ(m_pTransformCom->Get_Position());

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return E_FAIL;

	return _int();
}

HRESULT CParticle::Render_GameObject()
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin_Shader()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin_Pass(PASS_PARICLE_ALPHA)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render_VIBuffer()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->End_Pass()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->End_Shader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::Render_GameObject(CShader* pShader)
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;


	if (FAILED(pShader->Commit_Change()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render_VIBuffer()))
		return E_FAIL;

	_float defaultAlpha = 1;
	if (FAILED(pShader->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Meteor", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	matView = pManagement->Get_Transform(D3DTS_VIEW);
	matProj = pManagement->Get_Transform(D3DTS_PROJECTION);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShaderCom->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_iCurrentFrameIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &matView, sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &matProj, sizeof(_matrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}


CParticle * CParticle::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CParticle*	pInstance = new CParticle(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParticle::Clone_GameObject(void * pArg)
{
	CParticle*	pInstance = new CParticle(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	CGameObject::Free();
}

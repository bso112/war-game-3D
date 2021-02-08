#include "stdafx.h"
#include "Blend.h"




CBlend::CBlend(PDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderTarget(pGraphic_Device)
{
}

CBlend::CBlend(const CBlend & rhs)
	: CRenderTarget(rhs)
{
}


HRESULT CBlend::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CBlend::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(STATEDESC));

	Safe_AddRef(m_tDesc.pBlendTexture);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_ViewPort", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Blend", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return E_FAIL;

	m_pRawTexture = pManagement->Get_Texture(L"Target_Blend");
	Safe_AddRef(m_pRawTexture);


	m_pTransformCom->SetUp_Position(m_tDesc.tBaseDesc.vPos);
	m_pTransformCom->SetUp_Scale(m_tDesc.tBaseDesc.vSize);


	return S_OK;
}

_int CBlend::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CBlend::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CBlend::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pTransformCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;




	if (FAILED(m_pVIBufferCom->Set_Transform(m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Texture("g_BlendTexture", m_tDesc.pBlendTexture)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Texture("g_BaseTexture", m_pRawTexture)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin_Shader()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin_Pass(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->End_Pass()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->End_Shader()))
		return E_FAIL;

	return S_OK;
}

CBlend* CBlend::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlend*	pInstance = new CBlend(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CInfantry");
		Safe_Release(pInstance);
	}
	return pInstance;

}

CGameObject * CBlend::Clone_GameObject(void * pArg)
{
	CBlend*	pInstance = new CBlend(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CInfantry");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlend::Free()
{
	Safe_Release(m_tDesc.pBlendTexture);
	CRenderTarget::Free();
}

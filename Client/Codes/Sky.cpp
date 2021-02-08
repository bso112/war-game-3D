#include "stdafx.h"
#include "..\Headers\Sky.h"
#include "Management.h"
#include "PickingMgr.h"

CSky::CSky(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{

}

HRESULT CSky::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CSky::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CSky::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CSky::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(Set_Position_ToCamera()))
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return -1;

	return _int();
}

HRESULT CSky::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom || 
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(PASS_SKY);

	m_pVIBufferCom->Render_VIBuffer();	

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}

HRESULT CSky::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Sky", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Cube", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
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

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", 2)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CSky::Set_Position_ToCamera()
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_matrix ViewMatrixInverse;
	D3DXMatrixInverse(&ViewMatrixInverse, nullptr, &pManagement->Get_Transform(D3DTS_VIEW));

	_float3 vPosition;
	memcpy(&vPosition, (_float3*)ViewMatrixInverse.m[3], sizeof(_float3));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	Safe_Release(pManagement);

	return S_OK;
}

CSky * CSky::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CSky*	pInstance = new CSky(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSky::Clone_GameObject(void * pArg)
{
	CSky*	pInstance = new CSky(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSky::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}

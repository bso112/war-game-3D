#include "stdafx.h"
#include "..\Headers\UI.h"
#include "Management.h"

USING(Client)

CUI::CUI(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CUI::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_pTransformCom->SetUp_Scale(_float3(m_StateDesc.vProjSize.x, m_StateDesc.vProjSize.y, 1));
	m_pTransformCom->SetUp_Position(_float3(m_StateDesc.vProjPos.x, m_StateDesc.vProjPos.y, 0.f));

	return S_OK;
}

_int CUI::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;
	
	if (m_bDead)
		return -1;

	return NOERROR;
}

_int CUI::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;

	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return -1;

	return NOERROR;
}

HRESULT CUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(2);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_matrix Matrix = m_pTransformCom->Get_WorldMatrix();
	_float3 vPosition = *(_float3*)Matrix.m[3];
	vPosition.x -= g_iWinSizeX >> 1;
	vPosition.y = (g_iWinSizeY >> 1) - vPosition.y;
	memcpy(&Matrix.m[3], &vPosition, sizeof(_float3));
	m_pShaderCom->Set_Value("g_matWorld", &Matrix, sizeof(_matrix));

	D3DXMatrixIdentity(&Matrix);
	m_pShaderCom->Set_Value("g_matView", &Matrix, sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &m_ProjMatrix, sizeof(_matrix));

	Safe_Release(pManagement);
	return S_OK;
}

void CUI::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	CGameObject::Free();
}

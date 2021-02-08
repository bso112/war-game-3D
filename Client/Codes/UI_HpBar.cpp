#include "stdafx.h"
#include "..\Headers\UI_HpBar.h"
#include "Unit.h"

USING(Client)

CUI_HpBar::CUI_HpBar(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_HpBar::CUI_HpBar(const CUI_HpBar & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_HpBar::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_HpBar::Ready_GameObject(void * pArg)
{
	return CUI::Ready_GameObject(pArg);
}

_int CUI_HpBar::Update_GameObject(_double TimeDelta)
{
	if(nullptr == m_ppUnit || nullptr == *m_ppUnit)
		return S_OK;

	m_fPercent = (_float)(*m_ppUnit)->Get_Stat().iCurrHp / (_float)(*m_ppUnit)->Get_Stat().iMaxHp;

	return CUI::Update_GameObject(TimeDelta);
}

_int CUI_HpBar::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_HpBar::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CUI_HpBar::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_HpBar", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HpBar::SetUp_ConstantTable()
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
	m_pShaderCom->Set_Value("g_Percent", &m_fPercent, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_StateDesc.iTexIndex)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

CUI_HpBar * CUI_HpBar::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_HpBar* pInstance = new CUI_HpBar(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_HpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HpBar::Clone_GameObject(void * pArg)
{
	CUI_HpBar* pInstance = new CUI_HpBar(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_HpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HpBar::Free()
{
	CUI::Free();
}

#include "stdafx.h"
#include "..\Headers\UI_Icon.h"

USING(Client)

CUI_Icon::CUI_Icon(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_Icon::CUI_Icon(const CUI_Icon & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Icon::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_Icon::Ready_GameObject(void * pArg)
{
	return CUI::Ready_GameObject(pArg);
}

_int CUI_Icon::Update_GameObject(_double TimeDelta)
{
	return CUI::Update_GameObject(TimeDelta);
}

_int CUI_Icon::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Icon::Render_GameObject()
{
	return CUI::Render_GameObject();
}

HRESULT CUI_Icon::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Icon", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Icon::SetUp_ConstantTable()
{
	if (FAILED(CUI::SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_StateDesc.iTexIndex)))
		return E_FAIL;

	return S_OK;
}

CUI_Icon * CUI_Icon::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Icon* pInstance = new CUI_Icon(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Icon::Clone_GameObject(void * pArg)
{
	CUI_Icon* pInstance = new CUI_Icon(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Icon::Free()
{
	CUI::Free();
}

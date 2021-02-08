#include "stdafx.h"
#include "..\Headers\UI_Map.h"

USING(Client)

CUI_Map::CUI_Map(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_Map::CUI_Map(const CUI_Map & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Map::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_Map::Ready_GameObject(void * pArg)
{
	return CUI::Ready_GameObject(pArg);
}

_int CUI_Map::Update_GameObject(_double TimeDelta)
{
	return CUI::Update_GameObject(TimeDelta);
}

_int CUI_Map::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Map::Render_GameObject()
{
	return CUI::Render_GameObject();
}

HRESULT CUI_Map::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Map", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Map::SetUp_ConstantTable()
{
	if (FAILED(CUI::SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_StateDesc.iTexIndex)))
		return E_FAIL;

	return S_OK;
}

CUI_Map * CUI_Map::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Map* pInstance = new CUI_Map(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_Map");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Map::Clone_GameObject(void * pArg)
{
	CUI_Map* pInstance = new CUI_Map(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_Map");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Map::Free()
{
	CUI::Free();
}

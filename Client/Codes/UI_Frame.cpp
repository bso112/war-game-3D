#include "stdafx.h"
#include "..\Headers\UI_Frame.h"

USING(Client)

CUI_Frame::CUI_Frame(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_Frame::CUI_Frame(const CUI_Frame & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Frame::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_Frame::Ready_GameObject(void * pArg)
{
	return CUI::Ready_GameObject(pArg);
}

_int CUI_Frame::Update_GameObject(_double TimeDelta)
{
	return CUI::Update_GameObject(TimeDelta);
}

_int CUI_Frame::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Frame::Render_GameObject()
{
	return CUI::Render_GameObject();
}

HRESULT CUI_Frame::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Frame", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Frame::SetUp_ConstantTable()
{
	if (FAILED(CUI::SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_StateDesc.iTexIndex)))
		return E_FAIL;

	return S_OK;
}

CUI_Frame * CUI_Frame::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Frame* pInstance = new CUI_Frame(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Frame::Clone_GameObject(void * pArg)
{
	CUI_Frame* pInstance = new CUI_Frame(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Frame::Free()
{
	CUI::Free();
}

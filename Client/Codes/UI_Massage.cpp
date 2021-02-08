#include "stdafx.h"
#include "..\Headers\UI_Massage.h"

USING(Client)

CUI_Massage::CUI_Massage(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_Massage::CUI_Massage(const CUI_Massage & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Massage::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_Massage::Ready_GameObject(void * pArg)
{
	return CUI::Ready_GameObject(pArg);
}

_int CUI_Massage::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;

	if (m_bDead)
		return -1;

	m_dCurrentLifeTime += TimeDelta;
	if (m_dLifeTime <= m_dCurrentLifeTime)
	{
		m_dCurrentLifeTime = 0;
		m_bDead = true;
		return -1;
	}

	return NOERROR;
}

_int CUI_Massage::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Massage::Render_GameObject()
{
	return CUI::Render_GameObject();
}

HRESULT CUI_Massage::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Event", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Massage::SetUp_ConstantTable()
{
	if (FAILED(CUI::SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_StateDesc.iTexIndex)))
		return E_FAIL;

	return S_OK;
}

CUI_Massage * CUI_Massage::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Massage* pInstance = new CUI_Massage(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_Massage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Massage::Clone_GameObject(void * pArg)
{
	CUI_Massage* pInstance = new CUI_Massage(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_Massage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Massage::Free()
{
	CUI::Free();
}

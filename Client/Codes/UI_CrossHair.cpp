#include "stdafx.h"
#include "..\Headers\UI_CrossHair.h"

USING(Client)

CUI_CrossHair::CUI_CrossHair(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_CrossHair::CUI_CrossHair(const CUI_CrossHair & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_CrossHair::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_CrossHair::Ready_GameObject(void * pArg)
{
	return CUI::Ready_GameObject(pArg);
}

_int CUI_CrossHair::Update_GameObject(_double TimeDelta)
{
	return CUI::Update_GameObject(TimeDelta);
}

_int CUI_CrossHair::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_CrossHair::Render_GameObject()
{
	return CUI::Render_GameObject();
}

HRESULT CUI_CrossHair::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_CrossHair", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CrossHair::SetUp_ConstantTable()
{
	if (FAILED(CUI::SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_CrossHair * CUI_CrossHair::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_CrossHair* pInstance = new CUI_CrossHair(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_CrossHair");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CrossHair::Clone_GameObject(void * pArg)
{
	CUI_CrossHair* pInstance = new CUI_CrossHair(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_CrossHair");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CrossHair::Free()
{
	CUI::Free();
}

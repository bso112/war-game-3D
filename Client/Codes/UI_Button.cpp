#include "stdafx.h"
#include "UI_Button.h"
#include "KeyMgr.h"

USING(Client)

CUI_Button::CUI_Button(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CUI_Button::CUI_Button(const CUI_Button & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Button::Ready_GameObject_Prototype()
{
	return CUI::Ready_GameObject_Prototype();
}

HRESULT CUI_Button::Ready_GameObject(void * pArg)
{
	CKeyMgr::Get_Instance()->RegisterObserver(this);
	return CUI::Ready_GameObject(pArg);
}

_int CUI_Button::Update_GameObject(_double TimeDelta)
{
	return CUI::Update_GameObject(TimeDelta);
}

_int CUI_Button::Late_Update_GameObject(_double TimeDelta)
{
	return CUI::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Button::Render_GameObject()
{
	return CUI::Render_GameObject();
}

_bool CUI_Button::Check_In_Mouse()
{
	if (!m_bActive)
		return false;

	POINT point = {};
	GetCursorPos(&point);
	ScreenToClient(g_hWnd, &point);

	_float2 vCursorPos = _float2((_float)point.x, (_float)point.y);
	_float2 vProjSize = m_StateDesc.vProjSize * 0.5f;

	if (m_StateDesc.vProjPos.x + vProjSize.x > vCursorPos.x && m_StateDesc.vProjPos.x - vProjSize.x < vCursorPos.x
		&& m_StateDesc.vProjPos.y + vProjSize.y > vCursorPos.y && m_StateDesc.vProjPos.y - vProjSize.y < vCursorPos.y)
	{
		m_StateDesc.iTexIndex = 1;
		return true;
	}
	else
	{
		m_StateDesc.iTexIndex = 0;
		return false;
	}
}

HRESULT CUI_Button::Add_Component()
{
	if (FAILED(CUI::Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Button", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Button::SetUp_ConstantTable()
{
	if (FAILED(CUI::SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_StateDesc.iTexIndex)))
		return E_FAIL;

	return S_OK;
}

void CUI_Button::Free()
{
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);
	CUI::Free();
}

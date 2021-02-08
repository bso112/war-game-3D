#include "stdafx.h"
#include "..\Headers\UI_Button_Class.h"
#include "Management.h"
#include "KeyMgr.h"
#include "GameManager.h"
#include "UI_Manager.h"
#include "InputManager.h"

USING(Client)

CUI_Button_Class::CUI_Button_Class(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Button(pGraphic_Device)
{
}

CUI_Button_Class::CUI_Button_Class(const CUI_Button_Class & rhs)
	: CUI_Button(rhs)
{
}

HRESULT CUI_Button_Class::Ready_GameObject_Prototype()
{
	return CUI_Button::Ready_GameObject_Prototype();
}

HRESULT CUI_Button_Class::Ready_GameObject(void * pArg)
{
	return CUI_Button::Ready_GameObject(pArg);
}

_int CUI_Button_Class::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;

	if (FAILED(Check_In_Mouse()))
		return E_FAIL;

	return CUI_Button::Update_GameObject(TimeDelta);
}

_int CUI_Button_Class::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;

	return CUI_Button::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Button_Class::Render_GameObject()
{
	return CUI_Button::Render_GameObject();
}

HRESULT CUI_Button_Class::OnKeyDown(_int KeyCode)
{
	if (!m_bActive)
		return S_OK;

	if (KeyCode == VK_LBUTTON)
	{
		POINT point = {};
		GetCursorPos(&point);
		ScreenToClient(g_hWnd, &point);

		_float2 vCursorPos = _float2((_float)point.x, (_float)point.y);
		_float2 vProjSize = m_StateDesc.vProjSize * 0.5f;

		if (m_StateDesc.vProjPos.x + vProjSize.x > vCursorPos.x && m_StateDesc.vProjPos.x - vProjSize.x < vCursorPos.x
			&& m_StateDesc.vProjPos.y + vProjSize.y > vCursorPos.y && m_StateDesc.vProjPos.y - vProjSize.y < vCursorPos.y)
		{
			if (m_eOccupation != CInfantry::OCC_END)
			{
				CGameManager::Get_Instance()->Set_CurrentOccupation(m_eOccupation);
				CInputManager::Get_Instance()->Set_Mode(CInputManager::MODE_SPAWN);
			}
		}
	}
	return S_OK;
}

HRESULT CUI_Button_Class::OnKeyPressing(_int KeyCode)
{
	return S_OK;
}

HRESULT CUI_Button_Class::OnKeyUp(_int KeyCode)
{
	return S_OK;
}

CUI_Button_Class * CUI_Button_Class::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Button_Class* pInstance = new CUI_Button_Class(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_Button_Class");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Button_Class::Clone_GameObject(void * pArg)
{
	CUI_Button_Class* pInstance = new CUI_Button_Class(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_Button_Class");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Button_Class::Free()
{
	CUI_Button::Free();
}

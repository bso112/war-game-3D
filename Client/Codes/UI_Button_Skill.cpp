#include "stdafx.h"
#include "..\Headers\UI_Button_Skill.h"
#include "Management.h"
#include "KeyMgr.h"
#include "GameManager.h"
#include "UI_Manager.h"
#include "InputManager.h"

USING(Client)

CUI_Button_Skill::CUI_Button_Skill(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Button(pGraphic_Device)
{
}

CUI_Button_Skill::CUI_Button_Skill(const CUI_Button_Skill & rhs)
	: CUI_Button(rhs)
{
}

HRESULT CUI_Button_Skill::Ready_GameObject_Prototype()
{
	return CUI_Button::Ready_GameObject_Prototype();
}

HRESULT CUI_Button_Skill::Ready_GameObject(void * pArg)
{
	return CUI_Button::Ready_GameObject(pArg);
}

_int CUI_Button_Skill::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;

	if (FAILED(Check_In_Mouse()))
		return E_FAIL;

	return CUI_Button::Update_GameObject(TimeDelta);
}

_int CUI_Button_Skill::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return NOERROR;

	return CUI_Button::Late_Update_GameObject(TimeDelta);
}

HRESULT CUI_Button_Skill::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(12);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CUI_Button_Skill::SetUp_ConstantTable()
{
	CUI_Button::SetUp_ConstantTable();

	_float Cool = (_float)CGameManager::Get_Instance()->Get_CoolPercent(m_eSkill);

	m_pShaderCom->Set_Value("g_CoolDown", &Cool, sizeof(_float));
	m_pShaderCom->Set_Value("g_CenterPos", &m_pTransformCom->Get_Position(), sizeof(_float3));

	return S_OK;
}

//HRESULT CUI_Button_Skill::OnKeyDown(_int KeyCode)
//{
//	if (!m_bActive)
//		return S_OK;
//
//	if (VK_LBUTTON == KeyCode)
//	{
//		POINT point = {};
//		GetCursorPos(&point);
//		ScreenToClient(g_hWnd, &point);
//
//		_float2 vCursorPos = _float2((_float)point.x, (_float)point.y);
//		_float2 vProjSize = m_StateDesc.vProjSize * 0.5f;
//
//		if (m_StateDesc.vProjPos.x + vProjSize.x > vCursorPos.x && m_StateDesc.vProjPos.x - vProjSize.x < vCursorPos.x
//			&& m_StateDesc.vProjPos.y + vProjSize.y > vCursorPos.y && m_StateDesc.vProjPos.y - vProjSize.y < vCursorPos.y)
//		{
//			CCamera_Manager::Get_Instance()->Change_Camera_Skill(m_eSkill);
//		}
//	}
//	return S_OK;
//}

CUI_Button_Skill * CUI_Button_Skill::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Button_Skill* pInstance = new CUI_Button_Skill(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CUI_Button_Skill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Button_Skill::Clone_GameObject(void * pArg)
{
	CUI_Button_Skill* pInstance = new CUI_Button_Skill(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CUI_Button_Skill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Button_Skill::Free()
{
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);
	CUI_Button::Free();
}

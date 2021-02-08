#include "stdafx.h"
#include "..\Headers\UI_Manager.h"
#include "Management.h"
#include "GameManager.h"
#include "Camera_Manager.h"

USING(Client)
IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{

}

HRESULT CUI_Manager::Change_UI_State(UI_STATE _eState)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	CMyFont* pFont = Find_Font(L"Font_CamMode");
	if (nullptr == pFont)
	{
		Safe_Release(pManagement);
		return -1;
	}

	list<CGameObject*>* pListUI = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_UI");
	list<CGameObject*>* pListFree = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_UI_Free");
	list<CGameObject*>* pListCommand = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_UI_Command");
	list<CGameObject*>* pListBattle = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_UI_Battle");

	_tchar szBuf[MAX_PATH] = L"";

	switch (_eState)
	{
	case Client::CUI_Manager::UI_FREE:
		for (CGameObject* pGameObject : *pListUI)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListFree)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListCommand)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListBattle)
			pGameObject->Set_Active(false);

		wsprintf(szBuf, L"FREE_MODE");
		pFont->Set_Text(szBuf);
		break;

	case Client::CUI_Manager::UI_COMMAND:
		for (CGameObject* pGameObject : *pListUI)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListFree)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListCommand)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListBattle)
			pGameObject->Set_Active(false);

		wsprintf(szBuf, L"COMMAND_MODE");
		pFont->Set_Text(szBuf);
		break;

	case Client::CUI_Manager::UI_BATTLE:
		for (CGameObject* pGameObject : *pListUI)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListFree)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListCommand)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListBattle)
			pGameObject->Set_Active(true);

		wsprintf(szBuf, L"BATTLE_MODE");
		pFont->Set_Text(szBuf);
		break;

	case Client::CUI_Manager::UI_SKILL:
		for (CGameObject* pGameObject : *pListUI)
			pGameObject->Set_Active(true);

		for (CGameObject* pGameObject : *pListFree)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListCommand)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListBattle)
			pGameObject->Set_Active(true);

		wsprintf(szBuf, L"SKILL_MODE");
		pFont->Set_Text(szBuf);
		break;

	case Client::CUI_Manager::UI_NONE:
		for (CGameObject* pGameObject : *pListUI)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListFree)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListCommand)
			pGameObject->Set_Active(false);

		for (CGameObject* pGameObject : *pListBattle)
			pGameObject->Set_Active(false);

		wsprintf(szBuf, L"NONE_MODE");
		pFont->Set_Text(szBuf);
		break;
	default:
		break;
	}

	Safe_Release(pManagement);
	return S_OK;
}

_int CUI_Manager::Update_UI_Manager()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	CMyFont* pFont = Find_Font(L"Font_Resource");
	if (nullptr == pFont)
	{
		Safe_Release(pManagement);
		return -1;
	}
	_tchar szBuf[MAX_PATH] = L"0";

	CGameManager* pGameManager = CGameManager::Get_Instance();
	if (nullptr == pGameManager)
		return -1;

	Safe_AddRef(pGameManager);

#pragma region RESOURCE
	pFont = Find_Font(L"Font_Resource");
	swprintf_s(szBuf, L"%0.f", pGameManager->Get_Gold());
	pFont->Set_Text(szBuf);
#pragma endregion

#pragma region COUNT
	list<CGameObject*>* pLayer = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");
	if (nullptr == pLayer)
	{
		Safe_Release(pGameManager);
		Safe_Release(pManagement);
		return -1;
	}

	int WarriorCount = 0, ArcherCount = 0, MageCount = 0;

	for (CGameObject* pInfantry : *pLayer)
	{
		switch (((CInfantry*)pInfantry)->Get_OCC())
		{
		case CInfantry::OCC_WARRIOR:
			++WarriorCount;
			break;
		case CInfantry::OCC_ARCHER:
			++ArcherCount;
			break;
		case CInfantry::OCC_MAGICIAN:
			++MageCount;
			break;
		}
	}

	pGameManager->Set_WarriorCount(WarriorCount);
	pGameManager->Set_ArcherCount(ArcherCount);
	pGameManager->Set_MageCount(MageCount);

	wsprintf(szBuf, L"%d", WarriorCount);
	Find_Font(L"Font_WarriorCount")->Set_Text(szBuf);

	wsprintf(szBuf, L"%d", ArcherCount);
	Find_Font(L"Font_ArcherCount")->Set_Text(szBuf);

	wsprintf(szBuf, L"%d", MageCount);
	Find_Font(L"Font_MageCount")->Set_Text(szBuf);
#pragma endregion

	Safe_Release(pGameManager);
	Safe_Release(pManagement);
	return _int();
}

HRESULT CUI_Manager::Add_Button(CUI_Button * _pUI)
{
	if (nullptr == _pUI)
		return E_FAIL;

	m_pButtons.emplace_back(_pUI);
	Safe_AddRef(_pUI);
	return S_OK;
}

_bool CUI_Manager::Check_In_Mouse_Button()
{
	for (CUI_Button* pButton : m_pButtons)
		if (pButton->Check_In_Mouse())
			return true;
	return false;
}

HRESULT CUI_Manager::Add_Font(const _tchar * _pFontTag, CMyFont* _pFont)
{
	if (nullptr == _pFontTag || nullptr != Find_Font(_pFontTag))
		return E_FAIL;

	m_pFonts.emplace(_pFontTag, _pFont);
	return S_OK;
}

CMyFont * CUI_Manager::Find_Font(const _tchar * _pFontTag)
{
	if (nullptr == _pFontTag)
		return nullptr;

	FONTS::iterator iter = find_if(m_pFonts.begin(), m_pFonts.end(), CFinder_Tag(_pFontTag));
	if (m_pFonts.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CUI_Manager::Set_Font(const _tchar * _pFontTag, const _tchar * _pText)
{
	if (nullptr == _pFontTag || nullptr == _pText)
		return E_FAIL;

	CMyFont* pFont = Find_Font(_pFontTag);
	if (pFont == nullptr)
		return E_FAIL;

	pFont->Set_Text(_pText);
	return S_OK;
}

void CUI_Manager::Free()
{
	for (CUI_Button* pButton : m_pButtons)
		Safe_Release(pButton);

	m_pButtons.clear();
}

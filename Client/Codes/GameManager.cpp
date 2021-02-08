#include "stdafx.h"
#include "GameManager.h"
#include "PickingMgr.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
	: m_pCameraManager(CCamera_Manager::Get_Instance())
	, m_pUIManager(CUI_Manager::Get_Instance())
{
	Safe_AddRef(m_pCameraManager);
	Safe_AddRef(m_pUIManager);
}

_bool CGameManager::Use_Gold(_uint _Use)
{
	if (0 <= m_fGold - _Use)
	{
		CSoundMgr::Get_Instance()->PlaySound_Overwrite(L"coins_dropped_1.mp3", CSoundMgr::CHANNELID::UI);
		m_fGold -= _Use;
		return true;
	}
	else
		return false;
}
HRESULT CGameManager::Ready_GameManager()
{
	m_fGoldPerSec = 2.f;

	m_CoolDown[SKILL_HEAL] = 20;
	m_CoolDown[SKILL_METEOR] = 10;
	m_CoolDown[SKILL_SNIPE] = 5;

	for (int i = 0; i < SKILL_END; ++i)
	{
		m_bSkillReady[i] = false;
		m_CurrentCoolDown[i] = 0;
	}

	return S_OK;
}

_int CGameManager::Update_GameManager(_double _DeltaTime)
{
	Increase_Gold(_DeltaTime);

	if (0 > m_pUIManager->Update_UI_Manager())
		return -1;

	if (FAILED(Update_CoolDown(_DeltaTime)))
		return -1;

	CPickingMgr::Get_Instance()->Update_Blur_Center();

	return S_OK;
}

_int CGameManager::Late_Update_GameManager(_double _DeltaTime)
{
	return S_OK;
}

HRESULT CGameManager::Render_GameManager()
{
	return S_OK;
}

HRESULT CGameManager::Update_CoolDown(_double _DeltaTime)
{
	for (int i = 0; i < SKILL_END; ++i)
	{
		m_CurrentCoolDown[i] += _DeltaTime;
		if (m_CoolDown[i] <= m_CurrentCoolDown[i])
		{
			m_CurrentCoolDown[i] = m_CoolDown[i];
			m_bSkillReady[i] = true;
		}
		else
			m_bSkillReady[i] = false;
	}
	return S_OK;
}

void CGameManager::Free()
{
	Safe_Release(m_pUIManager);
	Safe_Release(m_pCameraManager);
}


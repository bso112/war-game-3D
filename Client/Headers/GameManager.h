#pragma once
#include "Infantry.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"

BEGIN(Client)

class CGameManager :public CBase
{
	DECLARE_SINGLETON(CGameManager)
private:
	CCamera_Manager*			m_pCameraManager = nullptr;
	CUI_Manager*				m_pUIManager = nullptr;
	
private:
	CInfantry::OCCUPATION		m_eSelectedOccupation = CInfantry::OCC_WARRIOR;
	SKILL						m_eCurrSkill = SKILL_END;
	_float						m_fSkillRange = 0.f;

	_bool						m_bSkillReady[SKILL_END];
	_double					m_CoolDown[SKILL_END];
	_double					m_CurrentCoolDown[SKILL_END];

private:
	_int		m_WarriorCount = 0, m_ArcherCount = 0, m_MageCount = 0;
	_float		m_fGold = 0;
	_float		m_fGoldPerSec = 0.f;

private:
	explicit CGameManager();
	virtual ~CGameManager() = default;

public:
	CInfantry::OCCUPATION	Get_CurrentOccupation() { return m_eSelectedOccupation; }
	SKILL Get_CurrSkill() { return m_eCurrSkill; }
	_float Get_SkillRange() { return m_fSkillRange; }

	_bool Get_SkillReady(SKILL _eSkill) { return m_bSkillReady[_eSkill]; }
	_double Get_CoolDown(SKILL _eSkill) { return m_CoolDown[_eSkill]; }
	_double Get_CurrCoolDown(SKILL _eSkill) { return m_CurrentCoolDown[_eSkill]; }
	_double Get_CoolPercent(SKILL _eSkill) { return m_CurrentCoolDown[_eSkill] / m_CoolDown[_eSkill]; }

	_int Get_WarriorCount() { return m_WarriorCount; }
	_int Get_ArcherCount() { return m_ArcherCount; }
	_int Get_MageCount() { return m_MageCount; }

	_float Get_Gold() { return m_fGold; }
	_float Get_GoldPerSec() { return m_fGoldPerSec; }

public:
	void Set_CurrentOccupation(CInfantry::OCCUPATION _eType) { m_eSelectedOccupation = _eType; }
	void Set_Skill(SKILL _eType) { m_eCurrSkill = _eType; }
	void Set_SkillRange(_float _Range) { m_fSkillRange = _Range; }
	void Use_Skill(SKILL _eSkill) { m_CurrentCoolDown[_eSkill] = 0; m_bSkillReady[_eSkill] = false; }

	void Set_WarriorCount(_int _Count) { m_WarriorCount = _Count; }
	void Set_ArcherCount(_int _Count) { m_ArcherCount = _Count; }
	void Set_MageCount(_int _Count) { m_MageCount = _Count; }

	void Set_Gold(_float _iGold) { m_fGold = _iGold; }
	void Set_GoldPerSec(_float _fGoldPerSec) { m_fGoldPerSec = _fGoldPerSec; }


public:
	void Alter_Gold(_float _Alter) { m_fGold += _Alter; }
	void Alter_GoldPerSec(_float _Alter) { m_fGoldPerSec += _Alter; }
	void Increase_Gold(_double _DeltaTime) { m_fGold += m_fGoldPerSec * (_float)_DeltaTime; }
	_bool Use_Gold(_uint _Use);

public:
	HRESULT Ready_GameManager();
	_int Update_GameManager(_double _DeltaTime);
	_int Late_Update_GameManager(_double _DeltaTime);
	HRESULT Render_GameManager();

public:
	HRESULT Update_CoolDown(_double _DeltaTime);

public:
	virtual void Free() override;
};

END
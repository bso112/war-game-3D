#pragma once
#include "UI_Button.h"

class CUI_Button_Skill final : public CUI_Button
{
private:
	SKILL			m_eSkill;
	_float			m_fCoolDown = 0.f;

private:
	explicit CUI_Button_Skill(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Button_Skill(const CUI_Button_Skill& rhs);
	virtual ~CUI_Button_Skill() = default;

public:
	SKILL Get_Skill() { return m_eSkill; }

public:
	void Set_Skill(SKILL _eSkill) { m_eSkill = _eSkill; }

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

public:
	virtual HRESULT SetUp_ConstantTable() override;

public:
	static CUI_Button_Skill * Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};


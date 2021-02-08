#pragma once
#include "UI_Button.h"
#include "Infantry.h"

class CUI_Button_Class final : public CUI_Button
{
private:
	CInfantry::OCCUPATION	m_eOccupation = CInfantry::OCC_END;

private:
	explicit CUI_Button_Class(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Button_Class(const CUI_Button_Class& rhs);
	virtual ~CUI_Button_Class() = default;

public:
	CInfantry::OCCUPATION Get_Occupation() { return m_eOccupation; }

public:
	void Set_Occupation(CInfantry::OCCUPATION _eOccu) { m_eOccupation = _eOccu; }

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

public:
	virtual HRESULT	OnKeyDown(_int KeyCode) override;
	virtual HRESULT OnKeyPressing(_int KeyCode) override;
	virtual HRESULT	OnKeyUp(_int KeyCode) override;

public:
	static CUI_Button_Class * Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};


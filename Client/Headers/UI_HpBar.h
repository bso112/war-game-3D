#pragma once
#include "UI.h"

BEGIN(Client)

class CUnit;
class CUI_HpBar final : public CUI
{
private:
	CUnit**		m_ppUnit = nullptr;
	_float		m_fPercent = 0.f;

private:
	explicit CUI_HpBar(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_HpBar(const CUI_HpBar& rhs);
	virtual ~CUI_HpBar() = default;

public:
	void Set_UnitPointer(CUnit** _ppUnit) { m_ppUnit = _ppUnit; }

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

private:
	virtual HRESULT Add_Component() override;
	virtual HRESULT SetUp_ConstantTable() override;

public:
	static CUI_HpBar* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};

END
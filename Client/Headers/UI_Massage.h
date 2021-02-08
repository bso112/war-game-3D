#pragma once
#include "UI.h"

class CUI_Massage final : public CUI
{
private:
	_double		m_dLifeTime = 3;
	_double		m_dCurrentLifeTime = 0;

private:
	explicit CUI_Massage(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Massage(const CUI_Massage& rhs);
	virtual ~CUI_Massage() = default;

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
	static CUI_Massage* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};


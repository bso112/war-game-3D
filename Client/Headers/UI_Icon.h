#pragma once
#include "UI.h"

class CUI_Icon final : public CUI
{
private:
	explicit CUI_Icon(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Icon(const CUI_Icon& rhs);
	virtual ~CUI_Icon() = default;

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
	static CUI_Icon* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};


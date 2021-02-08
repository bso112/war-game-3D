#pragma once
#include "UI.h"

class CUI_Frame final : public CUI
{
private:
	explicit CUI_Frame(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Frame(const CUI_Frame& rhs);
	virtual ~CUI_Frame() = default;

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
	static CUI_Frame* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};


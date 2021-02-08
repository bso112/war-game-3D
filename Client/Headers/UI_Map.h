#pragma once
#include "UI.h"

class CUI_Map final : public CUI
{
private:
	explicit CUI_Map(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Map(const CUI_Map& rhs);
	virtual ~CUI_Map() = default;

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
	static CUI_Map* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};


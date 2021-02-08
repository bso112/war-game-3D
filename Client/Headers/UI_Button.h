#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Button abstract : public CUI
{
protected:
	explicit CUI_Button(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI_Button(const CUI_Button& rhs);
	virtual ~CUI_Button() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

public:
	_bool Check_In_Mouse();

protected:
	virtual HRESULT Add_Component() override;
	virtual HRESULT SetUp_ConstantTable() override;

public:
	virtual void Free() override;
};

END
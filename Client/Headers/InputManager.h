#pragma once

#include "Base.h"
BEGIN(Client)
class CUnit;
//사용자의 인풋을 처리한다. 
class CInputManager : public CBase
{
	DECLARE_SINGLETON(CInputManager)

public:
	enum PickingMode { MODE_SPAWN ,MODE_SKILL, MODE_END};
private:
	explicit CInputManager();
	~CInputManager() = default;


public:
	virtual HRESULT	OnKeyDown(_int KeyCode);
	virtual HRESULT OnKeyPressing(_int KeyCode);
	virtual HRESULT	OnKeyUp(_int KeyCode);

public:
	void Clear_DeadObject();
	void Set_Mode(PickingMode _eMode) { m_eMode = _eMode; }
private:
	HRESULT	Pick_Object(POINT& pt);
	HRESULT Spawn_Object(_uint _eType);
	_bool	Can_Spawn_Object();
public:
	CUnit* Get_PickedUnit() { return m_pPicked; }
	CUnit** Get_PickedPointer() { return &m_pPicked; }

	
private:
	CUnit*			m_pPicked = nullptr; 
	PickingMode		m_eMode = MODE_END;
public:
	virtual void Free() override;
};
END

#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;
private:
	unsigned long	m_dwRefCnt = 0; // 레퍼런스카운트
public:
	virtual unsigned long AddRef(); // 레퍼런스카운트를 증가시킨다.
	unsigned long Release(); // 레퍼런스카운트를 감소시킨다(레카가 0이 아닐때). or 삭제한다.(레카가 0일때)
	unsigned long Release_Array();
public:
	virtual void Free() = 0;

public:
	//인풋을 받는 하위클래스에서 정의함
	virtual HRESULT	OnKeyDown(_int KeyCode);
	virtual HRESULT OnKeyPressing(_int KeyCode);
	virtual HRESULT	OnKeyUp(_int KeyCode);


};

END
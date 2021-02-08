#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;
private:
	unsigned long	m_dwRefCnt = 0; // ���۷���ī��Ʈ
public:
	virtual unsigned long AddRef(); // ���۷���ī��Ʈ�� ������Ų��.
	unsigned long Release(); // ���۷���ī��Ʈ�� ���ҽ�Ų��(��ī�� 0�� �ƴҶ�). or �����Ѵ�.(��ī�� 0�϶�)
	unsigned long Release_Array();
public:
	virtual void Free() = 0;

public:
	//��ǲ�� �޴� ����Ŭ�������� ������
	virtual HRESULT	OnKeyDown(_int KeyCode);
	virtual HRESULT OnKeyPressing(_int KeyCode);
	virtual HRESULT	OnKeyUp(_int KeyCode);


};

END
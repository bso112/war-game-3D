#pragma once
#include "Base.h"

BEGIN(Engine)

#define VK_MAX 0xff

/*
���� ���⼭ �������� ���� ���۷��� ī��Ʈ�� ���� 
�������� UnRegister�ϴ� ������ �ָ�������. Free()���� �ϸ�
UnRegisterObserver�� �Ҹ��װ�, UnRegisterObserver�ȿ����� Safe_Release(Observer)�� �ϹǷ� �� Free()�� �θ���.
��, ���ÿ����÷ο찡 ����.
*/
class ENGINE_DLL CKeyMgr : public CBase
{
	DECLARE_SINGLETON(CKeyMgr)

private:
	explicit CKeyMgr();
	virtual	 ~CKeyMgr() = default;

public:
	bool Key_Pressing(int _Key);
	bool Key_Down(int _Key);
	bool Key_Up(int _Key);
	void Key_Update();

public:
	HRESULT RegisterObserver(CBase* _pObserver);
	HRESULT UnRegisterObserver(CBase* _pObserver);
	HRESULT	ClearObservers();
private:
	list<CBase*> m_listObservers;



private:
	bool				m_bKeyState[VK_MAX];

	// CBase��(��) ���� ��ӵ�
	virtual void Free() override;
};

END
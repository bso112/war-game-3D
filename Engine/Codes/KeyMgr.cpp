#include "..\Headers\KeyMgr.h"


USING(Engine)

IMPLEMENT_SINGLETON(CKeyMgr)

CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));

}


bool CKeyMgr::Key_Pressing(int _Key)
{
	if ((GetAsyncKeyState(_Key) & 0x8000))
	{
		for (auto& observer : m_listObservers)
		{
			observer->OnKeyPressing(_Key);
		}

		return true;
	}
	return false;

}

bool CKeyMgr::Key_Down(int _Key)
{
	if (!m_bKeyState[_Key] && (GetAsyncKeyState(_Key) & 0x8000))
	{
		m_bKeyState[_Key] = !m_bKeyState[_Key];

		for (auto& observer : m_listObservers)
		{
			observer->OnKeyDown(_Key);
		}
		return true;
	}
	return false;
}

bool CKeyMgr::Key_Up(int _Key)
{
	if (m_bKeyState[_Key] && !(GetAsyncKeyState(_Key) & 0x8000))
	{
		m_bKeyState[_Key] = !m_bKeyState[_Key];

		for (auto& observer : m_listObservers)
		{
			observer->OnKeyUp(_Key);
		}
		return true;
	}
	return false;
}

void CKeyMgr::Key_Update()
{
	// Key 상태 복원 함수
	for (int i = 0; i < VK_MAX; ++i)
	{
		if (m_bKeyState[i] && !(GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];

		if (!m_bKeyState[i] && (GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];
	}
}



HRESULT CKeyMgr::RegisterObserver(CBase * _pObserver)
{

	if (nullptr == _pObserver)
		return E_FAIL;

	m_listObservers.push_back(_pObserver);

	return S_OK;
}

HRESULT CKeyMgr::UnRegisterObserver(CBase * _pObserver)
{
	if (nullptr == _pObserver)
		return E_FAIL;

	auto& iter = m_listObservers.begin();
	for (;iter != m_listObservers.end(); ++iter)
	{
		if (*iter == _pObserver)
		{
			m_listObservers.erase(iter);
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CKeyMgr::ClearObservers()
{
	m_listObservers.clear();
	return S_OK;
}



void CKeyMgr::Free()
{
	m_listObservers.clear();

}

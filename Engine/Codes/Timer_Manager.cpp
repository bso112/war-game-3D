#include "..\Headers\Timer_Manager.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{

}

HRESULT CTimer_Manager::Add_Timers(const _tchar * pTimerTag)
{
	if (nullptr != Find_Timers(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.insert(TIMERS::value_type(pTimerTag, pTimer));

	return S_OK;
}

_double CTimer_Manager::Compute_TimeDelta(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timers(pTimerTag);	
	if (nullptr == pTimer)
		return E_FAIL;

	return pTimer->Compute_TimeDelta();	
}

_double CTimer_Manager::Get_TimeDelta(const _tchar * pTimerTag)
{
	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), CFinder_Tag(pTimerTag));
	if (iter == m_Timers.end())
	{
		MSG_BOX("없는 타이머입니다");
		return 0.0;
	}

	return iter->second->Get_TimeDelta();
}

CTimer * CTimer_Manager::Find_Timers(const _tchar * pTimerTag)
{
	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), CFinder_Tag(pTimerTag));
	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}

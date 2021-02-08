#pragma once
#include "Base.h"

BEGIN(Client)

class CResourceManager final : public CBase
{
	DECLARE_SINGLETON(CResourceManager)

private:
	_int		m_iGold = 0;

private:
	explicit CResourceManager();
	virtual ~CResourceManager() = default;

public:
	_int Get_Gold() { return m_iGold; }

public:
	void Set_Gold(_int _iGold) { m_iGold = _iGold; }

public:
	void Alter_Gold(_int _Alter) { m_iGold += _Alter; }

public:
	virtual void Free() override;
};

END
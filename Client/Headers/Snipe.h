#pragma once
#include "GameObject.h"

BEGIN(Client)

class CSnipe final : public CBase
{
private:
	_int		m_iBulletCount;

private:
	explicit CSnipe( );
	virtual ~CSnipe() = default;

public:
	static CSnipe* Create();
	virtual void Free() override;
};

END
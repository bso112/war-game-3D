#pragma once

// 광원들을 보관한다.
#include "Base.h"

BEGIN(Engine)

class CLight;
class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;
public:
	const D3DLIGHT9* Get_LightDesc(_uint iIndex);
public:
	HRESULT Add_Light(PDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightDesc);
private:
	list<CLight*>			m_LightList;
	typedef list<CLight*>	LIGHTLIST;
public:
	virtual void Free();
};

END
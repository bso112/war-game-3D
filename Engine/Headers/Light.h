#pragma once

// 빛정보를 보관하기위한 객체.
#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	explicit CLight(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLight() = default;
public:
	const D3DLIGHT9* Get_LightDesc() const {
		return &m_LightDesc; }
public:
	HRESULT Ready_Light(const D3DLIGHT9& LightDesc);
private:
	PDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	D3DLIGHT9			m_LightDesc;
public:
	static CLight* Create(PDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightDesc);
	virtual void Free();
};

END
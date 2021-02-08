#pragma once

// 렌더타겟 하나를 보관하는 기능.
#include "Base.h"

BEGIN(Engine)
class CVIBuffer_ViewPort;
class CShader;
class CTexture;

class CTarget final : public CBase
{
private:
	explicit CTarget(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CTarget() = default;
public:
	HRESULT Ready_Target(_uint iSizeX, _uint iSizeY, D3DFORMAT eFormat);
	HRESULT SetUp_OnGraphicDev(_uint iIndex);
	HRESULT Release_OnGraphicDev(_uint iIndex);
	HRESULT Clear_Target();

public:
	LPDIRECT3DTEXTURE9	Get_Texture() { return m_pRawTexture; }
private:
	PDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	LPDIRECT3DTEXTURE9			m_pRawTexture = nullptr;
	LPDIRECT3DSURFACE9			m_pOldSurface = nullptr;
	LPDIRECT3DSURFACE9			m_pSurface = nullptr;

public:
	static CTarget* Create(PDIRECT3DDEVICE9 pGraphic_Device, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat);
	virtual void Free();
};

END
#pragma once

// 렌더 타겟들을 여러개 생성하여 보관한다.
#include "Target.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
public:
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager() = default;
public:
	HRESULT SetUp_OnGraphicDev(const _tchar* pTargetTag, _uint iIndex);
	HRESULT Release_OnGraphicDev(const _tchar* pTargetTag, _uint iIndex);
public:
	HRESULT Add_RenderTarget(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat);
public:
	LPDIRECT3DTEXTURE9 Get_Texture(const _tchar* pTargetTag);
	
private:
	map<const _tchar*, CTarget*>			m_Targets;
	typedef map<const _tchar*, CTarget*>	TARGETS;
private:
	CTarget* Find_RenderTarget(const _tchar* pTargetTag);
public:
	virtual void Free();
};

END
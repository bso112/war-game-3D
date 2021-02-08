#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCVIBuffer_Aim final : public CVIBuffer
{
private:
	explicit CCVIBuffer_Aim(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCVIBuffer_Aim(const CCVIBuffer_Aim& rhs);
	virtual ~CCVIBuffer_Aim() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
public:
	void PushBack(_float3 _vValue) { m_vectorAimPos.push_back(_vValue); }
	void ClearVector() { m_vectorAimPos.clear(); }
	void Update_VBuffer();
	_int Get_m_iVCnt() { return m_iVCnt; }
public:
	static CCVIBuffer_Aim* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
private:
	_int			m_iSegment = 8;
	_int			m_iVCnt = 30;
	_float			m_fRadius = 0.5f;
	_bool			m_bUVDefined = 0;
	vector<_float3>m_vectorAimPos;
};

END
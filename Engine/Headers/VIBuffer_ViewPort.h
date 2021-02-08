#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_ViewPort final : public CComponent
{
protected:
	explicit CVIBuffer_ViewPort(PDIRECT3DDEVICE9 _pGraphic_Device) : CComponent(_pGraphic_Device) {};
	explicit CVIBuffer_ViewPort(CVIBuffer_ViewPort& _rhs);
	virtual ~CVIBuffer_ViewPort() = default;

private:
	//정점들의 초기 위치(상수)
	_float4	m_vBasePos[4];
private:
	PDIRECT3DVERTEXBUFFER9	m_pVBuffer = nullptr;
	PDIRECT3DINDEXBUFFER9	m_pIBuffer = nullptr;
public:
	HRESULT Set_Transform(_matrix _matrix);
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	
public:
	HRESULT	Render();
public:
	static CVIBuffer_ViewPort* Create(PDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CComponent* Clone_Component(void * _pArg = nullptr) override;
	virtual void Free();



};
END

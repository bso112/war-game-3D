#pragma once
#include "Camera.h"

BEGIN(Client)

class CCamera_Command final : public CCamera
{
private:
	CCamera::CAM_TYPE		m_eNextType = CCamera::CAM_END;

	_float3			m_vSrcPosition;
	_float3			m_vDestPosition;
	_float3			m_vSrcLook;
	_float3			m_vDestLook;
	_float			m_fLerpTime = 0.f;
	_float			m_fT = 0.f;


private:
	explicit CCamera_Command(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Command(const CCamera_Command& rhs);
	virtual ~CCamera_Command() = default;

public:
	void Set_NextType(CCamera::CAM_TYPE _eNextType) { m_eNextType = _eNextType; }

public:
	void Set_LerpPosition(_float3 _vSrc, _float3 _vDest, _float3 _vDestLook, _float _LerpTime);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	HRESULT Lerp_Camera(_double _DeltaTime);

public:
	static CCamera_Command* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
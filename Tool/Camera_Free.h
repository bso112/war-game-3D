#pragma once
#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Tool)

class CCamera_Free final : public CCamera
{
private:
	_float3					m_vSrcLook;
	_float3					m_vDstLook;
	_bool					m_bLerp = false;
	_float					m_fLerpTime = 0.f;
	_float					m_fT = 0.f;

	_float3					m_prevPosition;
	_float3					m_prevLook;

private:
	explicit CCamera_Free(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	_float3 Get_CameraPosition() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	void Set_Lerp(_float3 _vSrcPos, _float3 _vDstPos, _float3 _vSrcLook, _float3 _vDstLook, _float _fLerpTime);
	void Set_PrevPosition(_float3 _prevPos) { m_prevPosition = _prevPos; }
	void Set_PrevLook(_float3 _prevLook) { m_prevLook = _prevLook; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	HRESULT Lerp_Camera(_double _DeltaTime);
	HRESULT Free_Move_Camera(_double _DeltaTime);

public:
	static CCamera_Free* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

END
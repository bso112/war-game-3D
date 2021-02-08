#pragma once
#include "Camera.h"

BEGIN(Client)

class CCamera_Third_Person final : public CCamera
{
private:
	_float3					m_vSrcLook;
	_bool					m_bLerp = false;
	_float					m_fLerpTime = 0.f;
	_float					m_fT = 0.f;

private:
	_float					m_fTargetDistance = 0.f;
	_float3					m_vOffSet;

private:
	explicit CCamera_Third_Person(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Third_Person(const CCamera_Third_Person& rhs);
	virtual ~CCamera_Third_Person() = default;

public:
	void Set_Lerp(_float3 _vSrcLook, _float _fLerpTime);
	void Set_TargetDistance(_float _Distance) { m_fTargetDistance = _Distance; }
	void Set_OffSet(_float3 _OffSet) { m_vOffSet = _OffSet; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual HRESULT	OnKeyDown(_int KeyCode) override;
	virtual HRESULT	Exit_Camera() override;
private:
	HRESULT Lerp_Target(_double _DeltaTime);
	HRESULT Look_Target();

public:
	static CCamera_Third_Person* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
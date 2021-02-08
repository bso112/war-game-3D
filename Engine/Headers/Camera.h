#pragma once

#include "GameObject.h"
#include "Transform.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum CAM_TYPE { CAM_FREE, CAM_ROTATE, CAM_THIRDPERSON, CAM_WAYPOINT, CAM_LERP, CAM_END };

public:
	typedef struct tagStateDesc
	{
		// For.View
		_float3		vEye;
		_float3		vAt;
		_float3		vAxisY;

		// For.Projection
		_float		fFovy;
		_float		fAspect;
		_float		fNear;
		_float		fFar;

		// For.Transform
		CTransform::STATEDESC	TransformDesc;

		// For. Status
		_float3		vSrc;
		_float3		vDst;

	}STATEDESC;

protected:
	CPipeLine*		m_pPipeLine = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CGameObject*	m_pTarget = nullptr;

protected:
	STATEDESC		m_StateDesc;
	_bool				m_bLock = false;

protected:
	explicit CCamera(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	_bool Get_Active() { return m_bActive; }
	_float3 Get_vSrc() { return m_StateDesc.vSrc; }
	_float3 Get_vDst() { return m_StateDesc.vDst; }
	_float Get_fFOV() { return m_StateDesc.fFovy; }
	CGameObject* Get_GameObject() { return m_pTarget; }
	CTransform* Get_Transform() const { return m_pTransformCom; }
	_bool Get_isLock() { return m_bLock; }

public:
	void Set_Target(CGameObject* _pGameObject) { m_pTarget = _pGameObject; }
	void Set_Active(_bool _Active) { m_bActive = _Active; }
	void Set_Src(_float3 _vSrc) { m_StateDesc.vSrc = _vSrc; }
	void Set_Dst(_float3 _vDst) { m_StateDesc.vDst = _vDst; }
	void Set_isLock(_bool _Lock) { m_bLock = _Lock; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual HRESULT Enter_Camera();
	virtual HRESULT Exit_Camera();

public:
	virtual CGameObject* Clone_GameObject(void* pArg) = 0;
	virtual void Free();
};

END
#pragma once
#include "Camera_Free.h"
#include "Camera_Skill.h"
#include "Camera_Command.h"
#include "Camera_Third_Person.h"
#include "Camera_WayPoint.h"

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
public:
	enum CAM_STATE { STATE_FREE, STATE_COMMAND, STATE_SKILL, STATE_BATTLE, STATE_WAYPOINT, STATE_END	};

private:
	CCamera*					m_pCameras[CCamera::CAM_END];
	CCamera*					m_pCurrentCamera = nullptr;
	CAM_STATE				m_eCamState = STATE_END;

	CCamera::CAM_TYPE	m_eCurrentType = CCamera::CAM_FREE;
	CCamera::CAM_TYPE	m_ePrevType = CCamera::CAM_END;

	_float3						m_FreePrevPosition;
	_float3						m_FreePrevLook;

public:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager();

public:
	CAM_STATE Get_CamState() { return m_eCamState; }
	CCamera* Get_CurrentCamera() { return m_pCurrentCamera; }

public:
	void Set_CamState(CAM_STATE _eState) { m_eCamState = _eState; }
	void Set_CurrentType(CCamera::CAM_TYPE _eType) { m_eCurrentType = _eType; }
	void Set_CurrentCamera(CCamera* _pCamera) { m_pCurrentCamera = _pCamera; }

public:
	HRESULT Add_Camera(CCamera::CAM_TYPE _eType, CCamera* _pCamera);
	HRESULT Resume_Camera();
	HRESULT Change_CamState(CAM_STATE _eState);
	HRESULT Change_Camera_Free(_bool _Lerp, CAM_STATE _eState, _float3 _vPosition, _float3 _vLook);
	HRESULT Change_Camera_Third_Person(CGameObject * _pGameObject, _float _ChangeTime);
	HRESULT Change_Camera_Skill(CGameObject* _pGameObject, SKILL _eSkill);
	HRESULT Change_Camera_WayPoint(_float3 _vStartLook, vector<CCamera_WayPoint::WAYPOINT> _vecWayPoint);

public:
	virtual void Free() override;
};


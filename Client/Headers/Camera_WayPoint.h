#pragma once
#include "Camera.h"

BEGIN(Client)

class CCamera_WayPoint final : public CCamera
{
public:
	typedef struct tagWayPoint
	{
		_float3			vWayPoint;
		_float3			vLook;
		_float			fTime;

		tagWayPoint() {};
		tagWayPoint(_float3 _vWayPoint, _float3 _vLook, _float _fTime) 
			: vWayPoint(_vWayPoint), vLook(_vLook), fTime(_fTime) {};
	}WAYPOINT;

private:
	vector<WAYPOINT>					m_WayPoints;
	typedef vector<WAYPOINT>			WAYPOINTS;

	_float3			m_vLook;
	_uint			m_iCurrentIndex = 0;
	_float			m_fT = 0.f;

private:
	explicit CCamera_WayPoint(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_WayPoint(const CCamera_WayPoint& rhs);
	virtual ~CCamera_WayPoint() = default;

public:
	void Set_WayPoint(vector<WAYPOINT> _vecWayPoint) {
		m_WayPoints.clear();
		m_WayPoints = _vecWayPoint;
	}
	void Set_Look(_float3 _vLook) { m_vLook = _vLook; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Move_WayPoint(_double _DeltaTime);

public:
	HRESULT Add_WayPoint(WAYPOINT _vPosition) { m_WayPoints.push_back(_vPosition); }
	HRESULT Clear_WayPoint() { m_WayPoints.clear(); }

public:
	static CCamera_WayPoint* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
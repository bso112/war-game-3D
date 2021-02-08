#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CTerrain;
class CWaypoint;
class CPickingMgr : public CBase
{
	DECLARE_SINGLETON(CPickingMgr)
private:
	explicit CPickingMgr();
	virtual ~CPickingMgr() = default;

public:
	HRESULT	Ready_PickingMgr(PDIRECT3DDEVICE9 pGraphic_Device);

public:
	void	Register_Observer(CGameObject* pGameObject);
	void	UnRegister_Observer(CGameObject* pGameObject);
	


public:
	//���ӿ�����Ʈ�� ��ŷ�Ѵ�.
	CGameObject*	Picking(POINT& pt);
	//vOrigin ���κ��� ���� ����� ������Ʈ�� ��ȯ�Ѵ�. Self�� �����Ѵ�.
	CGameObject*	Get_Closest_Object(_float3 vOrigin, CGameObject* pSelf = nullptr);
	//vOrigin ���κ��� fRadius ��ŭ�� ������ �ִ� ������Ʈ���� ��ȯ�Ѵ�. Self�� �����Ѵ�.
	vector<CGameObject*> OverlapShpere(_float3 vOrigin, _float fRadius, CGameObject* pSelf = nullptr);
	//vOrigin ���κ��� fRadius ��ŭ�� ������ �ִ� ������Ʈ���� ��ȯ�Ѵ�. Self�� �����Ѵ�.pLayerTag �� �ִ� ������Ʈ�� ��ȯ�Ѵ�.
	vector<CGameObject*> OverlapShpere(_float3 vOrigin, _float fRadius, SCENEID pLayerSceneID, const _tchar* pLayerTag, CGameObject* pSelf = nullptr);

public:
	_bool Get_TerrainPos(POINT& pt, _float3* _vTerrainPos);
	_bool Get_TerrainPos(_float3 vWorldPos, _float3& _vTerrainPos);
	_bool Get_TerrainNormal(_float3 vWorldPos, _float3& _vTerrainNormal);

	_float	Get_Blur_mDist() { return m_fBlurmDist; }
	_float	Get_Blur_MDist() { return m_fBlurMDist; }
	void	Set_Blur_mDist(_float fmDist) { m_fBlurmDist = fmDist; }
	void	Set_Blur_MDist(_float fMDist) { m_fBlurMDist = fMDist; }
	_float	Update_Blur_Dist();
	_float3 Get_Blur_Center();
	void	Update_Blur_Center();
	_float	Get_FOV() { return m_fFOV; }
	_bool	IsSniping() { return m_bIsSniping; }

private:
	void	Set_IsSniping(_bool _bSniping) { m_bIsSniping = _bSniping; }
	_float				m_fBlurMDist = 150.f;
	_float				m_fBlurmDist = 100.f;
	_bool				m_bIsSniping = 0;
	_float3				m_vOldFocus = _float3(0.f, 0.f, 0.f);
	_float3				m_vCurFocus = _float3(0.f, 0.f, 0.f);
	_float3				m_vTargetFocus = _float3(0.f,0.f,0.f);
	_double				m_dbl_FocusTime = 0.0;
	_float				m_fFOV = 60.f;

public:
	vector<WAYPOINT> Get_Waypoints();
	WAYPOINT		Get_Waypoint(size_t iIndex);
	WAYPOINT		Get_CurrentWaypoint();
	HRESULT			Occupy_Waypoint(size_t iIndex, _double dOccqupyRate);
	HRESULT			Spawn_Ally(size_t iWaypointIndex, _float3 vPos, _uint eUnitType);
	void			Clear_PickingMgr();

private:
	HRESULT	Create_Waypoint(PDIRECT3DDEVICE9 pGraphic_Device, WAYPOINT WaypointDesc);


private:
	CTerrain*			m_pTerrain = nullptr;
	list<CGameObject*>	m_listObserver;
	vector<CWaypoint*>	m_Waypoints;
	//���ɵ�
	_double				m_dOccupyRate = 0.0;

public:
	virtual void Free() override;
};
END

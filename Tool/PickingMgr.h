#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "Base.h"

BEGIN(Tool)

class CTerrain;
class CPickingMgr : public CBase
{
	DECLARE_SINGLETON(CPickingMgr)

private:
	explicit CPickingMgr();
	virtual ~CPickingMgr() = default;

public:
	HRESULT	Ready_PickingMgr();

public:
	void	Register_Observer(CGameObject* pGameObject);
	void	UnRegister_Observer(CGameObject* pGameObject);

public:
	//게임오브젝트를 피킹한다.
	CGameObject*	Picking(POINT& pt);
	//vOrigin 으로부터 가장 가까운 오브젝트를 반환한다. Self는 무시한다.
	CGameObject*	Get_Closest_Object(_float3 vOrigin, CGameObject* pSelf = nullptr);
	//vOrigin 으로부터 fRadius 만큼의 범위에 있는 오브젝트들을 반환한다. Self는 무시한다.
	vector<CGameObject*> OverlapShpere(_float3 vOrigin, _float fRadius, CGameObject* pSelf = nullptr);

public:
	_bool Get_TerrainPos(POINT& pt, _float3* _vTerrainPos);
	_bool Get_TerrainPos(_float3 vWorldPos, _float3& _vTerrainPos);

private:
	CTerrain*	m_pTerrain = nullptr;
	list<CGameObject*> m_listObserver;

public:
	virtual void Free() override;
};
END

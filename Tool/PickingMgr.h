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
	//���ӿ�����Ʈ�� ��ŷ�Ѵ�.
	CGameObject*	Picking(POINT& pt);
	//vOrigin ���κ��� ���� ����� ������Ʈ�� ��ȯ�Ѵ�. Self�� �����Ѵ�.
	CGameObject*	Get_Closest_Object(_float3 vOrigin, CGameObject* pSelf = nullptr);
	//vOrigin ���κ��� fRadius ��ŭ�� ������ �ִ� ������Ʈ���� ��ȯ�Ѵ�. Self�� �����Ѵ�.
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

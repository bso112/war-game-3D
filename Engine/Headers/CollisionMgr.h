#pragma once

// ȭ�鿡 �׷����� ��ü������ ��Ƴ��� ������ ���� �������� ���ش�.

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class  ENGINE_DLL CCollisionMgr final : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
public:
	//COL_STATIC : �׻� �浹ó��
	enum COLLISION_GROUP { COL_UNIT, COL_ARROW,COL_WEAPON, COL_BUILDING, COL_GATE, COL_TOWER, COL_SKILL, COL_END };
private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr() = default;
public:
	HRESULT Ready_Component_Prototype();
	HRESULT Ready_Component(void* pArg);
public:
	HRESULT Add_CollisionGroup(COLLISION_GROUP eGroup, CGameObject* pGameObject);
	HRESULT CheckCollision();

private:
	HRESULT	CheckCollision_Unit();
	HRESULT	CheckCollision_Weapon();
	HRESULT	CheckCollision_Arrow();
	HRESULT	CheckCollision_Building();
	HRESULT	CheckCollision_Gate();
	HRESULT	CheckCollision_SKILL();

private:
	//eMove �з����� �׷�, eNoneMove �ȹз����� �׷� 
	HRESULT	CheckCollision_NoneMove(COLLISION_GROUP eMove, COLLISION_GROUP eNoneMove);




private:
	vector<CGameObject*>			m_CollisionGroup[COL_END];
	typedef vector<CGameObject*>	COLLISIONVECTOR;


public:
	virtual void Free();
};

END
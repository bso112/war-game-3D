#pragma once

// 화면에 그려야할 객체들응ㄹ 모아놓고 순서에 따라 렌더콜을 해준다.

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class  ENGINE_DLL CCollisionMgr final : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
public:
	//COL_STATIC : 항상 충돌처리
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
	//eMove 밀려나는 그룹, eNoneMove 안밀려나는 그룹 
	HRESULT	CheckCollision_NoneMove(COLLISION_GROUP eMove, COLLISION_GROUP eNoneMove);




private:
	vector<CGameObject*>			m_CollisionGroup[COL_END];
	typedef vector<CGameObject*>	COLLISIONVECTOR;


public:
	virtual void Free();
};

END
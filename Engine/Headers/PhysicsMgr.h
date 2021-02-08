#pragma once

// 화면에 그려야할 객체들응ㄹ 모아놓고 순서에 따라 렌더콜을 해준다.

#include "Base.h"

BEGIN(Engine)
class CTransform;
class CCollider;
class CRigid_Body;
class CGameObject;
class ENGINE_DLL CPhysicsMgr final : public CBase
{
	DECLARE_SINGLETON(CPhysicsMgr)
private:
	explicit CPhysicsMgr();
	virtual ~CPhysicsMgr() = default;

public:
	enum PHYSGROUP { PHYS_NORMAL, PHYS_END };
	typedef struct tagPhysStruct
	{
		CTransform*		pTransform = nullptr;
		CCollider*		pCollider = nullptr;
		CRigid_Body*	pRigidBody = nullptr;
		_float3*		pNormal = nullptr;
	}PHYSSTRUCT;
public:
	HRESULT Ready_Component_Prototype();
	HRESULT Ready_Component(void* pArg);
public:
	HRESULT Add_PhysGroup(PHYSGROUP eGroup, CTransform* pTransform, CCollider* pCollider, CRigid_Body* pRigid_Body, _float3* pNormal = nullptr);
	HRESULT Compute_PhysGroup(_double dblTimeDelta);
	void Set_Gravity(_float fGravity) { m_fGravity = fGravity; };
	_float Get_Gravity() { return m_fGravity; }
	_float	Compute_Optimal_Power(_float fAngle, _float3 vShooter, _float3 vTarget);

private:
	//물리값 계산
	HRESULT Apply_Physics(_double dblTimeDeltak, PHYSSTRUCT* tagPhys);

private:
	list<PHYSSTRUCT*>			m_PhysGroup[PHYS_END];
	typedef list<PHYSSTRUCT*>	RENDERLIST;
	_float						m_fGravity = 0.f;

private:

public:
	//static CPhysicsMgr* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	//virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
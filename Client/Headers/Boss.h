#pragma once
#include "Client_Defines.h"
#include "Unit.h"

BEGIN(Client)

class CBossParts;
class CBoss final : public CUnit
{
private:
	explicit CBoss(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	void Set_IsWelcome(_bool bCheck) { m_isWelcome = bCheck; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	BASEDESC			m_BaseDesc;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider_Box*		m_pColliderCom = nullptr;

private:
	vector<CBossParts*>	   m_vecBossParts;
	typedef vector<CBossParts*> BOSSPARTS;

private:	// Movement
	_float3		m_vOriginalPos;
	_double		m_MoveTimer = 0.0;
	_bool		m_isWelcome = false;
	_bool		m_isStarted = false;
	_bool		m_isLooked = false;
	_bool		m_isMoved = false;
	_bool		m_isTurned = false;
	_bool		m_isBack = false;

private:	// Attack
	CUnit*		m_pTarget = nullptr;
	_float3		m_vTargetPos;
	_float3		m_vTargetDir;
	_double		m_AttackTimer = 0.0;
	_double		m_SavedTimer = 0.0;
	_float		m_fHeight = 0.f;
	_uint		m_iAttackType = 0;
	_bool		m_isNear = false;
	_bool		m_isFar = false;

private:
	HRESULT Add_Component();
	HRESULT Ready_Layer_BossParts(const _tchar* pLayerTag);
	_int Set_BossParts_ParentsMatrix(_double TimeDelta);
	_int Update_Welcome(_double TimeDelta);		// 성문 파괴시 한번 호출
	_int Update_Movement(_double TimeDelta);
	_int Update_Attack(_double TimeDelta);
	_int Get_Target(_float3& vTargetPos);

public:
	static CBoss* Create_Boss_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
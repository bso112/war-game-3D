#pragma once
#include "GameObject.h"
#include "Management.h"
BEGIN(Client)
class CUnit abstract : public CGameObject
{
public:
	typedef struct tagUnitStat
	{
		_int	iMaxHp = 0;
		_int	iCurrHp = 0;
		_int	iAtt = 0;
		_float	fRecogRange = 0.f;
		_float	fAggroRange = 0.f;
		_float	fAttackRange = 0.f;
		//공격속도
		_float	fAttackSpeed = 0.f;

	}UNITSTAT;

	typedef struct tagUnitDesc
	{
		_bool bFriendly = false;
		_bool bStatic = false;
	}UNITDESC;

protected:
	explicit CUnit(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUnit(const CUnit& rhs);
	virtual ~CUnit() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual void Render_Debug();

public:
	void	Heal(_int iAmount) { m_tStat.iCurrHp = min(m_tStat.iMaxHp, m_tStat.iCurrHp += iAmount); }
	UNITSTAT Get_Stat() { return m_tStat; }
	_matrix	Get_WorldMatrix();


public:
	_bool	Is_Ground() { return m_bGround;}
	void	Set_ControllMode(_bool bControllMode) { if (m_bFriendly) { m_bControlMode = bControllMode; if (bControllMode) OnControlModeEnter(); else OnControlModeExit(); } }
	_bool	Get_ControlMode() { return m_bControlMode; }
	void	Set_Picked(_bool _Picked) { m_bPicked = _Picked; }
	_bool	Get_Picked() { return m_bPicked; }
	virtual _int	Get_WaypointNum() { return 0; }
	virtual _float3	Get_Position();
public:
	virtual _bool	Is_Friendly() {return false;}
public:
	//사망 시 false, 생존 시 true
	_bool	TakeDamage(_int _iDamage);
	_bool	TakeDamage(_double _dDamage);


protected:
	_bool	IsInWaypoint();

protected:
 	virtual void	OnControlModeEnter();
	virtual	void	OnControlModeExit();

protected:
	virtual void	OnTakeDamage(_int _iDamage);

protected:
	UNITSTAT		 m_tStat;
	list<WAYPOINT>	m_WayPoints;
	_bool			m_bGround = false;
	_bool			m_bControlMode = false;
	_bool			m_bPicked = false;
	_bool			m_bFriendly = false;
	//무적인가
	_bool			m_bInvincible = false;
	//인식한 목표
	CUnit*			m_pTarget = nullptr;
	_float			m_fWalk_Force = 1500.f;
	_float			m_fJumpPower = 10.f;
	_float3			m_vGroundNormal = _float3(0.f, 0.f, 0.f);
	_float3			m_vTargetPos;
	_int			m_iMaxJump = 1;
	_int			m_iAvblJump = m_iMaxJump;
	_double			m_dblAfterJump = 0.0;
	_float3			m_vVelocity_Old = _float3(0.f,0.f,0.f);
	_float3			m_vWeaponLocalPos = _float3(0.f, 0.f, 0.f);

	_double			m_AccDamage = 0.f;

public:
	void	Free() override;
};

END
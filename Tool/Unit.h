#pragma once
#include "Tool_Defines.h"
#include "Management.h"

BEGIN(Tool)

class CUnit abstract : public CGameObject
{
public:
	typedef struct tagUnitStat
	{
		_int	iMaxHp = 0;;
		_int	iCurrHp = 0;
		_int	iAtt =0;
		_float	fRecogRange = 0.f;
		_float	fAggroRange = 0.f;
		_float	fAttackRange = 0.f;
		//공격속도
		_float	fAttackSpeed = 0.f;

	}UNITSTAT;

	typedef struct tagUnitDesc
	{
		_bool bFriendly = false;
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

public:
	void	Set_WayPoint(list<_float3>& _WayPoints) { m_WayPoints = _WayPoints; };
	_bool	Is_Ground() { return m_bGround;}
	void	Set_ControllMode(_bool bControllMode) { if (m_bFriendly) { m_bControlMode = bControllMode;  OnControlModeEnter(); } }
	_bool	Get_ControlMode() { return m_bControlMode; }

protected:
 	virtual void	OnControlModeEnter();
	virtual	void	OnControlModeExit();

public:
	_bool	TakeDamage(_int _iDamage);

protected:
	UNITSTAT		 m_tStat;
	list<_float3>	m_WayPoints;
	_bool			m_bGround = false;
	_bool			m_bControlMode = false;
	_bool			m_bFriendly = false;
	//무적인가
	_bool			m_bInvincible = false;
	//인식한 목표
	CUnit*			m_pTarget = nullptr;

public:
	void	Free() override;
};

END
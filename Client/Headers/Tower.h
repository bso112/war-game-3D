#pragma once
#include "Unit.h"
BEGIN(Client)
class CWeapon;
class CTower final : public CUnit
{
public:
	enum STATE
	{
		STATE_ATTACK,
		STATE_END
	};

	enum OCCUPATION
	{
		OCC_WARRIOR,
		OCC_ARCHER,
		OCC_MAGICIAN,
		OCC_END
	};

public:
	typedef struct tagAimFactors
	{
		_bool	bPossibility;
		_float	fAngle;
		_float	fPower;
	}AIM_FACTORS;

	typedef struct tagStateDesc : UNITDESC
	{
		BASEDESC tBasedesc;
		//직업
		OCCUPATION tOccupation = OCC_END;
		_bool	bStatic = false;
		_uint	iNumber = 0;
	}INFANTRYDESC;

protected:
	explicit CTower(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTower(const CTower& rhs);
	virtual ~CTower() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool	Picking(POINT& pt) override;



private:
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_Component();
	//위치, 크기, 스텟 등을 셋팅
	HRESULT	SetUp_State();

public:
	virtual _int	Get_WaypointNum() { return m_tDesc.iNumber; };
	virtual _float3	Get_Position() { return m_pTransformCom->Get_WorldPosition(); };

public:
	_bool	Load_Mesh();
public:
	virtual _bool	Is_Friendly() { return m_tDesc.bFriendly; }
	_matrix	Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }


protected:
	void	OnControlModeEnter() override;
	void	OnControlModeExit() override;
	void	OnTakeDamage(_int iDamage) override;
	virtual void OnCollisionEnter(CGameObject* _pOther);


private:
	_int	Battle(_double TimeDelta);
	AIM_FACTORS	Aim_Target(_float3 vTarget, CWeapon* pBow, _float fHAngle = 0.f);
	void	Attack_Archer(_float3	vTargetPos, _double TimeDelta);


private:
	HRESULT	SetUp_Occupation(OCCUPATION _eOccupation);

private:
	CUnit*	Get_ClosestEnemy(_float& fDist);

private:
	CTransform*			m_pTransformCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CCollider_Ellipse*	m_pCollderCom = nullptr;
	ID3DXMesh*                      m_pMesh = 0;
	vector<D3DMATERIAL9>			m_Mtrls;


	_bool	m_bIsHigh = 0;

private:
	INFANTRYDESC	m_tDesc;
	STATE			m_eState = STATE_ATTACK;
	_double			m_dAttackTimer = FLT_MAX;
	//idle 상태일때 주변에 적이 있는지 확인하는 타이머 
	_double			m_dCheckEnemyTimer = FLT_MAX;
	CWeapon*		m_pWeapon = nullptr;

	_float3			m_vTempForce = _float3(0.f, 0.f, 0.f);

private:
	_float			m_fXRot = 0.f;
	_float			m_fYRot = 0.f;

public:
	OCCUPATION Get_OCC() { return m_tDesc.tOccupation; }

public:
	static CTower* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	void	Free() override;
};

END
#pragma once
#include "Tool_Defines.h"
#include "Unit.h"

BEGIN(Tool)

class CInfantry final : public CUnit
{
public:
	enum STATE
	{
		STATE_IDLE,
		STATE_PATROL,
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
	typedef struct tagStateDesc : UNITDESC
	{
		BASEDESC tBasedesc;
		//직업
		OCCUPATION tOccupation = OCC_END;
		_bool	isSpecial = false;
		_uint	iNumber = 0;
	}INFANTRYDESC;

protected:
	explicit CInfantry(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CInfantry(const CInfantry& rhs);
	virtual ~CInfantry() = default;

public:
	const INFANTRYDESC Get_InfantryDesc();

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

	virtual _ulong AddRef(VOID);

public:
	_bool	Picking(POINT& pt) override;
	HRESULT Go_Straight(_float fZ);
	HRESULT Go_Backward(_float fZ);
	HRESULT Go_Right(_float fX);
	HRESULT Go_Left(_float fX);
	HRESULT Go_Up(_float fY);
	HRESULT Go_Down(_float fY);
	HRESULT Turn_Right(_double TimeDelta);
	HRESULT Turn_Left(_double TimeDelta);

private:
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_Component();
	HRESULT	SetUp_State();	//위치, 크기, 스텟 등을 셋팅

public:
	virtual void OnCollisionEnter(CGameObject* _pOther);

private:
	_int	Move(_double TimeDelta);
	_int	Patrol(_double TimeDelta);
	_int	CheckGround();
	_int	Move_ControlMode(_double TimeDelta);

private:
	HRESULT	SetUp_Occupation(OCCUPATION _eOccupation);

private:
	CUnit*	Get_ClosestEnemy(_float& fDist);

private:
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer*			m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CCollider_Ellipse*	m_pCollderCom = nullptr;

private:
	LPD3DXMESH		m_pMesh = nullptr;

private:
	INFANTRYDESC	m_tDesc;
	STATE			m_eState = STATE_PATROL;
	_double			m_dAttackTimer = FLT_MAX;
	//idle 상태일때 주변에 적이 있는지 확인하는 타이머 
	_double			m_dCheckEnemyTimer = FLT_MAX;

public:
	static CInfantry* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	void	Free() override;
};

END
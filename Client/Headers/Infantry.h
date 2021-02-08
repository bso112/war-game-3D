#pragma once
#include "Unit.h"
BEGIN(Client)
class CWeapon;
class CInfantry final : public CUnit
{
public:
	enum OCCUPATION
	{
		OCC_WARRIOR,
		OCC_ARCHER,
		OCC_MAGICIAN,
		OCC_END
	};
	enum COMMAND
	{
		COMMAND_WAIT,
		COMMAND_FOLLOW,
		COMMAND_ATTACK,
		COMMAND_END
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
		//����
		OCCUPATION tOccupation = OCC_END;
		_bool	bStatic = false;
		_uint	iNumber = 0;
	}INFANTRYDESC;

protected:
	explicit CInfantry(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CInfantry(const CInfantry& rhs);
	virtual ~CInfantry() = default;

public:
	void Set_CurHP(_double Damage);
	void Set_IsEvent1(_bool bCheck);
	void Set_IsEvent2(_bool bCheck); 

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

	virtual void Render_Debug();

public:
	_bool	Picking(POINT& pt) override;

protected:
	void	OnDead() override;

private:
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_Component();


public:
	virtual _int	Get_WaypointNum() { return m_tDesc.iNumber; };
public:
	virtual void OnCollisionEnter(CGameObject* _pOther);
	_bool	Load_Mesh();
public:
	virtual _bool	Is_Friendly() { return m_tDesc.bFriendly; }
	HRESULT Jump(_double TimeDelta);
	void Head_Direction(_double TimeDelta);
	void Fall_Damage(_double TimeDelta);
	_matrix	Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }
	//virtual _float3	Get_Position() { return m_pTransformCom->Get_WorldPosition(); };

public:
	HRESULT OnKeyDown(_int KeyCode) override;
	HRESULT OnKeyUp(_int KeyCode) override;

protected:
	virtual _float3 Get_Position() override;
	void	OnControlModeEnter() override;
	void	OnControlModeExit() override;
	void	OnTakeDamage(_int iDamage) override;

private:
	_int	CheckGround(_float fWeight = 0.1f);
	_int	Move_ControlMode(_double TimeDelta);
	void	Go_Dst(_float3 _vDst, _double _dblDeltaTime, _float* _fDist = nullptr, _float fStoppingDistance = 0.0f);
	HRESULT	Go_Dest(_float3 vDest, _double TimeDelta, _float fStopDist = 0.f);

private:
	_int	AI_Enemy(_double TimeDelta);
	_int	AI_Ally(_double TimeDelta);
	_int	Control_Commander(_double TimeDelta);

private:
	//���ϵ��� �Ϸķ� �̲���
	_int	Lead_Infantry_line(_double TimeDelta);
	//���ϵ��� ���ְ��� �߽����� ������.
	_int	Lead_Infantry(_double TimeDelta);
private:
	//�������. �ֺ��� ���� ������ false�� �����Ѵ�.
	_bool	Battle(_double TimeDelta);
	//Ŀ�Ǵ��� �ٶ󺸴� ������ �ٶ󺻴�.
	void	LookAt_CommanderLook();

private:
	//���ݸ�� �޾����� ����
	_int	OnCommnad_Charge();
	HRESULT	ShowComandIcon();

private:
	AIM_FACTORS	Aim_Target(_float3 vTarget, CWeapon* pBow, _float fHAngle = 0.f);
	void	Attack_Archer(_float3	vTargetPos, _double TimeDelta);
	void	Set_Commander(_bool isCommander) { m_isCommander = isCommander; }

private:
	HRESULT	SetUp_Occupation(OCCUPATION _eOccupation);

private:
	CUnit*	Get_ClosestEnemy(_float& fDist);
	CUnit* Get_ClosestAlly(_float& fDist);

private:
	CTransform*			m_pTransformCom = nullptr;
	//CVIBuffer*			m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CCollider_Ellipse*	m_pCollderCom = nullptr;
	CRigid_Body*		m_pRigidBodyCom = nullptr;
	ID3DXMesh*                      m_pMesh = 0;
	vector<D3DMATERIAL9>			m_Mtrls;

	_bool	m_bIsHigh = 0;
	_double m_SmokeTime = 0;

	//��� �¾��
	_bool	m_bJustBorn = false;

private:
	INFANTRYDESC	m_tDesc;
	_double			m_dAttackTimer = FLT_MAX;
	//idle �����϶� �ֺ��� ���� �ִ��� Ȯ���ϴ� Ÿ�̸� 
	_double			m_dCheckEnemyTimer = FLT_MAX;
	CWeapon*		m_pWeapon = nullptr;

	_float3			m_vTempForce = _float3(0.f, 0.f, 0.f);

	//���ְ��ΰ�
	_bool			m_isCommander = false;
	static COMMAND	m_eCommnad;
	_bool			m_isArrived = false;
	_bool			m_isCurved = false;
	_bool			m_isEvent1 = false;
	_bool			m_isEvent2 = false;

private:
	_float			m_fXRot = 0.f;
	_float			m_fYRot = 0.f;

private:
	//���ְ����� ���ݸ���� ���� ��ǥ����
	_float3			m_vTargetArea;

public:
	OCCUPATION Get_OCC() { return m_tDesc.tOccupation; }

public:
	static CInfantry* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	void	Free() override;

};

END
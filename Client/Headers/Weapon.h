#pragma once
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CWeapon abstract : public CGameObject
{
public:
	//무기 주인의 정보
	typedef	struct tagOWNER
	{
		int64_t	iOwnerID = 0;
		_bool	bFriendly = false;
	}OWNER;

public:
	enum CONTROLID { CONTROL_AI, CONTROL_USER, CONTROL_END };

	typedef struct tagStateDesc
	{
		_float3		vScale = _float3(0.f, 0.f, 0.f);
		_float3		vPos = _float3(0.f, 0.f, 0.f);
		_int		iAttack = 0;
	}STATEDESC;

protected:
	explicit CWeapon(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	const _int Get_Attack() const { return m_StateDesc.iAttack; }
	_float3		Get_WorldPosition() { return m_pTransform->Get_WorldPosition(); }
	_float3		Get_Position() { return m_pTransform->Get_Position(); }

public:
	void Set_ParentsMatrix(_matrix ParentsMatrix) { m_pTransform->Set_ParentsMatrix(ParentsMatrix); }
	void Set_IsControl(_bool bCheck) { m_eControlID = (CONTROLID)bCheck; }

public:
	void	Set_Owner(OWNER tOwner) { m_tOwner = tOwner; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Attack(_double TimeDelta);

protected:
	STATEDESC			m_StateDesc;
	CONTROLID			m_eControlID = CONTROL_AI;

	CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	CTransform*			m_pTransform = nullptr;
	CTexture*			m_pTexture = nullptr;
	CShader*			m_pShader = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRigid_Body*		m_pRigidBody = nullptr;

	//무기 주인
	OWNER				m_tOwner;

public:
	virtual CGameObject * Clone_GameObject(void * pArg) = 0;
	virtual void Free();
};

END
#pragma once
#include "Client_Defines.h"
#include "Management.h"
#include "Sword.h"

BEGIN(Client)

class CBossParts final : public CGameObject
{
public:
	enum OBJID { OBJ_HEAD, OBJ_BODY, OBJ_RIGHTARM, OBJ_LEFTARM, OBJ_RIGHTLEG, OBJ_LEFTLEG, OBJ_END };

	typedef struct tagStateDesc
	{
		OBJID		eObjID;
		BASEDESC	BaseDesc;
	}STATEDESC;

private:
	explicit CBossParts(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBossParts(const CBossParts& rhs);
	virtual ~CBossParts() = default;

public:
	const OBJID Get_ObjID() const { return m_StateDesc.eObjID; }
	const _bool Get_IsWelcome() const { return m_isWelcome; }
	const _bool Get_HasMoved() const { return m_hasMoved; }
	const _uint Get_AttackType() const { return m_iAttackType; }

public:
	void Set_ParentsMatrix(_matrix ParentsMatrix) { m_pTransformCom->Set_ParentsMatrix(ParentsMatrix); }
	void Set_HasMoved(_bool bCheck) { m_hasMoved = bCheck; }
	void Set_AttackType(_uint iAttackType) { m_iAttackType = iAttackType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_int Update_Welcome(_double TimeDelta);

private:
	STATEDESC				m_StateDesc;
	CVIBuffer_Cube*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	ID3DXMesh*              m_pMesh = 0;
	vector<D3DMATERIAL9>	m_Mtrls;

private:	// Movement
	_double		m_MoveTimer = 0.0;
	_double		m_isSaveTimer = 0.0;
	_bool		m_isWelcome = false;
	_bool		m_hasMoved = false;
	_bool		m_isSteped = false;		//	One step
	_bool		m_isLeft = true;		//	Left Step

private:	// Sword
	CSword*		m_pSword = nullptr;

private:	// Attack
	_double		m_AttackTimer = 0.0;
	_uint		m_iAttackType = 0;

private:
	_bool Load_Mesh();
	HRESULT Add_Component();
	HRESULT Set_ConstantTable();
	HRESULT Ready_Layer_Sword(const _tchar* pLayerTag);
	_int Set_Sword_ParentsMatrix(_double TimeDelta);
	_int Update_Movement(_double TimeDelta);
	_int Update_Attack(_double TimeDelta);

public:
	static CBossParts* Create_BossParts_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
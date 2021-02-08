#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

class CHealingZone final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float3		vPosition;
		_float		fRadius;
		_float		fRecoverHp;
		_float		fRecoverTime;
		_float		fLifeTime;
	}STATEDESC;
private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	//CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	ID3DXMesh*					m_pMesh = nullptr;
	vector<D3DMATERIAL9>	m_Mtrls;
	CCollider_Ellipse*		m_pColliderCom = nullptr;

private:
	STATEDESC			m_StateDesc;
	_double				m_dCurrentRecoverTime = 0.f;
	_double				m_dCurrentLifeTime = 0;
	
private:

private:
	explicit CHealingZone(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHealingZone(const CHealingZone& rhs);
	virtual ~CHealingZone() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Healing_Ally();

	_bool Load_Mesh();

public:
	static CHealingZone* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};


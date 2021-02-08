#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

class CMeteor final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float3		vScale;
		_float3		vPosition;
		_float3		vDstPosition;

		_float		fRadius;
		_float		fSpeed;
		_float		fDamage;
	}STATEDESC;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider_Ellipse*		m_pColliderCom = nullptr;

private:
	STATEDESC	m_StateDesc;

private:
	explicit CMeteor(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMeteor(const CMeteor& rhs);
	virtual ~CMeteor() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	virtual void OnCollisionEnter(CGameObject* _pOther);

public:
	static CMeteor* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};


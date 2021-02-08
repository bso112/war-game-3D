#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

class CBullet_Trail final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float3		vScale;
		_float3		vSrc;
		_float3		vDst;
		_float		fLifeTime;
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
	_float			m_fAlpha = 1.f;

private:
	explicit CBullet_Trail(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBullet_Trail(const CBullet_Trail& rhs);
	virtual ~CBullet_Trail() = default;

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
	static CBullet_Trail* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};


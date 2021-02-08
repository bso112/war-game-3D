#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCube final : public CGameObject
{
private:
	explicit CCube(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	void Set_WorldMatrix(_matrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	virtual	void OnCollisionEnter(CGameObject* _pOther);

public:
	void	Set_Controll() { m_bControl = true; }
private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CCollider_Box*		m_pColliderBoxCom = nullptr;

private:
	_bool				m_bControl = false;
	
public:
	static CCube* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
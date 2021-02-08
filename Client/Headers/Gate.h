#pragma once
#include "Unit.h"
#include "Management.h"

class CGate final :public CUnit
{
protected:
	explicit CGate(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGate(const CGate& rhs);
	virtual ~CGate() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	_bool		Load_Mesh();
	HRESULT		Add_Component();
	HRESULT		SetUp_ConstantTable();

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;
	CTexture*						m_pTexture = nullptr;
	CCollider_Box*					m_pCollider = nullptr;
	ID3DXMesh*                      m_pMesh = 0;
	vector<D3DMATERIAL9>			m_Mtrls;

	CGameObject*					m_pParent = nullptr;
	CTransform*						m_pOriginalTransform = nullptr;


public:
	static CGate* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};


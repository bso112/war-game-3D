#pragma once
#include "GameObject.h"
#include "Management.h"
class CCastle final :public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float3		vScale;
		_float3		vAxis;
		_float3		vPos;

		_float		fRadian;
	}STATEDESC;

protected:
	explicit CCastle(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCastle(const CCastle& rhs);
	virtual ~CCastle() = default;

public:
	void Set_WorldMatrix(_matrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }

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
	STATEDESC						m_StateDesc;
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;
	CTexture*						m_pTexture = nullptr;
	ID3DXMesh*                      m_pMesh = 0;
	vector<D3DMATERIAL9>			m_Mtrls;

public:
	static CCastle* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};


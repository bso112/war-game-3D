#pragma once
#include "Unit.h"
BEGIN(Client)
class CSphere final : public CGameObject
{
public:
protected:
	explicit CSphere(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSphere(const CSphere& rhs);
	virtual ~CSphere() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	void	OnDead() override;

private:
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_Component();

public:
	_bool	Load_Mesh();
public:
	_matrix	Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }
	//virtual _float3	Get_Position() { return m_pTransformCom->Get_WorldPosition(); };

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	ID3DXMesh*                      m_pMesh = 0;
	vector<D3DMATERIAL9>			m_Mtrls;

private:
	_float			m_fXRot = 0.f;
	_float			m_fYRot = 0.f;
	_float			m_fFov_Old = 60.f;
	_float			m_fIntensity = 0.f;
	_bool			m_bScope = 0;

private:
	//지휘관에게 돌격명령을 받은 목표지역
	_float3			m_vTargetArea;

public:
	static CSphere* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	void	Free() override;

};

END
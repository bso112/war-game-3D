#pragma once
#include "Tool_Defines.h"
#include "Management.h"

BEGIN(Tool)

class CCube final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		BASEDESC	tBasedesc;
		_float3		vAxis;
		_float		fAngle;
	}STATEDESC;

private:
	explicit CCube(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	const _matrix Get_WorldMatrix() const { return m_pTransformCom->Get_WorldMatrix(); }

public:
	void Set_WorldMatrix(_matrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool	Picking(POINT& pt) override;
	HRESULT Go_Straight(_float fZ);
	HRESULT Go_Backward(_float fZ);
	HRESULT Go_Right(_float fX);
	HRESULT Go_Left(_float fX);
	HRESULT Go_Up(_float fY);
	HRESULT Go_Down(_float fY);
	HRESULT Turn_Right(_double TimeDelta);
	HRESULT Turn_Left(_double TimeDelta);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	STATEDESC			m_StateDesc;
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CCollider_Box*		m_pColliderBoxCom = nullptr;

public:
	static CCube* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
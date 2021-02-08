#pragma once
#include "Tool_Defines.h"
#include "Management.h"

BEGIN(Tool)

class CBasePlane final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_uint		iNatureID;
		_bool		isSpecial = false;
		_float3		vScale;
		_float3		vAxis = _float3(0.f, 0.f, 0.f);
		_float3		vPos;
		_float		fRadian = 0.f;
	}STATEDESC;

	typedef struct tagWaypointDesc
	{
		_float3		vPosition;
		_float		fRadius = 0.f;
		_bool		bOccupied = false;
		_bool		isSpecial = false;
		_uint		iNumber = 0;
	}WAYPOINTDESC;

	typedef struct tagNatureDesc
	{
		_uint		iNatureID;
		_float3		vScale;
		_float3		vPos;
	}NATUREDESC;

private:
	explicit CBasePlane(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBasePlane(const CBasePlane& rhs);
	virtual ~CBasePlane() = default;

public:
	const WAYPOINTDESC Get_WaypointDesc();
	const NATUREDESC Get_NatureDesc();
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
	_bool	Picking(POINT& ptMouse) override;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	STATEDESC			m_StateDesc;
	WAYPOINTDESC		m_WaypointDesc;
	NATUREDESC			m_NatureDesc;
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CCollider_Box*		m_pColliderCom = nullptr;

public:
	static CBasePlane* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
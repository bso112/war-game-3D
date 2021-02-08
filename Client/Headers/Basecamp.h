#pragma once
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CBasecamp final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float3		vScale;
		_float3		vWaypointPos;
		_float3		vOffset;
	}STATEDESC;

private:
	explicit CBasecamp(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBasecamp(const CBasecamp& rhs);
	virtual ~CBasecamp() = default;

public:
	const _float3 Get_WorldPos() const { return m_pTransformCom->Get_WorldPosition(); }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	STATEDESC			m_StateDesc;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	static CBasecamp* Create_Basecamp_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
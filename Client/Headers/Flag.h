#pragma once
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CFlag final : public CGameObject
{
public:
	enum FLAGID { FLAG_RED, FLAG_BLUE, FLAG_END };

	typedef struct tagStateDesc
	{
		FLAGID		eFlagID = FLAG_END;
		_float3		vScale;
		_float3		vPos;
		_float		fMaxHeight = 0.f;
		_float		fMinHeight = 0.f;
	}STATEDESC;

private:
	explicit CFlag(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFlag(const CFlag& rhs);
	virtual ~CFlag() = default;

public:
	void Set_ParentsMatrix(_matrix ParentsMatrix) { m_pTransformCom->Set_ParentsMatrix(ParentsMatrix); }
	void Set_OccupiedRate(_double Rate) { m_OccupiedRate = Rate; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	STATEDESC			m_StateDesc;
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	_float3				m_vOriginalPos;

private: // Flag Movement
	_double		m_OccupiedRate = 0.0;
	_double		m_OldRate = 0.0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	_int Lift_Flag(_double TimeDelta);

public:
	static CFlag* Create_Flag_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
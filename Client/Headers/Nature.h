#pragma once
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CNature final : public CGameObject
{
public:
	enum NATUREID { NATURE_GRASS, NATURE_TREE, NATURE_END };

	typedef struct tagStateDesc
	{
		NATUREID	eNatureID = NATURE_END;
		_float3		vScale;
		_float3		vPos;
	}STATEDESC;

private:
	explicit CNature(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CNature(const CNature& rhs);
	virtual ~CNature() = default;

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

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	_int Look_Camera();

public:
	static CNature* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
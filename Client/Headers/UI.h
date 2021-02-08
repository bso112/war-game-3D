#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float2		vProjPos;
		_float2		vProjSize;
		_uint		iTexIndex;
		_int		iDepth;
	}STATEDESC;

protected:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

protected:
	_matrix				m_ProjMatrix;
	STATEDESC			m_StateDesc;

protected:
	explicit CUI(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual _int Get_Depth() { return m_StateDesc.iDepth; }
	_uint Get_TextureIndex() { return m_StateDesc.iTexIndex; }

public:
	void Set_TextureIndex(_uint _Index) { m_StateDesc.iTexIndex = _Index; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable();

public:
	virtual void Free();
};
END

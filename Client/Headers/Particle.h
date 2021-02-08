#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

BEGIN(Client)

class CParticle final : public CGameObject
{
	typedef struct tagStateDesc
	{
		SCENEID			eTextureScene;
		const _tchar*		pTextureKey;
		_uint				iTextureNum;
		_uint				iShaderPass;
		_float				fFrameTime;
	}STATEDESC;

private:
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

private:
	STATEDESC		m_StateDesc;
	_uint				m_iCurrentFrameIndex;
	_float				m_fAlpha;

private:
	explicit CParticle(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject(CShader* pShader);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CParticle* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};
END

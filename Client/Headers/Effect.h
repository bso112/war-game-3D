#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

BEGIN(Client)

class CEffect final : public CGameObject
{
public:
	typedef struct tagState
	{
		SCENEID			eTextureScene;
		const _tchar*		pTextureKey;
		_uint				iTextureNum;
		_uint				iShaderPass;
		_float				fFrameTime;
		_bool				bAlpha;

		_float3				vScale;
		_float3				vRotate;
		_float3				vPosition;
	}STATEDESC;

protected:
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;

protected:
	STATEDESC		m_StateDesc;
	_uint				m_iCurrentTexture = 0;
	_float				m_fCurrentFrameTime = 0.f;
	_float				m_fCurrentAlpha = 1.f;

protected:
	explicit CEffect(PDIRECT3DDEVICE9 _pGraphic_Device);
	explicit CEffect(const CEffect& _rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CEffect* Create(PDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};

END
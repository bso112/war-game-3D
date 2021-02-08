#pragma once
#include "GameObject.h"
#include "Management.h"
BEGIN(Client)
class CRenderTarget abstract :public CGameObject
{
protected:
	explicit CRenderTarget(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CRenderTarget(const CRenderTarget& rhs);
	virtual ~CRenderTarget() = default;

public:
	virtual HRESULT Process();

protected:
	CVIBuffer_ViewPort*			m_pVIBufferCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	LPDIRECT3DTEXTURE9			m_pRawTexture = nullptr;

public:
	void Free() override;
};
END

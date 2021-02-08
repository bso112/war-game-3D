#include "stdafx.h"
#include "..\Headers\RenderTarget.h"



CRenderTarget::CRenderTarget(PDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CRenderTarget::CRenderTarget(const CRenderTarget & rhs)
	:CGameObject(rhs)
{
}

HRESULT CRenderTarget::Process()
{
	return S_OK;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRawTexture);
	Safe_Release(m_pRendererCom);
	CGameObject::Free();
}

#include "..\Headers\Target.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_ViewPort.h"

USING(Engine)

CTarget::CTarget(PDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CTarget::Ready_Target(_uint iSizeX, _uint iSizeY, D3DFORMAT eFormat)
{
	// 1. �ؽ��ĸ� ��������.
	if (FAILED(D3DXCreateTexture(m_pGraphic_Device, iSizeX, iSizeY, 1, D3DUSAGE_RENDERTARGET, eFormat, D3DPOOL_DEFAULT, &m_pRawTexture)))
		return E_FAIL;

	// 2. ������ �ؽ��ķκ��� ����Ÿ���� ������ ������.
	if (FAILED(m_pRawTexture->GetSurfaceLevel(0, &m_pSurface)))
		return E_FAIL;	


	return S_OK;
}



HRESULT CTarget::SetUp_OnGraphicDev(_uint iIndex)
{
	if (nullptr == m_pSurface || 
		nullptr == m_pGraphic_Device)
		return E_FAIL;

	//���۷��� ī��Ʈ�� �ö�
	m_pGraphic_Device->GetRenderTarget(iIndex, &m_pOldSurface);

	m_pGraphic_Device->SetRenderTarget(iIndex, m_pSurface);

	return S_OK;
}

HRESULT CTarget::Release_OnGraphicDev(_uint iIndex)
{
	if (nullptr == m_pSurface ||
		nullptr == m_pGraphic_Device)
		return E_FAIL;
	

	m_pGraphic_Device->SetRenderTarget(iIndex, m_pOldSurface);

	//���۷��� ī��Ʈ ����
	Safe_Release(m_pOldSurface);


	return S_OK;
}

HRESULT CTarget::Clear_Target()
{
	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);

	return S_OK;
}



CTarget * CTarget::Create(PDIRECT3DDEVICE9 pGraphic_Device, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat)
{
	CTarget*	pInstance = new CTarget(pGraphic_Device);

	if (FAILED(pInstance->Ready_Target(iSizeX, iSizeY, eFormat)))
	{
		MessageBox(0, L"Failed To Creating CTarget", L"System Message", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget::Free()
{

	Safe_Release(m_pOldSurface);
	Safe_Release(m_pSurface);
	Safe_Release(m_pRawTexture);

	Safe_Release(m_pGraphic_Device);
}

#include "..\Headers\Light.h"

CLight::CLight(PDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CLight::Ready_Light(const D3DLIGHT9 & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

CLight * CLight::Create(PDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightDesc)
{
	CLight*	pInstance = new CLight(pGraphic_Device);

	if (FAILED(pInstance->Ready_Light(LightDesc)))
	{
		MSG_BOX("Failed To Creating CLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pGraphic_Device);
}

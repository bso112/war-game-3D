#include "..\Headers\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const D3DLIGHT9* CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_LightList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();	
}

HRESULT CLight_Manager::Add_Light(PDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightDesc)
{
	CLight*		pLight = CLight::Create(pGraphic_Device, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_LightList.push_back(pLight);

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_LightList)
		Safe_Release(pLight);

	m_LightList.clear();
}

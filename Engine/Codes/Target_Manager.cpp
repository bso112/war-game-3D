#include "..\Headers\Target_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}

HRESULT CTarget_Manager::SetUp_OnGraphicDev(const _tchar * pTargetTag, _uint iIndex)
{
	CTarget*	pTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->SetUp_OnGraphicDev(iIndex);	
}

HRESULT CTarget_Manager::Release_OnGraphicDev(const _tchar * pTargetTag, _uint iIndex)
{
	CTarget*	pTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Release_OnGraphicDev(iIndex);
}

HRESULT CTarget_Manager::Add_RenderTarget(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTargetTag, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CTarget*		pTarget = CTarget::Create(pGraphic_Device, iSizeX, iSizeY, eFormat);
	if (nullptr == pTarget)
		return E_FAIL;

	m_Targets.insert(TARGETS::value_type(pTargetTag, pTarget));

	return S_OK;
}

LPDIRECT3DTEXTURE9 CTarget_Manager::Get_Texture(const _tchar * pTargetTag)
{
	CTarget*	pTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pTarget)
		return nullptr;

	return pTarget->Get_Texture();
}


CTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_Targets.begin(), m_Targets.end(), CFinder_Tag(pTargetTag));

	if (iter == m_Targets.end())
		return nullptr;

	return iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_Targets)
		Safe_Release(Pair.second);

	m_Targets.clear();
}

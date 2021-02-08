#include "..\Headers\Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "PipeLine.h"
CRenderer::CRenderer(PDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

HRESULT CRenderer::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CRenderer::Ready_Component(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroup, CGameObject * pGameObject)
{
	if (RENDER_END <= eGroup ||
		nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroup[eGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_3DUI()))
		return E_FAIL;
	//if (FAILED(Render_UI()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_UI_Lately()
{
	if (FAILED(Render_UI()))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (FAILED(pGameObject->Render_GameObject()))
			return E_FAIL;

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		if (FAILED(pGameObject->Render_GameObject()))
			return E_FAIL;

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	////깊이를 내림차순으로 정렬
	//m_RenderGroup[RENDER_ALPHABLEND].sort([](CGameObject* pA, CGameObject* pB)
	//{
	//	CTransform* pTransformA = (CTransform*)pA->Find_Component(L"Com_Transform");
	//	CTransform* pTransformB = (CTransform*)pB->Find_Component(L"Com_Transform");
	//	if (nullptr == pTransformA || nullptr == pTransformB)
	//		return false;

	//	_float3 pPosA = pTransformA->Get_Position();
	//	_float3 pPosB = pTransformB->Get_Position();

	//	D3DXVec3TransformCoord(&pPosA, &pPosA, &CPipeLine::Get_Instance()->Get_Transform(D3DTS_VIEW));
	//	D3DXVec3TransformCoord(&pPosB, &pPosB, &CPipeLine::Get_Instance()->Get_Transform(D3DTS_VIEW));


	//	return pPosA.z < pPosB.z;
	//});
	m_RenderGroup[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return pSour->Get_ViewZ() > pDest->Get_ViewZ();	});

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHABLEND])
	{
		if (FAILED(pGameObject->Render_GameObject()))
			return E_FAIL;

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_3DUI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_3DUI])
	{
		if (FAILED(pGameObject->Render_GameObject()))
			return E_FAIL;

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_3DUI].clear();

	return S_OK;
	////////////////////
	//m_RenderGroup[RENDER_3DUI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
	//	return pSour->Get_ViewZ() > pDest->Get_ViewZ();	});

	//for (auto& pGameObject : m_RenderGroup[RENDER_3DUI])
	//{
	//	if (FAILED(pGameObject->Render_GameObject()))
	//		return E_FAIL;

	//	Safe_Release(pGameObject);
	//}

	//m_RenderGroup[RENDER_3DUI].clear();

	//return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderGroup[RENDER_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return pSour->Get_Depth() > pDest->Get_Depth();
	});

	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (FAILED(pGameObject->Render_GameObject()))
			return E_FAIL;

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_UI].clear();

	return S_OK;

}

HRESULT CRenderer::Render_Debug()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_DEBUG])
	{
		pGameObject->Render_Debug();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_DEBUG].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CRenderer*	pInstance = new CRenderer(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("Failed To Create CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent * CRenderer::Clone_Component(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}

	CComponent::Free();
}

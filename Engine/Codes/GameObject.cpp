#include "..\Headers\GameObject.h"
#include "Management.h"

int64_t CGameObject::m_iInstanceID_static = 0;
CGameObject::CGameObject(PDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
	m_iInstanceID = m_iInstanceID_static++;
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device),
	m_bActive(rhs.m_bActive)
{
	Safe_AddRef(m_pGraphic_Device);
	m_iInstanceID = m_iInstanceID_static++;
}

HRESULT CGameObject::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CGameObject::Ready_GameObject(void * pArg)
{


	return S_OK;
}

_int CGameObject::Update_GameObject(_double TimeDelta)
{

	return _int();
}

_int CGameObject::Late_Update_GameObject(_double TimeDelta)
{

	return _int();
}

HRESULT CGameObject::Render_GameObject()
{

	return S_OK;
}

void CGameObject::Render_Debug()
{
}

void CGameObject::Clear_DeadObject()
{
	auto& iter = m_listCollided.begin();
	while (iter != m_listCollided.end())
	{
		if ((*iter)->Get_Dead())
		{
			iter = m_listCollided.erase(iter);
		}
		else
			++iter;
	}

}


void CGameObject::OnCollisionEnter(CGameObject * _pOther)
{
}

void CGameObject::OnCollisionStay(CGameObject * _pOther)
{
}

void CGameObject::OnCollisionExit(CGameObject * _pOther)
{
}

HRESULT CGameObject::Add_Component(_uint iPrototypeSceneID, const _tchar* pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CComponent*	pComponent = pManagement->Clone_Component(iPrototypeSceneID, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.insert(COMPONENTS::value_type(pComponentTag, pComponent));

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CGameObject::Compute_ViewZ(_float3 vWorldPos)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_float3	vCamPosition = pManagement->Get_CamPosition();

	_float3	vDistance = vCamPosition - vWorldPos;

	m_fViewZ = D3DXVec3Length(&vDistance);

	Safe_Release(pManagement);

	return S_OK;
}

_bool CGameObject::Picking(POINT & pt)
{
	return _bool();
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CFinder_Tag(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pGraphic_Device);
}

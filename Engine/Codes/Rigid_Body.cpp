#include "..\Headers\Rigid_Body.h"

CRigid_Body::CRigid_Body(PDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CRigid_Body::CRigid_Body(const CRigid_Body & rhs)
	: CComponent(rhs)
{
	m_vExternForce = rhs.m_vExternForce;
	m_vVelocity = rhs.m_vVelocity;
}

HRESULT CRigid_Body::Ready_Component_Prototype()
{

	return S_OK;
}

HRESULT CRigid_Body::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tStateDesc, pArg, sizeof(STATEDESC));
	m_tStateDesc;
	return S_OK;
}

CRigid_Body * CRigid_Body::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CRigid_Body*	pInstance = new CRigid_Body(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("Failed To Create CRigid_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRigid_Body::Clone_Component(void * pArg)
{
	CRigid_Body*	pInstance = new CRigid_Body(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("Failed To Create CRigid_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRigid_Body::Free()
{
	CComponent::Free();
}

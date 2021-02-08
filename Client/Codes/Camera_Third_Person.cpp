#include "stdafx.h"
#include "..\Headers\Camera_Third_Person.h"
#include "Management.h"
#include "Camera_Manager.h"
#include "GameManager.h"
#include "Infantry.h"

CCamera_Third_Person::CCamera_Third_Person(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Third_Person::CCamera_Third_Person(const CCamera_Third_Person & rhs)
	: CCamera(rhs)
{

}

void CCamera_Third_Person::Set_Lerp(_float3 _vSrcLook, _float _fLerpTime) 
{ 
	m_bLerp = true; 
	m_vSrcLook = _vSrcLook; 
	m_fLerpTime = _fLerpTime; 
}

HRESULT CCamera_Third_Person::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Third_Person::Ready_GameObject(void * pArg)
{
	if (CCamera::Ready_GameObject(pArg))
		return E_FAIL;

	CKeyMgr::Get_Instance()->RegisterObserver(this);
	return S_OK;
}

_int CCamera_Third_Person::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (m_bLerp)
	{
		if (FAILED(Lerp_Target(TimeDelta)))
			return -1;
	}
	else
	{
		if (FAILED(Look_Target()))
			return -1;
	}

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Third_Person::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	CCamera_Manager* pCamManager = CCamera_Manager::Get_Instance();
	if (nullptr == pCamManager)
		return -1;

	CUnit* pUnit = dynamic_cast<CUnit*>(m_pTarget);
	if (nullptr == pUnit)
		return -1;

	if (pUnit->Get_Dead())
	{
		m_pTarget = nullptr;
		pCamManager->Resume_Camera();
	}

	return CCamera::Late_Update_GameObject(TimeDelta);
}

HRESULT CCamera_Third_Person::Render_GameObject()
{
	if (!m_bActive)
		return S_OK;
	return S_OK;
}

HRESULT CCamera_Third_Person::OnKeyDown(_int KeyCode)
{
	if (!m_bActive)
		return S_OK;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CInfantry* pInfantry = dynamic_cast<CInfantry*>(m_pTarget);
	if (nullptr == pInfantry)
		return E_FAIL;

	if (VK_RBUTTON == KeyCode)
	{
		switch (pInfantry->Get_OCC())
		{
		case CInfantry::OCC_WARRIOR:
			if (CGameManager::Get_Instance()->Get_SkillReady(SKILL_HEAL))
			{
				CCamera_Manager::Get_Instance()->Change_Camera_Skill(m_pTarget, SKILL_HEAL);
				pInfantry->Set_ControllMode(false);
			}
			break;
		case CInfantry::OCC_ARCHER:
			if (CGameManager::Get_Instance()->Get_SkillReady(SKILL_SNIPE))
			{
				CCamera_Manager::Get_Instance()->Change_Camera_Skill(m_pTarget, SKILL_SNIPE);
				pInfantry->Set_ControllMode(false);
			}
			break;
		case CInfantry::OCC_MAGICIAN:
			if (CGameManager::Get_Instance()->Get_SkillReady(SKILL_METEOR))
			{
				CCamera_Manager::Get_Instance()->Change_Camera_Skill(m_pTarget, SKILL_METEOR);
				pInfantry->Set_ControllMode(false);
			}
			break;
		default:
			break;
		}
	}
	
	if (VK_RETURN == KeyCode)
	{
		if (m_bLock)
		{
			CCamera_Manager::Get_Instance()->Change_CamState(CCamera_Manager::STATE_BATTLE);
			m_bLock = false;
		}
		else
		{
			CCamera_Manager::Get_Instance()->Change_CamState(CCamera_Manager::STATE_COMMAND);
			m_bLock = true;
		}
	}
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CCamera_Third_Person::Exit_Camera()
{
	return S_OK;
}

HRESULT CCamera_Third_Person::Lerp_Target(_double _DeltaTime)
{
	if (nullptr == m_pTarget)
		return E_FAIL;

	CTransform* pTargetTransform = (CTransform*)m_pTarget->Find_Component(L"Com_Transform");
	if (nullptr == pTargetTransform)
		return E_FAIL;

	//_float3 vLook = pTargetTransform->Get_Position() - m_pTransformCom->Get_Position();
	_float3 vLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vLook, &vLook);

	m_vOffSet = _float3(5.f, 4.f, 0.f);
	m_fTargetDistance = 15.f;

	_float3 vLerp = {};
	D3DXVec3Lerp(&vLerp, &m_vSrcLook, &vLook, m_fT);
	D3DXVec3Normalize(&vLerp, &vLerp);

	_float3 vAxisY = _float3(0.f, 1.f, 0.f);
	_float3 vRight = {};
	_float3 vUp = {};
	D3DXVec3Cross(&vRight, &vAxisY, &vLerp);
	D3DXVec3Cross(&vUp, &vLerp, &vRight);

	D3DXVec3Normalize(&vRight, &vRight);

	_float3 vCamPos = (pTargetTransform->Get_Position() - vLook * m_fTargetDistance) + vRight * m_vOffSet.x + vUp * m_vOffSet.y;
	_float3 vPosition = {};
	D3DXVec3Lerp(&vPosition, &m_StateDesc.vSrc, &vCamPos, m_fT);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLerp);

	m_fT += (_float)_DeltaTime / m_fLerpTime;
	if (m_fT >= 1)
	{
		m_fT = 0.f;
		m_bLerp = false;
	}
	return S_OK;
}

HRESULT CCamera_Third_Person::Look_Target()
{
	if (nullptr == m_pTarget)
		return E_FAIL;

	CTransform* pTargetTransform = (CTransform*)m_pTarget->Find_Component(L"Com_Transform");
	if (nullptr == pTargetTransform)
		return E_FAIL;

	//_float3 vLook = pTargetTransform->Get_Position() - m_pTransformCom->Get_Position();
	_float3 vLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vLook, &vLook);

	_float3 vAxisY = _float3(0.f, 1.f, 0.f);
	_float3 vRight = {};
	_float3 vUp = {};
	D3DXVec3Cross(&vRight, &vAxisY, &vLook);
	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vRight, &vRight);

	_float3 vCamPos = (pTargetTransform->Get_Position() - vLook * m_fTargetDistance) + vRight * m_vOffSet.x + vUp * m_vOffSet.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

CCamera_Third_Person * CCamera_Third_Person::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Third_Person*	pInstance = new CCamera_Third_Person(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCamera_Third_Person");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Third_Person::Clone_GameObject(void * pArg)
{
	CCamera_Third_Person*	pInstance = new CCamera_Third_Person(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating CCamera_Third_Person");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Third_Person::Free()
{
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);
	CCamera::Free();
}
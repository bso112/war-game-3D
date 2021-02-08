#include "stdafx.h"
#include "..\Headers\Camera_WayPoint.h"
#include "Management.h"
#include "Camera_Manager.h"

CCamera_WayPoint::CCamera_WayPoint(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_WayPoint::CCamera_WayPoint(const CCamera_WayPoint & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_WayPoint::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCamera_WayPoint::Ready_GameObject(void * pArg)
{
	if (CCamera::Ready_GameObject(pArg))
		return E_FAIL;
	
	return S_OK;
}

_int CCamera_WayPoint::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (FAILED(Move_WayPoint(TimeDelta)))
		return -1;

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_WayPoint::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	return CCamera::Late_Update_GameObject(TimeDelta);
}

HRESULT CCamera_WayPoint::Render_GameObject()
{
	if (!m_bActive)
		return S_OK;

	return S_OK;
}

HRESULT CCamera_WayPoint::Move_WayPoint(_double _DeltaTime)
{
	if (m_iCurrentIndex < m_WayPoints.size() - 1)
	{
		m_StateDesc.vSrc = m_WayPoints[m_iCurrentIndex].vWayPoint;
		m_StateDesc.vDst = m_WayPoints[m_iCurrentIndex + 1].vWayPoint;

		_float3 vPosition = {};
		D3DXVec3Lerp(&vPosition, &m_StateDesc.vSrc, &m_StateDesc.vDst, m_fT);
		m_pTransformCom->SetUp_Position(vPosition);

		//_float3 vLook = pTargetTransform->Get_Position() - m_pTransformCom->Get_Position();
		_float3 vDir = m_WayPoints[m_iCurrentIndex + 1].vLook - m_pTransformCom->Get_Position();
		D3DXVec3Normalize(&vDir, &vDir);
		D3DXVec3Normalize(&m_vLook, &m_vLook);
		_float3 vLerp = {};
		D3DXVec3Lerp(&vLerp, &m_vLook, &vDir, m_fT);

		//_float fRadian = acos(D3DXVec3Dot(&vDir, &m_vLook));
		//_float fAlpha = fRadian * m_fT;
		//_float3 vAxis = {};
		//D3DXVec3Cross(&vAxis, &m_vLook, &vDir);
		//D3DXVec3Normalize(&vAxis, &vAxis);
		//_matrix mRotateMatrix = {};
		//_float3 vLook = {};
		//D3DXMatrixRotationAxis(&mRotateMatrix, &vAxis, fAlpha);
		//D3DXVec3TransformNormal(&vLook, &m_vLook, &mRotateMatrix);

		_float3 vAxisY = _float3(0.f, 1.f, 0.f);
		_float3 vRight = {};
		_float3 vUp = {};
		D3DXVec3Cross(&vRight, &vAxisY, &vLerp);
		D3DXVec3Cross(&vUp, &vLerp, &vRight);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLerp);

		m_fT += (_float)_DeltaTime / m_WayPoints[m_iCurrentIndex + 1].fTime;
		if (m_fT >= 1)
		{
			D3DXVec3Normalize(&m_vLook, &m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			++m_iCurrentIndex;
			m_fT = 0.f;
		}
	}
	else 
	{
		CCamera_Manager::Get_Instance()->Change_Camera_Free(false, CCamera_Manager::STATE_FREE, m_pTransformCom->Get_Position(), m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	}

	return S_OK;
}

CCamera_WayPoint * CCamera_WayPoint::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_WayPoint*	pInstance = new CCamera_WayPoint(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCamera_WayPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_WayPoint::Clone_GameObject(void * pArg)
{
	CCamera_WayPoint*	pInstance = new CCamera_WayPoint(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating CCamera_WayPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_WayPoint::Free()
{
	CCamera::Free();
}
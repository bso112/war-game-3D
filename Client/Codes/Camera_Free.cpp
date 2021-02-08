#include "stdafx.h"
#include "..\Headers\Camera_Free.h"
#include "Management.h"
#include "Camera_Manager.h"
#include "PickingMgr.h"
#include "Meteor.h"
#include "HealingZone.h"

CCamera_Free::CCamera_Free(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera(rhs)
{

}

void CCamera_Free::Set_Lerp(_float3 _vSrcPos, _float3 _vDstPos, _float3 _vSrcLook, _float3 _vDstLook, _float _fLerpTime)
{
	m_bLerp = true;
	m_StateDesc.vSrc = _vSrcPos;
	m_StateDesc.vDst = _vDstPos;
	m_vSrcLook = _vSrcLook;
	m_vDstLook = _vDstLook;
	m_fLerpTime = _fLerpTime;
}

HRESULT CCamera_Free::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Ready_GameObject(void * pArg)
{
	if (CCamera::Ready_GameObject(pArg))
		return E_FAIL;
	
	CKeyMgr::Get_Instance()->RegisterObserver(this);
	return S_OK;
}

_int CCamera_Free::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (m_bLerp)
	{
		if (FAILED(Lerp_Camera(TimeDelta)))
			return -1;
	}
	else
	{
		if (FAILED(Free_Move_Camera(TimeDelta)))
			return -1;
	}

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Free::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	return CCamera::Late_Update_GameObject(TimeDelta);
}

HRESULT CCamera_Free::Render_GameObject()
{
	if (!m_bActive)
		return S_OK;
	return S_OK;
}

HRESULT CCamera_Free::Lerp_Camera(_double _DeltaTime)
{
	_float3 vPosition = {};
	D3DXVec3Lerp(&vPosition, &m_StateDesc.vSrc, &m_StateDesc.vDst, m_fT);
	m_pTransformCom->SetUp_Position(vPosition);

	_float3 vLerp = {};
	D3DXVec3Lerp(&vLerp, &m_vSrcLook, &m_vDstLook, m_fT);

	_float3 vAxisY = _float3(0.f, 1.f, 0.f);
	_float3 vRight = {};
	_float3 vUp = {};
	D3DXVec3Cross(&vRight, &vAxisY, &vLerp);
	D3DXVec3Cross(&vUp, &vLerp, &vRight);

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

HRESULT CCamera_Free::Free_Move_Camera(_double _DeltaTime)
{
	if (m_bLock)
		return S_OK;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (pManagement->Get_DIKeyState((_ubyte)DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Straight(_DeltaTime * 10);
	}

	if (pManagement->Get_DIKeyState((_ubyte)DIK_S) & 0x80)
	{
		m_pTransformCom->Go_BackWard(_DeltaTime * 10);
	}

	if (pManagement->Get_DIKeyState((_ubyte)DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(_DeltaTime * 10);
	}

	if (pManagement->Get_DIKeyState((_ubyte)DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(_DeltaTime * 10);
	}

	if (0x8000 & GetKeyState(VK_NUMPAD9))
		m_pTransformCom->SetUp_Position(_float3(1800.f, 200.f, 1800.f));
	if (0x8000 & GetKeyState(VK_NUMPAD5))
		m_pTransformCom->SetUp_Position(_float3(8.f * 129.f, 200.f, 8.f * 129.f));

	if (pManagement->Get_DIKeyState((_ubyte)DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(_DeltaTime * 10);
	}

	_long	MouseMove = 0;

	if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_X))
	{
		m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), _DeltaTime * (MouseMove / 10.0));
	}

	if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
	{
		m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _DeltaTime * (MouseMove / 10.0));
	}

	if (0 < pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_WHEEL))
		m_StateDesc.fFovy -= (_float)_DeltaTime;

	else if (0 > pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_WHEEL))
		m_StateDesc.fFovy += (_float)_DeltaTime;

	// Y Limit
	//_float3 vPosition = m_pTransformCom->Get_WorldMatrix().m[3];
	//m_pTransformCom->SetUp_Position(_float3(vPosition.x, 50, vPosition.z));

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CCamera_Free::Move_MyWayPoint()
{
	vector<WAYPOINT> wayPoints = CPickingMgr::Get_Instance()->Get_Waypoints();
//#undef max
//	_float MinLength = numeric_limits<float>::max();
//#define max
	_float3 vCamPos = m_pTransformCom->Get_Position();
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
//	_float wayPointLength = 0.f;
//	_int CurrentIndex = 0;
//	_int ResultIndex = 0;
//	for (WAYPOINT waypoint : wayPoints)
//	{
//		if (waypoint.bOccupied && !waypoint.bHide)
//		{
//			wayPointLength = D3DXVec3Length(&(waypoint.vPosition - vCamPos));
//			if (MinLength >= wayPointLength)
//			{
//				MinLength = wayPointLength;
//				ResultIndex = CurrentIndex;
//			}
//		}
//		++CurrentIndex;
//	}
	_int ResultIndex = CPickingMgr::Get_Instance()->Get_CurrentWaypoint().iNumber;

	if (ResultIndex >= wayPoints.size() - 1)
		ResultIndex = wayPoints.size() - 2;

	_float3 vLookWaypoint = wayPoints[ResultIndex + 1].vPosition - wayPoints[ResultIndex].vPosition;
	D3DXVec3Normalize(&vLookWaypoint, &vLookWaypoint);

	_float3 vDstPos = wayPoints[ResultIndex].vPosition - vLookWaypoint * 100.f + _float3(0.f, 1.f, 0.f) * 75.f;
	_float3 vWaypointCenter = wayPoints[ResultIndex].vPosition - vDstPos;// (wayPoints[ResultIndex + 1].vPosition + wayPoints[ResultIndex].vPosition) * 0.5f;
	D3DXVec3Normalize(&vWaypointCenter, &vWaypointCenter);
	Set_Lerp(vCamPos, vDstPos, vLook, vWaypointCenter, 1.5f);
	return S_OK;
}

HRESULT CCamera_Free::OnKeyDown(_int _KeyCode)
{
	if (!m_bActive)
		return S_OK;

	CCamera_Manager* pCameraManager = CCamera_Manager::Get_Instance();
	Safe_AddRef(pCameraManager);

	switch (_KeyCode)
	{
	case VK_RETURN:
		switch (pCameraManager->Get_CamState())
		{
		case CCamera_Manager::STATE_FREE:
			pCameraManager->Change_CamState(CCamera_Manager::STATE_COMMAND);
			m_bLock = true;
			break;
		case CCamera_Manager::STATE_COMMAND:
			pCameraManager->Change_CamState(CCamera_Manager::STATE_FREE);
			m_bLock = false;
			break;
		default:
			break;
		}
		break;
	case VK_SPACE:
		Move_MyWayPoint();
		break;
	default:
		break;
	}

	Safe_Release(pCameraManager);
	return S_OK;
}

CCamera_Free * CCamera_Free::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Free*	pInstance = new CCamera_Free(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CLoading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Free::Clone_GameObject(void * pArg)
{
	CCamera_Free*	pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating CLoading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free::Free()
{
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);
	CCamera::Free();
}

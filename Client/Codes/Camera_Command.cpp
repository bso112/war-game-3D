#include "stdafx.h"
#include "..\Headers\Camera_Command.h"
#include "Management.h"
#include "Camera_Manager.h"

CCamera_Command::CCamera_Command(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Command::CCamera_Command(const CCamera_Command & rhs)
	: CCamera(rhs)
{

}

void CCamera_Command::Set_LerpPosition(_float3 _vSrc, _float3 _vDest, _float3 _vDestLook, _float _LerpTime)
{
	m_vSrcPosition = _vSrc;
	m_vDestPosition = _vDest;
	m_vSrcLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_vDestLook = _vDestLook;
	m_fLerpTime = _LerpTime;
}
HRESULT CCamera_Command::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Command::Ready_GameObject(void * pArg)
{
	if (CCamera::Ready_GameObject(pArg))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Command::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (FAILED(Lerp_Camera(TimeDelta)))
		return E_FAIL;

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Command::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	return CCamera::Late_Update_GameObject(TimeDelta);
}

HRESULT CCamera_Command::Render_GameObject()
{
	if (!m_bActive)
		return S_OK;
	return S_OK;
}

HRESULT CCamera_Command::Lerp_Camera(_double _DeltaTime)
{
	_float3 vPosition = {};
	D3DXVec3Lerp(&vPosition, &m_StateDesc.vSrc, &m_StateDesc.vDst, m_fT);
	m_pTransformCom->SetUp_Position(vPosition);

	_float3 vDir = m_StateDesc.vDst - m_StateDesc.vSrc;
	D3DXVec3Normalize(&vDir, &vDir);
	_float3 vLerp = {};
	D3DXVec3Lerp(&vLerp, &m_vSrcLook, &m_vDestLook, m_fT);

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
	}
	return S_OK;
}

CCamera_Command * CCamera_Command::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Command*	pInstance = new CCamera_Command(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCamera_Command");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Command::Clone_GameObject(void * pArg)
{
	CCamera_Command*	pInstance = new CCamera_Command(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating CCamera_Command");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Command::Free()
{
	CCamera::Free();
}
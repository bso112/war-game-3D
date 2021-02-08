#include "stdafx.h"
#include "Camera_Free.h"
#include "Management.h"

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
	m_StateDesc.vSrc = _vSrcPos;
	m_StateDesc.vDst = _vDstPos;
	m_bLerp = true;
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

	return S_OK;
}

_int CCamera_Free::Update_GameObject(_double TimeDelta)
{
	if (FAILED(Free_Move_Camera(TimeDelta)))
		return -1;

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Free::Late_Update_GameObject(_double TimeDelta)
{
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
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (0x8000 & GetAsyncKeyState('W'))
		m_pTransformCom->Go_Straight(_DeltaTime * 10);

	if (0x8000 & GetAsyncKeyState('S'))
		m_pTransformCom->Go_BackWard(_DeltaTime * 10);

	if (0x8000 & GetAsyncKeyState('A'))
		m_pTransformCom->Go_Left(_DeltaTime * 10);

	if (0x8000 & GetAsyncKeyState('D'))
		m_pTransformCom->Go_Right(_DeltaTime * 10);

#ifdef _DEBUG
	if (0x8000 & GetKeyState(VK_NUMPAD9))
		m_pTransformCom->SetUp_Position(_float3(1800.f, 200.f, 1800.f));

	if (0x8000 & GetKeyState(VK_NUMPAD5))
		m_pTransformCom->SetUp_Position(_float3(1000.f, 200.f, 1000.f));

	if (0x8000 & GetKeyState(VK_NUMPAD1))
		m_pTransformCom->SetUp_Position(_float3(200.f, 200.f, 200.f));
#endif // _DEBUG

	if (0x8000 & GetAsyncKeyState('L'))
		m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), _DeltaTime / 5);

	if (0x8000 & GetAsyncKeyState('J'))
		m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -_DeltaTime / 5);

	if (0x8000 & GetAsyncKeyState('K'))
		m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _DeltaTime / 5);

	if (0x8000 & GetAsyncKeyState('I'))
		m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -_DeltaTime / 5);


	//if (0 < pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_WHEEL))
	//	m_StateDesc.fFovy -= (_float)_DeltaTime;

	//else if (0 > pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_WHEEL))
	//	m_StateDesc.fFovy += (_float)_DeltaTime;

	Safe_Release(pManagement);
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
	CCamera::Free();
}

#include "..\Headers\Transform.h"
#include "Rigid_Body.h"

CTransform::CTransform(PDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_RevolutionMatrix(rhs.m_RevolutionMatrix)
	, m_ParentsMatrix(rhs.m_ParentsMatrix)
{
}
 
_float3 CTransform::Get_WorldState(STATE eState)
{
	return *(_float3*)&Get_WorldMatrix().m[eState][0];
}

_float3 CTransform::Get_Position()
{
	_matrix worldMat = Get_WorldMatrix();
	return *(_float3*)&worldMat.m[CTransform::STATE_POSITION][0];
}

_matrix CTransform::Get_WorldMatrix()
{
	if (m_pParent != nullptr)
		m_ParentsMatrix = m_pParent->Get_WorldMatrix();
	return _matrix(m_WorldMatrix * m_RevolutionMatrix * m_ParentsMatrix);
}

_matrix CTransform::Get_WorldMatrix_Scale1()
{
	// 월드 행렬(크기 1)
	_matrix WorldMatrix = Get_WorldMatrix();
	_float3 vRight, vUp, vLook;

	D3DXVec3Normalize(&vRight, &(_float3)WorldMatrix.m[0]);
	D3DXVec3Normalize(&vUp, &(_float3)WorldMatrix.m[1]);
	D3DXVec3Normalize(&vLook, &(_float3)WorldMatrix.m[2]);

	memcpy(WorldMatrix.m[0], &vRight, sizeof(_float3));
	memcpy(WorldMatrix.m[1], &vUp, sizeof(_float3));
	memcpy(WorldMatrix.m[2], &vLook, sizeof(_float3));

	return WorldMatrix;
}

_matrix CTransform::Get_WorldMatrixInverse()
{
	_matrix		InverseWorldMatrix, InverseRevolutionMatrix, InverseParentsMatrix;

	D3DXMatrixInverse(&InverseWorldMatrix, nullptr, &m_WorldMatrix);
	D3DXMatrixInverse(&InverseRevolutionMatrix, nullptr, &m_RevolutionMatrix);
	D3DXMatrixInverse(&InverseParentsMatrix, nullptr, &m_ParentsMatrix);

	return _matrix(InverseParentsMatrix * InverseRevolutionMatrix * InverseWorldMatrix);
}

_matrix CTransform::Get_RevolutionMatrixInverse()
{
	_matrix InverseMatrix;

	D3DXMatrixInverse(&InverseMatrix, nullptr, &m_RevolutionMatrix);

	return InverseMatrix;
}

_matrix CTransform::Get_ParentsMatrixInverse()
{
	_matrix InverseMatrix;

	D3DXMatrixInverse(&InverseMatrix, nullptr, &m_ParentsMatrix);

	return InverseMatrix;
}

RECT CTransform::Get_Rect()
{
	//다음 위치의 Rect를 구한다.
	RECT rc = {};
	float fX = Get_State(STATE_POSITION).x;
	float fY = Get_State(STATE_POSITION).y;
	_float3 vSize = Get_Scaled();
	int iCX = (int)vSize.x;
	int iCY = (int)vSize.y;

	rc.left = (LONG)fX - (iCX >> 1);
	rc.right = (LONG)fX + (iCX >> 1);
	rc.top = (LONG)fY - (iCY >> 1);
	rc.bottom = (LONG)fY + (iCY >> 1);
	return rc;
}

void CTransform::Set_State(STATE eState, _float3 vState)
{
	memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_float3));
}

void CTransform::Set_ParentsPosition(_float3 _vParentPos)
{
	memcpy((_float3*)&m_ParentsMatrix.m[3][0], &_vParentPos, sizeof(_float3));
}





HRESULT CTransform::Ready_Component_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_RevolutionMatrix);
	D3DXMatrixIdentity(&m_ParentsMatrix);

	return S_OK;
}

HRESULT CTransform::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	return S_OK;
}

HRESULT CTransform::Go_Straight(_double TimeDelta)
{
	_float3		vPosition, vLook;

	vLook = Get_State(CTransform::STATE_LOOK);
	vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * _float(m_StateDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_BackWard(_double TimeDelta)
{
	_float3		vPosition, vLook;

	vLook = Get_State(CTransform::STATE_LOOK);
	vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition -= *D3DXVec3Normalize(&vLook, &vLook) * _float(m_StateDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta)
{
	_float3		vPosition, vRight;

	vRight = Get_State(CTransform::STATE_RIGHT);
	vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vRight, &vRight) * _float(m_StateDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta)
{
	_float3		vPosition, vRight;

	vRight = Get_State(CTransform::STATE_RIGHT);
	vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition -= *D3DXVec3Normalize(&vRight, &vRight) * _float(m_StateDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Up(_double TimeDelta)
{
	_float3		vPosition, vUp;

	vUp = Get_State(CTransform::STATE_UP);
	vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vUp, &vUp) * _float(m_StateDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Down(_double TimeDelta)
{
	_float3		vPosition, vUp;

	vUp = Get_State(CTransform::STATE_UP);
	vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition -= *D3DXVec3Normalize(&vUp, &vUp) * _float(m_StateDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Dst(_float3 _vDst, _double TimeDelta, _float* _fOutDist)
{
	_float3 vDir = _vDst - Get_State(STATE_POSITION);

	D3DXVec3Normalize(&vDir, &vDir);

	_float3 vNextPos = Get_State(STATE_POSITION) + vDir * _float(m_StateDesc.SpeedPerSec * TimeDelta);
	SetUp_Position(vNextPos);
	
	if(nullptr != _fOutDist)
		*_fOutDist = D3DXVec3Length(&(vNextPos - _vDst));
	return S_OK;
}

HRESULT CTransform::Go_Forward(_double TimeDelta)
{
	_float3		vPosition, vLook;

	vLook = Get_State(CTransform::STATE_LOOK);
	vPosition = Get_State(CTransform::STATE_POSITION);
	_float3 vNewPos;
	vNewPos = *D3DXVec3Normalize(&vLook, &vLook) * _float(m_StateDesc.SpeedPerSec * TimeDelta);
	vPosition += vNewPos;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::GO_ZNegative(_double TimeDelta)
{
	_float3		vPosition, vLook;

	vLook = Get_State(CTransform::STATE_LOOK);
	vPosition = Get_State(CTransform::STATE_POSITION);
	_float3 vNewPos;
	vNewPos = *D3DXVec3Normalize(&vLook, &vLook) * _float(m_StateDesc.SpeedPerSec * TimeDelta);
	vPosition -= vNewPos;

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::AddForce(_float3 _vForce)
{
	Set_State(STATE_POSITION, Get_State(STATE_POSITION) + _vForce);
	return S_OK;
}

HRESULT CTransform::LookAt(_float3 _vTarget)
{

	_float3 vDir;
	D3DXVec3Normalize(&vDir, &(_vTarget - Get_Position()));

	//회전
	_float3 vLook = vDir * Get_Scaled().z;
	_float3 vRight, vUp;

	D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	vRight *= Get_Scaled().x;

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	vUp *= Get_Scaled().y;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::LookAt_Radian(_float3 _vTarget, _float _fRadian)
{
	_float3 vDir = _vTarget - Get_WorldPosition();
	_float fDist = sqrtf(vDir.x*vDir.x + vDir.z*vDir.z);
	vDir.y = fDist*tanf(_fRadian);
	D3DXVec3Normalize(&vDir, &vDir);

	//회전
	_float3 vLook = vDir * Get_Scaled().z;
	_float3 vRight, vUp;

	D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	vRight *= Get_Scaled().x;

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	vUp *= Get_Scaled().y;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

void CTransform::ApplyPhysics(CRigid_Body* _pRigidBody, _double _TimeDelta)
{
	_float3 vPos = Get_Position();
	vPos += _pRigidBody->Get_Velocity()*_TimeDelta;
	Set_State(STATE_POSITION, vPos);
}

HRESULT CTransform::SetUp_Position(_float3 _vPos)
{
	Set_State(STATE_POSITION, _vPos);
	return S_OK;
}

HRESULT CTransform::SetUp_Scale(_float3 _vSize)
{
	_float3 vRight = *D3DXVec3Normalize(&Get_State(STATE_RIGHT), &Get_State(STATE_RIGHT)) * _vSize.x;
	_float3 vUp = *D3DXVec3Normalize(&Get_State(STATE_UP), &Get_State(STATE_UP)) * _vSize.y;
	_float3	vLook = *D3DXVec3Normalize(&Get_State(STATE_LOOK), &Get_State(STATE_LOOK)) * _vSize.z;
	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
	return S_OK;
}

// 항등상태에서 내가 원하는 회전값을 부여한다. != 현재 상태에서 내가 원하ㅡㄴㄴ 회전값을 추가로 부옇나다.
HRESULT CTransform::SetUp_Rotation(_float3 vAxis, _float fRadian)
{
	_float3		vRight, vUp, vLook;

	_float3		vScale = Get_Scaled();

	vRight = _float3(1.f, 0.f, 0.f) * vScale.x;
	vUp = _float3(0.f, 1.f, 0.f) * vScale.y;
	vLook = _float3(0.f, 0.f, 1.f) * vScale.z;


	_matrix		RotationMatrix;
	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, fRadian);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}


HRESULT CTransform::SetUp_RotationMatrix(_matrix rotationMatrix)
{
	_float3		vRight, vUp, vLook;

	_float3		vScale = Get_Scaled();


	Set_State(CTransform::STATE_RIGHT, _float3(1.f,0.f,0.f) * vScale.x);
	Set_State(CTransform::STATE_UP, _float3(0.f,1.f,0.f) * vScale.y);
	Set_State(CTransform::STATE_LOOK, _float3(0.f,0.f,1.f) * vScale.z);

	//자전하고 이동
	m_WorldMatrix = rotationMatrix * m_WorldMatrix;

	return S_OK;
}

HRESULT CTransform::Rotate(_float3 vAxis, _float fRadian)
{
	_float3		vRight, vUp, vLook;

	vRight = Get_State(CTransform::STATE_RIGHT);
	vUp = Get_State(CTransform::STATE_UP);
	vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix;
	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, fRadian);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Rotation_Axis(_float3 vAxis, _double TimeDelta)
{
	// 행렬에 회전의 상태를 담는다. == 행렬의 1, 2, 3행에 해당하는 방향벡터들을 회전시켜놓는다.
	_float3		vRight, vUp, vLook;

	vRight = Get_State(CTransform::STATE_RIGHT);
	vUp = Get_State(CTransform::STATE_UP);
	vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix;
	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, _float(m_StateDesc.RotatePerSec * TimeDelta));

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

CTransform * CTransform::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CTransform*	pInstance = new CTransform(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("Failed To Create CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone_Component(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("Failed To Create CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	Safe_Release(m_pParent);
	CComponent::Free();
}

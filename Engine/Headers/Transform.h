#pragma once

// 1. 객체의 상태를 가진다. (게임 세계 == 월드 스페이스) 월드변환 행렬.

#include "Component.h"

BEGIN(Engine)

class CRigid_Body;
class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
public:
	typedef struct tagStateDesc
	{
		_double SpeedPerSec = 0.0;
		_double RotatePerSec = 0.0;
	}STATEDESC;

private:
	explicit CTransform(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public: // Getter
	_float3 Get_State(STATE eState) const { return *(_float3*)&m_WorldMatrix.m[eState][0]; }
	_float3 Get_WorldState(STATE eState);
	_float3	Get_Position();
	_float3	Get_WorldPosition() { return (_float3)Get_WorldMatrix().m[3]; }
	_float3 Get_Scaled() const {
		return _float3(D3DXVec3Length(((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0])),
					   D3DXVec3Length(((_float3*)&m_WorldMatrix.m[STATE_UP][0])),
					   D3DXVec3Length(((_float3*)&m_WorldMatrix.m[STATE_LOOK][0])));}
	_matrix Get_WorldMatrix();
	_matrix Get_WorldMatrix_Scale1();
	_matrix Get_WorldMatrixInverse();
	_matrix Get_RevolutionMatrixInverse();
	_matrix Get_ParentsMatrixInverse();
	RECT	Get_Rect();

public: // Setter
	void Set_State(STATE eState, _float3 vState);
	void Set_RevolutionMatrix(_matrix RevolutionMatrix) { m_RevolutionMatrix = RevolutionMatrix; }
	void Set_ParentsMatrix(_matrix ParentsMatrix) { m_ParentsMatrix = ParentsMatrix; }
	void Set_ParentsPosition(_float3 _vParentPos);
	void Set_Parent(CTransform* pParent) { if (nullptr == pParent) return;  m_pParent = pParent; Safe_AddRef(m_pParent); }


public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);

public: // For.Movement
	HRESULT Go_Straight(_double TimeDelta); // 현재월드스페이스 상에서 바라보고 있는 방향으로 가.
	HRESULT Go_BackWard(_double TimeDelta);
	HRESULT Go_Right(_double TimeDelta);
	HRESULT Go_Left(_double TimeDelta);
	HRESULT Go_Up(_double TimeDelta);
	HRESULT Go_Down(_double TimeDelta);
	HRESULT Go_WorldUp(_double TimeDelta) {
		_float3 vPosition = Get_State(CTransform::STATE_POSITION);
		vPosition += _float3(0.f, 1.f, 0.f) * _float(m_StateDesc.SpeedPerSec * TimeDelta);
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	//_fOutDist으로 타깃까지의 거리를 리턴한다.
	HRESULT	Go_Dst(_float3 _vDst, _double TimeDelta, _float* _fOutDist = nullptr);
	HRESULT Go_Forward(_double TimeDelta);
	HRESULT GO_ZNegative(_double TimeDelta);
	HRESULT	AddForce(_float3 _vForce);
	HRESULT	LookAt(_float3 _vTarget);
	HRESULT LookAt_Radian(_float3 _vTarget, _float _fRadian);
	void	ApplyPhysics(CRigid_Body* _pRigidBody, _double _TimeDelta);

public:
	HRESULT	SetUp_Position(_float3 _vPos);
	HRESULT	SetUp_Scale(_float3 _vSize);
	void	Set_WorldMatrix(_matrix _Matrix) { m_WorldMatrix = _Matrix; }



public: // For.Rotation
	//항등상태에서 회전한다.
	HRESULT SetUp_Rotation(_float3 vAxis, _float fRadian);
	HRESULT	SetUp_RotationMatrix(_matrix rotationMatrix);
	//현재 상태에서 회전한다.
	HRESULT	Rotate(_float3 vAxis, _float fRadian);
	//축을 중심으로 현재상태에서 회전한다.
	HRESULT Rotation_Axis(_float3 vAxis, _double TimeDelta);

private:
	STATEDESC		m_StateDesc;
	_matrix			m_WorldMatrix;
	_matrix			m_RevolutionMatrix;
	_matrix			m_ParentsMatrix;

private:
	CTransform*		m_pParent = nullptr;

public:
	static CTransform* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
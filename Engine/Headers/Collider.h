#pragma once

#include "PipeLine.h"
#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	typedef struct tagStateDesc
	{
		_matrix		StateMatrix;
	}STATEDESC;
	
public:
	//OBB를 위한 투영영역
	class CProjection
	{
	public:
		CProjection(_float fMin, _float fMax){m_fMin = fMin; m_fMax = fMax;}
		_bool	Overlap(CProjection Other) {return max(m_fMin, Other.m_fMin) < min(m_fMax, Other.m_fMax); }
		_float	Get_Overlap(CProjection Other) { return min(m_fMax, Other.m_fMax) - max(m_fMin, Other.m_fMin);}
		
	private:
		_float m_fMin = 0.f;
		_float m_fMax = 0.f;
	};

public:
	enum TYPE { TYPE_BOX, TYPE_SPHERE, TYPE_END};
protected:
	explicit CCollider(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
	virtual _int Update_Collider(_matrix StateMatrix);
	virtual HRESULT Render_Collider();

public:
	void	Set_Trigger(_bool isTrigger) { m_bTrigger = isTrigger; }




public:
	virtual	_bool	Check_Collision(CCollider* pTarget, _float& fCollisionLength, _float3& vPushDir) = 0;

//OBB를 위함
public:
	virtual	vector<_float3>	Get_Axis();
	virtual CProjection Project(_float3 axis);


public:
	_float3	Get_Size();
	_float3	Get_Pos();
	_bool	IsTrigger() { return m_bTrigger; }
	TYPE	Get_Type() { return m_eType; }
protected:
	_matrix				m_StateMatrix;
	CPipeLine*			m_pPipeLine = nullptr;
	_bool				m_isColl = false;
	//밀어내지 않고 충돌만 감지하는 콜라이더인가?
	_bool				m_bTrigger = false;
	//콜라이더 타입
	TYPE				m_eType = TYPE_END;


public:
	virtual CComponent* Clone_Component(void* pArg) = 0;
	virtual void Free();
};

END
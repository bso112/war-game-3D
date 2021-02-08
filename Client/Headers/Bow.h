#pragma once
#include "Weapon.h"

BEGIN(Client)

class CArrow;
class CBow final : public CWeapon
{
private:
	explicit CBow(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBow(const CBow& rhs);
	virtual ~CBow() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Attack(_double TimeDelta);

	virtual HRESULT Attack(_double TimeDelta, _float Power);

	//getter
public:
	_float Get_MaxPower() { return m_fPower_Max; }

private:
	LPD3DXLINE	m_pLine = nullptr;

	_double		m_AccTimer = 0.0;
	_float		m_fPower_Min = 0.f;
	_float		m_fPower = 0.f;
	_float		m_fPower_Max = 0.f;
	_float		m_fPower_Stack = 0.f; // 최대값1 기준
	_float		m_fWidth = 1.f;
	_bool		m_isAttack = false;

	_double		m_dblHoldingTime = 0.0;
	_double		m_dblHoldingMax = 5.0;

private:
	virtual HRESULT OnKeyPressing(_int KeyCode);
	virtual HRESULT	OnKeyUp(_int KeyCode);

private:
	HRESULT Add_Component();
	HRESULT Ready_Layer_Arrow(const _tchar* pLayerTag);
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_ConstantTable_to_Default_Shader();
	HRESULT Draw_Line();

public:
	_float3	Predict_Position(_float3 vCurPos, _float3 vLook, _double dbleTime);

private:
	CCVIBuffer_Aim*	m_pCVIBuffer_Aim = nullptr;
	CShader*		m_pDefault_Shader = nullptr;
	_bool			m_bAiming = 0;

public:
	static CBow* Create_Bow_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free();
};

END
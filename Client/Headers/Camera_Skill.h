#pragma once
#include "Camera.h"

BEGIN(Client)

class CCamera_Skill final : public CCamera
{
private:
	SKILL		m_eSkill;
	_float		m_fSkillRange = 0.f;
	_float		m_FOV = 0.f;

private:
	_float3				m_vSrcLook;
	_float3				m_vDstLook;
	_bool					m_bLerp = false;
	_float					m_fLerpTime = 0.f;
	_float					m_fT = 0.f;

private:
	explicit CCamera_Skill(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Skill(const CCamera_Skill& rhs);
	virtual ~CCamera_Skill() = default;

public:
	SKILL Get_Skill() { return m_eSkill; }
	_float Get_SkillRange() { return m_fSkillRange; }

public:
	void Set_Lerp(_float3 _vSrcPos, _float3 _vDstPos, _float3 _vSrcLook, _float3 _vDstLook, _float _fLerpTime);
	void Set_Skill(SKILL _eSkill) { m_eSkill = _eSkill; }
	void Set_SkillRange(_float _Range) { m_fSkillRange = _Range; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	HRESULT Lerp_Camera(_double _DeltaTime);
	HRESULT Rotate_Camera(_double _DeltaTime);
	HRESULT Skill_End();

public:
	virtual HRESULT	OnKeyDown(_int KeyCode) override;
	virtual HRESULT	Exit_Camera() override;

public:
	static CCamera_Skill* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
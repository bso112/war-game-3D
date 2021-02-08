#include "stdafx.h"
#include "..\Headers\Camera_Skill.h"
#include "Management.h"
#include "Camera_Manager.h"
#include "GameManager.h"
#include "PickingMgr.h"
#include "Terrain.h"
#include "Meteor.h"
#include "HealingZone.h"
#include "Bullet_Trail.h"
#include "SoundMgr.h"
CCamera_Skill::CCamera_Skill(PDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Skill::CCamera_Skill(const CCamera_Skill & rhs)
	: CCamera(rhs)
{

}

void CCamera_Skill::Set_Lerp(_float3 _vSrcPos, _float3 _vDstPos, _float3 _vSrcLook, _float3 _vDstLook, _float _fLerpTime)
{
	m_StateDesc.vSrc = _vSrcPos;
	m_StateDesc.vDst = _vDstPos;
	m_bLerp = true;
	m_vSrcLook = _vSrcLook;
	m_vDstLook = _vDstLook;
	m_fLerpTime = _fLerpTime;
}

HRESULT CCamera_Skill::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Skill::Ready_GameObject(void * pArg)
{
	if (CCamera::Ready_GameObject(pArg))
		return E_FAIL;

	CKeyMgr::Get_Instance()->RegisterObserver(this);
	m_FOV = m_StateDesc.fFovy;
	return S_OK;
}

_int CCamera_Skill::Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (m_bLerp)
	{
		if (CGameManager::Get_Instance()->Get_CurrSkill() == SKILL_SNIPE)
		{
			CGameObject* pGameObject = CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Skill");
			pGameObject->Set_Active(true);
		}

		if (FAILED(Lerp_Camera(TimeDelta)))
			return E_FAIL;
	}
	else
	{
		if (CGameManager::Get_Instance()->Get_CurrSkill() == SKILL_SNIPE)
		{
			CGameObject* pGameObject = CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Skill");
			pGameObject->Set_Active(true);
		}

		if (FAILED(Rotate_Camera(TimeDelta)))
			return -1;
	}

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Skill::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	return CCamera::Late_Update_GameObject(TimeDelta);
}

HRESULT CCamera_Skill::Render_GameObject()
{
	if (!m_bActive)
		return S_OK;
	return S_OK;
}

HRESULT CCamera_Skill::Lerp_Camera(_double _DeltaTime)
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

HRESULT CCamera_Skill::Rotate_Camera(_double _DeltaTime)
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	switch (m_eSkill)
	{
	case SKILL::SKILL_METEOR:
		break;
	case SKILL::SKILL_HEAL:
		break;
	case SKILL::SKILL_SNIPE:
	{
		_long	MouseMove = 0;

		if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_X))
		{
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), _DeltaTime * (MouseMove / 20.0));
		}

		if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
		{
			m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _DeltaTime * (MouseMove / 20.0));
		}
		//100,150
		_float fMfov = 1.f;
		_float fmfov = 0.1f;
		_float fMblur = 150;
		_float fmBlur = 99.9;
		_float fMblur_skill = 15;
		_float fmBlur_skill = 5;

		if (0 < pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_WHEEL))
		{
			CSoundMgr::Get_Instance()->PlaySound_Overwrite(L"scope.wav", CSoundMgr::CHANNELID::EFFECT);
			m_StateDesc.fFovy -= (_float)_DeltaTime * 2.f;
			if (m_StateDesc.fFovy < fmfov)
				m_StateDesc.fFovy = fmfov;
		}

		else if (0 > pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_WHEEL))
		{
			CSoundMgr::Get_Instance()->PlaySound_Overwrite(L"scope.wav", CSoundMgr::CHANNELID::EFFECT);
			m_StateDesc.fFovy += (_float)_DeltaTime * 2.f;
			if (m_StateDesc.fFovy > fMfov)
				m_StateDesc.fFovy = fMfov;
		}
		CPickingMgr::Get_Instance()->Set_Blur_mDist((m_StateDesc.fFovy - fmfov) * (fmBlur - fmBlur_skill) / (fMfov - fmfov) + fmBlur_skill);
		CPickingMgr::Get_Instance()->Set_Blur_MDist((m_StateDesc.fFovy - fmfov) * (fMblur - fMblur_skill) / (fMfov - fmfov) + fMblur_skill);
		break;
	}
	default:
		break;
	}

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CCamera_Skill::Skill_End()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CTerrain* pTerrain = dynamic_cast<CTerrain*>(CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_Terrain"));
	if (nullptr == pTerrain)
	{
		MSG_BOX("NO TERRAIN");
		return E_FAIL;
	}
	pTerrain->Set_Skill(SKILL_END);

	CGameManager::Get_Instance()->Set_Skill(SKILL_END);
	CUI* pUI = dynamic_cast<CUI*>(pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Skill"));
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Active(false);
	m_StateDesc.fFovy = m_FOV;

	/////////////////
	_float fMblur = 150;
	_float fmBlur = 100;
	CPickingMgr::Get_Instance()->Set_Blur_mDist(fmBlur);
	CPickingMgr::Get_Instance()->Set_Blur_MDist(fMblur);
	/////////////////

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CCamera_Skill::OnKeyDown(_int KeyCode)
{
	if (!m_bActive)
		return S_OK;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_float3 Dst = {};
	POINT point = {};
	GetCursorPos(&point);
	ScreenToClient(g_hWnd, &point);

	CPickingMgr::Get_Instance()->Get_TerrainPos(point, &Dst);

	if (VK_LBUTTON == KeyCode)
	{
		switch (CGameManager::Get_Instance()->Get_CurrSkill())
		{
		case SKILL_METEOR:
		{
			CMeteor::STATEDESC metDesc;
			metDesc.fDamage = 100.f;
			metDesc.fSpeed = 300.f;
			metDesc.fRadius = m_fSkillRange;
			metDesc.vDstPosition = Dst;
			metDesc.vPosition = _float3(Dst.x, Dst.y + 1000.f, Dst.z);
			metDesc.vScale = _float3(100.f, 100.f, 1.f);

			pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Meteor", SCENE_STAGE, L"Layer_Skill", &metDesc);

			CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
			if (nullptr == pSoundMgr) return E_FAIL;

			pSoundMgr->PlaySound_Overwrite(L"meteor.mp3", CSoundMgr::CHANNELID::EFFECT);
			CGameManager::Get_Instance()->Use_Skill(SKILL_METEOR);
			break;
		}
		case SKILL_HEAL:
		{
			CHealingZone::STATEDESC HealDesc;
			HealDesc.vPosition = _float3(Dst.x, Dst.y + 0.1f, Dst.z);
			HealDesc.fRadius = m_fSkillRange;
			HealDesc.fRecoverHp = 5.f;
			HealDesc.fRecoverTime = 1.f;
			HealDesc.fLifeTime = 10.f;

			pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_HealingZone", SCENE_STAGE, L"Layer_Skill", &HealDesc);

			CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
			if (nullptr == pSoundMgr)
				return E_FAIL;

			pSoundMgr->PlaySound_Overwrite(L"heal.mp3", CSoundMgr::CHANNELID::EFFECT);
			CGameManager::Get_Instance()->Use_Skill(SKILL_HEAL);
			break;
		}
		case SKILL_SNIPE:
		{
			CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
			if (nullptr == pSoundMgr) return E_FAIL;

			pSoundMgr->PlaySound_Overwrite(L"snipe.mp3", CSoundMgr::CHANNELID::EFFECT);
			CGameManager::Get_Instance()->Use_Skill(SKILL_SNIPE);

			CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
			if (nullptr == pPickingMgr)
				return E_FAIL;

			POINT pt = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
			_float3 vPosition = {};
			pPickingMgr->Get_TerrainPos(pt, &vPosition);

			CBullet_Trail::STATEDESC bulletDesc;
			bulletDesc.fLifeTime = 0.5f;
			bulletDesc.vSrc = m_pTransformCom->Get_Position();
			bulletDesc.vDst = vPosition;
			bulletDesc.vScale = _float3(0.5f, 0.5f, 0.f);
			if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BulletTrail", SCENE_STAGE, L"Layer_BulletTrail", &bulletDesc)))
			{
				Safe_Release(pManagement);
				return E_FAIL;
			}

			//피킹한 오브젝트를 저장
			CGameObject* pPicked = pPickingMgr->Picking(pt);
			if (nullptr == pPicked)
				break;

			//피킹된 게임오브젝트로 할일 하기.
			CUnit* pUnit = dynamic_cast<CUnit*>(pPicked);
			if (nullptr != pUnit)
			{
				if (!pUnit->Is_Friendly())
					pUnit->TakeDamage(100);
			}
			break;
		}
		case SKILL_END:
		default:
			break;
		}
		if (nullptr != dynamic_cast<CInfantry*>(m_pTarget))
		{
			CCamera_Manager::Get_Instance()->Change_Camera_Third_Person(m_pTarget, 1);
		}
		else
		{
			CCamera_Manager::Get_Instance()->Change_Camera_Free(false, CCamera_Manager::STATE_FREE, m_pTransformCom->Get_Position(), m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		}
	}
	if (nullptr != m_pTarget)
	{
		((CInfantry*)m_pTarget)->Set_ControllMode(true);
	}
	if ('Q' == KeyCode)
	{
		if (nullptr != dynamic_cast<CInfantry*>(m_pTarget))
		{
			CCamera_Manager::Get_Instance()->Change_Camera_Third_Person(m_pTarget, 1);
		}
		Skill_End();
	}


	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CCamera_Skill::Exit_Camera()
{
	Skill_End();
	return S_OK;
}

CCamera_Skill * CCamera_Skill::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Skill*	pInstance = new CCamera_Skill(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CLoading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Skill::Clone_GameObject(void * pArg)
{
	CCamera_Skill*	pInstance = new CCamera_Skill(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating CLoading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Skill::Free()
{
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);
	CCamera::Free();
}

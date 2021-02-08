#include "stdafx.h"
#include "..\Headers\Camera_Manager.h"
#include "Management.h"
#include "UI_Manager.h"
#include "Terrain.h"
#include "GameManager.h"

USING(Client)
IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{
	ZeroMemory(m_pCameras, sizeof(CCamera*) * CCamera::CAM_END);
}

CCamera_Manager::~CCamera_Manager()
{
}

HRESULT CCamera_Manager::Add_Camera(CCamera::CAM_TYPE _eType, CCamera * _pCamera)
{
	if (CCamera::CAM_END <= _eType || nullptr != m_pCameras[_eType] || nullptr == _pCamera)
		return E_FAIL;

	m_pCameras[_eType] = _pCamera;
	Safe_AddRef(_pCamera);
	return S_OK;
}

HRESULT CCamera_Manager::Resume_Camera()
{
	Change_Camera_Free(true, STATE_FREE, m_FreePrevPosition, m_FreePrevLook);
	return S_OK;
}

HRESULT CCamera_Manager::Change_CamState(CAM_STATE _eState)
{
	if (STATE_END <= _eState)
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	if (nullptr == pUIManager)
		return E_FAIL;
	Safe_AddRef(pUIManager);

	switch (_eState)
	{
	case CCamera_Manager::STATE_FREE:
		pUIManager->Change_UI_State(CUI_Manager::UI_FREE);
		break;

	case CCamera_Manager::STATE_COMMAND:
		pUIManager->Change_UI_State(CUI_Manager::UI_COMMAND);
		break;

	case CCamera_Manager::STATE_BATTLE:
		pUIManager->Change_UI_State(CUI_Manager::UI_BATTLE);
		break;

	case CCamera_Manager::STATE_SKILL:
		pUIManager->Change_UI_State(CUI_Manager::UI_SKILL);
		break;

	case CCamera_Manager::STATE_WAYPOINT:
		pUIManager->Change_UI_State(CUI_Manager::UI_NONE);
		break;
	default:
		break;
	}

	Set_CamState(_eState);

	Safe_Release(pUIManager);
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera_Free(_bool _Lerp, CAM_STATE _eState, _float3 _vPosition, _float3 _vLook)
{
	Change_CamState(_eState);
	m_ePrevType = m_eCurrentType;

	_matrix PrevMatrix = m_pCurrentCamera->Get_Transform()->Get_WorldMatrix();
	m_pCurrentCamera->Set_Active(false);
	m_pCurrentCamera->Exit_Camera();

	m_pCurrentCamera = m_pCameras[CCamera::CAM_FREE];
	m_eCurrentType = CCamera::CAM_FREE;

	CCamera_Free* pCamera_Free = dynamic_cast<CCamera_Free*>(m_pCurrentCamera);
	if (nullptr == pCamera_Free)
		return E_FAIL;

	pCamera_Free->Set_Active(true);
	pCamera_Free->Set_Lerp(_Lerp);

	if (_eState == CAM_STATE::STATE_COMMAND)
		pCamera_Free->Set_isLock(true);
	else
		pCamera_Free->Set_isLock(false);

	if (_Lerp)
		pCamera_Free->Set_Lerp((_float3)PrevMatrix.m[3], _vPosition, (_float3)PrevMatrix.m[2], _vLook, 1.f);
	else
		pCamera_Free->Get_Transform()->Set_WorldMatrix(PrevMatrix);

	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera_Third_Person(CGameObject* _pGameObject, _float _ChangeTime)
{
	if (nullptr == _pGameObject)
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CUI* pUI = (CUI*)pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_UI_Battle", 1);
	if (nullptr == pUI)
		return E_FAIL;

	CInfantry* pUnit = dynamic_cast<CInfantry*>(_pGameObject);
	if (nullptr == pUnit)
		return E_FAIL;

	pUnit->Set_Picked(true);

	switch (pUnit->Get_OCC())
	{
	case CInfantry::OCC_WARRIOR:
		pUI->Set_TextureIndex(4);
		break;
	case CInfantry::OCC_ARCHER:
		pUI->Set_TextureIndex(5);
		break;
	case CInfantry::OCC_MAGICIAN:
		pUI->Set_TextureIndex(6);
		break;
	default:
		break;
	}

	Change_CamState(STATE_BATTLE);

	m_ePrevType = m_eCurrentType;
	m_pCurrentCamera->Set_Active(false);
	m_pCurrentCamera->Set_isLock(false);
	m_pCurrentCamera->Exit_Camera();

	m_FreePrevLook = m_pCurrentCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	m_FreePrevPosition = m_pCurrentCamera->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	m_pCurrentCamera = m_pCameras[CCamera::CAM_THIRDPERSON];
	m_eCurrentType = CCamera::CAM_THIRDPERSON;

	CCamera_Third_Person* pThird_PersonCam = dynamic_cast<CCamera_Third_Person*>(m_pCurrentCamera);
	if (nullptr == pThird_PersonCam)
		return E_FAIL;

	pThird_PersonCam->Set_Active(true);
	pThird_PersonCam->Set_Src(m_FreePrevPosition);
	pThird_PersonCam->Set_Lerp(m_FreePrevLook, _ChangeTime);
	pThird_PersonCam->Set_Target(_pGameObject);

	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera_Skill(CGameObject* _pGameObject, SKILL _eSkill)
{
	if (nullptr == _pGameObject)
		return E_FAIL;

	Change_CamState(STATE_SKILL);
	CGameManager::Get_Instance()->Set_Skill(_eSkill);

	_matrix vMatrix = m_pCurrentCamera->Get_Transform()->Get_WorldMatrix();
	_float3 vLook = (_float3)vMatrix.m[2];
	_float3 vBack = -(_float3)vMatrix.m[2];
	_float3 vPrevPos = (_float3)vMatrix.m[3];
	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vBack, &vBack);
	_float3 vLerpPos = vPrevPos + vBack * 100 +_float3(0.f, 1.f, 0.f) * 100;
	_float3 vLerpLook = (vPrevPos + vLook * 100) - vLerpPos;
	D3DXVec3Normalize(&vLerpLook, &vLerpLook);

	m_ePrevType = m_eCurrentType;
	m_pCurrentCamera->Set_Active(false);
	m_pCurrentCamera->Exit_Camera();

	m_pCurrentCamera = m_pCameras[CCamera::CAM_ROTATE];
	m_eCurrentType = CCamera::CAM_ROTATE;

	CCamera_Skill* pSkillCam = dynamic_cast<CCamera_Skill*>(m_pCurrentCamera);
	if (nullptr == pSkillCam)
		return E_FAIL;

	pSkillCam->Set_Active(true);
	pSkillCam->Set_Target(_pGameObject);
	pSkillCam->Set_Lerp((_float3)vMatrix.m[3], vLerpPos, vLook, vLerpLook, 0.5);
	pSkillCam->Set_Skill(_eSkill);

	switch (_eSkill)
	{
	case SKILL::SKILL_METEOR:
		pSkillCam->Set_SkillRange(30.f);
		break;
	case SKILL::SKILL_HEAL:
		pSkillCam->Set_SkillRange(50.f);
		break;
	default:
		break;
	}
	CGameManager::Get_Instance()->Set_SkillRange(pSkillCam->Get_SkillRange());

	CTerrain* pTerrain = dynamic_cast<CTerrain*>(CManagement::Get_Instance()->Get_ObjectPointer(SCENE_STAGE, L"Layer_Terrain"));
	if (nullptr == pTerrain)
		return E_FAIL;

	pTerrain->Set_Skill(_eSkill);
	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera_WayPoint(_float3 _vStartLook, vector<CCamera_WayPoint::WAYPOINT> _vecWayPoint)
{
	Change_CamState(STATE_WAYPOINT);

	m_ePrevType = m_eCurrentType;
	m_pCurrentCamera->Set_Active(false);
	m_pCurrentCamera->Exit_Camera();

	m_pCurrentCamera = m_pCameras[CCamera::CAM_WAYPOINT];
	m_eCurrentType = CCamera::CAM_WAYPOINT;

	CCamera_WayPoint* pWayPointCam = dynamic_cast<CCamera_WayPoint*>(m_pCurrentCamera);
	if (nullptr == pWayPointCam)
		return E_FAIL;

	pWayPointCam->Set_Active(true);
	pWayPointCam->Set_Look(*D3DXVec3Normalize(&_vStartLook, &_vStartLook));
	pWayPointCam->Set_WayPoint(_vecWayPoint);
	return S_OK;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
void CCamera_Manager::Free()
{
	for (CCamera* pCamera : m_pCameras)
		Safe_Release(pCamera);
}

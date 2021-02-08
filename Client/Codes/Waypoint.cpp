#include "stdafx.h"
#include "WayPoint.h"
#include "Unit.h"
#include "Flagpole.h"
#include "Flag.h"
#include "Infantry.h"
#include "InputManager.h"
#include "SoundMgr.h"
#include "GameManager.h"



USING(Client)

CWaypoint::CWaypoint(PDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CWaypoint::CWaypoint(const CWaypoint & rhs)
	: CGameObject(rhs)
{
}


HRESULT CWaypoint::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CWaypoint::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(STATEDESC));


	CTransform::STATEDESC tTransformDesc;
	tTransformDesc.RotatePerSec = D3DXToRadian(90.f);
	tTransformDesc.SpeedPerSec = 5.0;


	if (FAILED(Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Component(SCENE_STATIC, L"Component_Texture_Waypoint", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader_Rect", (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer_Rect", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;


	m_pTransform->SetUp_Position(m_tDesc.tWaypoint.vPosition);

	_float offSet = 60.f;
	m_pTransform->SetUp_Scale(_float3(m_tDesc.tWaypoint.fRadius + offSet, m_tDesc.tWaypoint.fRadius + offSet, m_tDesc.tWaypoint.fRadius + offSet));

	m_pTransform->SetUp_Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));

	Compute_ViewZ(m_pTransform->Get_Position());

	if (false == m_tDesc.tWaypoint.bHide)
	{
		// For.Layer_Flagpole
		if (FAILED(Ready_Layer_Flagpole(L"Layer_Flagpole")))
			return E_FAIL;
	}

#ifdef _DEBUG
	if (true == m_tDesc.tWaypoint.bHide)
	{
		// For.Layer_Flagpole
		if (FAILED(Ready_Layer_Flagpole(L"Layer_Flagpole")))
			return E_FAIL;
	}
#endif // _DEBUG

	if (false == m_tDesc.tWaypoint.bHide)
	{
		// For.Layer_Flagpole
		if (FAILED(Ready_Layer_Basecamp(L"Layer_Basecamp")))
			return E_FAIL;
	}

	Ready_Setting();

	// 0번 웨이포인트 > 아군 진지로 시작
	if (0 == m_tDesc.tWaypoint.iNumber)
		m_pFlagpole->Set_OccupiedRate(m_dOccupyRate);

	return S_OK;
}

_int CWaypoint::Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pBasecamp)
		return 0;

	//적군 웨이포인트에서만
	if (!m_tDesc.tWaypoint.bOccupied)
	{
		//최대 유닛수 제한
		if (Get_CurrEnemyCnt() >= m_UnitOccs.size())
			return 0;

		//시간마다 적군 유닛생성
		m_dSpawnEnemyTimer += TimeDelta;
		if (m_dSpawnEnemyTimer > SPAWN_SPEED)
		{
			CTransform* pBaseCampTransform = (CTransform*) m_pBasecamp->Find_Component(L"Com_Transform");
			if (nullptr == pBaseCampTransform)
				return 0;

			Spawn_Enemy(pBaseCampTransform->Get_Position());
			m_dSpawnEnemyTimer = 0.0;


		}

	}
	return _int();
}

_int CWaypoint::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;


	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	return _int();
}

HRESULT CWaypoint::Render_GameObject()
{
	if (m_tDesc.tWaypoint.bHide)
		return NO_ERROR;

	_matrix			matView, matProj;

	CManagement* pEnginMgr = CManagement::Get_Instance();
	if (nullptr == pEnginMgr) return E_FAIL;
	matView = pEnginMgr->Get_Transform(D3DTS_VIEW);
	matProj = pEnginMgr->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pTexture->Set_TextureOnShader(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matView", &matView, sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &matProj, sizeof(_matrix))))
		return E_FAIL;

	_int ShaderPass = 0;

	//만약 웨이포인트가 점령되지 않았으면 빨간색으로 렌더
	if (!m_tDesc.tWaypoint.bOccupied)
	{
		_float4 vColor = _float4(0.89f, 0.05f, 0.09f, 1.f);
		if (FAILED(m_pShader->Set_Value("g_Color", &vColor, sizeof(_float4))))
			return E_FAIL;

		ShaderPass = PASS_RECT_COLOR;
	}

	ALPHABLEND;

	if (FAILED(m_pShader->Begin_Shader()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin_Pass(ShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render_VIBuffer()))
		return E_FAIL;

	if (FAILED(m_pShader->End_Pass()))
		return E_FAIL;

	if (FAILED(m_pShader->End_Shader()))
		return E_FAIL;

	ALPHABLEND_END;

	return S_OK;
}

void CWaypoint::Occupy(_double dAccupyRate)
{
	//10번 웨이포인트는 보스가 죽기 전까지는 정복불가
	if (m_tDesc.tWaypoint.iNumber == 10)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement) return;
		list<CGameObject*>* pBoss = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Boss");
		if (!pBoss->empty())
			return;
	}

	m_dOccupyRate = min(100.0, m_dOccupyRate + dAccupyRate);
	m_dOccupyRate = max(0.0, m_dOccupyRate);

	if (m_dOccupyRate >= 100)
	{
		//전에 점령되지 않은 상태였으면
		if (m_tDesc.tWaypoint.bOccupied == false)
		{
			m_tDesc.tWaypoint.bOccupied = true;
			OnOccupy();
		}
		m_tDesc.tWaypoint.bOccupied = true;
	}
	else if (m_dOccupyRate <= 0)
	{
		//전에 점령된 상태였으면
		if (m_tDesc.tWaypoint.bOccupied == true)
		{
			m_tDesc.tWaypoint.bOccupied = false;
			OnOccupy();
		}
		m_tDesc.tWaypoint.bOccupied = false;

	}

	if (nullptr != m_pFlagpole)
		m_pFlagpole->Set_OccupiedRate(m_dOccupyRate);



	if (IsCommanderInWaypoint())
		OnCommanderInWaypoint();

}
void CWaypoint::OnCommanderInWaypoint()
{

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return;

	list<CGameObject*>* enemys = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");

	_float3 vDest;

	//장군이 숨겨진 웨이포인트에 진입했으면
	if (m_tDesc.tWaypoint.bHide)
	{
		//웨이포인트에 해당하는 게임오브젝트들 활성화
		for (auto& pEnemy : *enemys)
		{
			CUnit* pUnit = dynamic_cast<CUnit*>(pEnemy);
			if (m_tDesc.tWaypoint.iNumber == pUnit->Get_WaypointNum())
			{
				pEnemy->Set_Active(true);
			}
		}

		// 9번 웨이포인트 화성화
		if (9 == m_tDesc.tWaypoint.iNumber)
		{
			for (auto& pEnemy : *enemys)
			{
				CInfantry* pUnit = dynamic_cast<CInfantry*>(pEnemy);
				if (nullptr == pUnit)
					continue;

				if (5 == pUnit->Get_WaypointNum() || 7 == pUnit->Get_WaypointNum())
				{
					pUnit->Set_IsEvent2(true);
				}
			}
		}
	}
	// 6번 웨이포인트 활성화
	else
	{
		if (6 == m_tDesc.tWaypoint.iNumber)
		{
			for (auto& pEnemy : *enemys)
			{
				CInfantry* pUnit = dynamic_cast<CInfantry*>(pEnemy);
				if (nullptr == pUnit)
					continue;

				if (4 == pUnit->Get_WaypointNum())
				{
					pUnit->Set_IsEvent1(true);
				}
			}
		}
	}
}
void CWaypoint::OnSetActive(_bool bActive)
{
	if (nullptr != m_pFlag && nullptr != m_pFlagpole)
	{
		m_pFlag->Set_Active(false);
		m_pFlagpole->Set_Active(false);
	}
}
HRESULT CWaypoint::Spawn_Enemy(_float3 _vSpawnPos)
{
	//지휘관이 밟고 있으면 생성안됨.
	if (IsCommanderInWaypoint())
		return S_OK;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CInfantry::INFANTRYDESC InfantryDesc;
	CGameObject* pInfantry = nullptr;
	InfantryDesc.bFriendly = false;
	InfantryDesc.tBasedesc = BASEDESC(_vSpawnPos, _float3(2.f, 2.f, 2.f));
	InfantryDesc.tOccupation = m_UnitOccs[m_iSpawnUnitIndex++];
	InfantryDesc.bStatic = false;
	InfantryDesc.iNumber = m_tDesc.tWaypoint.iNumber;

	//Layer_Unit_Enemy
	if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, L"Layer_Unit_Enemy", &InfantryDesc)))
		return E_FAIL;

	if (m_iSpawnUnitIndex >= m_UnitOccs.size())
		m_iSpawnUnitIndex = 0;


	return S_OK;
}

HRESULT CWaypoint::Spawn_Ally(_float3 _vSpawnPos, _uint iUnitOccupation)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CInfantry::INFANTRYDESC InfantryDesc;
	CGameObject* pInfantry = nullptr;
	InfantryDesc.bFriendly = true;
	InfantryDesc.tBasedesc = BASEDESC(_vSpawnPos, _float3(2.f, 2.f, 2.f));
	InfantryDesc.tOccupation = (CInfantry::OCCUPATION)iUnitOccupation;
	InfantryDesc.bStatic = false;
	InfantryDesc.iNumber = m_tDesc.tWaypoint.iNumber;

	//Layer_Unit_Enemy
	if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, L"Layer_Unit_Ally", &InfantryDesc)))
		return E_FAIL;

	return S_OK;

}

_bool CWaypoint::IsCommanderInWaypoint()
{
	// 지휘관 웨이포이트 터치

	CInputManager* pInputMgr = CInputManager::Get_Instance();
	if (nullptr == pInputMgr)
		return false;

	CUnit* pCommnader = pInputMgr->Get_PickedUnit();
	if (nullptr == pCommnader)
		return false;

	if (m_tDesc.tWaypoint.fRadius > D3DXVec3Length(&(m_tDesc.tWaypoint.vPosition - pCommnader->Get_Position())))
		return true;

	return false;
}

void CWaypoint::OnOccupy()
{
	//아군점령
	if (m_tDesc.tWaypoint.bOccupied)
	{
		if (m_tDesc.tWaypoint.iNumber == 10)
		{
			CSoundMgr::Get_Instance()->StopAll();
			CSoundMgr::Get_Instance()->PlaySound(L"victory.mp3", CSoundMgr::CHANNELID::EFFECT);
			CSoundMgr::Get_Instance()->PlaySound(L"man_victory_3.ogg", CSoundMgr::CHANNELID::PLAYER_VOICE);
			CSoundMgr::Get_Instance()->PlaySound(L"man_victory_4.ogg", CSoundMgr::CHANNELID::WEAPON);
			CSoundMgr::Get_Instance()->PlaySound(L"man_victory_5.ogg", CSoundMgr::CHANNELID::UI);

		}
		else
			CSoundMgr::Get_Instance()->PlaySound(L"occupy.mp3", CSoundMgr::CHANNELID::EFFECT);
	}
	//적군점령
	else
		CSoundMgr::Get_Instance()->PlaySound(L"fail.mp3", CSoundMgr::CHANNELID::EFFECT);



}

size_t CWaypoint::Get_CurrEnemyCnt()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	list<CGameObject*>* enemys = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");

	size_t iUnitCnt = 0;
	for (auto* pEnemy : *enemys)
	{
		if (m_tDesc.tWaypoint.iNumber == ((CUnit*)pEnemy)->Get_WaypointNum())
			++iUnitCnt;
	}
	return iUnitCnt;
}

HRESULT CWaypoint::Ready_Layer_Flagpole(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_float3 vPos = m_pTransform->Get_WorldPosition();

	CFlagpole::STATEDESC StateDesc;
	StateDesc.vScale = _float3(m_tDesc.tWaypoint.fRadius * 0.5f, m_tDesc.tWaypoint.fRadius * 1.5f, 1.f);
	StateDesc.vPos = _float3(vPos.x, vPos.y + StateDesc.vScale.y * 0.4f, vPos.z);
	m_pFlagpole = dynamic_cast<CFlagpole*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Flagpole", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == m_pFlagpole)
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}


HRESULT CWaypoint::Ready_Layer_Flag(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_float3 vPos = m_pTransform->Get_WorldPosition();

	CFlag::STATEDESC StateDesc;
	StateDesc.eFlagID = CFlag::FLAG_RED;
	StateDesc.vScale = _float3(m_tDesc.tWaypoint.fRadius * 0.4f, m_tDesc.tWaypoint.fRadius * 0.4f, 1.f);
	StateDesc.vPos = _float3(vPos.x, vPos.y + m_tDesc.tWaypoint.fRadius - StateDesc.vScale.y, vPos.z);	// 깃대 높이 - 깃발 길이
	m_pFlag = dynamic_cast<CFlag*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Flag", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == m_pFlag)
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CWaypoint::Ready_Layer_Basecamp(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CBasecamp::STATEDESC StateDesc;
	StateDesc.vScale = _float3(1.f, 1.f, 1.f) * m_tDesc.tWaypoint.fRadius * 0.5f;
	StateDesc.vWaypointPos = m_tDesc.tWaypoint.vPosition;

	if (10 == m_tDesc.tWaypoint.iNumber)
	{
		Safe_Release(pManagement);
		return NO_ERROR;
	}
	if (0 == m_tDesc.tWaypoint.iNumber)
	{
		StateDesc.vOffset = _float3(-m_tDesc.tWaypoint.fRadius, StateDesc.vScale.y * 0.3f, m_tDesc.tWaypoint.fRadius);
		m_pBasecamp = dynamic_cast<CBasecamp*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Basecamp", SCENE_STAGE, pLayerTag, &StateDesc));
		if (nullptr == m_pBasecamp)
			return E_FAIL;
	}
	else
	{
		StateDesc.vOffset = _float3(m_tDesc.tWaypoint.fRadius, StateDesc.vScale.y * 0.4f, m_tDesc.tWaypoint.fRadius);
		m_pBasecamp = dynamic_cast<CBasecamp*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Basecamp", SCENE_STAGE, pLayerTag, &StateDesc));
		if (nullptr == m_pBasecamp)
			return E_FAIL;
	}

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CWaypoint::Ready_Setting()
{
	//본진
	if (m_tDesc.tWaypoint.iNumber == 0)
	{
		m_dOccupyRate = 100;
		m_tDesc.tWaypoint.bOccupied = true;
	}
#pragma region 생성유닛 패턴 정하기

	switch (m_tDesc.tWaypoint.iNumber)
	{
	case 1:
		m_UnitOccs.reserve(5);
		for (int i = 0; i < 5; ++i)
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);

		break;
	case 2:
		m_UnitOccs.reserve(6);
		for (int i = 0; i < 2; ++i)
		{
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
			m_UnitOccs.push_back(CInfantry::OCC_ARCHER);
		}
		break;
	case 4:
		m_UnitOccs.reserve(6);
		for (int i = 0; i < 2; ++i)
		{
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
			m_UnitOccs.push_back(CInfantry::OCC_ARCHER);
		}
		break;
	case 5:
		m_UnitOccs.reserve(5);
		for (int i = 0; i < 5; ++i)
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
		break;
	case 6:
		m_UnitOccs.reserve(5);
		for (int i = 0; i < 5; ++i)
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
		break;
	case 7:
		m_UnitOccs.reserve(5);
		for (int i = 0; i < 5; ++i)
			m_UnitOccs.push_back(CInfantry::OCC_WARRIOR);
		break;
	default:
		break;
	}

#pragma endregion



	return S_OK;
}


CWaypoint * CWaypoint::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CWaypoint*	pInstance = new CWaypoint(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CWaypoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWaypoint::Clone_GameObject(void * pArg)
{
	CWaypoint*	pInstance = new CWaypoint(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CWaypoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaypoint::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	CGameObject::Free();
}

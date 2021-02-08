#include "stdafx.h"
#include "..\Headers\Boss.h"
#include "BossParts.h"
#include "PickingMgr.h"
#include "Infantry.h"
#include "SoundMgr.h"
#include "GameManager.h"

CBoss::CBoss(PDIRECT3DDEVICE9 pGraphic_Device)
	: CUnit(pGraphic_Device)
{
}

CBoss::CBoss(const CBoss & rhs)
	: CUnit(rhs)
{
}

HRESULT CBoss::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBoss::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_BaseDesc, pArg, sizeof(BASEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_BaseDesc.vSize);
	m_pTransformCom->SetUp_Rotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(225.f));
	m_pTransformCom->SetUp_Position(m_BaseDesc.vPos);

	m_vOriginalPos = m_BaseDesc.vPos;

	m_fHeight = m_BaseDesc.vPos.y;

	m_tStat.iAtt = 30;
	m_tStat.iMaxHp = 100;
	m_tStat.iCurrHp = m_tStat.iMaxHp;
	m_tStat.fRecogRange = 30.f;
	m_tStat.fAggroRange = 30.f;
	m_tStat.fAttackRange = 5.f;
	m_tStat.fAttackSpeed = 1.f;

	// For.Layer_BossParts
	if (FAILED(Ready_Layer_BossParts(L"Layer_BossParts")))
		return E_FAIL;

	return S_OK;
}

_int CBoss::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
	{
		for (size_t i = 0; i < CBossParts::OBJ_END; i++)
			m_vecBossParts[i]->Set_Dead();

		return -1;
	}

	m_tStat.iCurrHp;

	if (0x8000 & GetKeyState(VK_NUMPAD8))
		m_pTransformCom->Go_Straight(TimeDelta);

	if (0x8000 & GetKeyState(VK_NUMPAD4))
		m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -TimeDelta);

	if (0x8000 & GetKeyState(VK_NUMPAD6))
		m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta);

	if (0x8000 & GetKeyState('B'))
		m_isWelcome = true;

	if (0x80000000 & Update_Welcome(TimeDelta))
		return -1;

	if (0x80000000 & Update_Movement(TimeDelta))
		return -1;

	if (0x80000000 & Get_Target(m_vTargetPos))
		return -1;

	if (0x80000000 & Update_Attack(TimeDelta))
		return -1;

	if (0x80000000 & Set_BossParts_ParentsMatrix(TimeDelta))
		return -1;

	return _int(0);
}

_int CBoss::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom ||
		nullptr == m_pColliderCom)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position()))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;

		if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_BUILDING, this)))
			return -1;
	}

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	Safe_Release(pManagement);

	return _int(0);
}

HRESULT CBoss::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pColliderCom->Render_Collider()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CBoss::Add_Component()
{
	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	CTransform::STATEDESC tTransformDesc;
	tTransformDesc.SpeedPerSec = 5.0;
	tTransformDesc.RotatePerSec = D3DXToRadian(90.f);
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Box", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Ready_Layer_BossParts(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CBossParts::STATEDESC StateDesc;
	CBossParts* pBossParts = nullptr;

#pragma region HEAD
	StateDesc.eObjID = CBossParts::OBJ_HEAD;
	StateDesc.BaseDesc.vSize = _float3(g_fUnitCX, g_fUnitCY, g_fUnitCZ);
	StateDesc.BaseDesc.vPos = _float3(0.f, 0.f, 0.f);
	pBossParts = dynamic_cast<CBossParts*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BossParts", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pBossParts)
		return E_FAIL;

	Safe_AddRef(pBossParts);

	m_vecBossParts.emplace_back(pBossParts);
#pragma endregion

#pragma region BODY
	StateDesc.eObjID = CBossParts::OBJ_BODY;
	StateDesc.BaseDesc.vSize = _float3(g_fUnitCX, g_fUnitCY * 1.5f, g_fUnitCZ * 0.5f);
	StateDesc.BaseDesc.vPos = _float3(0.f, 0.f, 0.f);
	pBossParts = dynamic_cast<CBossParts*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BossParts", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pBossParts)
		return E_FAIL;

	Safe_AddRef(pBossParts);

	m_vecBossParts.emplace_back(pBossParts);
#pragma endregion

#pragma region RIGHTARM
	StateDesc.eObjID = CBossParts::OBJ_RIGHTARM;
	StateDesc.BaseDesc.vSize = _float3(g_fUnitCX * 0.5f, g_fUnitCY * 1.5f, g_fUnitCZ * 0.5f);
	StateDesc.BaseDesc.vPos = _float3(0.f, g_fUnitCY * -0.5f, 0.f);
	pBossParts = dynamic_cast<CBossParts*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BossParts", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pBossParts)
		return E_FAIL;
	Safe_AddRef(pBossParts);

	m_vecBossParts.emplace_back(pBossParts);
#pragma endregion

#pragma region LEFTARM
	StateDesc.eObjID = CBossParts::OBJ_LEFTARM;
	StateDesc.BaseDesc.vSize = _float3(g_fUnitCX * 0.5f, g_fUnitCY * 1.5f, g_fUnitCZ * 0.5f);
	StateDesc.BaseDesc.vPos = _float3(0.f, g_fUnitCY * -0.5f, 0.f);
	pBossParts = dynamic_cast<CBossParts*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BossParts", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pBossParts)
		return E_FAIL;
	Safe_AddRef(pBossParts);

	m_vecBossParts.emplace_back(pBossParts);
#pragma endregion

#pragma region RIGHTLEG
	StateDesc.eObjID = CBossParts::OBJ_RIGHTLEG;
	StateDesc.BaseDesc.vSize = _float3(g_fUnitCX * 0.5f, g_fUnitCY * 1.5f, g_fUnitCZ * 0.5f);
	StateDesc.BaseDesc.vPos = _float3(0.f, g_fUnitCY * -0.75f, 0.f);
	pBossParts = dynamic_cast<CBossParts*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BossParts", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pBossParts)
		return E_FAIL;
	Safe_AddRef(pBossParts);

	m_vecBossParts.emplace_back(pBossParts);
#pragma endregion

#pragma region LEFTLEG
	StateDesc.eObjID = CBossParts::OBJ_LEFTLEG;
	StateDesc.BaseDesc.vSize = _float3(g_fUnitCX * 0.5f, g_fUnitCY * 1.5f, g_fUnitCZ * 0.5f);
	StateDesc.BaseDesc.vPos = _float3(0.f, g_fUnitCY * -0.75f, 0.f);
	pBossParts = dynamic_cast<CBossParts*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_BossParts", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pBossParts)
		return E_FAIL;
	Safe_AddRef(pBossParts);

	m_vecBossParts.emplace_back(pBossParts);
#pragma endregion

	Safe_Release(pManagement);

	return S_OK;
}

_int CBoss::Set_BossParts_ParentsMatrix(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	_matrix ParentsMatrix = m_pTransformCom->Get_WorldMatrix_Scale1();

	for (size_t i = 0; i < m_vecBossParts.size(); i++)
	{
		// ParentsMatrix
		_matrix TempMatrix = ParentsMatrix;
		_float3 vRight = (_float3)ParentsMatrix.m[0];
		_float3 vUp = (_float3)ParentsMatrix.m[1];
		_float3 vPos = (_float3)ParentsMatrix.m[3];

		// Nomalize
		D3DXVec3Normalize(&vRight, &vRight);
		D3DXVec3Normalize(&vUp, &vUp);

		// Offset
		CBossParts::OBJID eObjID = m_vecBossParts[i]->Get_ObjID();

		switch (eObjID)
		{
		case Client::CBossParts::OBJ_HEAD:
			vPos += vUp * (g_fUnitCY * 1.5f);
			break;
		case Client::CBossParts::OBJ_BODY:
			vPos += vUp * (g_fUnitCY * 0.25f);
			break;
		case Client::CBossParts::OBJ_RIGHTARM:
			vPos += vRight * (g_fUnitCX * 0.75f) + vUp * (g_fUnitCY * 0.75f);
			break;
		case Client::CBossParts::OBJ_LEFTARM:
			vPos += vRight * (g_fUnitCX * -0.75f) + vUp * (g_fUnitCY * 0.75f);
			break;
		case Client::CBossParts::OBJ_RIGHTLEG:
			vPos += vRight * (g_fUnitCX * 0.25f) + vUp * (g_fUnitCY * -0.5f);
			break;
		case Client::CBossParts::OBJ_LEFTLEG:
			vPos += vRight * (g_fUnitCX * -0.25f) + vUp * (g_fUnitCY * -0.5f);
			break;
		}

		memcpy(TempMatrix.m[3], &vPos, sizeof(_float3));
		m_vecBossParts[i]->Set_ParentsMatrix(TempMatrix);
	}

	return _int(0);
}

_int CBoss::Update_Welcome(_double TimeDelta)
{
	if (false == m_isWelcome)
		return 0;

	for (size_t i = CBossParts::OBJ_RIGHTARM; i < CBossParts::OBJ_RIGHTLEG; i++)
		m_vecBossParts[i]->Update_Welcome(TimeDelta);

	if (false == m_vecBossParts[CBossParts::OBJ_LEFTARM]->Get_IsWelcome())
	{
		m_isWelcome = false;
		m_isStarted = true;
	}

	return _int(0);
}

_int CBoss::Update_Movement(_double TimeDelta)
{
	if (false == m_isMoved || false == m_isStarted)
		return 0;

	if (nullptr != m_pTarget)
	{
		m_isMoved = false;
		return 0;
	}

	_float3 vScale = m_pTransformCom->Get_Scaled();
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 vPos = m_pTransformCom->Get_Position();
	_float3 vDir = m_vOriginalPos - vPos;
	_float3 vDoor = _float3(m_vOriginalPos.x - 10.f, m_vOriginalPos.y, m_vOriginalPos.z - 10.f);	// 성문 방향 위치
	_float fDist = D3DXVec3Length(&vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vLook, &vLook);

	// 방향 바라보기
	if (false == m_isLooked)
	{
		// 외적
		_float3 vUp;
		_float3 vLookDoor = vDoor - vPos;
		D3DXVec3Normalize(&vLookDoor, &vLookDoor);
		D3DXVec3Cross(&vUp, &vLook, &vLookDoor);

		_float fCosTheta = D3DXVec3Dot(&vLook, &vLookDoor);
		_float fAngle = acosf(fCosTheta);

		if (0 < vUp.y)	// 우측에 있다
			fAngle *= -1.f;

		if (0.1 < fAngle)
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -TimeDelta);
		else if (-0.1 > fAngle)
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta);
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLookDoor * vScale.z);

			_float3 vRight;
			D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLookDoor);
			D3DXVec3Normalize(&vRight, &vRight);
			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);

			m_isLooked = true;
		}
	}

	if (false == m_isBack && true == m_isLooked)
	{
		if (false == m_isTurned)
			m_MoveTimer += TimeDelta;

		// 자리 배치
		if (3.0 > m_MoveTimer && false == m_isTurned)
		{
			for (size_t i = CBossParts::OBJ_RIGHTARM; i < CBossParts::OBJ_END; i++)
				m_vecBossParts[i]->Set_HasMoved(true);

			m_pTransformCom->Go_Straight(TimeDelta * 3.0);
		}
		else if (5.0 > m_MoveTimer && false == m_isTurned)
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -TimeDelta * 0.5);
		else if (5.0 <= m_MoveTimer && false == m_isTurned)
		{
			m_MoveTimer = 0.0;
			m_isTurned = true;
		}

		// 순회
		if (true == m_isTurned)
		{
			for (size_t i = CBossParts::OBJ_RIGHTARM; i < CBossParts::OBJ_END; i++)
				m_vecBossParts[i]->Set_HasMoved(true);

			m_pTransformCom->Go_Straight(TimeDelta * 3.0);
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -TimeDelta * 0.5);
		}
	}
	else if (true == m_isBack && true == m_isLooked)	// 되돌아가기
	{
		if (true == m_isTurned)	// 방향 보기
		{
			// 외적
			_float3 vUp;
			D3DXVec3Cross(&vUp, &vLook, &vDir);

			_float fCosTheta = D3DXVec3Dot(&vLook, &vDir);
			_float fAngle = acosf(fCosTheta);

			if (0 < vUp.y)	// 좌측에 있다w
				fAngle *= -1.f;

			if (0.2 < fAngle)
				m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -TimeDelta);
			else if (-0.2 > fAngle)
				m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta);
			else
			{
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir * vScale.z);

				_float3 vRight;
				D3DXVec3Cross(&vRight, &vUp, &vDir);
				D3DXVec3Normalize(&vRight, &vRight);
				m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);

				m_isTurned = false;
			}
		}
		else
		{
			if (100.f < fDist)
			{
				// 다시 돌아가기
				m_isBack = false;
				m_isLooked = false;
			}
			if (1.f < fDist)
			{
				for (size_t i = CBossParts::OBJ_RIGHTARM; i < CBossParts::OBJ_END; i++)
					m_vecBossParts[i]->Set_HasMoved(true);

				m_pTransformCom->Go_Straight(TimeDelta * 3.0);
			}
			else
			{
				m_pTransformCom->SetUp_Position(m_vOriginalPos);
				m_isBack = false;
				m_isLooked = false;
			}
		}
	}

	return _int(0);
}

_int CBoss::Update_Attack(_double TimeDelta)
{
	if (true == m_isMoved || false == m_isStarted)
		return 0;

	if (nullptr == m_pTarget && true == m_isNear && 0 == m_iAttackType)
	{
		m_isMoved = true;
		m_isTurned = true;
		m_isBack = true;
		m_isNear = false;
		return 0;
	}
	else if (nullptr == m_pTarget && 0 == m_iAttackType)
	{
		m_isMoved = true;
		return 0;
	}

	// 일정 범위 밖 돌아가기
	//_float3 vDir = m_vOriginalPos - vPos;
	//_float3 vDoor = _float3(m_vOriginalPos.x - 10.f, m_vOriginalPos.y, m_vOriginalPos.z - 10.f);	// 성문 방향 위치
	//_float fDist = D3DXVec3Length(&vDir);

	//m_pTarget = nullptr;
	//m_isBack = false;
	//m_isLooked = false;
	//m_isFar = true;

	if (false == m_isNear && 0 == m_iAttackType)
	{
		m_iAttackType = 1;

		m_vTargetDir = m_vTargetPos - m_pTransformCom->Get_Position();
		D3DXVec3Normalize(&m_vTargetDir, &m_vTargetDir);

		for (size_t i = CBossParts::OBJ_RIGHTARM; i < CBossParts::OBJ_END; i++)
			m_vecBossParts[i]->Set_AttackType(m_iAttackType);
	}
	else if (true == m_isNear && 0 == m_iAttackType)
	{
		m_iAttackType = 2;

		for (size_t i = CBossParts::OBJ_RIGHTARM; i < CBossParts::OBJ_END; i++)
			m_vecBossParts[i]->Set_AttackType(m_iAttackType);
	}

	if (1 == m_iAttackType)
	{
		m_AttackTimer += TimeDelta;

		if (0.5 >= m_AttackTimer)
		{
			m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta);

			// 외적
			_float3 vUp;	// 외적의 결과
			_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK); \

				D3DXVec3Normalize(&vLook, &vLook);
			D3DXVec3Cross(&vUp, &vLook, &m_vTargetDir);

			// 내적
			_float fCosTheta = D3DXVec3Dot(&vLook, &m_vTargetDir);
			_float fAngle = acosf(fCosTheta);

			if (0 < vUp.y)				// 우측
				fAngle *= -1.f;

			if (0.1 < fAngle)			// 좌측
				m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), -TimeDelta);
			else if (-0.1 > fAngle)		// 우측
				m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta);
		}
		else if (1.0 >= m_AttackTimer)
		{
			m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -TimeDelta);
			m_pTransformCom->AddForce(_float3(m_vTargetDir.x * g_fUnitCX * 10.f * (_float)TimeDelta, g_fUnitCY * 14.f * (_float)TimeDelta, m_vTargetDir.z * g_fUnitCZ * 10.f * (_float)TimeDelta));
		}
		else if (1.5 >= m_AttackTimer)
		{
			m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta);
			m_pTransformCom->AddForce(_float3(m_vTargetDir.x * g_fUnitCX * 4.f * (_float)TimeDelta, g_fUnitCY * 14.f * -(_float)TimeDelta, m_vTargetDir.z * g_fUnitCZ * 4.f * (_float)TimeDelta));
		}
		else if (2.0 > m_AttackTimer)
		{
			m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -TimeDelta);

			CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
			if (nullptr == pSoundMgr)
				return -1;

			_tchar* pSoundTags = { L"JumpSkill.wav" };

			pSoundMgr->PlaySound_Overwrite(pSoundTags, CSoundMgr::BOSS);
		}
		else if (2.0 <= m_AttackTimer)
		{
			m_pTransformCom->SetUp_Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 0.f);
			m_iAttackType = 9;
		}
	}
	else if (2 == m_iAttackType)	// 회전 공격
	{
		m_AttackTimer += TimeDelta;

		vector<CGameObject*> vecClone = CPickingMgr::Get_Instance()->OverlapShpere(m_pTransformCom->Get_WorldPosition(), g_fUnitCX * 3.f, SCENE_STAGE, L"Layer_Unit_Ally");
		for (auto& pClone : vecClone)
			if (nullptr != pClone)
				dynamic_cast<CInfantry*>(pClone)->TakeDamage(TimeDelta);

		if (0.5 >= m_AttackTimer)
		{
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), m_AttackTimer);
			m_SavedTimer = m_AttackTimer;

			CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
			if (nullptr == pSoundMgr)
				return -1;

			_tchar* pSoundTags = { L"Tornado.wav" };

			pSoundMgr->PlaySound_Overwrite(pSoundTags, CSoundMgr::BOSS);
		}
		else
		{
			if (0 != m_vecBossParts[CBossParts::OBJ_LEFTLEG]->Get_AttackType())
			{
				m_vTargetDir = m_vTargetPos - m_pTransformCom->Get_Position();
				D3DXVec3Normalize(&m_vTargetDir, &m_vTargetDir);

				m_pTransformCom->AddForce(_float3(m_vTargetDir.x * 0.5f, 0.f, m_vTargetDir.z * 0.5f));
				m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), m_SavedTimer);
			}
			else
			{
				m_SavedTimer -= TimeDelta;
				if (0 >= m_SavedTimer)
				{
					m_iAttackType = 0;
					m_AttackTimer = 0.0;
					m_SavedTimer = 0.0;
				}

				m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), m_SavedTimer);
			}
		}
	}
	else if (9 == m_iAttackType)
	{
		if (0 == m_vecBossParts[CBossParts::OBJ_LEFTLEG]->Get_AttackType())
		{
			_float3 vPos = m_pTransformCom->Get_Position();
			m_pTransformCom->SetUp_Position(_float3(vPos.x, m_fHeight, vPos.z));

			m_iAttackType = 0;
			m_AttackTimer = 0.0;
			m_isNear = true;
		}
	}

	return _int(0);
}

_int CBoss::Get_Target(_float3 & vTargetPos)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	// 초기화
	m_pTarget = nullptr;

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");
	if (nullptr == plistClone || true == plistClone->empty())
	{
		Safe_Release(pManagement);
		return 0;
	}

	_float3 vInfantryPos;	// 위치 정보
	_float fMinDist = m_tStat.fRecogRange;
	CGameObject* pClosest = nullptr;

	for (auto& pInfantry : *plistClone)
	{
		CTransform* pInfantry_Transform = dynamic_cast<CTransform*>(pInfantry->Find_Component(L"Com_Transform"));
		if (nullptr == pInfantry_Transform)
			return -1;

		vInfantryPos = pInfantry_Transform->Get_Position();

		_float fDist = D3DXVec3Length(&(vInfantryPos - m_pTransformCom->Get_Position()));
		if (m_tStat.fRecogRange >= fDist && fMinDist > fDist)
		{
			fMinDist = fDist;
			pClosest = pInfantry;
		}
	}

	m_pTarget = dynamic_cast<CUnit*>(pClosest);
	vTargetPos = vInfantryPos;

	Safe_Release(pManagement);

	return _int(0);
}

CBoss * CBoss::Create_Boss_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss*	pInstance = new CBoss(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBoss Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss::Clone_GameObject(void * pArg)
{
	CBoss*	pInstance = new CBoss(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CBoss Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss::Free()
{
	for (auto& pBossParts : m_vecBossParts)
		if (nullptr != pBossParts)
			Safe_Release(pBossParts);

	m_vecBossParts.clear();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

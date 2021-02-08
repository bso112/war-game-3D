#include "stdafx.h"
#include "..\Headers\Infantry.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "Terrain.h"
#include "PickingMgr.h"
#include "Management.h"
#include "Sword.h"
#include "Staff.h"
#include "Bow.h"
#include "PickingMgr.h"
#include "Gate.h"
#include "InputManager.h"
#include "Waypoint.h"
#include "Image3D.h"
#include "UI_Massage.h"
#include "Tower.h"
#include "SoundMgr.h"
#include "Effect.h"


CInfantry::COMMAND CInfantry::m_eCommnad = CInfantry::COMMAND_END;

CInfantry::CInfantry(PDIRECT3DDEVICE9 pGraphic_Device)
	:CUnit(pGraphic_Device)
{
}

CInfantry::CInfantry(const CInfantry & rhs)
	: CUnit(rhs)
{
}

void CInfantry::Set_CurHP(_double Damage)
{
	m_tStat.iCurrHp -= Damage;
}

HRESULT CInfantry::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CInfantry::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(INFANTRYDESC));

	if (FAILED(CUnit::Ready_GameObject(pArg)))
		return E_FAIL;

	CPickingMgr::Get_Instance()->Register_Observer(this);
	CKeyMgr::Get_Instance()->RegisterObserver(this);

	if (!Load_Mesh())
		MSG_BOX("Fail to Load mesh");

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	//트랜스폼으로 크기를 지정하므로 지름이 1 되도록 만들자.
	//D3DXCreateSphere(m_pGraphic_Device, 0.5f, 10, 5, &m_pMesh, NULL);


	m_bFriendly = m_tDesc.bFriendly;

	SetUp_Occupation(m_tDesc.tOccupation);

	m_bJustBorn = true;


	return S_OK;
}

_int CInfantry::Update_GameObject(_double TimeDelta)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	//Fall_Damage(TimeDelta);

	if (m_bDead)
		return -1;
	if (!m_bActive)
		return 0;

	if (nullptr == m_pTransformCom)
		return -1;

#pragma region 물리
	if (m_iAvblJump != m_iMaxJump)
		m_dblAfterJump += TimeDelta;

	if (Is_Ground() == 1 && m_dblAfterJump > 0.2)
	{
		m_dblAfterJump = 0.0;
		m_iAvblJump = m_iMaxJump;
	}
	/////////////////////////////////////////고각 이용 여부
	if (GetAsyncKeyState(VK_F1))
		m_bIsHigh = 0;

	if (GetAsyncKeyState(VK_F2))
		m_bIsHigh = 1;
	////////////////////////////////////////

#pragma endregion


#pragma region 컨트롤

	if (!m_bFriendly)
		AI_Enemy(TimeDelta);
	else if (m_isCommander)
		Control_Commander(TimeDelta);
	else
		AI_Ally(TimeDelta);

#pragma endregion


#pragma region 자연스러움을 위한 처리


	//땅을 체크해서 땅을 타고다니게 한다.
	if (!m_tDesc.bStatic)
		CheckGround();

	//컨트롤모드가 아니고, 아군이면서 장군을 따라가고 있지 않으면
	//이동방향 바라보기 (미끄러지면 미끄러지는 방향봄)
	if (!m_bControlMode /*&& (m_bFriendly && m_eCommnad != COMMAND_FOLLOW)*/)
		Head_Direction(TimeDelta);

#pragma endregion

	_float3 velocity = m_pRigidBodyCom->Get_Velocity();
	_float Length = D3DXVec3Length(&velocity);

	if (m_bGround && Length > 1)
	{
		m_SmokeTime += TimeDelta;
		if (0.5 <= m_SmokeTime)
		{
			m_SmokeTime = 0;

			CEffect::STATEDESC EffectDesc;
			ZeroMemory(&EffectDesc, sizeof(CEffect::STATEDESC));
			//EffectDesc.bAlpha = false;
			EffectDesc.eTextureScene = SCENE_STAGE;
			EffectDesc.pTextureKey = L"Component_Texture_Effect_Smoke";
			EffectDesc.iTextureNum = 8;
			EffectDesc.iShaderPass = 5;
			EffectDesc.fFrameTime = 0.1;

			EffectDesc.vPosition = _float3(m_pTransformCom->Get_Position().x, m_pTransformCom->Get_Position().y - m_pTransformCom->Get_Scaled().y * 0.25f, m_pTransformCom->Get_Position().z);
			EffectDesc.vScale = _float3(5.f, 5.f, 1.f);

			pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect", &EffectDesc);
		}
	}

	// 이벤트 1
	if (true == m_isEvent1)
	{
		CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
		if (nullptr == pPickingMgr)
			return -1;

		WAYPOINT tWaypoint = pPickingMgr->Get_Waypoint(2);
		Go_Dest(tWaypoint.vPosition, TimeDelta, 10.f);



	}
	// 이벤트 2
	else if (true == m_isEvent2)
	{
		CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
		if (nullptr == pPickingMgr)
			return -1;

		if (false == m_isArrived && false == m_isCurved)
		{
			WAYPOINT tWaypoint = pPickingMgr->Get_Waypoint(10);
			Go_Dest(tWaypoint.vPosition, TimeDelta, 130.f);
		}
		else if (true == m_isArrived && false == m_isCurved)
		{
			m_isArrived = false;
			m_isCurved = true;
		}
		else if (false == m_isArrived && true == m_isCurved)
		{
			WAYPOINT tWaypoint = pPickingMgr->Get_Waypoint(9);
			Go_Dest(tWaypoint.vPosition, TimeDelta, 10.f);
		}
	}

	if (nullptr == m_pWeapon)
		return -1;

	//무기 업데이트
	m_pWeapon->Update_GameObject(TimeDelta);


	return _int();
}

_int CInfantry::Late_Update_GameObject(_double TimeDelta)
{

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;


	if (nullptr == m_pRendererCom ||
		nullptr == m_pWeapon)
		return -1;

	if (!m_bActive)
		return 0;



	Safe_AddRef(pManagement);
	if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position(), 2.f))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;

		if (pManagement->IS_DebugMode())
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DEBUG, this)))
				return -1;
		}

		if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_UNIT, this)))
			return -1;


#pragma region 무기를 충돌매니저에 등록

		if (nullptr != m_pWeapon)
		{
			CComponent* pCollider = m_pWeapon->Find_Component(L"Com_Collider");
			if (nullptr != pCollider)
			{
				if (m_bControlMode)
				{
					pCollider->Set_Enable(true);
					if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_WEAPON, m_pWeapon)))
						return -1;
				}
				else
					pCollider->Set_Enable(false);
			}
		}
#pragma endregion
	}

	if (0x80000000 & m_pCollderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix()))
		return -1;


	//PhysMgr 등록
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();

	_float3* pGroundNormal = nullptr;

	_bool bOnGround = Is_Ground();

	if (bOnGround && pPickingMgr->Get_TerrainNormal(m_pTransformCom->Get_WorldPosition(), m_vGroundNormal))
		pGroundNormal = &m_vGroundNormal;

	if (FAILED(CPhysicsMgr::Get_Instance()->Add_PhysGroup(CPhysicsMgr::PHYS_NORMAL, m_pTransformCom, m_pCollderCom, m_pRigidBodyCom, pGroundNormal)))
		return -1;


	//무기 레잇업데이트
	m_pWeapon->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix_Scale1());
	m_pWeapon->Late_Update_GameObject(TimeDelta);


	Safe_Release(pManagement);
	return 0;
}

HRESULT CInfantry::Render_GameObject()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (!m_bActive)
		return 0;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;




	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(CManagement::Get_Instance()->IS_DebugMode());

	for (_uint i = 0; i < m_Mtrls.size(); i++)
	{
		m_pMesh->DrawSubset(i);
	}


	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();


	///////////////////노멀,방향 축

	///////////////////

	return S_OK;
}

void CInfantry::Render_Debug()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, true);

	m_pRigidBodyCom->GetStateDesc_Direct().m_vSlideForce;
	m_pRigidBodyCom->GetStateDesc_Direct().m_vDownForce;
	m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce;

	_float3 fSlFrc = m_pRigidBodyCom->GetStateDesc().m_vSlideForce;
	_float3 fDwFrc = m_pRigidBodyCom->GetStateDesc().m_vDownForce;
	_float3 fInFrc = m_pRigidBodyCom->GetStateDesc().m_vInputForce;

	if (D3DXVec3Length(&fSlFrc) > 5.f)
	{
		D3DXVec3Normalize(&m_pRigidBodyCom->GetStateDesc_Direct().m_vSlideForce, &m_pRigidBodyCom->GetStateDesc_Direct().m_vSlideForce);
		m_pRigidBodyCom->GetStateDesc_Direct().m_vSlideForce *= 5.f;
	}
	if (D3DXVec3Length(&fDwFrc) > 5.f)
	{
		D3DXVec3Normalize(&m_pRigidBodyCom->GetStateDesc_Direct().m_vDownForce, &m_pRigidBodyCom->GetStateDesc_Direct().m_vDownForce);
		m_pRigidBodyCom->GetStateDesc_Direct().m_vDownForce *= 5.f;

	}
	if (D3DXVec3Length(&fInFrc) > 5.f)
	{
		D3DXVec3Normalize(&m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce, &m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce);
		m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce *= 5.f;

	}

	D3DXVECTOR3 SlideForce[2];
	SlideForce[0].x = 0.f;
	SlideForce[0].y = 0.f;
	SlideForce[0].z = 0.f;
	SlideForce[1].x = m_pRigidBodyCom->GetStateDesc().m_vSlideForce.x;
	SlideForce[1].y = m_pRigidBodyCom->GetStateDesc().m_vSlideForce.y;
	SlideForce[1].z = m_pRigidBodyCom->GetStateDesc().m_vSlideForce.z;

	D3DXVECTOR3 DownForce[2];
	DownForce[0].x = 0.f;
	DownForce[0].y = 0.f;
	DownForce[0].z = 0.f;
	DownForce[1].x = m_pRigidBodyCom->GetStateDesc().m_vDownForce.x;
	DownForce[1].y = m_pRigidBodyCom->GetStateDesc().m_vDownForce.y;
	DownForce[1].z = m_pRigidBodyCom->GetStateDesc().m_vDownForce.z;

	D3DXVECTOR3 VelDe[2];
	VelDe[0].x = 0.f;
	VelDe[0].y = 0.f;
	VelDe[0].z = 0.f;
	VelDe[1].x = m_pRigidBodyCom->GetStateDesc().m_vInitForce.x * 10.f;
	VelDe[1].y = m_pRigidBodyCom->GetStateDesc().m_vInitForce.y * 10.f;
	VelDe[1].z = m_pRigidBodyCom->GetStateDesc().m_vInitForce.z * 10.f;

	D3DXVECTOR3 LocalPos[2];
	LocalPos[0].x = 0.f;
	LocalPos[0].y = 0.f;
	LocalPos[0].z = 0.f;
	LocalPos[1].x = m_pRigidBodyCom->GetStateDesc().m_vLocalPos.x;
	LocalPos[1].y = m_pRigidBodyCom->GetStateDesc().m_vLocalPos.y;
	LocalPos[1].z = m_pRigidBodyCom->GetStateDesc().m_vLocalPos.z;

	D3DXVECTOR3 TempForce[2];
	TempForce[0].x = 0.f;
	TempForce[0].y = 0.f;
	TempForce[0].z = 0.f;
	TempForce[1].x = m_vTempForce.x;
	TempForce[1].y = m_vTempForce.y;
	TempForce[1].z = m_vTempForce.z;

	D3DXVECTOR3 Force[2];
	Force[0].x = 0.f;
	Force[0].y = 0.f;
	Force[0].z = 0.f;
	Force[1].x = m_pRigidBodyCom->GetStateDesc().m_vInputForce.x;
	Force[1].y = m_pRigidBodyCom->GetStateDesc().m_vInputForce.y;
	Force[1].z = m_pRigidBodyCom->GetStateDesc().m_vInputForce.z;

	D3DXVECTOR3 n[2];
	n[0].x = 0.f;
	n[0].y = 0.f;
	n[0].z = 0.f;
	n[1].x = m_vGroundNormal.x * 5.f;
	n[1].y = m_vGroundNormal.y * 5.f;
	n[1].z = m_vGroundNormal.z * 5.f;

	D3DXVECTOR3 sn[2];
	sn[0].x = 0.f;
	sn[0].y = 0.f;
	sn[0].z = 0.f;
	sn[1].x = m_pRigidBodyCom->GetStateDesc().m_vSlideNormal.x * 5.f;
	sn[1].y = m_pRigidBodyCom->GetStateDesc().m_vSlideNormal.y * 5.f;
	sn[1].z = m_pRigidBodyCom->GetStateDesc().m_vSlideNormal.z * 5.f;

	D3DXVECTOR3 nr[2];
	nr[0].x = 0.f;
	nr[0].y = 0.f;
	nr[0].z = 0.f;
	nr[1].x = m_pRigidBodyCom->GetStateDesc().m_vTempRight.x * 5.f;
	nr[1].y = m_pRigidBodyCom->GetStateDesc().m_vTempRight.y * 5.f;
	nr[1].z = m_pRigidBodyCom->GetStateDesc().m_vTempRight.z * 5.f;

	D3DXVECTOR3 p[6];
	_float3 tempUp = m_pTransformCom->Get_State(Engine::CTransform::STATE_UP);
	p[0].x = 0.f;
	p[0].y = 0.f;
	p[0].z = 0.f;
	p[1].x = tempUp.x * 1.5f;
	p[1].y = tempUp.y * 1.5f;
	p[1].z = tempUp.z * 1.5f;

	_float3 tempLook = m_pTransformCom->Get_State(Engine::CTransform::STATE_LOOK);
	p[2].x = 0.f;
	p[2].y = 0.f;
	p[2].z = 0.f;
	p[3].x = tempLook.x * 1.5f;
	p[3].y = tempLook.y * 1.5f;
	p[3].z = tempLook.z * 1.5f;

	_float3 tempRight = m_pTransformCom->Get_State(Engine::CTransform::STATE_RIGHT);
	p[4].x = 0.f;
	p[4].y = 0.f;
	p[4].z = 0.f;
	p[5].x = tempRight.x * 1.5f;
	p[5].y = tempRight.y * 1.5f;
	p[5].z = tempRight.z * 1.5f;

	_matrix matPos;
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	D3DXMatrixIdentity(&matPos);
	memcpy((_float3*)matPos.m[3], (_float3*)matWorld.m[3], sizeof(_float3));

	m_pGraphic_Device->BeginScene();
	ID3DXLine *Line;
	D3DXCreateLine(m_pGraphic_Device, &Line);
	Line->SetWidth(5);
	Line->SetAntialias(true);
	Line->Begin();
	Line->DrawTransform(p, 6, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(n, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(sn, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(nr, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(Force, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
																																																		 //Line->DrawTransform(TempForce, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(LocalPos, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(VelDe, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.f, 0.f, 0.f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(SlideForce, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(1.f, 0.75f, 0.f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->DrawTransform(DownForce, 2, &(matPos *CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW)*CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(1.f, 0.75f, 0.f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	Line->End();
	Line->Release();
	m_pGraphic_Device->EndScene();
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, true);
}


#pragma region Control

_int CInfantry::AI_Enemy(_double TimeDelta)
{
	//주변에 적이 없으면
	if (!Battle(TimeDelta) && false == m_isEvent1 && false == m_isEvent2)
	{
		//자신의 웨이포인트로 돌아감
		CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
		if (nullptr == pPickingMgr) return -1;
		WAYPOINT tWaypoint = pPickingMgr->Get_Waypoint(m_tDesc.iNumber);
		Go_Dst(tWaypoint.vPosition, TimeDelta, nullptr, g_fUnitCX * 1.5f);
	}

	//웨이포인트들과 거리를 비교해서 적군이 웨이포인트 안에 있으면 일정시간 후 점령
	for (auto& waypoint : m_WayPoints)
	{
		_float fDist = D3DXVec3Length(&(m_pTransformCom->Get_Position() - waypoint.vPosition));

		//웨이포인트 진입시 점령표시
		if (fDist < m_WayPoints.front().fRadius)
		{
			CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
			if (nullptr == pPickingMgr) return -1;
			pPickingMgr->Occupy_Waypoint(waypoint.iNumber, -5.0 * TimeDelta);
		}
	}


	return 0;
}

_int CInfantry::AI_Ally(_double TimeDelta)
{
	CInputManager* pInputMgr = CInputManager::Get_Instance();
	if (nullptr == pInputMgr)
		return -1;

	//방금 태어났으면 
	if (m_bJustBorn)
	{
		_float dist = 0.f;
		//웨이포인트로
		CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
		if (nullptr == pPickingMgr) return -1;
		WAYPOINT tWaypoint = pPickingMgr->Get_Waypoint(m_tDesc.iNumber);
		Go_Dst(tWaypoint.vPosition, TimeDelta, &dist, 15.f);
		if (dist < 15.f)
			m_bJustBorn = false;
	}
	else
	{
		//지휘관이 없으면 기본 배틀상태
		if (nullptr == pInputMgr->Get_PickedUnit())
			Battle(TimeDelta);
		else
		{
			switch (m_eCommnad)
			{
			case Client::CInfantry::COMMAND_WAIT:
				break;
			case Client::CInfantry::COMMAND_FOLLOW:
				//LookAt_CommanderLook();
				break;
			case Client::CInfantry::COMMAND_ATTACK:
				Battle(TimeDelta);
				break;
			case Client::CInfantry::COMMAND_END:
				break;
			default:
				break;
			}
		}
	}





	return _int();
}

_int CInfantry::Control_Commander(_double TimeDelta)
{
	if (!m_isCommander || !m_tDesc.bFriendly)
		return 0;

	Move_ControlMode(TimeDelta);

	//웨이포인트들과 거리를 비교해서 지휘관이 웨이포인트 안에 있으면 일정시간 후 점령
	for (auto& waypoint : m_WayPoints)
	{
		_float fDist = D3DXVec3Length(&(m_pTransformCom->Get_Position() - waypoint.vPosition));

		//웨이포인트 진입시 점령표시
		if (fDist < m_WayPoints.front().fRadius)
		{
			CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
			if (nullptr == pPickingMgr) return -1;
			pPickingMgr->Occupy_Waypoint(waypoint.iNumber, 20 * TimeDelta);
		}
	}

	if (m_eCommnad == COMMAND_FOLLOW)
		Lead_Infantry(TimeDelta);



	return _int();
}


_int CInfantry::Lead_Infantry_line(_double TimeDelta)
{
	if (!m_isCommander || nullptr == m_pTransformCom)
		return 0;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	list<CGameObject*> pAllyList;
	if (m_bFriendly)
		pAllyList = *pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");


	//아군들을 지휘관과 떨어진 거리를 기준으로 오름차순 정렬
	pAllyList.sort([&](CGameObject* pA, CGameObject* pB)
	{
		CTransform* pTransformA = (CTransform*)pA->Find_Component(L"Com_Transform");
		CTransform* pTransformB = (CTransform*)pB->Find_Component(L"Com_Transform");
		if (nullptr == pTransformA || nullptr == pTransformB)
			return false;

		_float fDistA = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pTransformA->Get_Position()));
		_float fDistB = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pTransformB->Get_Position()));

		return fDistA < fDistB;
	});

	_float fMargin = 0.f;
	for (auto& pAlly : pAllyList)
	{
		if (pAlly == this)
			continue;

		//아군들을 지휘관의 Look의 반대방향으로 일정하게 떨어져서 정렬한다.
		CTransform* pAllyTransform = (CTransform*)pAlly->Find_Component(L"Com_Transform");
		if (nullptr == pAllyTransform) return -1;

		_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		D3DXVec3Normalize(&vLook, &(vLook));
		_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		D3DXVec3Normalize(&vRight, &(vRight));

		_float3 vDst = m_pTransformCom->Get_Position();

		_float3 vTerrainPos;
		CPickingMgr::Get_Instance()->Get_TerrainPos(vDst, vTerrainPos);

		fMargin += 10.f;
		//목적지에 가까우면 대기
		if (fMargin > D3DXVec3Length(&(vTerrainPos - pAllyTransform->Get_Position())))
			continue;

		pAllyTransform->Go_Dst(vTerrainPos, TimeDelta);

	}

	pAllyList.clear();
	return _int();
}

_int CInfantry::Lead_Infantry(_double TimeDelta)
{
	if (!m_isCommander || nullptr == m_pTransformCom)
		return 0;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	list<CGameObject*> pAllyList;
	if (m_bFriendly)
		pAllyList = *pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");


	////아군들을 지휘관과 떨어진 거리를 기준으로 오름차순 정렬
	//pAllyList.sort([&](CGameObject* pA, CGameObject* pB)
	//{
	//	CTransform* pTransformA = (CTransform*)pA->Find_Component(L"Com_Transform");
	//	CTransform* pTransformB = (CTransform*)pB->Find_Component(L"Com_Transform");
	//	if (nullptr == pTransformA || nullptr == pTransformB)
	//		return false;

	//	_float fDistA = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pTransformA->Get_Position()));
	//	_float fDistB = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pTransformB->Get_Position()));

	//	return fDistA < fDistB;
	//});


	for (auto& pAlly : pAllyList)
	{
		if (pAlly == this)
			continue;

		//아군들을 지휘관의 Look의 반대방향으로 일정하게 떨어져서 정렬한다.
		CTransform* pAllyTransform = (CTransform*)pAlly->Find_Component(L"Com_Transform");
		if (nullptr == pAllyTransform) return -1;

		_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		D3DXVec3Normalize(&vLook, &(vLook));
		_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		D3DXVec3Normalize(&vRight, &(vRight));

		_float3 vDst = m_pTransformCom->Get_Position();

		_float3 vTerrainPos;
		CPickingMgr::Get_Instance()->Get_TerrainPos(vDst, vTerrainPos);

		//목적지에 가까우면 대기
		if (15.f > D3DXVec3Length(&(vTerrainPos - pAllyTransform->Get_Position())))
			continue;

		((CInfantry*)pAlly)->Go_Dst(vTerrainPos, TimeDelta);
		//pAllyTransform->Go_Dst(vTerrainPos, TimeDelta);

	}

	pAllyList.clear();
	return _int();
}

_bool CInfantry::Battle(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return false;

	_float fEnemyDist = 0.f;
	CGameObject* pTarget = Get_ClosestEnemy(fEnemyDist);
	if (nullptr != pTarget)
	{
		CUnit* pEnemy = dynamic_cast<CUnit*>(pTarget);

		//대상이 있으면
		if (nullptr != pEnemy)
		{
			CTransform* pTargetTransform = (CTransform*)pEnemy->Find_Component(L"Com_Transform");
			if (nullptr == pTargetTransform)
				return false;
			_float fDist = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pTargetTransform->Get_Position()));

			//공격범위안에 있으면 공격
			if (fDist < m_tStat.fAttackRange)
			{
				m_dAttackTimer += TimeDelta;
				if (m_dAttackTimer > m_tStat.fAttackSpeed)
				{
					if (m_tDesc.tOccupation == OCC_ARCHER)
						Attack_Archer(pTargetTransform->Get_Position(), TimeDelta);
					else
					{
						m_pWeapon->Attack(TimeDelta);
						pEnemy->TakeDamage(m_tStat.iAtt);
					}


					m_dAttackTimer = 0.0;
				}
			}
			//타깃이 공격범위 안에 없으면 추격
			else
			{
				Go_Dst(pTargetTransform->Get_Position(), TimeDelta);
			}

		}
	}
	//대상이 없으면 돌격
	else
	{
		//m_vTargetArea.x > -3000는 m_vTargetArea 쓰레기값 방지
		if (!m_isCommander && m_bFriendly && m_eCommnad == COMMAND_ATTACK && m_vTargetArea.x > -3000 && (m_vTargetArea.x != 0) && (m_vTargetArea.y != 0))
			Go_Dst(m_vTargetArea, TimeDelta, nullptr, g_fUnitCX * 1.5f);

		return false;
	}
	return true;
}

void CInfantry::LookAt_CommanderLook()
{
	if (nullptr == m_pTransformCom)
		return;
	CInputManager* pInputMgr = CInputManager::Get_Instance();
	if (nullptr == pInputMgr) return;
	CUnit* pCommander = pInputMgr->Get_PickedUnit();
	if (nullptr == pCommander) return;
	CTransform* pCommanderTransform = (CTransform*)pCommander->Find_Component(L"Com_Transform");
	if (nullptr == pCommanderTransform) return;
	_float3 vCommanderLook = pCommanderTransform->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vCommanderLook, &vCommanderLook);
	m_pTransformCom->LookAt(pCommanderTransform->Get_Position() + vCommanderLook * 200.f);
}



_int CInfantry::Move_ControlMode(_double TimeDelta)
{

	CCamera_Manager* pCameraManager = CCamera_Manager::Get_Instance();
	if (pCameraManager->Get_CamState() == CCamera_Manager::STATE_COMMAND)
		return -1;


	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement || nullptr == m_pTransformCom)
		return -1;

	/////////////////////////////
	m_vTempForce *= 0.f;
	/////////////////////////////
	_float3 vVelocity = m_pRigidBodyCom->Get_Velocity();
	_float	fMaxSpeed = m_pRigidBodyCom->GetStateDesc().fMaxSpeed;
	m_pRigidBodyCom->GetStateDesc_Direct().m_vForce *= 0.f;
	_float fWalk_Force = m_fWalk_Force;

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_LSHIFT))
	{
		fMaxSpeed = fMaxSpeed * 3;
		fWalk_Force = m_fWalk_Force * 3;
	}


	//움직임
	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_W))
	{
		if (D3DXVec3Length(&(m_pRigidBodyCom->Get_Velocity())) < fMaxSpeed)
		{
			_float3		vLook;
			vLook = m_pTransformCom->Get_WorldState(CTransform::STATE_LOOK);
			_float3		vDir = vLook;
			vDir.y = 0.f;
			m_vTempForce += *D3DXVec3Normalize(&vDir, &vDir) * fWalk_Force;
			m_pRigidBodyCom->Add_Force(m_vTempForce);
		}
		//m_pTransformCom->Go_Forward(TimeDelta);

	}

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_A))
	{

		if (D3DXVec3Length(&(m_pRigidBodyCom->Get_Velocity())) < fMaxSpeed)
		{
			_float3		vRight;
			vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			_float3		vDir = vRight;
			vDir.y = 0.f;
			m_vTempForce += *D3DXVec3Normalize(&vDir, &vDir) * fWalk_Force * -1.f;
			m_pRigidBodyCom->Add_Force(m_vTempForce);
		}
		//m_pTransformCom->Go_Left(TimeDelta);
	}

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_S))
	{
		if (D3DXVec3Length(&(m_pRigidBodyCom->Get_Velocity())) < fMaxSpeed)
		{
			_float3		vLook;
			vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float3		vDir = vLook;
			vDir.y = 0.f;
			m_vTempForce += *D3DXVec3Normalize(&vDir, &vDir) * fWalk_Force * -1.f;
			m_pRigidBodyCom->Add_Force(m_vTempForce);
		}
		//m_pTransformCom->Go_BackWard(TimeDelta);

	}

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_D))
	{
		if (D3DXVec3Length(&(m_pRigidBodyCom->Get_Velocity())) < fMaxSpeed)
		{
			_float3		vRight;
			vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			_float3		vDir = vRight;
			vDir.y = 0.f;
			m_vTempForce += *D3DXVec3Normalize(&vDir, &vDir) * fWalk_Force;
			m_pRigidBodyCom->Add_Force(m_vTempForce);
		}
		//m_pTransformCom->Go_Right(TimeDelta);
	}

	m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce = m_vTempForce;

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_SPACE))
	{
		Jump(TimeDelta);
	}



	_long MouseMove = 0;


	//회전
	if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_X))
	{
		m_fXRot += TimeDelta * MouseMove * 5;
	}

	if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
	{
		//회전각 제한
		m_fYRot = min(35.f, max(-35.f, m_fYRot + TimeDelta * MouseMove * 5));


	}

	_matrix rotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, D3DXToRadian(m_fXRot), D3DXToRadian(m_fYRot), D3DXToRadian(0.f));
	m_pTransformCom->SetUp_RotationMatrix(rotationMatrix);



	return _int();
}


_int CInfantry::OnCommnad_Charge()
{
	if (!m_bFriendly)
		return 0;

	CInputManager* pInputMgr = CInputManager::Get_Instance();
	if (nullptr == pInputMgr) return 0;
	CUnit* pCommander = pInputMgr->Get_PickedUnit();
	if (nullptr == pCommander) return 0;
	CTransform* pCommanderTransform = (CTransform*)pCommander->Find_Component(L"Com_Transform");
	if (nullptr == pCommanderTransform) return 0;
	_float3 vChargeDir = pCommanderTransform->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vChargeDir, &vChargeDir);
	//지휘관 앞으로 30만큼 떨어진 곳까지 돌격. 
	m_vTargetArea = pCommanderTransform->Get_Position() + vChargeDir * 200.f;
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMgr) return 0;
	pPickingMgr->Get_TerrainPos(m_vTargetArea, m_vTargetArea);

	if (m_isCommander)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement) return -1;
		CGameObject* pImge;
		CImage3D::STATEDESC tArrowDesc;
		tArrowDesc.bBillboard = true;
		tArrowDesc.eTextureSceneID = SCENE_STATIC;
		tArrowDesc.pTextureTag = L"Component_Texture_DownArrow";
		tArrowDesc.dLifeTime = 3.0;
		_float3 vSize = _float3(40.f, 30.f, 1.f);
		_float3 vPos = m_vTargetArea;
		vPos.y += vSize.y * 0.5f;
		tArrowDesc.tBaseDesc = BASEDESC(vPos, vSize);
		if (nullptr == (pImge = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Image3D", SCENE_STAGE, L"Layer_Image3D", &tArrowDesc)))
			return -1;
		int i = 3;
	}

	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	if (nullptr == pSoundMgr) return -1;
	_tchar* pSoundTags[4] = { L"man_shortyell_5.mp3", L"man_shortyell_6.mp3" , L"man_shortyell_9.mp3" , L"man_shortyell_11.mp3" };

	pSoundMgr->PlaySound_Overwrite(pSoundTags[rand() % 4], CSoundMgr::CHANNELID::PLAYER);

	return _int();
}

HRESULT CInfantry::ShowComandIcon()
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	if (m_isCommander || !m_bFriendly)
		return E_FAIL;


	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return E_FAIL;

	CGameObject* pIcon = nullptr;
	CImage3D::STATEDESC tIconDesc;
	tIconDesc.bBillboard = true;
	tIconDesc.dLifeTime = 2.f;
	tIconDesc.eTextureSceneID = SCENE_STATIC;

	switch (m_eCommnad)
	{
	case COMMAND_WAIT:
		tIconDesc.pTextureTag = L"Component_Texture_Halt";
		tIconDesc.tBaseDesc = BASEDESC(_float3(0.f, 1.f, 0.f), _float3(4.f, 4.f, 1.f));
		break;
	case COMMAND_ATTACK:
		tIconDesc.pTextureTag = L"Component_Texture_Attack";
		tIconDesc.tBaseDesc = BASEDESC(_float3(0.f, 1.f, 0.f), _float3(4.f, 4.f, 1.f));
		break;
	case COMMAND_FOLLOW:
		tIconDesc.pTextureTag = L"Component_Texture_Flag";
		tIconDesc.tBaseDesc = BASEDESC(_float3(0.f, 1.f, 0.f), _float3(8.f, 8.f, 1.f));
		break;
	}

	if (nullptr == (pIcon = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Image3D", SCENE_STAGE, L"Image3D", &tIconDesc)))
		return E_FAIL;
	CTransform* pIconTransform = (CTransform*)pIcon->Find_Component(L"Com_Transform");
	if (nullptr == pIconTransform) return E_FAIL;

	pIconTransform->Set_Parent(m_pTransformCom);

	CSoundMgr::Get_Instance()->PlaySound_Overwrite(L"bell.mp3", CSoundMgr::CHANNELID::UI);


	return S_OK;
}

HRESULT CInfantry::OnKeyDown(_int KeyCode)
{
	if (KeyCode == '1')
	{
		m_eCommnad = COMMAND_FOLLOW;
		ShowComandIcon();
	}
	else if (KeyCode == '2')
	{
		m_eCommnad = COMMAND_WAIT;
		ShowComandIcon();
	}
	else if (KeyCode == '3')
	{
		m_eCommnad = COMMAND_ATTACK;
		ShowComandIcon();
		OnCommnad_Charge();
	}
	//자살
	else if (KeyCode == 'O')
	{
		if (m_isCommander)
			m_bDead = true;
	}
	else if (KeyCode == 'P')
	{
		m_bDead = true;
	}

	if (KeyCode == VK_LBUTTON && m_isCommander && m_tDesc.tOccupation == OCC_ARCHER)
	{
		CSoundMgr::Get_Instance()->PlaySound(L"bow_pullback.ogg", CSoundMgr::CHANNELID::WEAPON);
	}

	return S_OK;
}

HRESULT CInfantry::OnKeyUp(_int KeyCode)
{
	if (KeyCode == VK_LBUTTON && m_isCommander && m_tDesc.tOccupation == OCC_ARCHER)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::WEAPON);
	}
	return S_OK;
}

#pragma endregion

_bool CInfantry::Picking(POINT & ptMouse)
{
	if (nullptr == m_pTransformCom)
		return false;

	//레이 생성
	CManagement* pManageMent = CManagement::Get_Instance();
	if (nullptr == pManageMent) return false;
	MYRAY tRay = Make_Ray(ptMouse, pManageMent->Get_Transform(D3DTS_PROJECTION), pManageMent->Get_Transform(D3DTS_VIEW));

	//레이의 오리진으로 향하는 벡터
	_float3 vToOrigin = tRay.vOrigin - m_pTransformCom->Get_Position();
	//레이의 방향에 투영된 vToOrigin 벡터의 길이 (길이를 구하기 위해선 레이의 방향을 반대로 해야한다)
	_float vProjectedLength = D3DXVec3Dot(&vToOrigin, &(tRay.vDir * -1));

	//수선의 높이
	_float fH = sqrtf(powf(D3DXVec3Length(&vToOrigin), 2) - powf(vProjectedLength, 2));

	//구의 반지름
	_float radius = m_pTransformCom->Get_Scaled().x * 0.5f;
	if (fH <= radius)
		return true;

	return false;
}

void CInfantry::OnDead()
{
	if (m_bPicked)
	{
		CCamera_Manager* pCameraManager = CCamera_Manager::Get_Instance();
		if (nullptr == pCameraManager)
			return;

		CCamera* pCamera = pCameraManager->Get_CurrentCamera();
		if (nullptr == pCamera)
			return;

		pCamera->Set_Target(nullptr);

		CTransform* pTransform = pCamera->Get_Transform();
		if (nullptr == pTransform)
			return;

		_float3 vLook = pTransform->Get_State(CTransform::STATE_LOOK);
		_float3 vPostion = pTransform->Get_State(CTransform::STATE_POSITION);

		if (FAILED(pCameraManager->Change_Camera_Free(false, CCamera_Manager::STATE_FREE, vPostion, vLook)))
			return;
	}

	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	if (nullptr == pSoundMgr) return;
	_tchar* pSoundTags[2] = { L"man_death_1.mp3", L"man_death_8.mp3" };

	pSoundMgr->PlaySound(pSoundTags[rand() % 2], CSoundMgr::PLAYER_VOICE);

	CEffect::STATEDESC EffectDesc;
	ZeroMemory(&EffectDesc, sizeof(CEffect::STATEDESC));
	EffectDesc.eTextureScene = SCENE_STATIC;
	EffectDesc.pTextureKey = L"Component_Texture_Explosion";
	EffectDesc.iTextureNum = 41;
	EffectDesc.iShaderPass = 4;
	EffectDesc.fFrameTime = 0.015f;

	EffectDesc.vPosition = m_pTransformCom->Get_Position();
	EffectDesc.vScale = _float3(15.f, 15.f, 1.f);
	CManagement::Get_Instance()->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect", &EffectDesc);
}



HRESULT CInfantry::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShaderCom->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));



	if (m_bFriendly)
		m_pShaderCom->Set_Value("g_vColor", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));
	else
		m_pShaderCom->Set_Value("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));


	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_bFriendly)))
		return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CInfantry::SetUp_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Transform
	CTransform::STATEDESC StateDesc;
	StateDesc.SpeedPerSec = 10.0;
	StateDesc.RotatePerSec = D3DXToRadian(30.f);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &StateDesc)))
		return E_FAIL;

	m_pTransformCom->SetUp_Position(m_tDesc.tBasedesc.vPos);
	m_pTransformCom->SetUp_Scale(_float3(g_fUnitCX, g_fUnitCY, g_fUnitCZ));

	//// For.Com_VIBuffer
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
	//	return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Unit", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Collider
	CCollider_Ellipse::STATEDESC_DERIVED tColliderDesc;
	tColliderDesc.fRadius = m_pTransformCom->Get_Scaled().x * 0.5f;
	D3DXMatrixIdentity(&tColliderDesc.StateMatrix);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Ellipse", L"Com_Collider", (CComponent**)&m_pCollderCom, &tColliderDesc)))
		return E_FAIL;

	if (m_tDesc.bStatic)
		m_pCollderCom->Set_Trigger(true);

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Unit", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_RigidBody
	Engine::CRigid_Body::STATEDESC RigideDesc;
	RigideDesc.fBounce = 0.f;

	RigideDesc.fFriction_Dynamic = 1.f;
	RigideDesc.fFriction_Static = 1.15f;
	RigideDesc.fGravity_Multiply = m_tDesc.bStatic ? 0.f : 1.f;

	RigideDesc.fMass = 60.f;
	RigideDesc.fMaxFallSpeed = 0.f;
	RigideDesc.fMaxSpeed = 15.f;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_RigidBody", L"Com_RigidBody", (CComponent**)&m_pRigidBodyCom, &RigideDesc)))
		return E_FAIL;

	return S_OK;
}


void CInfantry::OnCollisionEnter(CGameObject * _pOther)
{


	int a = 0;
}

_bool CInfantry::Load_Mesh()
{
	HRESULT hr = 0;

	//
	// Load the XFile data.
	//

	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		L"../Bin/Meshs/unit.x",
		D3DXMESH_MANAGED,
		m_pGraphic_Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&m_pMesh);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXLoadMeshFromX() - FAILED", 0, 0);
		return false;
	}


	//
	// Extract the materials, and load textures.
	//

	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		vector<IDirect3DBaseTexture9*> Textures;

		for (_uint i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;


			// save the ith material
			m_Mtrls.push_back(mtrls[i].MatD3D);


			WCHAR pTextureFileName[MAX_PATH] = { '\0', };
			MultiByteToWideChar(CP_ACP, 0, mtrls[i].pTextureFilename, sizeof(mtrls[i].pTextureFilename), pTextureFileName, _countof(pTextureFileName));


			// check if the ith material has an associative texture
			if (mtrls[i].pTextureFilename != 0)
			{
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					m_pGraphic_Device,
					pTextureFileName,
					&tex);

				Textures.push_back(tex);

			}
			else
			{
				// no texture for the ith subset
				Textures.push_back(0);
			}
			// save the loaded texture

		}

		//m_pTexture = CTexture::Create(m_pGraphic_Device, &Textures);
	}

	Safe_Release(mtrlBuffer);


	//
	// Optimize the mesh.
	//

	hr = m_pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	Safe_Release(adjBuffer);

	if (FAILED(hr))
	{
		::MessageBox(0, L"OptimizeInplace() - FAILED", 0, 0);
		return false;


	}
	return true;

}

HRESULT CInfantry::Jump(_double TimeDelta)
{
	if (m_iAvblJump > 0)
	{
		--m_iAvblJump;
		//_float3 vUp = m_pTransformCom->Get_WorldState(Engine::CTransform::STATE_UP);
		//D3DXVec3Normalize(&vUp, &vUp);
		m_pRigidBodyCom->Add_Velocity(_float3(0.f, 1.f, 0.f)*m_fJumpPower);
	}
	return S_OK;
}

void CInfantry::Fall_Damage(_double TimeDelta)
{
	_float3 vVelocity_Cur = m_pRigidBodyCom->Get_Velocity();
	//_float fDiff_Velocity = fabs(D3DXVec3Length(&vVelocity_Cur) - D3DXVec3Length(&m_vVelocity_Old)) / TimeDelta * 0.1f;
	_float fDiff_Velocity = (vVelocity_Cur.y - m_vVelocity_Old.y) / TimeDelta * 0.1f;
	fDiff_Velocity -= (m_fJumpPower + 0.f) / 0.1f;
	_int iDmg = _int(fDiff_Velocity*60.f / CPhysicsMgr::Get_Instance()->Get_Gravity() / 2.f);
	if (iDmg > 0 && m_vVelocity_Old.y < 0.f)
		TakeDamage(iDmg);

	m_vVelocity_Old = vVelocity_Cur;
}

void CInfantry::OnControlModeEnter()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Set_IsControl(true);
	m_isCommander = true;
}

void CInfantry::OnControlModeExit()
{
	if (nullptr == m_pWeapon)
		return;


	m_pWeapon->Set_IsControl(false);
	m_isCommander = false;
}

void CInfantry::OnTakeDamage(_int iDamage)
{
	if (nullptr == m_pTransformCom)
		return;


	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	if (nullptr == pSoundMgr) return;
	_tchar* pSoundTags[3] = { L"arrow_hit_body_1.mp3", L"arrow_hit_body_2.mp3" ,L"arrow_hit_body_3.mp3" };

	pSoundMgr->PlaySound_Overwrite(pSoundTags[rand() % 3], CSoundMgr::CHANNELID::PLAYER);

	_float3 vPosition = m_pTransformCom->Get_Position();
	CEffect::STATEDESC EffectDesc;
	ZeroMemory(&EffectDesc, sizeof(CEffect::STATEDESC));
	EffectDesc.eTextureScene = SCENE_STAGE;
	EffectDesc.pTextureKey = L"Component_Texture_Effect_Hit";
	EffectDesc.iTextureNum = 14;
	EffectDesc.iShaderPass = 11;
	EffectDesc.fFrameTime = 0.07;

	EffectDesc.vPosition = _float3(m_pTransformCom->Get_Position().x, m_pTransformCom->Get_Position().y + m_pTransformCom->Get_Scaled().y * 0.25f, m_pTransformCom->Get_Position().z);
	EffectDesc.vScale = _float3(20.f, 20.f, 1.f);
	CManagement::Get_Instance()->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect", &EffectDesc);
}






void CInfantry::Head_Direction(_double TimeDelta)
{
	_float fCX = D3DXVec3Length(&m_pTransformCom->Get_State(Engine::CTransform::STATE_RIGHT));
	_float fCY = D3DXVec3Length(&m_pTransformCom->Get_State(Engine::CTransform::STATE_UP));
	_float fCZ = D3DXVec3Length(&m_pTransformCom->Get_State(Engine::CTransform::STATE_LOOK));

	_float3 vVel = m_pRigidBodyCom->Get_Velocity();
	m_pRigidBodyCom->GetStateDesc_Direct().m_vMoveDir = vVel;
	_float3 vTemp = vVel;
	vTemp.y = 0.f;
	_float3 vVelNor, vOldNor;
	vVelNor = m_pRigidBodyCom->Get_Velocity();
	vOldNor = m_vVelocity_Old;
	D3DXVec3Normalize(&vVelNor, &vVelNor);
	D3DXVec3Normalize(&vOldNor, &vOldNor);
	_float fDegree = D3DXVec3Dot(&vVelNor, &vOldNor);

	if (D3DXVec3Length(&vTemp) < 0.01f /*|| fDegree / TimeDelta < D3DXToRadian(2.5f)*/)
		return;

	D3DXVec3Normalize(&vVel, &vVel);
	_float3 vNewLook, vNewRight, vNewUp;
	vNewLook = vVel;
	D3DXVec3Cross(&vNewRight, &_float3(0.f, 0.1, 0.f), &vNewLook);
	D3DXVec3Normalize(&vNewRight, &vNewRight);
	D3DXVec3Cross(&vNewUp, &vNewLook, &vNewRight);

	m_pTransformCom->Set_State(Engine::CTransform::STATE_RIGHT, vNewRight * fCX);
	m_pTransformCom->Set_State(Engine::CTransform::STATE_UP, vNewUp * fCY);
	m_pTransformCom->Set_State(Engine::CTransform::STATE_LOOK, vNewLook * fCZ);
}

_int CInfantry::CheckGround(_float fWeight)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return -1;

	Safe_AddRef(pManagement);

	//지형의 높이를 받아 위치에 적용한다.
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_Terrain"));
	if (nullptr == pTerrain) return -1;
	_float3	vCurrPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fCY = m_pTransformCom->Get_Scaled().y;
	_float3 vTerrainPos;
	if (pTerrain->Get_TerrainPos(vCurrPos, vTerrainPos))
	{
		//객체가 이동해야할 위치
		//현재 위치와 이동해야할 위치 사이의 보간된 위치
		//_float3 vNewPos;
		//D3DXVec3Lerp(&vNewPos, &vCurrPos, &vDst, 0.1f);

		//if (abs(vCurrPos.y - vNewPos.y) < 0.15f)
		//	m_bGround = true;
		//else
		//	m_bGround = false;

		if (vTerrainPos.y > vCurrPos.y - fCY * 0.5f)
		{
			//_float3 vDst = _float3(vCurrPos.x, vTerrainPos.y + fCY * 0.5f, vCurrPos.z);
			//////////////////////////////////////////////////////////////////////////////////////선형보간. 제거예정
			_float3 vTempPos = m_pTransformCom->Get_Position();
			_float3 vNextPos = vTempPos;
			vNextPos.y = vTerrainPos.y + fCY * 0.5f;
			D3DXVec3Lerp(&vNextPos, &vTempPos, &vNextPos, fWeight);

			m_pTransformCom->SetUp_Position(vNextPos);
			//////////////////////////////////////////////////////////////////////////////////////
			m_bGround = true;
		}
		else
			m_bGround = false;

		//m_pTransformCom->SetUp_Position(vNewPos);


	}

	Safe_Release(pManagement);

	return 0;
}

void CInfantry::Go_Dst(_float3 _vDst, _double _dblDeltaTime, _float* _fDist, _float fStoppingDistance)
{
	_float	fMaxSpeed = m_pRigidBodyCom->GetStateDesc().fMaxSpeed;
	if (!m_isCommander && m_bFriendly && m_eCommnad == COMMAND_FOLLOW)
	{
		CInputManager* pInputMgr = CInputManager::Get_Instance();
		if (nullptr == pInputMgr) return;
		CUnit* pCommander = pInputMgr->Get_PickedUnit();
		if (nullptr == pCommander) return;
		CTransform* pCommanderTransform = (CTransform*)pCommander->Find_Component(L"Com_Transform");
		_float fDist = D3DXVec3Length(&(pCommanderTransform->Get_Position() - m_pTransformCom->Get_Position()));

		if (fDist > 50.f)
		{
			m_pRigidBodyCom->GetStateDesc_Direct().m_vForce *= 0.f;
			_float fWalk_Force = m_fWalk_Force;
			fMaxSpeed *= 3;
			fWalk_Force = m_fWalk_Force * 3;
		}
	}

	m_pRigidBodyCom->GetStateDesc_Direct().m_vForce *= 0.f;
	_float3 vDir = _vDst - m_pTransformCom->Get_Position();
	_float fDist = D3DXVec3Length(&vDir);
	_float fWalk_Force = m_fWalk_Force;
	if (_fDist != nullptr)
		*_fDist = fDist;

	if (fDist < fStoppingDistance)
		return;

	vDir.y = 0.f;
	D3DXVec3Normalize(&vDir, &vDir);
	if (D3DXVec3Length(&m_pRigidBodyCom->Get_Velocity()) < fMaxSpeed)
		m_pRigidBodyCom->Add_Force(fWalk_Force * vDir);

	m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce = fWalk_Force * vDir;
}

HRESULT CInfantry::Go_Dest(_float3 vDest, _double TimeDelta, _float fStopDist)
{
	if (true == m_isArrived)
		return S_OK;

	//m_pTransformCom->LookAt(vDest);

	//_float3 vDist = vDest - m_pTransformCom->Get_WorldPosition();
	//_float fLength = D3DXVec3Length(&vDist);

	//if (fStopDist >= fLength)
	//	m_isArrived = true;



	//m_pTransformCom->Go_Straight(TimeDelta);





	_float	fMaxSpeed = m_pRigidBodyCom->GetStateDesc().fMaxSpeed;

	_float3 vDir = vDest - m_pTransformCom->Get_WorldPosition();
	_float fLength = D3DXVec3Length(&vDir);

	if (fStopDist + 50.f <= fLength && true == m_isEvent2)
	{
		m_pRigidBodyCom->GetStateDesc_Direct().m_vForce *= 0.f;
		_float fWalk_Force = m_fWalk_Force;
		fMaxSpeed *= 3;
		fWalk_Force = m_fWalk_Force * 3;
	}

	m_pRigidBodyCom->GetStateDesc_Direct().m_vForce *= 0.f;

	_float fWalk_Force = m_fWalk_Force;

	if (fLength < fStopDist)
	{
		m_isArrived = true;
		return S_OK;
	}

	vDir.y = 0.f;
	D3DXVec3Normalize(&vDir, &vDir);
	if (D3DXVec3Length(&m_pRigidBodyCom->Get_Velocity()) < fMaxSpeed)
		m_pRigidBodyCom->Add_Force(fWalk_Force * vDir);

	m_pRigidBodyCom->GetStateDesc_Direct().m_vInputForce = fWalk_Force * vDir;
	return S_OK;
}

CInfantry::AIM_FACTORS CInfantry::Aim_Target(_float3 vTarget, CWeapon* pBow, _float fHAngle)
{
	if (nullptr == m_pTextureCom ||
		nullptr == pBow)
		return CInfantry::AIM_FACTORS();

	_float3 vTempWeaponLocal = m_vWeaponLocalPos;
	vTempWeaponLocal.y = 0.f;
	_float3	vShooter = m_pTransformCom->Get_WorldPosition();
	_float3 vToTarget = vTarget - vShooter;

	D3DXVec3Normalize(&vToTarget, &vToTarget);
	_float	fRadYToTarget = D3DXVec3Dot(&vToTarget, &vTempWeaponLocal);
	_float3 vVecNor;
	D3DXVec3Cross(&vVecNor, &vToTarget, &_float3(0.f, 0.f, 1.f));
	if (vVecNor.y < 0.f)
		fRadYToTarget *= -1;

	_float	fPower = 0.f;
	_float	fPower_Max = dynamic_cast<CBow*>(pBow)->Get_MaxPower();
	CManagement* pManagement = CManagement::Get_Instance();

	_float3 vTempRight = m_pTransformCom->Get_WorldState(Engine::CTransform::STATE_RIGHT);
	vTempRight.y = 0.f;
	D3DXVec3Normalize(&vTempRight, &vTempRight);

	_float fTempAngle = 0.f;
	AIM_FACTORS tagResult[2];

	for (int n = 0; n < 2; ++n)
	{
		_bool	bIsPossible = 0;

		for (int i = -75; i <= 75; i += 1)
		{
			fTempAngle = (_float)i;

			if (n == 1)
				fTempAngle *= -1;

			//vShooter 좌표 보정////////////////////////////////////////////////////////////////////
			_matrix matTempRot;
			_matrix matTempRotY;
			D3DXMatrixIdentity(&matTempRot);
			D3DXMatrixIdentity(&matTempRotY);

			D3DXMatrixRotationAxis(&matTempRot, &_float3(1.f, 0.f, 0.f), D3DXToRadian(-fTempAngle));
			D3DXMatrixRotationAxis(&matTempRotY, &_float3(0.f, 1.f, 0.f), -fRadYToTarget);
			//D3DXMatrixRotationAxis(&matTempRot, &m_pTransformCom->Get_WorldState(Engine::CTransform::STATE_RIGHT), D3DXToRadian(-fTempAngle));

			vTempWeaponLocal = m_vWeaponLocalPos;
			D3DXVec3TransformNormal(&vTempWeaponLocal, &vTempWeaponLocal, &matTempRot);
			D3DXVec3TransformNormal(&vTempWeaponLocal, &vTempWeaponLocal, &matTempRotY);

			fPower = pManagement->Compute_Optimal_Power(fTempAngle, vShooter + vTempWeaponLocal, vTarget);
			if (fPower <= 0)
				continue;

			if (fPower <= fPower_Max)
			{
				bIsPossible = 1;
				break;
			}
		}
		if (n == 1 && bIsPossible == 1)
			int dfwseg = 1;
		tagResult[n].bPossibility = bIsPossible;
		tagResult[n].fAngle = fTempAngle;
		tagResult[n].fPower = fPower;

		if (bIsPossible == 0)
			return tagResult[n];
	}

	_float fAddDegree = (tagResult[1].fAngle - tagResult[0].fAngle) * fHAngle;

	AIM_FACTORS tagFinal;

	tagFinal.fAngle = tagResult[0].fAngle + fAddDegree;

	_matrix matTempRot;
	_matrix matTempRotY;
	D3DXMatrixIdentity(&matTempRot);
	D3DXMatrixIdentity(&matTempRotY);

	D3DXMatrixRotationAxis(&matTempRot, &_float3(1.f, 0.f, 0.f), D3DXToRadian(-fTempAngle));
	D3DXMatrixRotationAxis(&matTempRotY, &_float3(0.f, 1.f, 0.f), -fRadYToTarget);
	//D3DXMatrixRotationAxis(&matTempRot, &m_pTransformCom->Get_WorldState(Engine::CTransform::STATE_RIGHT), D3DXToRadian(-fTempAngle));

	vTempWeaponLocal = m_vWeaponLocalPos;
	D3DXVec3TransformNormal(&vTempWeaponLocal, &vTempWeaponLocal, &matTempRot);
	D3DXVec3TransformNormal(&vTempWeaponLocal, &vTempWeaponLocal, &matTempRotY);

	tagFinal.fPower = pManagement->Compute_Optimal_Power(tagFinal.fAngle, vShooter + vTempWeaponLocal, vTarget);

	m_pRigidBodyCom->GetStateDesc_Direct().m_vLocalPos = vTempWeaponLocal;

	if (fPower <= fPower_Max && fPower > 0.f)
		tagFinal.bPossibility = 1;
	else
		tagFinal.bPossibility = 0;

	return tagFinal;
}

void CInfantry::Attack_Archer(_float3	vTargetPos, _double TimeDelta)
{
	_float3 vTarget = vTargetPos;
	_float3 vSelf = m_pTransformCom->Get_WorldPosition();
	_float3 vDir = vTarget - vSelf;

	_float fRange = 0.3f;
	_int iDiff = 10;
	_int iTempProb = rand() % ((iDiff >> 1) * 2 + 1);
	iTempProb -= (iDiff >> 1);
	_float fHValue = iTempProb / (_float)iDiff * fRange;

	AIM_FACTORS tagTemp;

	if (D3DXVec3Length(&vDir) <= 30.f)
		tagTemp = Aim_Target(vTarget, m_pWeapon, 0.0f);
	else
		tagTemp = Aim_Target(vTarget, m_pWeapon, 0.5f + fHValue);

	if (tagTemp.bPossibility)
	{
		D3DXVec3Normalize(&vDir, &vDir);
		_float fTempRad = D3DXVec3Dot(&vDir, &_float3(0.f, 0.f, 1.f));

		_float3 vCross;
		D3DXVec3Cross(&vCross, &vDir, &_float3(0.f, 0.f, 1.f));

		if (vCross.y > 0)
			fTempRad *= -1;

		m_pTransformCom->LookAt_Radian(vTarget, D3DXToRadian(tagTemp.fAngle));

		dynamic_cast<CBow*>(m_pWeapon)->Attack(TimeDelta, tagTemp.fPower);


	}


}

HRESULT CInfantry::SetUp_Occupation(OCCUPATION _eOccupation)
{
	//클래스별 스텟 부여
	CWeapon::STATEDESC tWeaponDesc;
	switch (_eOccupation)
	{
	case OCC_WARRIOR:
	{
		m_tStat.iAtt = 3;
		m_tStat.iMaxHp = 50;
		m_tStat.iCurrHp = m_tStat.iMaxHp;
		m_tStat.fRecogRange = 70.f;
		m_tStat.fAggroRange = 70.f;
		m_tStat.fAttackRange = 15.f;
		m_tStat.fAttackSpeed = 1.f;
		tWeaponDesc.vScale = _float3(g_fUnitCX * 0.5f, g_fUnitCY * 1.5f, 1.f);
		tWeaponDesc.vPos = _float3(g_fUnitCX * 0.5f, tWeaponDesc.vScale.y * 0.25f, 0.f);
		if (nullptr == (m_pWeapon = CSword::Create_Sword_Prototype(m_pGraphic_Device)))
			return E_FAIL;

		break;
	}
	case OCC_MAGICIAN:
	{
		m_tStat.iAtt = 3;
		m_tStat.iMaxHp = 20;
		m_tStat.iCurrHp = m_tStat.iMaxHp;
		m_tStat.fRecogRange = 70.f;
		m_tStat.fAggroRange = 70.f;
		m_tStat.fAttackRange = 15.f;
		m_tStat.fAttackSpeed = 3.f;
		tWeaponDesc.vScale = _float3(g_fUnitCX, g_fUnitCY * 2.f, g_fUnitCZ);
		tWeaponDesc.vPos = _float3(g_fUnitCX * 0.5f, tWeaponDesc.vScale.y * 0.25f, 0.f);
		if (nullptr == (m_pWeapon = CStaff::Create_Staff_Prototype(m_pGraphic_Device)))
			return E_FAIL;

		break;
	}
	case OCC_ARCHER:
	{
		m_tStat.iAtt = 6;
		m_tStat.iMaxHp = 30;
		m_tStat.iCurrHp = m_tStat.iMaxHp;
		m_tStat.fRecogRange = 70.f;
		m_tStat.fAggroRange = 70.f;
		m_tStat.fAttackRange = 70.f;
		m_tStat.fAttackSpeed = 1.5f;

		tWeaponDesc.vScale = _float3(g_fUnitCX * 0.25f, g_fUnitCY, g_fUnitCZ * 0.5f);
		tWeaponDesc.vPos = _float3(0.f, tWeaponDesc.vScale.y * 0.15f, tWeaponDesc.vScale.z + m_pTransformCom->Get_Scaled().z * 0.5f - 2.f);
		tWeaponDesc.vScale = _float3(10.f, 10.f, 10.f);

		if (nullptr == (m_pWeapon = CBow::Create_Bow_Prototype(m_pGraphic_Device)))
			return E_FAIL;
		break;
	}
	}

	m_vWeaponLocalPos = tWeaponDesc.vPos;

	if (nullptr == m_pWeapon)
		MSG_BOX("Fail to Create Weapon");
	else
	{
		m_pWeapon->Ready_GameObject(&tWeaponDesc);
		//주인을 설정
		CWeapon::OWNER tOwner;
		tOwner.bFriendly = m_tDesc.bFriendly;
		tOwner.iOwnerID = Get_InstanceID();
		m_pWeapon->Set_Owner(tOwner);
	}

	return S_OK;
}

CUnit* CInfantry::Get_ClosestEnemy(_float& fDist)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return nullptr;

	list<CGameObject*> EnemyList;
	if (m_bFriendly)
	{
		list<CGameObject*>* pEnemyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");
		if (nullptr != pEnemyList)
			EnemyList = *pEnemyList;

		list<CGameObject*>* BossList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Boss");
		if (nullptr != BossList)
		{
			for (auto& pUnit : *BossList)
				EnemyList.push_back(pUnit);
		}

	}
	else
	{
		list<CGameObject*>* pAllyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");
		if (nullptr != pAllyList)
			EnemyList = *pAllyList;
	}

	//가장 가까이에 있는 적 구하기
	_float fMinDist = FLT_MAX;
	CGameObject* pClosest = nullptr;
	for (auto& pEnemy : EnemyList)
	{
		if (!pEnemy->Get_Active())
			continue;

		//적이 타워고, 내가 궁수가 아니면 무시
		if (dynamic_cast<CTower*>(pEnemy) != nullptr && m_tDesc.tOccupation != OCC_ARCHER)
			continue;

		//pEnemy->Get_Ac
		CTransform* pEnemyTransform = (CTransform*)pEnemy->Find_Component(L"Com_Transform");
		if (nullptr == pEnemyTransform)
			return nullptr;
		_float fDist = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pEnemyTransform->Get_Position()));
		if (fDist < m_tStat.fRecogRange && fDist < fMinDist)
		{
			fMinDist = fDist;
			pClosest = pEnemy;
		}
	}

	fDist = fMinDist;
	return dynamic_cast<CUnit*>(pClosest);

}


CUnit* CInfantry::Get_ClosestAlly(_float& fDist)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return nullptr;

	list<CGameObject*>* pAllyList = nullptr;
	if (m_bFriendly)
		pAllyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");
	else
		pAllyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");

	if (nullptr != pAllyList)
	{

		//가장 가까이에 있는 적 구하기
		_float fMinDist = FLT_MAX;
		CGameObject* pClosest = nullptr;
		for (auto& pAlly : *pAllyList)
		{
			if (!pAlly->Get_Active())
				continue;
			if (pAlly == this)
				continue;

			//pEnemy->Get_Ac
			CTransform* pAllyTransform = (CTransform*)pAlly->Find_Component(L"Com_Transform");
			if (nullptr == pAllyTransform)
				return nullptr;
			_float fDist = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pAllyTransform->Get_Position()));
			if (fDist < m_tStat.fRecogRange && fDist < fMinDist)
			{
				fMinDist = fDist;
				pClosest = pAlly;
			}
		}

		fDist = fMinDist;
		return dynamic_cast<CUnit*>(pClosest);


	}

	return nullptr;
}


void CInfantry::Set_IsEvent1(_bool bCheck)
{
	m_isEvent1 = bCheck;
	if (bCheck)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		// 택스트 출력
		CUI_Massage::STATEDESC maDesc;
		maDesc.iDepth = -1;
		maDesc.iTexIndex = 0;
		maDesc.vProjPos = _float2(_float(g_iWinSizeX >> 1), 300);
		maDesc.vProjSize = _float2(_float(g_iWinSizeX >> 1), 100);
		pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Massage", SCENE_STAGE, L"Layer_UI", &maDesc);

	}

}

void CInfantry::Set_IsEvent2(_bool bCheck)
{
	m_isEvent2 = bCheck;

	if (bCheck)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		// 택스트 출력
		CUI_Massage::STATEDESC maDesc;
		maDesc.iDepth = -1;
		maDesc.iTexIndex = 1;
		maDesc.vProjPos = _float2(_float(g_iWinSizeX >> 1), 200);
		maDesc.vProjSize = _float2(_float(g_iWinSizeX >> 1), 100);
		pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Massage", SCENE_STAGE, L"Layer_UI", &maDesc);

	}

}
CInfantry * CInfantry::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CInfantry*	pInstance = new CInfantry(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CInfantry");
		Safe_Release(pInstance);
	}
	return pInstance;
}




CGameObject * CInfantry::Clone_GameObject(void * pArg)
{
	CInfantry*	pInstance = new CInfantry(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CInfantry");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInfantry::Free()
{
	CPickingMgr::Get_Instance()->UnRegister_Observer(this);
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);


	Safe_Release(m_pWeapon);
	Safe_Release(m_pMesh);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCollderCom);
	Safe_Release(m_pRigidBodyCom);
	CUnit::Free();
}

_float3 CInfantry::Get_Position()
{
	if (nullptr == m_pTransformCom)
		return _float3();
	return m_pTransformCom->Get_Position();
}

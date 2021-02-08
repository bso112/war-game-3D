#include "stdafx.h"
#include "Infantry.h"

#include "Terrain.h"
#include "Object_Manager.h"
#include "PickingMgr.h"

typedef	struct tagRay
{
	D3DXVECTOR3 vOrigin;
	D3DXVECTOR3 vDir;

	tagRay() {};
	tagRay(D3DXVECTOR3 _vOrigin, D3DXVECTOR3 _vDir)
	{
		vOrigin = _vOrigin;
		vDir = _vDir;
	}
}MYRAY;

//마우스 좌표에서부터 레이를 만든다.
MYRAY Make_Ray(POINT& ptMouse, D3DXMATRIX ProjMatrix, D3DXMATRIX ViewMatrix)
{
	D3DXVECTOR4		vMousePos = D3DXVECTOR4((float)ptMouse.x, (float)ptMouse.y, 0.0f, 0.0f);

	// 2차원 투영 스페이스로 변환한다.(0, 0, g_iWinCX, g_iWinCY -> -1, 1, 1, -1)
	vMousePos.x = vMousePos.x / (g_iToolViewCX * 0.5f) - 1.f;
	vMousePos.y = vMousePos.y / -(g_iToolViewCY * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;


	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	// 뷰스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec4Transform(&vMousePos, &vMousePos, &ProjMatrix);
	D3DXVECTOR3		vMousePivot = D3DXVECTOR3(0.f, 0.f, 0.f);
	D3DXVECTOR3		vMouseRay = D3DXVECTOR3(vMousePos.x, vMousePos.y, vMousePos.z) - vMousePivot;

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// 월드스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec3TransformCoord(&vMousePivot, &vMousePivot, &ViewMatrix);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &ViewMatrix);
	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	return MYRAY(vMousePivot, vMouseRay);
}

CInfantry::CInfantry(PDIRECT3DDEVICE9 pGraphic_Device)
	:CUnit(pGraphic_Device)
{
}

CInfantry::CInfantry(const CInfantry & rhs)
	: CUnit(rhs)
{
}

const CInfantry::INFANTRYDESC CInfantry::Get_InfantryDesc()
{
	m_tDesc.tBasedesc.vPos = m_pTransformCom->Get_Position();

	return INFANTRYDESC(m_tDesc);
}

HRESULT CInfantry::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CInfantry::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(INFANTRYDESC));

	CPickingMgr::Get_Instance()->Register_Observer(this);

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_State()))
		return E_FAIL;

	//트랜스폼으로 크기를 지정하므로 지름이 1 되도록 만들자.
	D3DXCreateSphere(m_pGraphic_Device, 0.5f, 30, 10, &m_pMesh, NULL);

	m_bFriendly = m_tDesc.bFriendly;

	SetUp_Occupation(m_tDesc.tOccupation);

	return S_OK;
}

_int CInfantry::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
		return -1;

	if (!m_bActive)
		return 0;

	if (nullptr == m_pTransformCom)
		return -1;


	//땅을 체크해서 땅을 타고다니게 한다.
	//CheckGround();

	return _int(0);
}

_int CInfantry::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (!m_bActive)
		return 0;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_UNIT, this)))
		return -1;

	if (0x80000000 & m_pCollderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix()))
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return -1;

	Safe_Release(pManagement);

	return 0;
}

HRESULT CInfantry::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (!m_bActive)
		return 0;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);

	m_pMesh->DrawSubset(0);

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

_ulong CInfantry::AddRef(VOID)
{
	int a = 10;

	return CBase::AddRef();
}

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

HRESULT CInfantry::Go_Straight(_float fZ)
{
	if (FAILED(m_pTransformCom->AddForce(_float3(0.f, 0.f, fZ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Go_Backward(_float fZ)
{
	if (FAILED(m_pTransformCom->AddForce(_float3(0.f, 0.f, fZ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Go_Right(_float fX)
{
	if (FAILED(m_pTransformCom->AddForce(_float3(fX, 0.f, 0.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Go_Left(_float fX)
{
	if (FAILED(m_pTransformCom->AddForce(_float3(fX, 0.f, 0.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Go_Up(_float fY)
{
	if (FAILED(m_pTransformCom->AddForce(_float3(0.f, fY, 0.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Go_Down(_float fY)
{
	if (FAILED(m_pTransformCom->AddForce(_float3(0.f, fY, 0.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Turn_Right(_double TimeDelta)
{
	if (FAILED(m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::Turn_Left(_double TimeDelta)
{
	if (FAILED(m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	if (m_bFriendly)
		m_pShaderCom->Set_Value("g_vColor", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));
	else
		m_pShaderCom->Set_Value("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));


	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", 1)))
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CInfantry::SetUp_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Default", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	CTransform::STATEDESC StateDesc;
	StateDesc.SpeedPerSec = 4.0;
	StateDesc.RotatePerSec = D3DXToRadian(30.f);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &StateDesc)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Unit", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Collider
	CCollider_Ellipse::STATEDESC_DERIVED tColliderDesc;
	tColliderDesc.fRadius = m_tDesc.tBasedesc.vSize.x * 0.5f;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Ellipse", L"Com_Collider", (CComponent**)&m_pCollderCom, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfantry::SetUp_State()
{
	m_pTransformCom->SetUp_Position(m_tDesc.tBasedesc.vPos);
	m_pTransformCom->SetUp_Scale(m_tDesc.tBasedesc.vSize);

	m_tStat.iAtt = 5;
	m_tStat.iMaxHp = 30;
	m_tStat.iCurrHp = m_tStat.iMaxHp;

	return S_OK;
}

void CInfantry::OnCollisionEnter(CGameObject * _pOther)
{
	int a = 0;
}

_int CInfantry::Move(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (m_bControlMode)
	{
		Move_ControlMode(TimeDelta);
	}
	else
	{
		//웨이포인트 따라가기
		if (!m_WayPoints.empty())
		{
			if (m_pTransformCom->Go_Dst(m_WayPoints.front(), TimeDelta))
			{
				m_WayPoints.pop_front();
			}
		}

		//주변에 가까운 적이 있으면 그쪽으로 가서 공격
		Patrol(TimeDelta);
	}

	Safe_Release(pManagement);

	return 0;
}

_int CInfantry::Patrol(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	_float fDist = 0.f;
	CUnit* pClosest = Get_ClosestEnemy(fDist);
	//가장 가까운 적이 있으면
	if (nullptr != pClosest)
	{
		CTransform* pTargetTransfrom = (CTransform*)pClosest->Find_Component(L"Com_Transform");
		if (nullptr == pTargetTransfrom)
			return -1;

		//적을 향해 가기
		m_pTransformCom->Go_Dst(pTargetTransfrom->Get_Position(), TimeDelta);

		//공격범위안에 들어오면
		if (fDist < m_tStat.fAttackRange)
		{
			m_eState = STATE_ATTACK;
			m_pTarget = dynamic_cast<CUnit*>(pClosest);
		}
	}

	return 0;
}

_int CInfantry::CheckGround()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return -1;

	Safe_AddRef(pManagement);

	//지형의 높이를 받아 위치에 적용한다.
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pManagement->Get_ObjectPointer(SCENE_STATIC, L"Layer_Terrain"));
	if (nullptr == pTerrain)
	{
		Safe_Release(pManagement);
		return -1;
	}

	_float3	vCurrPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fCY = m_pTransformCom->Get_Scaled().y;
	_float3 vTerrainPos;
	if (pTerrain->Get_TerrainPos(vCurrPos, vTerrainPos))
	{
		//객체가 이동해야할 위치
		_float3 vDst = _float3(vCurrPos.x, vTerrainPos.y + fCY * 0.5f, vCurrPos.z);
		//현재 위치와 이동해야할 위치 사이의 보간된 위치
		_float3 vNewPos;
		D3DXVec3Lerp(&vNewPos, &vCurrPos, &vDst, 0.1f);

		if (abs(vCurrPos.y - vNewPos.y) < 0.15f)
			m_bGround = true;
		else
			m_bGround = false;

		m_pTransformCom->SetUp_Position(vNewPos);
	}

	Safe_Release(pManagement);

	return 0;
}

_int CInfantry::Move_ControlMode(_double TimeDelta)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement || nullptr == m_pTransformCom)
		return -1;

	//움직임
	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_W))
		m_pTransformCom->Go_Straight(TimeDelta);

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_A))
		m_pTransformCom->Go_Left(TimeDelta);

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_S))
		m_pTransformCom->Go_BackWard(TimeDelta);

	if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_D))
		m_pTransformCom->Go_Right(TimeDelta);

	_long MouseMove = 0;

	//회전
	if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_X))
	{
		m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta * (MouseMove / 5));
	}

	if (MouseMove = pManagement->Get_DIMouseMoveState(CInput_Device::DIMM_Y))
	{
		m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * (MouseMove / 5));
	}

	return _int();
}

HRESULT CInfantry::SetUp_Occupation(OCCUPATION _eOccupation)
{

	return S_OK;
}

CUnit * CInfantry::Get_ClosestEnemy(_float& fDist)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return nullptr;

	list<CGameObject*>* pEnemyList = nullptr;
	if (m_bFriendly)
		pEnemyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Enemy");
	else
		pEnemyList = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");

	if (nullptr != pEnemyList)
	{
		//가장 가까이에 있는 적 구하기
		_float fMinDist = FLT_MAX;
		CGameObject* pClosest = nullptr;
		for (auto& pEnemy : *pEnemyList)
		{
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

	return nullptr;
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

	m_pTarget = nullptr;

	Safe_Release(m_pMesh);
	Safe_Release(m_pCollderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CUnit::Free();
}

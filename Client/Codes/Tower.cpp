#include "stdafx.h"
#include "Tower.h"
#include "Object_Manager.h"
#include "Terrain.h"
#include "PickingMgr.h"
#include "Management.h"
#include "Sword.h"
#include "Staff.h"
#include "Bow.h"
#include "PickingMgr.h"


CTower::CTower(PDIRECT3DDEVICE9 pGraphic_Device)
	:CUnit(pGraphic_Device)
{
}

CTower::CTower(const CTower & rhs)
	: CUnit(rhs)
{
}

HRESULT CTower::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CTower::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(INFANTRYDESC));

	if (FAILED(CUnit::Ready_GameObject(pArg)))
		return E_FAIL;

	CPickingMgr::Get_Instance()->Register_Observer(this);

	if (!Load_Mesh())
		MSG_BOX("Fail to Load mesh");

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	if (FAILED(SetUp_State()))
		return E_FAIL;


	m_bFriendly = m_tDesc.bFriendly;

	SetUp_Occupation(m_tDesc.tOccupation);

	if (m_tDesc.bStatic)
		m_eState = STATE_ATTACK;

	return S_OK;
}

_int CTower::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
		return -1;
	if (!m_bActive)
		return 0;

	if (nullptr == m_pTransformCom)
		return -1;

	/////////////////////////////////////////고각 이용 여부
	//if (GetAsyncKeyState(VK_F1))
	//	m_bIsHigh = 0;

	//if (GetAsyncKeyState(VK_F2))
	//	m_bIsHigh = 1;
	////////////////////////////////////////


	if (nullptr == m_pWeapon)
		return -1;


	Battle(TimeDelta);

	//무기 업데이트
	m_pWeapon->Update_GameObject(TimeDelta);



	return _int();
}

_int CTower::Late_Update_GameObject(_double TimeDelta)
{

	if (nullptr == m_pRendererCom ||
		nullptr == m_pWeapon ||
		nullptr == m_pCollderCom)
		return -1;

	if (!m_bActive)
		return 0;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);
	if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position()))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;

		if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_TOWER, this)))
			return -1;
	}

	//무기 레잇업데이트
	m_pWeapon->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix_Scale1());
	m_pWeapon->Late_Update_GameObject(TimeDelta);

	m_pCollderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());


	Safe_Release(pManagement);
	return 0;
}

HRESULT CTower::Render_GameObject()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (!m_bActive)
		return 0;

	m_pWeapon->Render_GameObject();

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

	m_pCollderCom->Render_Collider();


	return S_OK;
}

_bool CTower::Picking(POINT & ptMouse)
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

HRESULT CTower::SetUp_ConstantTable()
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

HRESULT CTower::SetUp_Component()
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
	m_pTransformCom->SetUp_Scale(_float3(6.f, 6.f, 6.f));

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Unit", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Unit", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Collider
	CCollider_Ellipse::STATEDESC_DERIVED tColliderDesc;
	tColliderDesc.fRadius = m_pTransformCom->Get_Scaled().x * 0.5f;
	D3DXMatrixIdentity(&tColliderDesc.StateMatrix);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Ellipse", L"Com_Collider", (CComponent**)&m_pCollderCom, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTower::SetUp_State()
{
	m_tStat.iAtt = 5;
	m_tStat.iMaxHp = 30;
	m_tStat.iCurrHp = m_tStat.iMaxHp;

	return S_OK;
}

_bool CTower::Load_Mesh()
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


void CTower::OnControlModeEnter()
{
	if (nullptr == m_pWeapon)
		return;


	m_pWeapon->Set_IsControl(true);
}

void CTower::OnControlModeExit()
{
	if (nullptr == m_pWeapon)
		return;


	m_pWeapon->Set_IsControl(false);
}

void CTower::OnTakeDamage(_int iDamage)
{
	if (nullptr == m_pTransformCom)
		return;
	int i = 0;
}

void CTower::OnCollisionEnter(CGameObject * _pOther)
{
	int i = 0;
}


_int CTower::Battle(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	_float fEnemyDist = 0.f;
	CUnit* pEnemy = Get_ClosestEnemy(fEnemyDist);

	//대상이 있으면
	if (nullptr != pEnemy)
	{
		//타깃이 죽었으면 패트롤
		if (!pEnemy->Get_Dead())
		{
			CTransform* pTargetTransform = (CTransform*)pEnemy->Find_Component(L"Com_Transform");
			if (nullptr == pTargetTransform)
				return -1;

			_float fDist = D3DXVec3Length(&(m_pTransformCom->Get_Position() - pTargetTransform->Get_Position()));

			//어그로 범위안에 있고 공격범위안에 있으면 공격
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
		}
	}

	return 0;
}





CTower::AIM_FACTORS CTower::Aim_Target(_float3 vTarget, CWeapon* pBow, _float fHAngle)
{
	if (nullptr == m_pTextureCom ||
		nullptr == pBow)
		return CTower::AIM_FACTORS();

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

	//m_pRigidBodyCom->GetStateDesc_Direct().m_vLocalPos = vTempWeaponLocal;

	if (fPower <= fPower_Max && fPower > 0.f)
		tagFinal.bPossibility = 1;
	else
		tagFinal.bPossibility = 0;

	return tagFinal;
}

void CTower::Attack_Archer(_float3	vTargetPos, _double TimeDelta)
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

HRESULT CTower::SetUp_Occupation(OCCUPATION _eOccupation)
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
		m_tStat.fRecogRange = 80.f;
		m_tStat.fAggroRange = 80.f;
		m_tStat.fAttackRange = 3.5f;
		m_tStat.fAttackSpeed = 1.f;
		tWeaponDesc.vPos = _float3(1.f, 1.f, 0.f);
		tWeaponDesc.vScale = _float3(0.5f, 2.f, 0.5f);
		if (nullptr == (m_pWeapon = CSword::Create_Sword_Prototype(m_pGraphic_Device)))
			return E_FAIL;

		break;
	}
	case OCC_MAGICIAN:
	{
		m_tStat.iAtt = 3;
		m_tStat.iMaxHp = 20;
		m_tStat.iCurrHp = m_tStat.iMaxHp;
		m_tStat.fRecogRange = 80.f;
		m_tStat.fAggroRange = 80.f;
		m_tStat.fAttackRange = 50.f;
		m_tStat.fAttackSpeed = 3.f;
		tWeaponDesc.vPos = _float3(1.f, 1.f, 0.f);
		tWeaponDesc.vScale = _float3(0.5f, 2.f, 0.5f);
		if (nullptr == (m_pWeapon = CStaff::Create_Staff_Prototype(m_pGraphic_Device)))
			return E_FAIL;

		break;
	}
	case OCC_ARCHER:
	{
		m_tStat.iAtt = 2;
		m_tStat.iMaxHp = 30;
		m_tStat.iCurrHp = m_tStat.iMaxHp;
		m_tStat.fRecogRange = 100.f;
		m_tStat.fAggroRange = 100.f;
		m_tStat.fAttackRange = 100.f;
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

CUnit* CTower::Get_ClosestEnemy(_float& fDist)
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
			if (!pEnemy->Get_Active())
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

	return nullptr;
}


CTower * CTower::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CTower*	pInstance = new CTower(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CTower");
		Safe_Release(pInstance);
	}
	return pInstance;
}




CGameObject * CTower::Clone_GameObject(void * pArg)
{
	CTower*	pInstance = new CTower(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CTower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTower::Free()
{
	CPickingMgr::Get_Instance()->UnRegister_Observer(this);

	Safe_Release(m_pCollderCom);
	Safe_Release(m_pWeapon);
	Safe_Release(m_pMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

	CUnit::Free();
}

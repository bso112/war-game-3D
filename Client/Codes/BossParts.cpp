#include "stdafx.h"
#include "..\Headers\BossParts.h"
#include "PickingMgr.h"

CBossParts::CBossParts(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBossParts::CBossParts(const CBossParts & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossParts::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBossParts::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.BaseDesc.vSize);
	m_pTransformCom->SetUp_Position(m_StateDesc.BaseDesc.vPos);

	if (OBJ_RIGHTARM == m_StateDesc.eObjID || OBJ_LEFTARM == m_StateDesc.eObjID)
		if (FAILED(Ready_Layer_Sword(L"Layer_Sword")))
			return E_FAIL;

	if (OBJ_HEAD == m_StateDesc.eObjID)
		if (!Load_Mesh())
			MSG_BOX("Fail to Load mesh");

	return S_OK;
}

_int CBossParts::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
	{
		if (OBJ_RIGHTARM == m_StateDesc.eObjID || OBJ_LEFTARM == m_StateDesc.eObjID)
			m_pSword->Set_Dead();

		return -1;
	}

	if (0x80000000 & Update_Movement(TimeDelta))
		return -1;

	if (0x80000000 & Update_Attack(TimeDelta))
		return -1;

	if (0x80000000 & Set_Sword_ParentsMatrix(TimeDelta))
		return -1;

	return _int(0);
}

_int CBossParts::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position()))
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;

	Safe_Release(pManagement);

	return _int(0);
}

HRESULT CBossParts::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(Set_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(PASS_DEFAULT);

	if (OBJ_HEAD == m_StateDesc.eObjID)
		for (_uint i = 0; i < m_Mtrls.size(); i++)
			m_pMesh->DrawSubset(i);
	else
		m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

_int CBossParts::Update_Welcome(_double TimeDelta)
{
	m_isWelcome = true;

	_matrix RotationMatrix;
	_float3 vAxis = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	m_MoveTimer += TimeDelta;

	switch (m_StateDesc.eObjID)
	{
	case Client::CBossParts::OBJ_RIGHTARM:
		if (1.0 >= m_MoveTimer)
		{
			m_pSword->Set_IsBoss(true);
			D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(90.f) * -(_float)m_MoveTimer);
			m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);

			m_isSaveTimer = m_MoveTimer;
		}
		else if (false == m_pSword->Get_IsBoss())
		{
			m_isSaveTimer -= TimeDelta;
			if (0.0 >= m_isSaveTimer)
			{
				m_isSaveTimer = 0.0;
				m_MoveTimer = 0.0;
				m_isWelcome = false;
			}

			D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(90.f) * -(_float)m_isSaveTimer);
			m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
		}
		break;
	case Client::CBossParts::OBJ_LEFTARM:
		if (1.0 >= m_MoveTimer)
		{
			m_pSword->Set_IsBoss(true);
			D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(90.f) * -(_float)m_MoveTimer);
			m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);

			m_isSaveTimer = m_MoveTimer;
		}
		else if (false == m_pSword->Get_IsBoss())
		{
			m_isSaveTimer -= TimeDelta;
			if (0.0 >= m_isSaveTimer)
			{
				m_isSaveTimer = 0.0;
				m_MoveTimer = 0.0;
				m_isWelcome = false;
			}

			D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(90.f) * -(_float)m_isSaveTimer);
			m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
		}
		break;
	}

	return _int(0);
}

_bool CBossParts::Load_Mesh()
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

HRESULT CBossParts::Add_Component()
{
	if (OBJ_HEAD == m_StateDesc.eObjID)
	{
		// For.Com_Shader
		if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Unit", L"Com_Shader", (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		// For.Com_Shader
		if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Cube", L"Com_Shader", (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	CTransform::STATEDESC tTransformDesc;
	tTransformDesc.SpeedPerSec = 5.0;
	tTransformDesc.RotatePerSec = D3DXToRadian(90.f);
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;

	if (OBJ_HEAD == m_StateDesc.eObjID)
	{
		// For.Com_Texture
		if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Boss", L"Com_Texture", (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else
	{
		// For.Com_Texture
		if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Sky", L"Com_Texture", (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossParts::Set_ConstantTable()
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

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CBossParts::Ready_Layer_Sword(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CSword::STATEDESC StateDesc;
	StateDesc.vScale = _float3(1.f, g_fUnitCY * 3.f, g_fUnitCZ * 1.5f);
	StateDesc.vPos = _float3(0.f, StateDesc.vScale.y * 0.4f, 0.f);
	StateDesc.iAttack = 10;
	if (FAILED(m_pSword = dynamic_cast<CSword*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Sword", SCENE_STAGE, pLayerTag, &StateDesc))))
		return E_FAIL;

	Safe_AddRef(m_pSword);

	m_pSword->Set_HasBoss(true);

	Safe_Release(pManagement);

	return S_OK;
}

_int CBossParts::Set_Sword_ParentsMatrix(_double TimeDelta)
{
	if (OBJ_RIGHTARM != m_StateDesc.eObjID && OBJ_LEFTARM != m_StateDesc.eObjID)
		return 0;

	if (nullptr == m_pTransformCom)
		return -1;

	// ParentsMatrix
	_matrix ParentsMatrix = m_pTransformCom->Get_WorldMatrix_Scale1();
	_float3 vUp = (_float3)ParentsMatrix.m[1];
	_float3 vPos = (_float3)ParentsMatrix.m[3];

	// Nomalize
	D3DXVec3Normalize(&vUp, &vUp);

	// Offset
	vPos += vUp * (g_fUnitCY * -0.5f);

	memcpy(ParentsMatrix.m[3], &vPos, sizeof(_float3));
	m_pSword->Set_ParentsMatrix(ParentsMatrix);

	return _int(0);
}

_int CBossParts::Update_Movement(_double TimeDelta)
{
	if (false == m_hasMoved)
		return 0;

	m_MoveTimer += false == m_isSteped ? TimeDelta : -TimeDelta;
	if (0.25 <= m_MoveTimer)			// 0.5 Step
		m_isSteped = true;
	else if (0.0 >= m_MoveTimer)	// One Step
	{
		m_isSteped = false;
		m_hasMoved = false;
		m_isLeft = true == m_isLeft ? false : true;
	}

	_matrix RotationMatrix;
	_float3 vAxis = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float fTimer = 0.f;	// 공전 시간

	switch (m_StateDesc.eObjID)
	{
	case Client::CBossParts::OBJ_RIGHTARM:
		fTimer = true == m_isLeft ? -(_float)m_MoveTimer : (_float)m_MoveTimer;
		D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(180.f) * fTimer);
		m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
		break;
	case Client::CBossParts::OBJ_LEFTARM:
		fTimer = true == m_isLeft ? (_float)m_MoveTimer : -(_float)m_MoveTimer;
		D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(180.f) * fTimer);
		m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
		break;
	case Client::CBossParts::OBJ_RIGHTLEG:
		fTimer = true == m_isLeft ? (_float)m_MoveTimer : -(_float)m_MoveTimer;
		D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(180.f) * fTimer);
		m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
		break;
	case Client::CBossParts::OBJ_LEFTLEG:
		fTimer = true == m_isLeft ? -(_float)m_MoveTimer : (_float)m_MoveTimer;
		D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(180.f) * fTimer);
		m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
		break;
	}

	return _int(0);
}

_int CBossParts::Update_Attack(_double TimeDelta)
{
	if (0 == m_iAttackType)
		return 0;

	_matrix RotationMatrix;
	_float3 vRightAxis = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float3 vLookAxis = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_AttackTimer += TimeDelta;

	if (1 == m_iAttackType)
	{
		switch (m_StateDesc.eObjID)
		{
		case Client::CBossParts::OBJ_RIGHTARM:
			if (1.0 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(180) * -(_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 >= m_AttackTimer)
			{
				_double Timer = 2.0 - m_AttackTimer;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(180) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vRightAxis, 0.f);
			}
			break;
		case Client::CBossParts::OBJ_LEFTARM:
			if (1.0 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(180) * -(_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 >= m_AttackTimer)
			{
				_double Timer = 2.0 - m_AttackTimer;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(180) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vRightAxis, 0.f);
			}
			break;
		case Client::CBossParts::OBJ_RIGHTLEG:
			if (0.5 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (1.0 >= m_AttackTimer)
			{
				_double Timer = 1.0 - m_AttackTimer;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (1.5 >= m_AttackTimer)
			{
				_double Timer = m_AttackTimer - 1.0;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 >= m_AttackTimer)
			{
				_double Timer = 2.0 - m_AttackTimer;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vRightAxis, 0.f);
			}
			break;
		case Client::CBossParts::OBJ_LEFTLEG:
			if (0.5 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (1.0 >= m_AttackTimer)
			{
				_double Timer = 1.0 - m_AttackTimer;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (1.5 >= m_AttackTimer)
			{
				_double Timer = m_AttackTimer - 1.0;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 >= m_AttackTimer)
			{
				_double Timer = 2.0 - m_AttackTimer;
				D3DXMatrixRotationAxis(&RotationMatrix, &vRightAxis, D3DXToRadian(90.f) * -(_float)Timer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (2.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vRightAxis, 0.f);
			}
			break;
		}
	}

	else if (2 == m_iAttackType)
	{
		switch (m_StateDesc.eObjID)
		{
		case Client::CBossParts::OBJ_RIGHTARM:
			if (1.0 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vLookAxis, D3DXToRadian(90.f) * (_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (3.0 >= m_AttackTimer) {}
			else if (4.0 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vLookAxis, D3DXToRadian(90.f) * -(_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (5.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vLookAxis, 0.f);
			}
			break;
		case Client::CBossParts::OBJ_LEFTARM:
			if (1.0 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vLookAxis, D3DXToRadian(90.f) * -(_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (3.0 >= m_AttackTimer) {}
			else if (4.0 >= m_AttackTimer)
			{
				D3DXMatrixRotationAxis(&RotationMatrix, &vLookAxis, D3DXToRadian(90.f) * (_float)m_AttackTimer);
				m_pTransformCom->Set_RevolutionMatrix(RotationMatrix);
			}
			else if (5.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vLookAxis, 0.f);
			}
			break;
		case Client::CBossParts::OBJ_RIGHTLEG:
			if (5.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vLookAxis, 0.f);
			}
			break;
		case Client::CBossParts::OBJ_LEFTLEG:
			if (5.0 < m_AttackTimer)
			{
				m_iAttackType = 0;
				m_AttackTimer = 0.0;
				m_pTransformCom->SetUp_Rotation(vLookAxis, 0.f);
			}
			break;
		}
	}

	return _int(0);
}

CBossParts * CBossParts::Create_BossParts_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossParts*	pInstance = new CBossParts(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBossParts Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossParts::Clone_GameObject(void * pArg)
{
	CBossParts*	pInstance = new CBossParts(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CBossParts Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossParts::Free()
{
	if (OBJ_RIGHTARM == m_StateDesc.eObjID || OBJ_LEFTARM == m_StateDesc.eObjID)
	{
		m_pSword->Set_Dead();
		Safe_Release(m_pSword);
	}

	if (OBJ_HEAD == m_StateDesc.eObjID)
		Safe_Release(m_pMesh);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

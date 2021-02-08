#include "stdafx.h"
#include "..\Headers\HealingZone.h"
#include "Management.h"
#include "ParticleSystem.h"
#include "Unit.h"

CHealingZone::CHealingZone(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CHealingZone::CHealingZone(const CHealingZone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHealingZone::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CHealingZone::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	Load_Mesh();

	m_StateDesc.fRadius *= 2.f; // 반지름의 2배의 스케일이 되어야함
	m_pTransformCom->SetUp_Scale(_float3(m_StateDesc.fRadius, m_StateDesc.fRadius, m_StateDesc.fRadius));
	m_pTransformCom->SetUp_Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	m_pTransformCom->SetUp_Position(m_StateDesc.vPosition);

	CParticleSystem::STATEDESC ParticleDesc;
	ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATEDESC));
	ParticleDesc.eType = CParticleSystem::PARTICLE_UP;
	ParticleDesc.eTextureScene = SCENE_STAGE;
	ParticleDesc.pTextureKey = L"Component_Texture_HealingZone";
	ParticleDesc.iShaderPass = 0;
	ParticleDesc.dDuration = m_StateDesc.fLifeTime;
	ParticleDesc.dParticleLifeTime = 2;
	ParticleDesc.dSpawnTime = 0.1;
	ParticleDesc.fParticlePerSecond = 100;
	ParticleDesc.fVelocityDeviation = 1.f;
	ParticleDesc.vPosition = m_pTransformCom->Get_Position();
	ParticleDesc.iMaxParticleCount = 5000;
	ParticleDesc.vParticleScale = _float2(m_StateDesc.fRadius, m_StateDesc.fRadius);
	ParticleDesc.vParticleDeviation = _float3(m_StateDesc.fRadius * 0.6f, 0.f, m_StateDesc.fRadius * 0.6f);
	ParticleDesc.vVelocity = _float3(0.f, 2.f, 0.f);
	if (FAILED(CManagement::Get_Instance()->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_ParticleSystem", SCENE_STAGE, L"Layer_Particle", &ParticleDesc)))
		return E_FAIL;


	return S_OK;
}

_int CHealingZone::Update_GameObject(_double TimeDelta)
{
	m_dCurrentLifeTime += TimeDelta;
	m_dCurrentRecoverTime += TimeDelta;
	if (m_dCurrentLifeTime >= m_StateDesc.fLifeTime)
		m_bDead = true;

	if (m_dCurrentRecoverTime >= m_StateDesc.fRecoverTime)
	{
		m_dCurrentRecoverTime = 0.f;
		Healing_Ally();
	}
	return NOERROR;
}

_int CHealingZone::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	Compute_ViewZ(m_pTransformCom->Get_Position());

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return -1;

	return _int();
}

HRESULT CHealingZone::Render_GameObject()
{
	if (	nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);

	for (_uint i = 0; i < m_Mtrls.size(); i++)
		m_pMesh->DrawSubset(i);

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CHealingZone::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Unit", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_HealingSphere", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHealingZone::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CHealingZone::Healing_Ally()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	list<CGameObject*>* pLayer = pManagement->Get_ObjectList(SCENE_STAGE, L"Layer_Unit_Ally");
	if (nullptr == pLayer)
	{
		Safe_Release(pManagement);
		return E_FAIL;
	}

	for (CGameObject* pGameObject : *pLayer)
	{
		CTransform* pTransform = (CTransform*)pGameObject->Find_Component(L"Com_Transform");
		_float3 DirVec = m_pTransformCom->Get_Position() - pTransform->Get_Position();
		if (m_StateDesc.fRadius > D3DXVec3Length(&DirVec))
		{
			CUnit* pUnit = (CUnit*)pGameObject;
			pUnit->Heal((_int)m_StateDesc.fRecoverHp);
		}
	}

	Safe_Release(pManagement);
	return S_OK;
}

_bool CHealingZone::Load_Mesh()
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

CHealingZone * CHealingZone::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CHealingZone*	pInstance = new CHealingZone(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CHealingZone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHealingZone::Clone_GameObject(void * pArg)
{
	CHealingZone*	pInstance = new CHealingZone(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CHealingZone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHealingZone::Free()
{
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}

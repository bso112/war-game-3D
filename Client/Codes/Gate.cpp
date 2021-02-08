#include "stdafx.h"
#include "Management.h"
#include "..\Headers\Gate.h"
#include "PickingMgr.h"
#include "Boss.h"
#include "Effect.h"
#include "SoundMgr.h"

CGate::CGate(PDIRECT3DDEVICE9 pGraphic_Device)
	:CUnit(pGraphic_Device)
{
}

CGate::CGate(const CGate & rhs)
	: CUnit(rhs)
{
}

HRESULT CGate::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CGate::Ready_GameObject(void * pArg)
{
	if (!Load_Mesh())
		MSG_BOX("Fail to Load mesh");

	if (FAILED(Add_Component()))
		MSG_BOX("Fail to Add Component of Castle");

	m_pParent = (CGameObject*)pArg;

	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);
	m_pTransformCom->Set_WorldMatrix(matTemp);
	m_pOriginalTransform->SetUp_Position(_float3(0.f, 0.f, -10.5f));
	//m_pTransformCom->SetUp_Scale(_float3(4.f, 4.f, 4.f));
	//m_pTransformCom->SetUp_Rotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(45.f));

	CPickingMgr::Get_Instance()->Register_Observer(this);

	m_tStat.iMaxHp = 200;
	m_tStat.iCurrHp = m_tStat.iMaxHp;

	return S_OK;
}

_int CGate::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
	{
		CManagement* pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return -1;

		Safe_AddRef(pManagement);

		CBoss* pBoss = dynamic_cast<CBoss*>(pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_Boss"));
		if (nullptr == pBoss)
			return -1;

		pBoss->Set_IsWelcome(true);

		CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
		if (nullptr == pSoundMgr)
			return -1;

		_tchar* pSoundTags[2] = { L"Boss_Stage0.wav", L"Boss_Stage1.wav"};

		pSoundMgr->StopAll();
		pSoundMgr->PlayBGM(pSoundTags[rand() % 2]);

		CEffect::STATEDESC EffectDesc;
		ZeroMemory(&EffectDesc, sizeof(CEffect::STATEDESC));
		EffectDesc.bAlpha = true;
		EffectDesc.eTextureScene = SCENE_STAGE;
		EffectDesc.pTextureKey = L"Component_Texture_Effect_Break";
		EffectDesc.iTextureNum = 91;
		EffectDesc.iShaderPass = 4;
		EffectDesc.fFrameTime = 0.03f;

		EffectDesc.vPosition = m_pTransformCom->Get_Position();
		EffectDesc.vScale = _float3(50.f, 50.f, 1.f);
		pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect", &EffectDesc);

		Safe_Release(pManagement);

		return -1;
	}

	_matrix matOri = m_pOriginalTransform->Get_WorldMatrix();
	_matrix matParent = ((CTransform*)m_pParent->Find_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pTransformCom->Set_WorldMatrix(matOri*matParent);
	//_matrix matParent = CManagement::Get_Instance()->Get_ComponentPointer(m_pParent->Get_Name())

	m_pCollider->Update_Collider(m_pTransformCom->Get_WorldMatrix());
	return _int();
}

_int CGate::Late_Update_GameObject(_double TimeDelta)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position()))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;
		if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_GATE, this)))
			return -1;
	}

	Safe_Release(pManagement);
	return _int();
}

HRESULT CGate::Render_GameObject()
{

	if (nullptr == m_pMesh ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(0);

	for (_uint i = 0; i < m_Mtrls.size(); i++)
	{
		m_pMesh->DrawSubset(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();



	return S_OK;
}


_bool CGate::Load_Mesh()
{
	HRESULT hr = 0;

	//
	// Load the XFile data.
	//

	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		L"../Bin/Meshs/gate.x",
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

HRESULT CGate::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Original_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Original_Transform", (CComponent**)&m_pOriginalTransform)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Shader_Gate", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Box", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;


	//////test

	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Gate", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;
	//////

	return S_OK;

}

HRESULT CGate::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));


	for (_uint i = 0; i < m_Mtrls.size(); i++)
	{
		m_pGraphic_Device->SetMaterial(&m_Mtrls[i]);
		if (FAILED(m_pTexture->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", i)))
			continue;
	}

	D3DLIGHT9	LightDesc = *pManagement->Get_LightDesc(0);

	m_pShaderCom->Set_Value("g_vLightDir", &_float4(LightDesc.Direction, 0.f), sizeof(_float4));
	m_pShaderCom->Set_Value("g_vLightDiffuse", &LightDesc.Diffuse, sizeof(_float4));
	m_pShaderCom->Set_Value("g_vLightAmbient", &LightDesc.Ambient, sizeof(_float4));
	m_pShaderCom->Set_Value("g_vLightSpecular", &LightDesc.Specular, sizeof(_float4));
	m_pShaderCom->Set_Value("g_vCamPosition", &_float4(pManagement->Get_CamPosition(), 1.f), sizeof(_float4));

	m_pShaderCom->Set_Value("g_vMtrlAmbient", &m_Mtrls[0].Ambient, sizeof(_float4));
	m_pShaderCom->Set_Value("g_vMtrlSpecular", &m_Mtrls[0].Specular, sizeof(_float4));

	Safe_Release(pManagement);

	return S_OK;



}

CGate * CGate::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{	
	CGate*	pInstance = new CGate(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CGate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGate::Clone_GameObject(void * pArg)
{
	CGate*	pInstance = new CGate(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CGate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGate::Free()
{
	CPickingMgr::Get_Instance()->UnRegister_Observer(this);
	Safe_Release(m_pCollider);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pOriginalTransform);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	CUnit::Free();
}

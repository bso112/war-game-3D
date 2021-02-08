#include "stdafx.h"
#include "..\Headers\Sphere.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "PickingMgr.h"
#include "Management.h"
#include "PickingMgr.h"
#include "InputManager.h"
#include "SoundMgr.h"

CSphere::CSphere(PDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CSphere::CSphere(const CSphere & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSphere::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CSphere::Ready_GameObject(void * pArg)
{
	//if (nullptr != pArg)
	//	memcpy(&m_tDesc, pArg, sizeof(INFANTRYDESC));

	//CPickingMgr::Get_Instance()->Register_Observer(this);
	//CKeyMgr::Get_Instance()->RegisterObserver(this);

	if (!Load_Mesh())
		MSG_BOX("Fail to Load mesh");

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Position(((BASEDESC*)pArg)->vPos);
	m_pTransformCom->SetUp_Scale(((BASEDESC*)pArg)->vSize);
	//트랜스폼으로 크기를 지정하므로 지름이 1 되도록 만들자.
	//D3DXCreateSphere(m_pGraphic_Device, 0.5f, 10, 5, &m_pMesh, NULL);

	return S_OK;
}

_int CSphere::Update_GameObject(_double TimeDelta)
{



	return _int();
}

_int CSphere::Late_Update_GameObject(_double TimeDelta)
{

	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float3 vVel;
	_float vMaxSpeed = 10.f;
	_float vSpeed = 0.f;
	m_bScope = 0;

	CUnit* pPlayer = CInputManager::Get_Instance()->Get_PickedUnit();
	if (pPlayer != nullptr)
	{
		vVel = ((CRigid_Body*)(pPlayer->Find_Component(L"Com_RigidBody")))->Get_Velocity();
		vMaxSpeed = ((CRigid_Body*)(pPlayer->Find_Component(L"Com_RigidBody")))->GetStateDesc().fMaxSpeed;
		vSpeed = D3DXVec3Length(&vVel);
	}


	if (vMaxSpeed * 2.5f < vSpeed)
	{
		m_bActive = 1;
		m_fIntensity = pow((vSpeed - vMaxSpeed * 2.5) / 180.f,2.f);
		if (m_fIntensity < 0.f)
			m_fIntensity = 0.f;
	}
	//if (GetAsyncKeyState(VK_SHIFT))
	//	m_bActive = 1;
	else
		m_bActive = 0;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;
	//Fall_Damage(TimeDelta);

	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	_float3 vPos = pPickingMgr->Get_Blur_Center();
	_float3 vCam = pManagement->Get_CamPosition();
	m_pTransformCom->SetUp_Position(vCam);
	m_pTransformCom->SetUp_Scale(_float3(60.f, 60.f, 60.f));
	_float fFOV = pPickingMgr->Get_FOV();

	_bool bIsSniping = pPickingMgr->IsSniping();

	if (bIsSniping)
	{
		m_bScope = 1;
		m_bActive = 1;
		//0.1 - 1
		_float3 vDir = pManagement->Get_CamLook();
		D3DXVec3Normalize(&vDir, &vDir);
		//0.1 - 2.5
		//1 - 5
		_float fm = 2.5f;
		_float fM = 17.5f;
		m_fIntensity = .75f;
		_float fX = (fFOV - 0.1f)* (fM - fm) / 0.9f + fm;
		m_pTransformCom->SetUp_Scale(_float3(fX, fX, fX));
		m_pTransformCom->SetUp_Position(vCam + vDir * 20.f);
	}




	if (m_bDead)
		return -1;


	if (nullptr == m_pTransformCom)
		return -1;

	//CManagement* pManagement = CManagement::Get_Instance();
	//if (nullptr == pManagement)
	//	return -1;

	//if (!m_bActive)
	//	return 0;

	Safe_AddRef(pManagement);
	//if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position(), 2.f))
	//{
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_3DUI, this)))
		return -1;

	//	if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_UNIT, this)))
	//		return -1;
	//}
	Safe_Release(pManagement);
}

HRESULT CSphere::Render_GameObject()
{
	if (m_bActive == 1)
	{
		if (nullptr == m_pShaderCom)
			return E_FAIL;

		if (!m_bActive)
			return 0;


		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;




		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(m_bScope);

		for (_uint i = 0; i < m_Mtrls.size(); i++)
		{
			m_pMesh->DrawSubset(i);
		}


		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}

}

void CSphere::OnDead()
{
}

HRESULT CSphere::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pShaderCom->Set_Value("g_Center", &_float4(m_pTransformCom->Get_WorldPosition(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pShaderCom->Set_Value("g_fInput", &m_fIntensity, sizeof(_float));

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CSphere::SetUp_Component()
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

	//m_pTransformCom->SetUp_Position(m_tDesc.tBasedesc.vPos);
	//m_pTransformCom->SetUp_Scale(_float3(g_fUnitCX, g_fUnitCY, g_fUnitCZ));

	//// For.Com_VIBuffer
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
	//	return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Target_Img", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

_bool CSphere::Load_Mesh()
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


CSphere * CSphere::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CSphere*	pInstance = new CSphere(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}




CGameObject * CSphere::Clone_GameObject(void * pArg)
{
	CSphere*	pInstance = new CSphere(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSphere::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	CGameObject::Free();
}

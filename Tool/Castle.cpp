#include "stdafx.h"
#include "Castle.h"

CCastle::CCastle(PDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CCastle::CCastle(const CCastle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCastle::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCastle::Ready_GameObject(void * pArg)
{
	if(nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (!Load_Mesh())
		MSG_BOX("Fail to Load mesh");

	if (FAILED(Add_Component()))
		MSG_BOX("Fail to Add Component of Castle");

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Rotation(m_StateDesc.vAxis, m_StateDesc.fRadian);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPos);

	return S_OK;
}

_int CCastle::Update_GameObject(_double TimeDelta)
{
	return _int(0);
}

_int CCastle::Late_Update_GameObject(_double TimeDelta)
{
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return -1;

	return _int(0);
}

HRESULT CCastle::Render_GameObject()
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

_bool CCastle::Load_Mesh()
{
	HRESULT hr = 0;

	//
	// Load the XFile data.
	//

	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		L"../Client/Bin/Meshs/castle.x",
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

HRESULT CCastle::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Castle", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Castle", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;

}

HRESULT CCastle::SetUp_ConstantTable()
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

	Safe_Release(pManagement);

	return S_OK;
}

CCastle * CCastle::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCastle*	pInstance = new CCastle(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CCastle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCastle::Clone_GameObject(void * pArg)
{
	CCastle*	pInstance = new CCastle(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CCastle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCastle::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	CGameObject::Free();
}

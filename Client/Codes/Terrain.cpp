#include "stdafx.h"
#include "..\Headers\Terrain.h"
#include "GameManager.h"
#include "PickingMgr.h"

CTerrain::CTerrain(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CTerrain::Update_GameObject(_double TimeDelta)
{
	//if (GetAsyncKeyState(VK_F12))
	//	m_bWireframe = 1;
	//if (GetAsyncKeyState(VK_F11))
	//	m_bWireframe = 0;

	return _int();
}

_int CTerrain::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	Chunk_Culling();

	return m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTerrain::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_int iSegX = m_pVIBufferCom->Get_SegX();
	_int iSegZ = m_pVIBufferCom->Get_SegZ();

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(CManagement::Get_Instance()->IS_DebugMode());//here!!

	for (int i = 0; i < iSegX*iSegZ; ++i)
	{
		if (m_pVIBufferCom->Get_Chunk_Bool(i))
			m_pVIBufferCom->Render_VIBuffer(i);
	}


	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

_bool CTerrain::Get_TerrainPos(_float3 _vWolrdPos, _float3 & vTerrainPos)
{
	if (nullptr == m_pVIBufferCom)
		return false;
	return m_pVIBufferCom->Get_TerrainPos(_vWolrdPos, vTerrainPos);
}

_bool CTerrain::Get_TerrainPos(POINT & ViewPortPt, _float3* vTerrainPos)
{
	if (nullptr == m_pTransformCom)
		return false;

	return m_pVIBufferCom->Picking(ViewPortPt, m_pTransformCom->Get_WorldMatrix(), vTerrainPos);
}

_bool CTerrain::Get_TerrainNormal(_float3 _vWolrdPos, _float3& vTerrainNor)
{
	if (nullptr == m_pVIBufferCom)
		return false;
	return m_pVIBufferCom->Get_Normal(_vWolrdPos, vTerrainNor);
}

HRESULT CTerrain::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_VIBuffer_Terrain", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Shader
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;	
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Shader_Terrain", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Terrain", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Texture_Circle
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_MagicCircle", L"Com_Texture_Circle", (CComponent**)&m_pTextureMagicCom)))
		return E_FAIL;
	 
	// For.Component_Texture_HealCircle
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_HealCircle", L"Com_Texture_Healng", (CComponent**)&m_pTextureCircleCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_vCamPosition", &_float4(pManagement->Get_CamPosition(),1.f),sizeof(_float4));

	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShaderCom->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));

	m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", 0);
	m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture_1", 1);


	switch (m_eSkill)
	{
	case SKILL_METEOR:
	{
		POINT cursor;
		_float3 vPosition;
		_float fRange = CGameManager::Get_Instance()->Get_SkillRange();

		m_pTextureMagicCom->Set_TextureOnShader(m_pShaderCom, "g_BrushTexture");

		GetCursorPos(&cursor);
		ScreenToClient(g_hWnd, &cursor);
		CPickingMgr::Get_Instance()->Get_TerrainPos(cursor, &vPosition);

		m_bBrushOn = true;
		m_pShaderCom->Set_Bool("g_bBrushOn", m_bBrushOn);
		m_pShaderCom->Set_Value("g_vBrushPos", &vPosition, sizeof(_float3));
		m_pShaderCom->Set_Value("g_fBrushRange", &fRange, sizeof(_float));
		break;
	}
	case SKILL_HEAL:
	{
		POINT cursor;
		_float3 vPosition;
		_float fRange = CGameManager::Get_Instance()->Get_SkillRange();

		m_pTextureCircleCom->Set_TextureOnShader(m_pShaderCom, "g_BrushTexture");

		GetCursorPos(&cursor);
		ScreenToClient(g_hWnd, &cursor);
		CPickingMgr::Get_Instance()->Get_TerrainPos(cursor, &vPosition);

		m_bBrushOn = true;
		m_pShaderCom->Set_Bool("g_bBrushOn", m_bBrushOn);
		m_pShaderCom->Set_Value("g_vBrushPos", &vPosition, sizeof(_float3));
		m_pShaderCom->Set_Value("g_fBrushRange", &fRange, sizeof(_float));
		break; 
	}
	default:
		m_bBrushOn = false;
		m_pShaderCom->Set_Bool("g_bBrushOn", m_bBrushOn);
		break;
	}


	D3DLIGHT9	LightDesc = *pManagement->Get_LightDesc(0);

	m_pShaderCom->Set_Value("g_vLightDir", &_float4(LightDesc.Direction, 0.f), sizeof(_float4));
	m_pShaderCom->Set_Value("g_vLightDiffuse", &LightDesc.Diffuse, sizeof(_float4));
	m_pShaderCom->Set_Value("g_vLightAmbient", &LightDesc.Ambient, sizeof(_float4));
	m_pShaderCom->Set_Value("g_vLightSpecular", &LightDesc.Specular, sizeof(_float4));

	////////////////////////////////////////////// Picking
	//////////////////////////////////////////////



	//m_pShaderCom->Set_Value("g_vCamPosition", &_float4(pManagement->Get_CamPosition(), 1.f), sizeof(_float4));


	Safe_Release(pManagement);

	return S_OK;
}

void CTerrain::Chunk_Culling()
{
	_float3 vCamPos = CManagement::Get_Instance()->Get_CamPosition();
	CVIBuffer_Terrain::CHUNK_TAG	tagIdx = m_pVIBufferCom->Get_Idx(vCamPos);

	_int iSegX = m_pVIBufferCom->Get_SegX();
	_int iSegZ = m_pVIBufferCom->Get_SegZ();
	_float fRadius = m_pVIBufferCom->Get_Chunk_Radius();

	for (int i = 0; i < iSegX * iSegZ; ++i)
	{
		_bool bRender = CManagement::Get_Instance()->Is_InFrustum(m_pVIBufferCom->Get_Chunk_Pos(i), fRadius);
		m_pVIBufferCom->Set_Chunk_Bool(i, bRender);
	}
}

CTerrain * CTerrain::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain*	pInstance = new CTerrain(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone_GameObject(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCircleCom);
	Safe_Release(m_pTextureMagicCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}

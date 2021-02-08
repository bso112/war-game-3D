#include "stdafx.h"
#include "MainApp.h"

#include "PickingMgr.h"
#include "Camera_Free.h"
#include "Terrain.h"
#include "Infantry.h"
#include "Cube.h"
#include "Castle.h"
#include "BasePlane.h"

IMPLEMENT_SINGLETON(CMainApp)

CMainApp::CMainApp()
	: m_pManagement(CManagement::Get_Instance())
{
	Safe_AddRef(m_pManagement);
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Default_Component()))
		return E_FAIL;

	if (FAILED(Ready_Default_GameObject()))
		return E_FAIL;

	// For.Layer_Camera
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	// For.Layer_Terrain
	if (FAILED(Ready_Layer_Terrain(L"Layer_Terrain")))
		return E_FAIL;

	// For.Layer_Castle
	if (FAILED(Ready_Layer_Castle(L"Layer_Castle")))
		return E_FAIL;

	if (FAILED(CPickingMgr::Get_Instance()->Ready_PickingMgr()))
		return E_FAIL;

	return S_OK;
}

_int CMainApp::Update_MainApp(_double TimeDelta)
{
	if (nullptr == m_pManagement)
		return -1;

	if (0x80000000 & m_pManagement->Update_Object_Manager(TimeDelta))
		return -1;

	if (FAILED(m_pManagement->Update_PipeLine()))
		return -1;

	if (FAILED(m_pManagement->Late_Update_Object_Manager(TimeDelta)))
		return -1;

	if (FAILED(m_pManagement->CheckCollision()))
		return -1;

	return _int(0);
}

HRESULT CMainApp::Render_MainApp()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pManagement ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);

	m_pGraphic_Device->BeginScene();

	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;

	m_pGraphic_Device->EndScene();

	m_pGraphic_Device->Present(nullptr, nullptr, 0, nullptr);

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	// 엔진 초기화.
	if (FAILED(m_pManagement->Ready_Engine(SCENE_END)))
		return E_FAIL;

	// 장치 초기화.
	if (FAILED(m_pManagement->Ready_Graphic_Device(g_hWnd, CGraphic_Device::TYPE_WIN, g_iToolViewCX, g_iToolViewCY, &m_pGraphic_Device)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Default_GameObject()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	// For.GameObject_Camera_Free
	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Camera_Free", CCamera_Free::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Terrain
	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Infantry
	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Infantry", CInfantry::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Cube
	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Cube", CCube::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Castle
	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Castle", CCastle::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_BasePlane
	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_BasePlane", CBasePlane::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Component()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	// For.Component_Shader_Terrain
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Terrain", CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;

	// For.Component_Shader_Default
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Default", CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Default.fx"))))
		return E_FAIL;

	// For.Component_Shader_Cube
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Cube", CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Cube.fx"))))
		return E_FAIL;

	// For.Component_Shader_Rect
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Rect", CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Rect.fx"))))
		return E_FAIL;

	// For.Component_Shader_Castle
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Castle", CShader::Create(m_pGraphic_Device, L"ShaderFiles/Shader_Castle.fx"))))
		return E_FAIL;

	// For.Component_VIBuffer_Terrain
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pGraphic_Device, L"../Client/Bin/Textures/Terrain/Height.bmp"))))
		return E_FAIL;

	// For.Component_VIBuffer_Cube
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_Cube", CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.Component_VIBuffer_Rect
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_Rect", CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.Component_Transform
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Transform", CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.Component_Texture_Terrain
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Terrain", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Client/Bin/Textures/Terrain/Grass_%d.tga", 2))))
		return E_FAIL;

	// For.Component_Texture_Unit
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Unit", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Client/Bin/Textures/Unit/brick%d.jpg", 5))))
		return E_FAIL;

	// For.Component_Texture_Castle
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Castle", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Client/Bin/Textures/Castle/Castle%d.tga", 1))))
		return E_FAIL;

	// For.Component_Renderer
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.Component_Collider_Ellipse
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_Ellipse", CCollider_Ellipse::Create(m_pGraphic_Device, L"../Client/Bin/Textures/Collider/Ellipse.png"))))
		return E_FAIL;

	// For.Component_Collider_Box
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_Box", CCollider_Box::Create(m_pGraphic_Device))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	CCamera::STATEDESC			StateDesc;
	ZeroMemory(&StateDesc, sizeof(CCamera::STATEDESC));
	StateDesc.vEye = _float3(256.f, 300.f, -50.f);
	StateDesc.vAt = _float3(256.f, 0.f, 256.f);
	StateDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	StateDesc.fFovy = D3DXToRadian(60.0f);
	StateDesc.fAspect = _float(g_iToolViewCX) / g_iToolViewCY;
	StateDesc.fNear = 0.2f;
	StateDesc.fFar = 2000.f;

	StateDesc.TransformDesc.SpeedPerSec = 5.f;
	StateDesc.TransformDesc.RotatePerSec = D3DXToRadian(90.0f);

	if (nullptr == m_pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Camera_Free", SCENE_STATIC, pLayerTag, &StateDesc))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Terrain", SCENE_STATIC, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Layer_Castle(const _tchar * pLayerTag)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	CCastle::STATEDESC StateDesc;
	StateDesc.vScale = _float3(8.f, 8.f, 8.f);
	StateDesc.vAxis = _float3(0.f, 1.f, 0.f);
	StateDesc.vPos = _float3(1800.f, 109.f, 1783.f);
	StateDesc.fRadian = D3DXToRadian(45.f);
	if (FAILED(m_pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Castle", SCENE_STATIC, pLayerTag, &StateDesc)))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("Failed To Create CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pManagement);

	CManagement::Release_Engine();

	if (0 != CPickingMgr::Destroy_Instance())
		MSG_BOX("Failed to Destroy PickingMgr");
}
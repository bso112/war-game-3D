#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "BackGround.h"
#include "Scene_Loading.h"
#include "Scene_Manager.h"
#include "Infantry.h"
#include "PickingMgr.h"
#include "InputManager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "ResourceManager.h"
#include "Cube.h"
#include "GameManager.h"
#include "Image3D.h"
#include "GameManager.h"
#include "MyImage.h"
#include "Tower.h"
#include "Blur.h"
#include "Radial.h"
#include "SoundMgr.h"
//#include "vld.h"
#include "Sphere.h"

USING(Client)


CMainApp::CMainApp()
	: m_pManagement(CManagement::Get_Instance()),
	m_pKeyMgr(CKeyMgr::Get_Instance())
{
	Safe_AddRef(m_pManagement);
	Safe_AddRef(m_pKeyMgr);
}

HRESULT CMainApp::Ready_MainApp()
{
	_tchar szText[MAX_PATH] = L"Massacre";
	SetWindowText(g_hWnd, szText);

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Default_Component()))
		return E_FAIL;

	if (FAILED(Ready_Default_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Start_Scene(SCENE_STAGE)))
		return E_FAIL;

	if (FAILED(Ready_RenderTarget()))
		return E_FAIL;

	CSoundMgr::Get_Instance()->Initialize();




	return S_OK;
}

_int CMainApp::Update_MainApp(_double TimeDelta)
{
	if (nullptr == m_pManagement)
		return -1;

#ifdef _DEBUG
	m_TimeAcc += TimeDelta;
#endif // _DEBUG

	//키체크
	m_pKeyMgr->Key_Down('Q');
	m_pKeyMgr->Key_Down('Z');

	m_pKeyMgr->Key_Down(VK_LBUTTON);
	m_pKeyMgr->Key_Up(VK_LBUTTON);
	m_pKeyMgr->Key_Pressing(VK_LBUTTON);
	m_pKeyMgr->Key_Down(VK_RBUTTON);
	m_pKeyMgr->Key_Down(VK_RETURN);
	m_pKeyMgr->Key_Down(VK_SPACE);
	m_pKeyMgr->Key_Down(VK_TAB);

	m_pKeyMgr->Key_Down('1');
	m_pKeyMgr->Key_Down('2');
	m_pKeyMgr->Key_Down('3');
	m_pKeyMgr->Key_Down('E');
	m_pKeyMgr->Key_Down('R');
	m_pKeyMgr->Key_Down('T');

	m_pKeyMgr->Key_Down('O');
	m_pKeyMgr->Key_Down('P');


	m_pKeyMgr->Key_Down('9');



	m_pKeyMgr->Key_Update();

	CInputManager::Get_Instance()->Clear_DeadObject();
	CGameManager::Get_Instance()->Update_GameManager(TimeDelta);

	return m_pManagement->Update_Engine(TimeDelta);
}

HRESULT CMainApp::Render_MainApp()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pManagement ||
		nullptr == m_pRenderer ||
		nullptr == m_pBlur)
		return E_FAIL;

	m_pManagement->SetUp_OnGraphicDev(L"Target_Blur", 1);
	m_pManagement->SetUp_OnGraphicDev(L"Target_Blend", 2);
	m_pManagement->SetUp_OnGraphicDev(L"Target_Radial", 3);

	SCENEID eCurScene = SCENEID(CManagement::Get_Instance()->Get_CurrScene());

	//if(m_Cur)
	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(25.f / 256.f, 24.f / 256.f, 25.f / 256.f, 0.f), 1.f, 0);

	//else
	//m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(25.f / 256.f, 24.f / 256.f, 25.f / 256.f, 1.f), 1.f, 0);


	m_pGraphic_Device->BeginScene();

	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;

	if (FAILED(m_pManagement->Render_Engine()))
		return E_FAIL;

	if (m_pManagement->Get_CurrScene() == _uint(SCENE_STAGE))
	{
	if (FAILED(m_pBlur->Render_GameObject()))
		return E_FAIL;


	//if (GetAsyncKeyState(VK_CONTROL))
	//{
		if (FAILED(m_pRadial->Render_GameObject()))
			return E_FAIL;
	}

	if (CManagement::Get_Instance()->IS_DebugMode())
	{
		if (FAILED(m_pRenderer->Render_Debug()))
			return E_FAIL;
	}

	if (FAILED(m_pRenderer->Render_UI_Lately()))
		return E_FAIL;

	//}
	m_pManagement->Release_OnGraphicDev(L"Target_Blur", 1);
	m_pManagement->Release_OnGraphicDev(L"Target_Blend", 2);
	m_pManagement->Release_OnGraphicDev(L"Target_Radial", 3);


#ifdef _DEBUG
	++m_dwDrawNum;
	if (1.0 <= m_TimeAcc)
	{
		wsprintf(m_szFPS, L"FPS %d", m_dwDrawNum);
		m_TimeAcc = 0.0;
		m_dwDrawNum = 0;
	}

	_matrix TranslationMatrix;
	m_pSprite->SetTransform(D3DXMatrixTranslation(&TranslationMatrix, 10.f, 10.f, 0.f));
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_pFont->DrawTextW(m_pSprite, m_szFPS, lstrlen(m_szFPS), nullptr, DT_LEFT, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pSprite->End();
#endif // _DEBUG

	map<const _tchar*, CMyFont*>* FontMap = CUI_Manager::Get_Instance()->Get_FontMap();
	for (auto& pair : *FontMap)
	{
		if (pair.second->Get_Active())
			pair.second->Render_GameObject();
	}


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

#ifdef _DEBUG
	CGraphic_Device::WINMODE eMode = CGraphic_Device::TYPE_WIN;
#else
	CGraphic_Device::WINMODE eMode = CGraphic_Device::TYPE_FULL;		// 창화면
	//CGraphic_Device::WINMODE eMode = CGraphic_Device::TYPE_FULL;	// 풀화면
#endif // _DEBUG

	// 장치 초기화.
	if (FAILED(m_pManagement->Ready_Graphic_Device(g_hWnd, eMode, g_iWinSizeX, g_iWinSizeY, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(m_pManagement->Ready_Input_Device(g_hInst, g_hWnd)))
		return E_FAIL;

	if (FAILED(CGameManager::Get_Instance()->Ready_GameManager()))
		return E_FAIL;

	RECT Clip;
	GetClientRect(g_hWnd, &Clip);
	ClientToScreen(g_hWnd, (LPPOINT)&Clip);
	ClientToScreen(g_hWnd, (LPPOINT)(&Clip.right));
	ClipCursor(&Clip);

#ifdef _DEBUG
	if (FAILED(D3DXCreateSprite(m_pGraphic_Device, &m_pSprite)))
		return E_FAIL;

	// Font 초기화.
	D3DXFONT_DESC FontDesc;
	ZeroMemory(&FontDesc, sizeof(D3DXFONT_DESC));
	FontDesc.Width = 16;
	FontDesc.Height = 16;
	FontDesc.Weight = FW_BOLD;
	FontDesc.CharSet = HANGUL_CHARSET;
	lstrcpy(FontDesc.FaceName, L"궁서체");

	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &FontDesc, &m_pFont)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CMainApp::Ready_Start_Scene(SCENEID eNextSceneID)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_CurrentScene(CScene_Loading::Create(m_pGraphic_Device, eNextSceneID), eNextSceneID)))
		return E_FAIL;

	CInputManager::Get_Instance();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_GameObject()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_BackGround", CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Camera_Free", CCamera_Free::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Camera_Skill", CCamera_Skill::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Camera_Third_Person", CCamera_Third_Person::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Camera_WayPoint", CCamera_WayPoint::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Camera_Lerp", CCamera_Command::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Infantry", CInfantry::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Tower", CTower::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Cube", CCube::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Sphere", CSphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Image3D", CImage3D::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Image", CMyImage::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Blur", CBlur::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Radial", CRadial::Create(m_pGraphic_Device))))
		return E_FAIL;



	return S_OK;
}

HRESULT CMainApp::Ready_Default_Component()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Transform", CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_Aim", CCVIBuffer_Aim::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_Rect", CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_Cube", CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_ViewPort", CVIBuffer_ViewPort::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Default", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Default.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Rect", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Rect.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Cube", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Cube.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_ViewPort", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_ViewPort.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Unit", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Unit.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_Box", CCollider_Box::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_Ellipse", CCollider_Ellipse::Create(m_pGraphic_Device, L"../Bin/Textures/Collider/Ellipse.png"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_RigidBody", CRigid_Body::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Unit", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Unit/Unit%d.png", 2))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Waypoint", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Waypoint/waypoint.png"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Blood", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Particle/blood.png"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Arrow", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Arrow/Arrow%d.png", 4))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Bow", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Weapon/Bow%d.png", 1))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Dust", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Particle/dust.png"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Explosion", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Effect/Bomb/%d.png", 41))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_DownArrow", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/down_arrow.png"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Halt", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/halt.png"))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Attack", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/sword.png"))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Flag", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/flag.png"))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Particle", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Particle.fx"))))
		return E_FAIL;


	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Blur", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Target_Blur.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Radial", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Target_Radial.fx"))))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Target_Img", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Target_Img.fx"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_RenderTarget()
{
	if (FAILED(m_pManagement->Add_RenderTarget(m_pGraphic_Device, L"Target_Blur", g_iWinSizeX, g_iWinSizeY, D3DFMT_A8R8G8B8)))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_RenderTarget(m_pGraphic_Device, L"Target_Blend", g_iWinSizeX, g_iWinSizeY, D3DFMT_A8R8G8B8)))
		return E_FAIL;

	if (FAILED(m_pManagement->Add_RenderTarget(m_pGraphic_Device, L"Target_Radial", g_iWinSizeX, g_iWinSizeY, D3DFMT_A8R8G8B8)))
		return E_FAIL;

	CBlur::STATEDESC tBlurDesc;
	tBlurDesc.pBlendTexture = m_pManagement->Get_Texture(L"Target_Blend");
	tBlurDesc.tBaseDesc = BASEDESC(_float3(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 1.f), _float3((_float)g_iWinSizeX, (_float)g_iWinSizeY, 1.f));
	if (nullptr == (m_pBlur = (CBlur*)m_pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Blur", SCENE_STAGE, L"Layer_RenderTarget", &tBlurDesc)))
		return E_FAIL;

	CRadial::STATEDESC tRadialDesc;
	tRadialDesc.pBlurTexture = m_pManagement->Get_Texture(L"Target_Blur");
	tRadialDesc.tBaseDesc = BASEDESC(_float3(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 1.f), _float3((_float)g_iWinSizeX, (_float)g_iWinSizeY, 1.f));
	if (nullptr == (m_pRadial = (CRadial*)m_pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Radial", SCENE_STAGE, L"Layer_RenderTarget_Radial", &tBlurDesc)))
		return E_FAIL;

	Safe_AddRef(m_pBlur);
	Safe_AddRef(m_pRadial);
	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("Failed To Create CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);
#endif // _DEBUG

	Safe_Release(m_pRenderer);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pManagement);
	Safe_Release(m_pKeyMgr);
	Safe_Release(m_pRadial);

	Safe_Release(m_pBlur);

#ifdef _DEBUG
	if (0 != CResourceManager::Destroy_Instance())
		MSG_BOX("Fail to Destroy CResourceManager");

	if (0 != CGameManager::Destroy_Instance())
		MSG_BOX("Fail to Destroy CGameManager");

	if (0 != CUI_Manager::Destroy_Instance())
		MSG_BOX("Fail to Destroy CUI_Manager");

	// 게임오브젝트 전에 디스트로이
	if (0 != CCamera_Manager::Destroy_Instance())
		MSG_BOX("Fail to Destroy CCamera_Manager");

	//키매니저 디스트로이 전에 파괴되어야함.
	if (0 != CInputManager::Destroy_Instance())
		MSG_BOX("Fail to Destroy CInputManager");

	if(0 != CSoundMgr::Destroy_Instance())
		MSG_BOX("Fail to Destroy CSoundMgr");

	CPickingMgr::Get_Instance()->Clear_PickingMgr();

	CManagement::Release_Engine();

	//게임오브젝트매니저가 디스트로이 된다음에 디스트로이 해야함.
	if (0 != CPickingMgr::Destroy_Instance())
		MSG_BOX("Fail to Destroy PickingMgr");

#else // _DEBUG
	CResourceManager::Destroy_Instance();

	CGameManager::Destroy_Instance();

	CUI_Manager::Destroy_Instance();

	CCamera_Manager::Destroy_Instance();

	CInputManager::Destroy_Instance();

	CSoundMgr::Destroy_Instance();

	CPickingMgr::Get_Instance()->Clear_PickingMgr();

	CManagement::Release_Engine();

	CPickingMgr::Destroy_Instance();
#endif
}
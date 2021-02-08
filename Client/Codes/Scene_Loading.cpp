#include "stdafx.h"
#include "..\Headers\Scene_Loading.h"
#include "Management.h"

#include "Scene_Menu.h"
#include "Scene_Stage.h"

#include "BackGround.h"
#include "FrameImage.h"

CScene_Loading::CScene_Loading(PDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Loading::Ready_Scene(SCENEID eNextSceneID)
{
	m_eNextSceneID = eNextSceneID;

	if (FAILED(Ready_Prototype_Object()))
		return E_FAIL;

	if (FAILED(Ready_Layer_LoadingScreen(L"Layer_Loading")))
		return E_FAIL;

	// For.Layer_BackGround
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphic_Device, eNextSceneID);
	if (nullptr == m_pLoading)
		return E_FAIL;

#pragma region FONT
	if (FAILED(D3DXCreateSprite(m_pGraphic_Device, &m_pSprite)))
		return E_FAIL;

	// Font 초기화.
	D3DXFONT_DESC FontDesc;
	ZeroMemory(&FontDesc, sizeof(D3DXFONT_DESC));
	FontDesc.Width = 20;
	FontDesc.Height = 30;
	FontDesc.Weight = FW_BOLD;
	FontDesc.CharSet = HANGUL_CHARSET;
	lstrcpy(FontDesc.FaceName, L"궁서체");

	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &FontDesc, &m_pFont)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

_int CScene_Loading::Update_Scene(_double TimeDelta)
{
	return _int();
}

HRESULT CScene_Loading::Render_Scene()
{
#pragma region FONT
	m_pGraphic_Device->BeginScene();

	_matrix MatrixPos;
	m_pSprite->SetTransform(D3DXMatrixTranslation(&MatrixPos, (_float)g_iWinSizeX - 220.f, (_float)g_iWinSizeY - 140.f, 0.f));
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_pFont->DrawTextW(m_pSprite, m_pLoading->Get_LoadingText(), lstrlen(m_pLoading->Get_LoadingText()), nullptr, DT_RIGHT, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	m_pSprite->End();

	m_pGraphic_Device->EndScene();
	m_pGraphic_Device->Present(nullptr, nullptr, 0, nullptr);
#pragma endregion

	if (true == m_pLoading->Get_Finished())
	{
		CManagement*	pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;

		Safe_AddRef(pManagement);

		CScene*		pScene = nullptr;

		switch (m_eNextSceneID)
		{
		case SCENE_MENU:
			pScene = CScene_Menu::Create(m_pGraphic_Device);
			break;
		case SCENE_STAGE:
			pScene = CScene_Stage::Create(m_pGraphic_Device);
			break;
		}

		if (nullptr == pScene)
			return E_FAIL;

		if (FAILED(pManagement->SetUp_CurrentScene(pScene, m_eNextSceneID)))
			return E_FAIL;

		if (FAILED(pManagement->Clear_Engine(SCENE_LOADING)))
			return E_FAIL;

		Safe_Release(pManagement);

		return S_OK;
	}

	return S_OK;
}

HRESULT CScene_Loading::Change_Scene()
{	
	return S_OK;
}

HRESULT CScene_Loading::Ready_Prototype_Object()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

#pragma region GAMEOBJECT
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_LOADING, L"Loading_Screen", CFrameImage::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region COMPONENT
	// For.Component_Texture_Loading_Back
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_LOADING, L"Component_Texture_Loading_Back", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Scene/Loading_Back%d.png", 3))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Component_Prototype(SCENE_LOADING, L"Component_Texture_Loading", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Loading/%d.bmp", 41))))
		return E_FAIL;
#pragma endregion


	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Loading::Ready_Layer_BackGround(const _tchar * _LayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CBackGround::STATEDESC StateDesc;
	StateDesc.eSceneID = SCENE_LOADING;
	StateDesc.pTextureTag = L"Component_Texture_Loading_Back";
	StateDesc.vScale = _float3((_float)g_iWinSizeX * 0.7f, (_float)g_iWinSizeY * 0.7f, 1.f);
	StateDesc.vPos = _float3(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.4f, 0.f);
	StateDesc.isMenuColor = false;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BackGround", SCENE_LOADING, _LayerTag, &StateDesc)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Loading::Ready_Layer_LoadingScreen(const _tchar * _LayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	CFrameImage::STATEDESC Desc;
	ZeroMemory(&Desc, sizeof(CFrameImage::STATEDESC));
	Desc.eTextureScene = SCENE_LOADING;
	Desc.pTextureKey = L"Component_Texture_Loading";
	Desc.iTextureNum = 41;
	Desc.fFrameTime = 0.02f;
	Desc.iShaderPass = 9;
	Desc.vPosition = _float3(0.f, -0.2f, 0.f);
	Desc.vScale = _float3(_float(g_iWinSizeX), _float(g_iWinSizeY), 1);
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_LOADING, L"Loading_Screen", SCENE_LOADING, _LayerTag, &Desc)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

CScene_Loading * CScene_Loading::Create(PDIRECT3DDEVICE9 pGraphic_Device, SCENEID eNextSceneID)
{
	CScene_Loading*	pInstance = new CScene_Loading(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene(eNextSceneID)))
	{
		MSG_BOX("Failed To Create CScene_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScene_Loading::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);
	Safe_Release(m_pLoading);

	CScene::Free();
}

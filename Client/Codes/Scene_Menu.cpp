#include "stdafx.h"
#include "..\Headers\Scene_Menu.h"

#include "Scene_Loading.h"
#include "Management.h"
#include "BackGround.h"

CScene_Menu::CScene_Menu(PDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Menu::Ready_Scene()
{
	// For.BackGround_Layer
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	return S_OK;
}

_int CScene_Menu::Update_Scene(_double TimeDelta)
{
	if (0x8000 & GetKeyState(VK_LBUTTON))
		if (0x80000000 & Change_Scene(SCENE_STAGE))
			return -1;

	return _int(0);
}

HRESULT CScene_Menu::Render_Scene()
{
	return S_OK;
}

HRESULT CScene_Menu::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CBackGround::STATEDESC StateDesc;
	StateDesc.eSceneID = SCENE_MENU;

	// BackColor
	for (size_t i = 0; i < 4; i++)
	{
		StateDesc.pTextureTag = L"Component_Texture_MenuColor";
		StateDesc.vScale = _float3(200.f, 40.f, 1.f);
		StateDesc.vPos = _float3(100.f, 420.f + (46.f * i), 0.f);
		StateDesc.isMenuColor = true;
		if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BackGround", SCENE_MENU, pLayerTag, &StateDesc)))
			return E_FAIL;
	}

	// Menu
	StateDesc.pTextureTag = L"Component_Texture_Menu";
	StateDesc.vScale = _float3((_float)g_iWinSizeX, (_float)g_iWinSizeY, 1.f);
	StateDesc.vPos = _float3(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f);
	StateDesc.isMenuColor = false;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BackGround", SCENE_MENU, pLayerTag, &StateDesc)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

_int CScene_Menu::Change_Scene(SCENEID eNextScene)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (FAILED(pManagement->SetUp_CurrentScene(CScene_Loading::Create(m_pGraphic_Device, eNextScene), eNextScene)))
	{
		Safe_Release(pManagement);
		return -1;
	}

	Safe_Release(pManagement);

	return _int(0);
}

CScene_Menu * CScene_Menu::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Menu*	pInstance = new CScene_Menu(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Create CScene_Menu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Menu::Free()
{
	CManagement::Get_Instance()->Clear_Engine(SCENE_MENU);

	CScene::Free();
}

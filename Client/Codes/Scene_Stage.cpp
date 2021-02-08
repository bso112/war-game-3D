#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Management.h"
#include "UI_Manager.h"
#include "InputManager.h"
#include "PickingMgr.h"
#include "Camera_Manager.h"
#include "ResourceManager.h"

#include "Terrain.h"
#include "Castle.h"
#include "Waypoint.h"
#include "Nature.h"
#include "Cube.h"
#include "Infantry.h"
#include "Image3D.h"
#include "GameManager.h"
#include "MyImage.h"
#include "Tower.h"

#include "SoundMgr.h"

CScene_Stage::CScene_Stage(PDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Stage::Ready_Scene()
{
	CManagement*	pManagement = CManagement::Get_Instance();

	pManagement->Set_Gravity(9.8f * 1.5f);

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(L"Layer_Terrain")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Building(L"Layer_Building")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Skill(L"Layer_UI_Skill")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Battle(L"Layer_UI_Battle")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Command(L"Layer_UI_Command")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Free(L"Layer_UI_Free")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	// For.Layer_Unit_Enemy
	if (FAILED(Ready_Layer_Unit_Enemy_Tool(L"Layer_Unit_Enemy")))
		return E_FAIL;


	if (FAILED(Ready_Layer_Boss(L"Layer_Boss")))
		return E_FAIL;

	// For.Layer_ColliderCube
	if (FAILED(Ready_Layer_ColliderCube_Tool(L"Layer_ColliderCube")))
		return E_FAIL;

	// For.Layer_Nature
	if (FAILED(Ready_Layer_Nature_Tool(L"Layer_Nature")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sphere(L"Layer_Sphere")))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Unit_Ally(L"Layer_Unit_Ally")))
	//	return E_FAIL;

	CSoundMgr::Get_Instance()->PlayBGM(L"bgm2.mp3");
	CGameManager::Get_Instance()->Set_Gold(0);
	return S_OK;
}

_int CScene_Stage::Update_Scene(_double TimeDelta)
{

	//if (GetAsyncKeyState('K') & 0x80000000)
	//{
	//	if (FAILED(Ready_Layer_Unit_Enemy_Tool(L"Layer_Unit_Enemy")))
	//		return E_FAIL;
	//}
	if (GetAsyncKeyState(VK_F12))
		CManagement::Get_Instance()->Set_DebugMode(1);
	if (GetAsyncKeyState(VK_F11))
		CManagement::Get_Instance()->Set_DebugMode(0);

	return _int();
}


HRESULT CScene_Stage::Ready_Light()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	// Directional
	D3DLIGHT9			LightDesc;
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Direction = _float3(-1.f, -1.f, -1.f);

	if (FAILED(pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CCamera_Manager* pCameraManager = CCamera_Manager::Get_Instance();
	if (nullptr == pCameraManager)
		return E_FAIL;

	Safe_AddRef(pManagement);

	///
	/// Camera Free
	///
	CCamera::STATEDESC			StateDesc;
	ZeroMemory(&StateDesc, sizeof(CCamera::STATEDESC));
	StateDesc.vEye = _float3(0.f, 10.0f, -12.0f);
	StateDesc.vAt = _float3(0.f, 0.f, 0.f);
	StateDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	StateDesc.fFovy = D3DXToRadian(60.0f);
	StateDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	StateDesc.fNear = 0.2f;
	StateDesc.fFar = 1200.f;

	StateDesc.TransformDesc.SpeedPerSec = 5.f;
	StateDesc.TransformDesc.RotatePerSec = D3DXToRadian(90.0f);

	CCamera* pCamera = (CCamera*)(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Camera_Free", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pCamera)
		return E_FAIL;
	pCamera->Set_Active(false);
	pCameraManager->Add_Camera(CCamera::CAM_FREE, pCamera);
	pCameraManager->Set_CurrentCamera(pCamera);

	///
	/// Camera SkillView
	///
	ZeroMemory(&StateDesc, sizeof(CCamera::STATEDESC));
	StateDesc.vEye = _float3(0.f, 0.f, -10.f);
	StateDesc.vAt = _float3(0.f, 0.f, 0.f);
	StateDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	StateDesc.fFovy = D3DXToRadian(60.0f);
	StateDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	StateDesc.fNear = 0.2f;
	StateDesc.fFar = 1200.f;

	StateDesc.TransformDesc.SpeedPerSec = 5.f;
	StateDesc.TransformDesc.RotatePerSec = D3DXToRadian(90.0f);

	pCamera = (CCamera*)(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Camera_Skill", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pCamera)
		return E_FAIL;
	pCamera->Set_Active(false);
	pCameraManager->Add_Camera(CCamera::CAM_ROTATE, pCamera);

	///
	/// Camera Third_Person
	///
	ZeroMemory(&StateDesc, sizeof(CCamera::STATEDESC));
	StateDesc.vEye = _float3(0.f, 0.f, -10.f);
	StateDesc.vAt = _float3(0.f, 0.f, 0.f);
	StateDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	StateDesc.fFovy = D3DXToRadian(60.0f);
	StateDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	StateDesc.fNear = 0.2f;
	StateDesc.fFar = 1200.f;
	pCamera = (CCamera*)(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Camera_Third_Person", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pCamera)
		return E_FAIL;
	pCamera->Set_Active(false);
	pCameraManager->Add_Camera(CCamera::CAM_THIRDPERSON, pCamera);

	///
	/// Camera WayPoint
	///
	ZeroMemory(&StateDesc, sizeof(CCamera::STATEDESC));
	StateDesc.vEye = _float3(0.f, 0.f, -10.f);
	StateDesc.vAt = _float3(0.f, 0.f, 0.f);
	StateDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	StateDesc.fFovy = D3DXToRadian(60.0f);
	StateDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	StateDesc.fNear = 0.2f;
	StateDesc.fFar = 1200.f;
	pCamera = (CCamera*)(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Camera_WayPoint", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pCamera)
		return E_FAIL;
	pCamera->Set_Active(true);

	vector<WAYPOINT> WayPoints = CPickingMgr::Get_Instance()->Get_Waypoints();
	_float3 vSrcPos = _float3(160.f, 500.f, 280.f);
	_float3 vWayLook = WayPoints[1].vPosition - WayPoints[0].vPosition;
	D3DXVec3Normalize(&vWayLook, &vWayLook);

	_float3 vDstPos = WayPoints[0].vPosition - vWayLook * 150.f + _float3(0.f, 1.f, 0.f) * 100.f;
	_float3 vDstLook = vDstPos - WayPoints[0].vPosition;
	D3DXVec3Normalize(&vDstLook, &vDstLook);

	vector<CCamera_WayPoint::WAYPOINT> CamWayPoints;
	CamWayPoints.emplace_back(CCamera_WayPoint::WAYPOINT(vSrcPos, _float3(0.f, 0.f, 0.f), 0.f));
	CamWayPoints.emplace_back(CCamera_WayPoint::WAYPOINT(vDstPos, WayPoints[0].vPosition, 3.f));

	pCameraManager->Add_Camera(CCamera::CAM_WAYPOINT, pCamera);
	pCameraManager->Change_Camera_WayPoint(WayPoints[0].vPosition, CamWayPoints);

	///
	/// Camera Lerp
	///
	ZeroMemory(&StateDesc, sizeof(CCamera::STATEDESC));
	StateDesc.vEye = _float3(0.f, 0.f, -10.f);
	StateDesc.vAt = _float3(0.f, 0.f, 0.f);
	StateDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	StateDesc.fFovy = D3DXToRadian(60.0f);
	StateDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	StateDesc.fNear = 0.2f;
	StateDesc.fFar = 1200.f;
	pCamera = (CCamera*)(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Camera_Lerp", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == pCamera)
		return E_FAIL;
	pCamera->Set_Active(false);
	pCameraManager->Add_Camera(CCamera::CAM_LERP, pCamera);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Terrain(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Terrain", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	CPickingMgr::Get_Instance()->Ready_PickingMgr(m_pGraphic_Device);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Building(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CWaypoint* pWaypoint = dynamic_cast<CWaypoint*>(pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_Waypoint", 10));
	if (nullptr == pWaypoint)
		return E_FAIL;

	CCastle::STATEDESC StateDesc;
	StateDesc.vScale = _float3(8.f, 8.f, 8.f);
	StateDesc.vAxis = _float3(0.f, 1.f, 0.f);
	StateDesc.vPos = pWaypoint->Get_Pos();
	StateDesc.fRadian = D3DXToRadian(45.f);
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Castle", SCENE_STAGE, pLayerTag, &StateDesc)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI_Skill(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CUI::STATEDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(_float(g_iWinSizeX >> 1), _float(g_iWinSizeY >> 1));
	UIDesc.vProjSize = _float2(_float(g_iWinSizeX), _float(g_iWinSizeY));
	UIDesc.iTexIndex = 1;
	pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Frame", SCENE_STAGE, pLayerTag, &UIDesc)->Set_Active(false);

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI_Battle(const _tchar * pLayerTag)
{

	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	if (nullptr == pUIManager)
		return E_FAIL;

	Safe_AddRef(pUIManager);

	// Button
	CUI::STATEDESC UIDesc;
	CUI_Button_Skill* pButton_Skill = nullptr;
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(1200.f, 650.f);
	UIDesc.vProjSize = _float2(120.f, 120.f);
	UIDesc.iTexIndex = 0;
	pButton_Skill = dynamic_cast<CUI_Button_Skill*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Button_Skill", SCENE_STAGE, pLayerTag, &UIDesc));
	pButton_Skill->Set_Skill(SKILL_HEAL);

	// Icon 1
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(1200.f, 650.f);
	UIDesc.vProjSize = _float2(100.f, 100.f);
	UIDesc.iTexIndex = 4;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// CrossHair
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(_float(g_iWinSizeX >> 1), _float(g_iWinSizeY >> 1));
	UIDesc.vProjSize = _float2(100.f, 100.f);
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_CrossHair", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// UI_HpBar
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(150.f, 50.f);
	UIDesc.vProjSize = _float2(200.f, 50.f);
	CUI_HpBar* pHpBar = (CUI_HpBar*)pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_HpBar", SCENE_STAGE, pLayerTag, &UIDesc);
	if (nullptr == pHpBar)
		return E_FAIL;
	pHpBar->Set_UnitPointer(CInputManager::Get_Instance()->Get_PickedPointer());

	list<CGameObject*>* pListBattle = pManagement->Get_ObjectList(SCENE_STAGE, pLayerTag);
	for (CGameObject* pGameObject : *pListBattle)
		pGameObject->Set_Active(false);

	Safe_Release(pUIManager);
	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI_Command(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	if (nullptr == pUIManager)
		return E_FAIL;

	Safe_AddRef(pManagement);
	Safe_AddRef(pUIManager);

	// Warrior
	CUI::STATEDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(460.f, 650.f);
	UIDesc.vProjSize = _float2(90.f, 90.f);
	UIDesc.iTexIndex = 0;
	CUI_Button_Class* pButton_Class = dynamic_cast<CUI_Button_Class*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Button_Class", SCENE_STAGE, pLayerTag, &UIDesc));
	pButton_Class->Set_Occupation(CInfantry::OCCUPATION::OCC_WARRIOR);
	pUIManager->Add_Button(pButton_Class);

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(460.f, 650.f);
	UIDesc.vProjSize = _float2(80.f, 80.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// Archer
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(_float(g_iWinSizeX >> 1), 650);
	UIDesc.vProjSize = _float2(90.f, 90.f);
	UIDesc.iTexIndex = 0;
	pButton_Class = dynamic_cast<CUI_Button_Class*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Button_Class", SCENE_STAGE, pLayerTag, &UIDesc));
	pButton_Class->Set_Occupation(CInfantry::OCCUPATION::OCC_ARCHER);
	pUIManager->Add_Button(pButton_Class);

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(_float(g_iWinSizeX >> 1), 650);
	UIDesc.vProjSize = _float2(80.f, 80.f);
	UIDesc.iTexIndex = 1;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// Mage
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(820.f, 650.f);
	UIDesc.vProjSize = _float2(90.f, 90.f);
	UIDesc.iTexIndex = 0;
	pButton_Class = dynamic_cast<CUI_Button_Class*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Button_Class", SCENE_STAGE, pLayerTag, &UIDesc));
	pButton_Class->Set_Occupation(CInfantry::OCCUPATION::OCC_MAGICIAN);
	pUIManager->Add_Button(pButton_Class);

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(820.f, 650.f);
	UIDesc.vProjSize = _float2(90.f, 90.f);
	UIDesc.iTexIndex = 2;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;


	Safe_Release(pManagement);
	Safe_Release(pUIManager);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI_Free(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	if (nullptr == pUIManager)
		return E_FAIL;

	Safe_AddRef(pManagement);
	Safe_AddRef(pUIManager);

#pragma region FONT
	CMyFont::STATEDESC FontDesc;
	ZeroMemory(&FontDesc, sizeof(CMyFont::STATEDESC));
	FontDesc.FontDesc.Height = 30;
	FontDesc.FontDesc.Width = 15;
	FontDesc.FontDesc.Weight = FW_NORMAL;
	FontDesc.FontDesc.MipLevels = 1;
	FontDesc.FontDesc.Italic = false;
	FontDesc.FontDesc.CharSet = HANGUL_CHARSET;
	FontDesc.FontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	FontDesc.FontDesc.Quality = DEFAULT_QUALITY;
	FontDesc.FontDesc.PitchAndFamily = FF_DONTCARE;
	//lstrcpy(FontDesc.FontDesc.FaceName, L"궁서체");

	FontDesc.eType = CMyFont::FONT_UI;
	FontDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
	FontDesc.vPosition = _float3(200.f, 625.f, 0.f);
	FontDesc.vMultiScale = _float3(1.5f, 1.5f, 1.f);
	FontDesc.tRect = {};
	FontDesc.fAlpha = 1.f;
	FontDesc.iDepth = -1;
	if (FAILED(pUIManager->Add_Font(L"Font_Resource", (CMyFont*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Font", SCENE_STAGE, pLayerTag, &FontDesc))))
		return E_FAIL;


	FontDesc.eType = CMyFont::FONT_UI;
	FontDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
	FontDesc.vPosition = _float3(150.f, 85.f, 0.f);
	FontDesc.vMultiScale = _float3(1.f, 1.f, 1.f);
	FontDesc.tRect = {};
	FontDesc.fAlpha = 1.f;
	FontDesc.iDepth = -1;
	if (FAILED(pUIManager->Add_Font(L"Font_WarriorCount", (CMyFont*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Font", SCENE_STAGE, pLayerTag, &FontDesc))))
		return E_FAIL;


	FontDesc.eType = CMyFont::FONT_UI;
	FontDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
	FontDesc.vPosition = _float3(150.f, 155.f, 0.f);
	FontDesc.vMultiScale = _float3(1.f, 1.f, 1.f);
	FontDesc.tRect = {};
	FontDesc.fAlpha = 1.f;
	FontDesc.iDepth = -1;
	if (FAILED(pUIManager->Add_Font(L"Font_ArcherCount", (CMyFont*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Font", SCENE_STAGE, pLayerTag, &FontDesc))))
		return E_FAIL;


	FontDesc.eType = CMyFont::FONT_UI;
	FontDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
	FontDesc.vPosition = _float3(150.f, 225.f, 0.f);
	FontDesc.vMultiScale = _float3(1.f, 1.f, 1.f);
	FontDesc.tRect = {};
	FontDesc.fAlpha = 1.f;
	FontDesc.iDepth = -1;
	if (FAILED(pUIManager->Add_Font(L"Font_MageCount", (CMyFont*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Font", SCENE_STAGE, pLayerTag, &FontDesc))))
		return E_FAIL;
#pragma endregion

	// Resource
	CUI::STATEDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(180.f, 650.f);
	UIDesc.vProjSize = _float2(300.f, 50.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Frame", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(70.f, 650.f);
	UIDesc.vProjSize = _float2(100.f, 100.f);
	UIDesc.iTexIndex = 3;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// Class
	// W
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(150.f, 100.f);
	UIDesc.vProjSize = _float2(200.f, 50.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Frame", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(80.f, 100.f);
	UIDesc.vProjSize = _float2(80.f, 80.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// A
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(150.f, 170.f);
	UIDesc.vProjSize = _float2(200.f, 50.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Frame", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(80.f, 170.f);
	UIDesc.vProjSize = _float2(70.f, 70.f);
	UIDesc.iTexIndex = 1;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	// M
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(150.f, 240.f);
	UIDesc.vProjSize = _float2(200.f, 50.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Frame", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(80.f, 240.f);
	UIDesc.vProjSize = _float2(80.f, 80.f);
	UIDesc.iTexIndex = 2;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Icon", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;


	Safe_Release(pUIManager);
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	if (nullptr == pUIManager)
		return E_FAIL;

	Safe_AddRef(pManagement);
	Safe_AddRef(pUIManager);

	CUI::STATEDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(g_iWinSizeX - 75, 25.f);
	UIDesc.vProjSize = _float2(150.f, 50.f);
	UIDesc.iTexIndex = 0;
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Frame", SCENE_STAGE, pLayerTag, &UIDesc)))
		return E_FAIL;

	ZeroMemory(&UIDesc, sizeof(CUI::STATEDESC));
	UIDesc.vProjPos = _float2(g_iWinSizeX >> 1, g_iWinSizeY >> 1);
	UIDesc.vProjSize = _float2(g_iWinSizeX, g_iWinSizeY);
	UIDesc.iTexIndex = 0;
	UIDesc.iDepth = -100;
	CGameObject* pObject = pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_UI_Map", SCENE_STAGE, L"Layer_UI_Map", &UIDesc);
	if (nullptr == pObject)
		return E_FAIL;
	pObject->Set_Active(false);

	CMyFont::STATEDESC FontDesc;
	ZeroMemory(&FontDesc, sizeof(CMyFont::STATEDESC));
	FontDesc.FontDesc.Height = 15;
	FontDesc.FontDesc.Width = 8;
	FontDesc.FontDesc.Weight = FW_NORMAL;
	FontDesc.FontDesc.MipLevels = 1;
	FontDesc.FontDesc.Italic = false;
	FontDesc.FontDesc.CharSet = HANGUL_CHARSET;
	FontDesc.FontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	FontDesc.FontDesc.Quality = DEFAULT_QUALITY;
	FontDesc.FontDesc.PitchAndFamily = FF_DONTCARE;
	//lstrcpy(FontDesc.FontDesc.FaceName, L"궁서체");

	FontDesc.eType = CMyFont::FONT_UI;
	FontDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
	FontDesc.vPosition = _float3(g_iWinSizeX - 135, 20.f, 0.f);
	FontDesc.vMultiScale = _float3(1.f, 1.f, 1.f);
	FontDesc.tRect = {};
	FontDesc.fAlpha = 1.f;
	FontDesc.iDepth = -1;
	if (FAILED(pUIManager->Add_Font(L"Font_CamMode", (CMyFont*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Font", SCENE_STAGE, pLayerTag, &FontDesc))))
		return E_FAIL;

	list<CGameObject*>* pListUI = pManagement->Get_ObjectList(SCENE_STAGE, pLayerTag);
	for (CGameObject* pGameObject : *pListUI)
		pGameObject->Set_Active(true);


	Safe_Release(pUIManager);
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Unit_Enemy(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);


	////웨이포인트 생성
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(30.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(70.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(130.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;
	//if (FAILED(Create_Waypoint(pGraphic_Device, _float3(200.f, 0.f, 50.f), 15.f)))
	//	return E_FAIL;

	for (int i = 0; i < 10; ++i)
	{
		CInfantry::INFANTRYDESC tInfantryDesc;
		CInfantry* pInfantry = nullptr;
		tInfantryDesc.tBasedesc = BASEDESC(_float3(20.f + (i * 3), 0.f, 40.f + (i * 3)), _float3(2.f, 2.f, 2.f));
		tInfantryDesc.bFriendly = false;	// flase == 적군
		tInfantryDesc.tOccupation = CInfantry::OCC_WARRIOR;
		if (nullptr == (pInfantry = (CInfantry*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &tInfantryDesc)))
			return E_FAIL;

	}

	for (int i = 0; i < 10; ++i)
	{
		CInfantry::INFANTRYDESC tInfantryDesc;
		CInfantry* pInfantry = nullptr;
		tInfantryDesc.tBasedesc = BASEDESC(_float3(60.f + (i * 3), 0.f, 30.f + (i * 3)), _float3(2.f, 2.f, 2.f));
		tInfantryDesc.bFriendly = false;	// flase == 적군
		tInfantryDesc.tOccupation = CInfantry::OCC_WARRIOR;
		if (nullptr == (pInfantry = (CInfantry*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &tInfantryDesc)))
			return E_FAIL;

	}

	for (int i = 0; i < 10; ++i)
	{
		CInfantry::INFANTRYDESC tInfantryDesc;
		CInfantry* pInfantry = nullptr;
		tInfantryDesc.tBasedesc = BASEDESC(_float3(120.f + (i * 3), 0.f, 30.f + (i * 3)), _float3(2.f, 2.f, 2.f));
		tInfantryDesc.bFriendly = false;	// flase == 적군
		tInfantryDesc.tOccupation = CInfantry::OCC_WARRIOR;
		if (nullptr == (pInfantry = (CInfantry*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &tInfantryDesc)))
			return E_FAIL;

	}

	for (int i = 0; i < 10; ++i)
	{
		CInfantry::INFANTRYDESC tInfantryDesc;
		CInfantry* pInfantry = nullptr;
		tInfantryDesc.tBasedesc = BASEDESC(_float3(190.f + (i * 3), 0.f, 30.f + (i * 3)), _float3(2.f, 2.f, 2.f));
		tInfantryDesc.bFriendly = false;	// flase == 적군
		tInfantryDesc.tOccupation = CInfantry::OCC_WARRIOR;
		if (nullptr == (pInfantry = (CInfantry*)pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &tInfantryDesc)))
			return E_FAIL;

	}

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Boss(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CWaypoint* pWaypoint = dynamic_cast<CWaypoint*>(pManagement->Get_ObjectPointer(SCENE_STAGE, L"Layer_Waypoint", 10));
	if (nullptr == pWaypoint)
		return E_FAIL;

	BASEDESC BaseDesc;
	BaseDesc.vSize = _float3(g_fUnitCX, g_fUnitCY * 4.f, g_fUnitCZ);
	BaseDesc.vPos = _float3(pWaypoint->Get_Pos().x, pWaypoint->Get_Pos().y + BaseDesc.vSize.y * 0.5f, pWaypoint->Get_Pos().z);
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Boss", SCENE_STAGE, pLayerTag, &BaseDesc)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Sphere(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_float3 vPos = CPickingMgr::Get_Instance()->Get_Waypoint(0).vPosition;


	BASEDESC BaseDesc;
	BaseDesc.vPos = vPos + _float3(0.f,2.f,20.f);
	BaseDesc.vSize = _float3(20.f,20.f,20.f);
	if (FAILED(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Sphere", SCENE_STATIC, pLayerTag, &BaseDesc)))
		return E_FAIL;

	Safe_Release(pManagement);
}

HRESULT CScene_Stage::Ready_Layer_Unit_Enemy_Tool(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	HANDLE hFile = 0;
	_ulong dwByte = 0;

#pragma region INFANTRY3
	hFile = CreateFile(L"../Bin/ToolFiles/Infantry3.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// Load List Size
	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);


	// Load Infantry Desc
	CInfantry::INFANTRYDESC InfantryDesc;
	CGameObject* pInfantry = nullptr;
	for (size_t i = 0; i < iSize; i++)
	{
		ReadFile(hFile, &InfantryDesc, sizeof(CInfantry::INFANTRYDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		InfantryDesc.iNumber = 3;

		if (InfantryDesc.bStatic)
		{
			if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Tower", SCENE_STAGE, pLayerTag, &InfantryDesc)))
				return E_FAIL;
		}
		else
		{
			if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &InfantryDesc)))
				return E_FAIL;
		}

		pInfantry->Set_Active(false);
	}

	CloseHandle(hFile);
#pragma endregion

#pragma region INFANTRY8
	hFile = CreateFile(L"../Bin/ToolFiles/Infantry8.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// Load List Size
	iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	// Load Infantry Desc
	for (size_t i = 0; i < iSize; i++)
	{
		ReadFile(hFile, &InfantryDesc, sizeof(CInfantry::INFANTRYDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		InfantryDesc.iNumber = 8;

		if (InfantryDesc.bStatic)
		{
			if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Tower", SCENE_STAGE, pLayerTag, &InfantryDesc)))
				return E_FAIL;
		}
		else
		{
			if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &InfantryDesc)))
				return E_FAIL;
		}

		pInfantry->Set_Active(false);
	}

	CloseHandle(hFile);
#pragma endregion

#pragma region INFANTRY9
	hFile = CreateFile(L"../Bin/ToolFiles/Infantry9.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// Load List Size
	iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	// Load Infantry Desc
	for (size_t i = 0; i < iSize; i++)
	{
		ReadFile(hFile, &InfantryDesc, sizeof(CInfantry::INFANTRYDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		InfantryDesc.iNumber = 9;

		if (InfantryDesc.bStatic)
		{
			if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Tower", SCENE_STAGE, pLayerTag, &InfantryDesc)))
				return E_FAIL;
		}
		else
		{
			if (nullptr == (pInfantry = pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STAGE, pLayerTag, &InfantryDesc)))
				return E_FAIL;
		}

		pInfantry->Set_Active(true);
	}

	CloseHandle(hFile);
#pragma endregion

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_ColliderCube_Tool(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	HANDLE hFile = 0;
	_ulong dwByte = 0;

	CCube* pCube = nullptr;
	_matrix WorldMatrix;

	hFile = CreateFile(L"../Bin/ToolFiles/Collider.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &WorldMatrix, sizeof(_matrix), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		pCube = dynamic_cast<CCube*>(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Cube", SCENE_STAGE, pLayerTag));
		if (nullptr == pCube)
			return E_FAIL;

		pCube->Set_WorldMatrix(WorldMatrix);
	}

	CloseHandle(hFile);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Castle_Tool(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	HANDLE hFile = 0;
	_ulong dwByte = 0;

	CCastle* pCastle = nullptr;
	_matrix WorldMatrix;

	hFile = CreateFile(L"../Bin/ToolFiles/Castle.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &WorldMatrix, sizeof(_matrix), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		pCastle = dynamic_cast<CCastle*>(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Cube", SCENE_STAGE, pLayerTag));
		if (nullptr == pCastle)
			return E_FAIL;

		pCastle->Set_WorldMatrix(WorldMatrix);
	}

	CloseHandle(hFile);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Nature_Tool(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	HANDLE hFile = 0;
	_ulong dwByte = 0;

	CNature::STATEDESC StateDesc;

	hFile = CreateFile(L"../Bin/ToolFiles/NatureGrass.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &StateDesc, sizeof(CNature::STATEDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Nature", SCENE_STAGE, pLayerTag, &StateDesc))
			return E_FAIL;
	}

	CloseHandle(hFile);

#pragma region Tree70
	hFile = CreateFile(L"../Bin/ToolFiles/NatureTree70.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &StateDesc, sizeof(CNature::STATEDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Nature", SCENE_STAGE, pLayerTag, &StateDesc))
			return E_FAIL;
	}

	CloseHandle(hFile);
#pragma endregion

#pragma region Tree100
	hFile = CreateFile(L"../Bin/ToolFiles/NatureTree100.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		ReadFile(hFile, &StateDesc, sizeof(CNature::STATEDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Nature", SCENE_STAGE, pLayerTag, &StateDesc))
			return E_FAIL;
	}

	CloseHandle(hFile);
#pragma endregion

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Unit_Ally(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	if (nullptr == pPickingMgr) return E_FAIL;
	WAYPOINT waypoint = pPickingMgr->Get_Waypoint(0);
	//pPickingMgr->Spawn_Ally(0, waypoint.vBaseCampPos, CInfantry::OCC_WARRIOR);
	//pPickingMgr->Spawn_Ally(0, waypoint.vBaseCampPos, CInfantry::OCC_MAGICIAN);
	//pPickingMgr->Spawn_Ally(0, waypoint.vBaseCampPos, CInfantry::OCC_ARCHER);



	Safe_Release(pManagement);

	return S_OK;
}

CScene_Stage * CScene_Stage::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Stage*	pInstance = new CScene_Stage(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Create CScene_Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage::Free()
{
	if (0 != CPhysicsMgr::Get_Instance()->Destroy_Instance())
		MSG_BOX("Fail to Destroy CPhysicsMgr");

	CScene::Free();
}

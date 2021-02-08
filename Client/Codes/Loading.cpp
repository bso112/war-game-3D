#include "stdafx.h"
#include "..\Headers\Loading.h"
#include "Management.h"

#include "MyFont.h"
#include "Bullet_Trail.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "UI_CrossHair.h"
#include "UI_Button_Class.h"
#include "UI_Button_Skill.h"
#include "UI_Icon.h"
#include "UI_Frame.h"
#include "UI_Map.h"
#include "UI_HpBar.h"
#include "UI_Massage.h"
#include "Meteor.h"
#include "HealingZone.h"

#include "Terrain.h"
#include "Sky.h"
#include "Castle.h"
#include "Gate.h"
#include "Waypoint.h"
#include "Flagpole.h"
#include "Flag.h"
#include "Basecamp.h"
#include "Nature.h"

#include "Sword.h"
#include "Bow.h"
#include "Arrow.h"
#include "Staff.h"
#include "EnergyBall.h"

#include "Boss.h"
#include "BossParts.h"

CLoading::CLoading(PDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

_uint APIENTRY Thread_Main(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	HRESULT	hr = 0;

	EnterCriticalSection(pLoading->Get_CriticalSection());

	switch (pLoading->Get_SceneID())
	{
	case SCENE_MENU:
		hr = pLoading->Loading_ForMenu();
		break;
	case SCENE_STAGE:
		hr = pLoading->Loading_ForStage();
		break;
	}

	LeaveCriticalSection(pLoading->Get_CriticalSection());

	if (FAILED(hr))
		return -1;

	return 0;
}

HRESULT CLoading::Ready_Loading(SCENEID eSceneID)
{
	m_eSceneID = eSceneID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading::Loading_ForMenu()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

#pragma region GAMEOBJECT

#pragma endregion

#pragma region COMPONENT
	// For.Component_Texture_Menu
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_MENU, L"Component_Texture_Menu", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Scene/Menu.png"))))
		return E_FAIL;

	// For.Component_Texture_MenuColor
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_MENU, L"Component_Texture_MenuColor", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Scene/MenuColor.png"))))
		return E_FAIL;
#pragma endregion

	Safe_Release(pManagement);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoading::Loading_ForStage()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

#pragma region GAMEOBJECT_PROTOTYPE
	lstrcpy(m_szLoading, L"0%");
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Sky", CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Castle", CCastle::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Gate", CGate::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Nature
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Nature", CNature::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Sword
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Sword", CSword::Create_Sword_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Bow
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Bow", CBow::Create_Bow_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Arrow
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Arrow", CArrow::Create_Arrow_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Staff
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Staff", CStaff::Create_Staff_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_EnergyBall
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_EnergyBall", CEnergyBall::Create_EnergyBall_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Boss
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Boss", CBoss::Create_Boss_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_BossParts
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_BossParts", CBossParts::Create_BossParts_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_BuletTrail
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_BulletTrail", CBullet_Trail::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_CrossHair
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_CrossHair", CUI_CrossHair::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_Button_Class
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_Button_Class", CUI_Button_Class::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_Button_Skill
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_Button_Skill", CUI_Button_Skill::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_Icon
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_Icon", CUI_Icon::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_Frame
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_Frame", CUI_Frame::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_HpBar
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_HpBar", CUI_HpBar::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Map
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_Map", CUI_Map::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_UI_Massage
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_UI_Massage", CUI_Massage::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Meteor
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Meteor", CMeteor::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_HealingZone
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_HealingZone", CHealingZone::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Effect
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Effect", CEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Font
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STATIC, L"GameObject_Font", CMyFont::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Waypoint
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Waypoint", CWaypoint::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Flagpole
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Flagpole", CFlagpole::Create_Flagpole_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Flag
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Flag", CFlag::Create_Flag_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Basecamp
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_Basecamp", CBasecamp::Create_Basecamp_Prototype(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_ParticleSystem
	if (FAILED(pManagement->Add_GameObject_Prototype(SCENE_STAGE, L"GameObject_ParticleSystem", CParticleSystem::Create(m_pGraphic_Device))))
		return E_FAIL;


#pragma endregion

#pragma region COMPONENT_PROTOTYPE
	// For.Component_Shader_Terrain
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Shader_Terrain", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"30%");

	// For.Component_Texture_Terrain
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Terrain", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Terrain/Terrain%d.tga",2))))
		return E_FAIL;

	// For.Component_Texture_Nature
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Nature", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Terrain/Nature_%d.png", 2))))
		return E_FAIL;

	// For.Component_Texture_Sky
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Sky", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE, L"../Bin/Textures/SkyBox/Burger%d.dds", 4))))
		return E_FAIL;

	// For.Component_Texture_Castle
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Castle", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Castle/Castle%d.tga", 1))))
		return E_FAIL;

	// For.Component_Texture_Gate
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Gate", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Castle/Gate%d.tga", 1))))
		return E_FAIL;

	// For.Component_Texture_Flagpole
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Flagpole", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Waypoint/Flagpole.png"))))
		return E_FAIL;

	// For.Component_Texture_Flag
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Flag", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Waypoint/Flag.png"))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"50%");

	// For.Component_Texture_CrossHair
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_CrossHair", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/CrossHair.png", 1))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Basecamp", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE, L"../Bin/Textures/Waypoint/tent.dds"))))
		return E_FAIL;

	// For.Component_Texture_Boss
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Boss", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Unit/Boss%d.png", 1))))
		return E_FAIL;

	// For.Component_Texture_Sword
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Sword", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Weapon/Sword%d.png", 2))))
		return E_FAIL;

	// For.Component_Texture_Staff
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Staff", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Weapon/Staff.png"))))
		return E_FAIL;

	// For.Component_Texture_EnergyBall
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_EnergyBall", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Weapon/EnergyBall%d.png", 10))))
		return E_FAIL;

	// For.Component_Texture_BulletTrail
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_BulletTrail", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/BulletTrail.png"))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"77%");

	// For.Component_Texture_Map
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Map", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Map.png"))))
		return E_FAIL;

	// For.Component_Texture_Button
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Button", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/Button/%d.png", 2))))
		return E_FAIL;

	// For.Component_Texture_Icon
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Icon", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/Class/%d.png", 7))))
		return E_FAIL;

	// For.Component_Texture_Frame
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Frame", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/Frame/%d.png", 2))))
		return E_FAIL;

	// For.Component_Texture_HpBar
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_HpBar", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/Hp_Bar.png"))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"90%");

	// For.Component_Texture_Meteor
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Meteor", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Skill/Meteor.png"))))
		return E_FAIL;


	// For.Component_Texture_Effect_Explosion
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Effect_Explosion", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Effect/Explosion/%d.png", 18))))
		return E_FAIL;

	// For.Component_Texture_Effect_Hit
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Effect_Hit", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Effect/Hit/%d.png", 14))))
		return E_FAIL;

	// For.Component_Texture_Effect_Smoke
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Effect_Smoke", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Effect/Smoke/%d.png", 8))))
		return E_FAIL;

	// For.Component_Texture_Effect_Break
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Effect_Break", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Effect/Break/%d.png", 91))))
		return E_FAIL;



	// For.Component_Texture_MagicCircle
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_MagicCircle", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/UI/MagicCircle.png"))))
		return E_FAIL;

	// For.Component_Texture_HealingSphere
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_HealingSphere", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Skill/HealSphere.png"))))
		return E_FAIL; 

	// For.Component_Texture_Particle_Blood
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Particle_Blood", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Particle/Blood.png"))))
		return E_FAIL;

	// For.Component_Texture_Heal
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_HealingZone", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Skill/Heal.png"))))
		return E_FAIL;

	// For.Component_Texture_HealCircle
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_HealCircle", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Skill/HealCircle.png"))))
		return E_FAIL;

	// For.Component_Texture_Event
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Texture_Event", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Textures/Event/%d.png", 2))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"98%");

	// For.Component_VIBuffer_Terrain
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pGraphic_Device, L"../Bin/Textures/Terrain/Height.bmp"))))
		return E_FAIL;

	// For. Component_Shader_Castle
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Shader_Castle", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Castle.fx"))))
		return E_FAIL;

	// For. Component_Shader_Gate
	if (FAILED(pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Shader_Gate", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Gate.fx"))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"100%");
#pragma endregion

	m_isFinished = true;

	Safe_Release(pManagement);

	return S_OK;
}

CLoading * CLoading::Create(PDIRECT3DDEVICE9 pGraphic_Device, SCENEID eSceneID)
{
	CLoading*	pInstance = new CLoading(pGraphic_Device);

	if (FAILED(pInstance->Ready_Loading(eSceneID)))
	{
		MSG_BOX("Failed To Creating CLoading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pGraphic_Device);
}

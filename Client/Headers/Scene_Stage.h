#pragma once

// 로딩씬을 의미하는 클래스.
// 내부적으로 로딩객체를 생성.
#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

BEGIN(Client)


class CScene_Stage final : public CScene
{
private:
	explicit CScene_Stage(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);

private:
	HRESULT Ready_Light();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Building(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_Skill(const _tchar * pLayerTag);
	HRESULT Ready_Layer_UI_Battle(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_Command(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI_Free(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Unit_Ally(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Unit_Enemy(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Boss(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Sphere(const _tchar* pLayerTag);

private:	// Tool
	HRESULT	Ready_Layer_Unit_Enemy_Tool(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_ColliderCube_Tool(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Castle_Tool(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Nature_Tool(const _tchar* pLayerTag);



public:
	static CScene_Stage* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free();
};

END
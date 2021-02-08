#pragma once

// 로딩씬을 의미하는 클래스.
// 내부적으로 로딩객체를 생성.
#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

BEGIN(Client)

class CScene_Loading final : public CScene
{
private:
	explicit CScene_Loading(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Loading() = default;
public:
	virtual HRESULT Ready_Scene(SCENEID eNextSceneID);
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();
public:
	HRESULT Change_Scene();

private:
	SCENEID			m_eNextSceneID = SCENE_END;
	CLoading*		m_pLoading = nullptr;	

private:
	LPD3DXFONT		m_pFont = nullptr;
	LPD3DXSPRITE	m_pSprite = nullptr;

private:
	HRESULT Ready_Prototype_Object();
	HRESULT Ready_Layer_BackGround(const _tchar* _LayerTag);
	HRESULT Ready_Layer_LoadingScreen(const _tchar* _LayerTag);
public:
	static CScene_Loading* Create(PDIRECT3DDEVICE9 pGraphic_Device, SCENEID eNextSceneID);
	virtual void Free();
};

END
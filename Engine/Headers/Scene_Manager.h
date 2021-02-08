#pragma once

#include "Scene.h"

BEGIN(Engine)

class CScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CScene_Manager)
private:
	explicit CScene_Manager();
	virtual ~CScene_Manager() = default;
public:
	HRESULT SetUp_CurrentScene(CScene* pCurrentScene, _uint _iSceneID);
	_int Update_CurrentScene(_double TimeDelta);
	HRESULT Render_CurrentScene();
	_uint	Get_CurrScene();
private:
	CScene*			m_pCurrentScene = nullptr;
	_uint			m_iCurrScene = 0;
public:
	virtual void Free();
};

END
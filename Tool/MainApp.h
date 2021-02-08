#pragma once
#include "Tool_Defines.h"
#include "Management.h"

BEGIN(Tool)

class CMainApp final : public CBase
{
	DECLARE_SINGLETON(CMainApp)

private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Ready_MainApp();
	_int Update_MainApp(_double TimeDelta);
	HRESULT Render_MainApp();

private:
	PDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	CManagement*			m_pManagement = nullptr;
	CRenderer*				m_pRenderer = nullptr;

private:
	HRESULT Ready_Default_Setting();
	HRESULT Ready_Default_GameObject();
	HRESULT Ready_Default_Component();

private: // Create_Layer
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Castle(const _tchar* pLayerTag);

public:
	static CMainApp* Create();
	virtual void Free();
};

END
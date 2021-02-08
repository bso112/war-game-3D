#pragma once

#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)
class CBlur;
class CRadial;
class CMainApp final : public CBase
{
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
	CKeyMgr*				m_pKeyMgr = nullptr;

private:
	CBlur*	m_pBlur = nullptr;
	CRadial* m_pRadial = nullptr;


#ifdef _DEBUG
private:
	_tchar			m_szFPS[MAX_PATH] = L"";
	_ulong			m_dwDrawNum = 0;
	_double			m_TimeAcc = 0.0;

	LPD3DXFONT		m_pFont = nullptr;
	LPD3DXSPRITE	m_pSprite = nullptr;
#endif // _DEBUG

private:
	HRESULT Ready_Default_Setting();
	HRESULT Ready_Start_Scene(SCENEID eNextSceneID);
	HRESULT Ready_Default_GameObject();
	HRESULT Ready_Default_Component();
	HRESULT	Ready_RenderTarget();

public:
	static CMainApp* Create();
	virtual void Free();
};

END
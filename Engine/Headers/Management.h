#pragma once

// 1.클라이언트에서 중점적으로 사용해야할 기능들을 묶어놓은 클래스.
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Timer_Manager.h"
#include "Scene_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "CollisionMgr.h"
#include "KeyMgr.h"
#include "PhysicsMgr.h"
#include "Frustum.h"
#include "Target_Manager.h"
BEGIN(Engine)

class ENGINE_DLL CManagement final : public CBase
{
	DECLARE_SINGLETON(CManagement)
private:
	explicit CManagement();
	virtual ~CManagement() = default;

public:
	HRESULT Ready_Engine(_uint iNumScenes);
	_int Update_Engine(_double TimeDelta);
	HRESULT Render_Engine();
	HRESULT Clear_Engine(_uint iSceneID);

public:
#pragma region DEVICE
	HRESULT Ready_Graphic_Device(HWND hWnd, CGraphic_Device::WINMODE eMode, _uint iBackSizeX, _uint iBackSizeY, PDIRECT3DDEVICE9* ppGraphic_Device);
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	_byte Get_DIKeyState(_ubyte byKey);
	_byte Get_DIMouseKeyState(CInput_Device::DIMOUSEKEY eMouseKey);
	_long Get_DIMouseMoveState(CInput_Device::DIMOUSEMOVE eMouseMove);
#pragma endregion

#pragma region TIMER
	HRESULT Add_Timers(const _tchar* pTimerTag);
	_double Compute_TimeDelta(const _tchar* pTimerTag);
	_double	Get_TimeDelta(const _tchar* pTimerTag);
#pragma endregion

#pragma region SCENE
	HRESULT SetUp_CurrentScene(CScene* pCurrentScene, _uint iSceneID);
	unsigned int Get_CurrScene();
#pragma endregion

#pragma region GAMEOBJECT
	CGameObject* Get_ObjectPointer(_uint iSceneID, const _tchar* pLayerTag, _uint iIndex = 0);
	CComponent* Get_ComponentPointer(_uint iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject_Prototype(_uint iSceneID, const _tchar* pPrototypeTag, CGameObject* pPrototype);
	CGameObject* Add_Object_ToLayer(_uint iPrototypeSceneID, const _tchar* pPrototypeTag, _uint iLayerSceneID, const _tchar* pLayerTag, void* pArg = nullptr);
	list<CGameObject*>* Get_ObjectList(_uint iSceneID, const _tchar* pLayerTag);
	_int Update_Object_Manager(_double TimeDelta);
	_int Late_Update_Object_Manager(_double TimeDelta);
#pragma endregion

#pragma region COMPONENT
	CComponent* Clone_Component(_uint eSceneID, const _tchar* pComponentTag, void* pArg = nullptr);
	HRESULT Add_Component_Prototype(_uint eSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT Clear_Component_Manager(_uint eSceneID);
#pragma endregion

#pragma region PIPELINE
	_matrix Get_Transform(D3DTRANSFORMSTATETYPE eType);
	_float3 Get_CamPosition();
	_float3 Get_CamLook();
	HRESULT Update_PipeLine();
#pragma endregion

#pragma region LIGHT
	const D3DLIGHT9* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(PDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9& LightDesc);
#pragma endregion

#pragma region COLLISION
	HRESULT CheckCollision();

#pragma endregion

#pragma region PHYSICS
	void Set_Gravity(_float fGravity);
	_float Compute_Optimal_Power(_float fAngle, _float3 vShooter, _float3 vTarget);
#pragma endregion

#pragma region FRUSTUM
	_bool Is_InFrustum(_float3 vWorldPos, _float fRadius = 0.f);
#pragma endregion

#pragma region ETC
	_bool	IS_DebugMode() { return m_bDebug; }
	void	Set_DebugMode(_bool bValue) { m_bDebug = bValue; }

#pragma endregion

#pragma region RENDER_TARGET
public:
	HRESULT SetUp_OnGraphicDev(const _tchar* pTargetTag, _uint iIndex);
	HRESULT Release_OnGraphicDev(const _tchar* pTargetTag, _uint iIndex);
public:
	HRESULT Add_RenderTarget(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat);

public:
	LPDIRECT3DTEXTURE9 Get_Texture(const _tchar* pTargetTag);

#pragma endregion


public:
	static void Release_Engine();

private:
	CGraphic_Device*	m_pGraphic_Device = nullptr;
	CInput_Device*		m_pInput_Device = nullptr;
	CTimer_Manager*		m_pTimer_Manager = nullptr;
	CScene_Manager*		m_pScene_Manager = nullptr;
	CObject_Manager*	m_pObject_Manager = nullptr;
	CComponent_Manager*	m_pComponent_Manager = nullptr;
	CPipeLine*			m_pPipeLine = nullptr;
	CLight_Manager*		m_pLight_Manager = nullptr;
	CCollisionMgr*		m_pCollisionMgr = nullptr;
	CPhysicsMgr*		m_pPhysicsMgr = nullptr;
	CFrustum*			m_pFrustum = nullptr;
	CTarget_Manager*	m_pTargetMgr = nullptr;

	_bool				m_bDebug = 0;
public:
	virtual void Free();
};

END
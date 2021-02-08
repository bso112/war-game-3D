#include "..\Headers\Management.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pScene_Manager(CScene_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pCollisionMgr(CCollisionMgr::Get_Instance())
	, m_pPhysicsMgr(CPhysicsMgr::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pTargetMgr(CTarget_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pScene_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pCollisionMgr);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pPhysicsMgr);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pTargetMgr);
}


HRESULT CManagement::Ready_Engine(_uint iNumScenes)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Object_Manager(iNumScenes)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Component_Manager(iNumScenes)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Ready_Frustum()))
		return E_FAIL;

	return S_OK;
}

_int CManagement::Update_Engine(_double TimeDelta)
{
	if (nullptr == m_pScene_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine ||
		nullptr == m_pInput_Device)
		return -1;

	if (TimeDelta > 0.1)
		TimeDelta = 0.1;

	if (FAILED(m_pInput_Device->Set_Input_Device_State()))
		return -1;

	if (0x80000000 & m_pObject_Manager->Update_Object_Manager(TimeDelta))
		return -1;

	if (FAILED(m_pPipeLine->Update_PipeLine()))
		return -1;

	if (FAILED(m_pFrustum->Update_Frustum()))
		return -1;

	if (0x80000000 & m_pObject_Manager->Late_Update_Object_Manager(TimeDelta))
		return -1;

	if (FAILED(m_pPhysicsMgr->Compute_PhysGroup(TimeDelta)))
		return -1;

	if (FAILED(m_pCollisionMgr->CheckCollision()))
		return -1;


	if (0x80000000 & m_pScene_Manager->Update_CurrentScene(TimeDelta))
		return -1;

	return _int();
}

HRESULT CManagement::Render_Engine()
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	if (FAILED(m_pScene_Manager->Render_CurrentScene()))
		return E_FAIL;

	//렌더 끝나고 죽은 오브젝트 클리어
	if (FAILED(m_pObject_Manager->Clear_DeadObject(m_pScene_Manager->Get_CurrScene())))
		return E_FAIL;


	return S_OK;
}

HRESULT CManagement::Clear_Engine(_uint iSceneID)
{
	if (nullptr == m_pComponent_Manager ||
		nullptr == m_pObject_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Clear_Object_Manager(iSceneID)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Clear_Component_Manager(iSceneID)))
		return E_FAIL;

	return S_OK;
}


#pragma region DEVICE



HRESULT CManagement::Ready_Graphic_Device(HWND hWnd, CGraphic_Device::WINMODE eMode, _uint iBackSizeX, _uint iBackSizeY, PDIRECT3DDEVICE9 * ppGraphic_Device)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Ready_Graphic_Device(hWnd, eMode, iBackSizeX, iBackSizeY, ppGraphic_Device);
}

HRESULT CManagement::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	return m_pInput_Device->Ready_Input_Device(hInst, hWnd);
}

_byte CManagement::Get_DIKeyState(_ubyte byKey)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKey);
}

_byte CManagement::Get_DIMouseKeyState(CInput_Device::DIMOUSEKEY eMouseKey)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseKeyState(eMouseKey);
}

_long CManagement::Get_DIMouseMoveState(CInput_Device::DIMOUSEMOVE eMouseMove)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMoveState(eMouseMove);
}

#pragma endregion

#pragma region TIMER

HRESULT CManagement::Add_Timers(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timers(pTimerTag);
}

_double CManagement::Compute_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

_double CManagement::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pCollisionMgr)
		return 0.0;
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

#pragma endregion

#pragma  region SCENE

HRESULT CManagement::SetUp_CurrentScene(CScene * pCurrentScene, _uint iSceneID)
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	return m_pScene_Manager->SetUp_CurrentScene(pCurrentScene, iSceneID);
}

unsigned int CManagement::Get_CurrScene()
{
	return m_pScene_Manager->Get_CurrScene();
}

#pragma endregion


#pragma region GAMEOBJECT

CGameObject * CManagement::Get_ObjectPointer(_uint iSceneID, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ObjectPointer(iSceneID, pLayerTag, iIndex);
}

CComponent * CManagement::Get_ComponentPointer(_uint iSceneID, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPointer(iSceneID, pLayerTag, pComponentTag, iIndex);
}

HRESULT CManagement::Add_GameObject_Prototype(_uint iSceneID, const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(iSceneID, pPrototypeTag, pPrototype);
}

CGameObject* CManagement::Add_Object_ToLayer(_uint iPrototypeSceneID, const _tchar * pPrototypeTag, _uint iLayerSceneID, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_Object_ToLayer(iPrototypeSceneID, pPrototypeTag, iLayerSceneID, pLayerTag, pArg);
}

list<CGameObject*>* CManagement::Get_ObjectList(_uint iSceneID, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ObjectList(iSceneID, pLayerTag);
}

_int CManagement::Update_Object_Manager(_double TimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	return m_pObject_Manager->Update_Object_Manager(TimeDelta);
}

_int CManagement::Late_Update_Object_Manager(_double TimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	return m_pObject_Manager->Late_Update_Object_Manager(TimeDelta);
}

//CComponent * CManagement::Get_ComponentFromPointer(CGameObject * _pGameObj, const _tchar * _pTag)
//{
//	return _pGameObj->Find_Component(_pTag);
//}
#pragma endregion

#pragma region COMPONENT

CComponent * CManagement::Clone_Component(_uint eSceneID, const _tchar * pComponentTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_ComponentPointer(eSceneID, pComponentTag, pArg);
}

HRESULT CManagement::Add_Component_Prototype(_uint eSceneID, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Component(eSceneID, pComponentTag, pComponent);
}

HRESULT CManagement::Clear_Component_Manager(_uint eSceneID)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Clear_Component_Manager(eSceneID);
}

#pragma endregion

_matrix CManagement::Get_Transform(D3DTRANSFORMSTATETYPE eType)
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_Transform(eType);
}

_float3 CManagement::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float3();

	return m_pPipeLine->Get_CamPosition();
}

_float3 CManagement::Get_CamLook()
{
	if (nullptr == m_pPipeLine)
		return _float3();

	return m_pPipeLine->Get_CamLook();
}

HRESULT CManagement::Update_PipeLine()
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Update_PipeLine();
}

const D3DLIGHT9* CManagement::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CManagement::Add_Light(PDIRECT3DDEVICE9 pGraphic_Device, const D3DLIGHT9 & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pGraphic_Device, LightDesc);
}

#pragma region COLLISION
HRESULT CManagement::CheckCollision()
{
	if (nullptr == m_pCollisionMgr)
		return E_FAIL;

	return m_pCollisionMgr->CheckCollision();
}
#pragma endregion

#pragma region PHYSICS
void CManagement::Set_Gravity(_float fGravity)
{
	m_pPhysicsMgr->Set_Gravity(fGravity);
}
#pragma endregion

#pragma region FRUSTUM
_bool CManagement::Is_InFrustum(_float3 vWorldPos, _float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->Is_InFrustum(vWorldPos, fRadius);
}

#pragma endregion

#pragma region RENDER_TARGET


HRESULT CManagement::SetUp_OnGraphicDev(const _tchar * pTargetTag, _uint iIndex)
{
	if (nullptr == m_pTargetMgr)
		return E_FAIL;
	return m_pTargetMgr->SetUp_OnGraphicDev(pTargetTag, iIndex);
}
HRESULT CManagement::Release_OnGraphicDev(const _tchar * pTargetTag, _uint iIndex)
{
	if (nullptr == m_pTargetMgr)
		return E_FAIL;
	return m_pTargetMgr->Release_OnGraphicDev(pTargetTag, iIndex);
}
HRESULT CManagement::Add_RenderTarget(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTargetTag, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat)
{
	if (nullptr == m_pTargetMgr)
		return E_FAIL;
	return m_pTargetMgr->Add_RenderTarget(pGraphic_Device, pTargetTag, iSizeX, iSizeY, eFormat);
}
LPDIRECT3DTEXTURE9 CManagement::Get_Texture(const _tchar * pTargetTag)
{
	if (nullptr == m_pTargetMgr)
		return nullptr;
	return m_pTargetMgr->Get_Texture(pTargetTag);
}

#pragma endregion


_float CManagement::Compute_Optimal_Power(_float fAngle, _float3 vShooter, _float3 vTarget)
{
	return m_pPhysicsMgr->Compute_Optimal_Power(fAngle, vShooter, vTarget);
}

void CManagement::Release_Engine()
{
	if (0 != CManagement::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CManagement");

	if (0 != CCollisionMgr::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Relese CCollisionMgr");

	if (0 != CTarget_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Relese CTarget_Manager");

	if (0 != CObject_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CObject_Manager");

	if (0 != CComponent_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CComponent_Manager");

	if (0 != CScene_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CScene_Manager");

	if (0 != CTimer_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CTimer_Manager");

	if (0 != CPipeLine::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CPipeLine");

	if (0 != CInput_Device::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CInput_Device");

	if (0 != CLight_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CLight_Manager");

	//키매니저를 사용하는 것 밑에서 삭제
	if (0 != CKeyMgr::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Relese CKeyMgr");

	if (0 != CPhysicsMgr::Get_Instance()->Destroy_Instance())
		MSG_BOX("Fail to Destroy CPhysicsMgr");

	if (0 != CFrustum::Get_Instance()->Destroy_Instance())
		MSG_BOX("Fail to Destroy CFrustum");

	if (0 != CGraphic_Device::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CGraphic_Device");
}


void CManagement::Free()
{
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pScene_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pCollisionMgr);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pPhysicsMgr);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTargetMgr);
}

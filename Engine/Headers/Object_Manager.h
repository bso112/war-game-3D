#pragma once

// 0. 원형객체들을 보관한다.
// 1. 화면내에 동작해야할 객체들을 내 기준에 따라 그룹핑해서 보관한다.

#include "Layer.h"

BEGIN(Engine)

class CComponent;
class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	CGameObject*		Get_ObjectPointer(_uint iSceneID, const _tchar* pLayerTag, _uint iIndex = 0);
	CComponent*			Get_ComponentPointer(_uint iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	list<CGameObject*>*	Get_ObjectList(_uint iSceneID, const _tchar* pLayerTag);
public:
	HRESULT Reserve_Object_Manager(_uint iNumScene);
	HRESULT Add_Prototype(_uint iSceneID, const _tchar* pPrototypeTag, CGameObject* pPrototype);
	CGameObject* Add_Object_ToLayer(_uint iPrototypeSceneID, const _tchar* pPrototypeTag, _uint iLayerSceneID, const _tchar* pLayerTag, void* pArg = nullptr);
	_int	Update_Object_Manager(_double TimeDelta);
	_int	Late_Update_Object_Manager(_double TimeDelta);
	HRESULT Clear_Object_Manager(_uint iSceneID);
	//오브젝트 m_bDead true시에 바로지우는게 아니라 이 함수를 통해 렌더끝나고 지운다.
	HRESULT	Clear_DeadObject(_uint iSceneID);
	
private:
	_uint			m_iNumScenes = 0;


private: // For.Prototype_Object
	map<const _tchar*, CGameObject*>*		 m_pPrototypes = nullptr;
	typedef map<const _tchar*, CGameObject*> PROTOTYPES;

private: // For.Active_Object
	map<const _tchar*, CLayer*>*			m_pLayers = nullptr;
	typedef map<const _tchar*, CLayer*>		LAYERS;
public:
	CGameObject* Find_Prototype(_uint iPrototypeSceneID, const _tchar* pPrototypeTag);
	CLayer* Find_Layer(_uint iSceneID, const _tchar* pLayerTag);
public:
	virtual void Free();
};

END
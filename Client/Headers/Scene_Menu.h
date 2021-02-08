#pragma once

// �ε����� �ǹ��ϴ� Ŭ����.
// ���������� �ε���ü�� ����.
#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Client)

class CScene_Menu final : public CScene
{
private:
	explicit CScene_Menu(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Menu() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	_int Change_Scene(SCENEID eNextScene);

public:
	static CScene_Menu* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free();
};

END
#pragma once
#include <d3d9.h>
#include <d3dx9.h>

namespace Tool
{
	const unsigned int	g_iToolViewCX = 1280;
	const unsigned int	g_iToolViewCY = 720;

	const unsigned int	g_iSideViewCX = 400;
	const unsigned int	g_iSideViewCY = 720;

	enum SCENEID { SCENE_STATIC, SCENE_LOADING, SCENE_MENU, SCENE_STAGE, SCENE_END };
	enum OBJECTID { OBJ_INFANTRY, OBJ_CUBE, OBJ_CASTLE, OBJ_WAYPOINT, OBJ_NATURE, OBJ_END };

	typedef struct tagBaseDesc
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vSize;

		tagBaseDesc()
		{
			ZeroMemory(this, sizeof(tagBaseDesc));
		};

		tagBaseDesc(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vSize)
		{
			vPos = _vPos;
			vSize = _vSize;
		}
	}BASEDESC;
}

using namespace Tool;

extern HWND g_hWnd;

extern HWND g_Main_hWnd;
extern HINSTANCE g_Main_hInst;

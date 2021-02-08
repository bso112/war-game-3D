#pragma once

namespace Client
{
	const unsigned int	g_iWinSizeX = 1280;
	const unsigned int	g_iWinSizeY = 720;

	const float			g_fUnitCX = 6.f;
	const float			g_fUnitCY = 6.f;
	const float			g_fUnitCZ = 6.f;


	enum SCENEID { SCENE_STATIC, SCENE_LOADING, SCENE_MENU, SCENE_STAGE, SCENE_END };
}	

using namespace Client;

extern HWND g_hWnd;
extern HINSTANCE g_hInst;


// »ç¿îµå
#include <io.h>
#include "fmod.h"
#pragma comment (lib, "fmodex_vc.lib")


#include <functional>
#include <time.h>
#include "Client_Struct.h"
#include "Client_Macro.h"
#include "Client_Constant.h"
#include "process.h"

extern MYRAY Make_Ray(POINT& ptMouse, D3DXMATRIX ProjMatrix, D3DXMATRIX ViewMatrix);


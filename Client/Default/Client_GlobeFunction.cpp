
#include "stdafx.h"


//마우스 좌표에서부터 레이를 만든다.
MYRAY Make_Ray(POINT& ptMouse, D3DXMATRIX ProjMatrix, D3DXMATRIX ViewMatrix)
{

	D3DXVECTOR4		vMousePos = D3DXVECTOR4((float)ptMouse.x, (float)ptMouse.y, 0.0f, 0.0f);

	// 2차원 투영 스페이스로 변환한다.(0, 0, g_iWinCX, g_iWinCY -> -1, 1, 1, -1)
	vMousePos.x = vMousePos.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = vMousePos.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;


	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	// 뷰스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec4Transform(&vMousePos, &vMousePos, &ProjMatrix);
	D3DXVECTOR3		vMousePivot = D3DXVECTOR3(0.f, 0.f, 0.f);
	D3DXVECTOR3		vMouseRay = D3DXVECTOR3(vMousePos.x, vMousePos.y, vMousePos.z) - vMousePivot;

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// 월드스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec3TransformCoord(&vMousePivot, &vMousePivot, &ViewMatrix);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &ViewMatrix);
	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	return MYRAY(vMousePivot, vMouseRay);

}

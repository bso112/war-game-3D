
#include "stdafx.h"


//���콺 ��ǥ�������� ���̸� �����.
MYRAY Make_Ray(POINT& ptMouse, D3DXMATRIX ProjMatrix, D3DXMATRIX ViewMatrix)
{

	D3DXVECTOR4		vMousePos = D3DXVECTOR4((float)ptMouse.x, (float)ptMouse.y, 0.0f, 0.0f);

	// 2���� ���� �����̽��� ��ȯ�Ѵ�.(0, 0, g_iWinCX, g_iWinCY -> -1, 1, 1, -1)
	vMousePos.x = vMousePos.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = vMousePos.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;


	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	// �佺���̽� ���� ���콺 ����, ���콺�� �������� ���Ѵ�.
	D3DXVec4Transform(&vMousePos, &vMousePos, &ProjMatrix);
	D3DXVECTOR3		vMousePivot = D3DXVECTOR3(0.f, 0.f, 0.f);
	D3DXVECTOR3		vMouseRay = D3DXVECTOR3(vMousePos.x, vMousePos.y, vMousePos.z) - vMousePivot;

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// ���彺���̽� ���� ���콺 ����, ���콺�� �������� ���Ѵ�.
	D3DXVec3TransformCoord(&vMousePivot, &vMousePivot, &ViewMatrix);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &ViewMatrix);
	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	return MYRAY(vMousePivot, vMouseRay);

}

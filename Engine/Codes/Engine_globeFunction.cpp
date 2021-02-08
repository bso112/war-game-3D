#include "Engine_Defines.h"

CUBE Make_Cube(_float3 _vPos, _float3 _fSize)
{
	CUBE cube;
	//¾Õ¸é 
	cube.vVertices[0] = _float3(_vPos.x - _fSize.x * 0.5f, _vPos.y + _fSize.y * 0.5f, _vPos.z - _fSize.z * 0.5f);
	cube.vVertices[1] = _float3(_vPos.x + _fSize.x * 0.5f, _vPos.y + _fSize.y * 0.5f, _vPos.z - _fSize.z * 0.5f);
	cube.vVertices[2] = _float3(_vPos.x + _fSize.x * 0.5f, _vPos.y - _fSize.y * 0.5f, _vPos.z - _fSize.z * 0.5f);
	cube.vVertices[3] = _float3(_vPos.x - _fSize.x * 0.5f, _vPos.y - _fSize.y * 0.5f, _vPos.z - _fSize.z * 0.5f);

	//µÞ¸é
	cube.vVertices[4] = _float3(_vPos.x - _fSize.x * 0.5f, _vPos.y + _fSize.y * 0.5f, _vPos.z + _fSize.z * 0.5f);
	cube.vVertices[5] = _float3(_vPos.x + _fSize.x * 0.5f, _vPos.y + _fSize.y * 0.5f, _vPos.z + _fSize.z * 0.5f);
	cube.vVertices[6] = _float3(_vPos.x + _fSize.x * 0.5f, _vPos.y - _fSize.y * 0.5f, _vPos.z + _fSize.z * 0.5f);
	cube.vVertices[7] = _float3(_vPos.x - _fSize.x * 0.5f, _vPos.y - _fSize.y * 0.5f, _vPos.z + _fSize.z * 0.5f);

	return cube;
}
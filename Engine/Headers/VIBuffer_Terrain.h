#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct tagChunkInfo
	{
		_bool	bOutOfRange = 0;
		_int	iIdx = 0;
		_int	iSegX = 0;
		_int	iSegZ = 0;
	}CHUNK_TAG;

private:
	explicit CVIBuffer_Terrain(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	virtual HRESULT Ready_Component_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, _float fInterval);
	virtual HRESULT Ready_Component_Prototype(const _tchar* pHeightMapPath, _float fInterval);
	virtual HRESULT Ready_Component(void* pArg);
	virtual HRESULT Render_VIBuffer(_int iIdx);

public:
	//해당 WorldPos에 가까운 지형의 버텍스 위치를 _vTerrainPos로 반환한다.
	_bool	Get_TerrainPos(_float3 _vWorldPos, _float3& _vTerrainPos);
	_bool	Get_Normal(_float3 _vWorldPos, _float3& _vTerrainPos);
	virtual _bool Picking(POINT& ptMouse, _matrix WorldMatrix, _float3* pOut);

	CHUNK_TAG	Get_Idx(_float3 vPos);
	_float		Get_Chunk_YPos() { return m_fChunkYPos; }
	_float		Get_Chunk_Radius() { return m_fChunkRadius; }

	_int	Get_SegX() { return m_iSegX; };
	_int	Get_SegZ() { return m_iSegZ; };
	_int	Get_LocalVX() { return m_iLocalVX; };
	_int	Get_LocalVZ() { return m_iLocalVZ; };
	_float3 Get_Chunk_Pos(_int iIdx) { return _float3(m_p_vwChunkRender[iIdx].x, m_p_vwChunkRender[iIdx].y, m_p_vwChunkRender[iIdx].z); }
	_bool	Get_Chunk_Bool(_int iIdx) { if (m_p_vwChunkRender[iIdx].w == 0.f) return false; else return true; }

	void	Set_Chunk_Bool(_int iIdx, _bool bOn) { m_p_vwChunkRender[iIdx].w = bOn; }

private:
	BITMAPFILEHEADER			m_fh;
	BITMAPINFOHEADER			m_ih;

	vector<PDIRECT3DVERTEXBUFFER9>		m_pVBufferSeg;
	PDIRECT3DINDEXBUFFER9		m_pIBufferSeg = nullptr;

	_int						m_iSegX = 1;
	_int						m_iSegZ = 1;
	_int						m_iLocalVX = 1;
	_int						m_iLocalVZ = 1;
	_float						m_fChunkYPos = 0.f;
	_float						m_fChunkRadius = 0.f;

	_uint						m_iNumIndices_Local = 0;
	_uint						m_iNumPrimitive_Local = 0;
	_uint						m_iNumVertices_Local = 0;
	vector<_float4>				m_p_vwChunkRender;

	vector<vector<_float3>>		m_ppVerticesSegPos;

public:
	static CVIBuffer_Terrain* Create(PDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ, _float fInterval = 1.f);
	static CVIBuffer_Terrain* Create(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pHeightMapPath, _float fInterval = 1.f);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END
#include "..\Headers\VIBuffer_Terrain.h"
#include "PipeLine.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(PDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_pVBufferSeg(rhs.m_pVBufferSeg)
	, m_pIBufferSeg(rhs.m_pIBufferSeg)
	, m_iNumVertices_Local(rhs.m_iNumVertices_Local)
	, m_iNumIndices_Local(rhs.m_iNumIndices_Local)
	, m_iNumPrimitive_Local(rhs.m_iNumPrimitive_Local)
{
	m_fh = rhs.m_fh;
	m_ih = rhs.m_ih;
	m_iSegX = rhs.m_iSegX;
	m_iSegZ = rhs.m_iSegZ;
	m_iLocalVX = rhs.m_iLocalVX;
	m_iLocalVZ = rhs.m_iLocalVZ;
	m_p_vwChunkRender = rhs.m_p_vwChunkRender;
	m_fChunkYPos = rhs.m_fChunkYPos;
	m_fChunkRadius = rhs.m_fChunkRadius;
	m_ppVerticesSegPos = rhs.m_ppVerticesSegPos;

	for (auto SEG : m_pVBufferSeg)
		Safe_AddRef(SEG);
	Safe_AddRef(m_pIBufferSeg);
}

HRESULT CVIBuffer_Terrain::Ready_Component_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, _float fInterval)
{
	// 1. � ����, � �ε������� �����Ұ���. 
	// For.Vertices
	m_iStride = sizeof(VTXNORTEX);
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesY = 1;
	m_iNumVerticesZ = iNumVerticesZ;

	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	// For.Indices
	m_iIndexSize = sizeof(_ulong);
	m_iNumPrimitive = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPrimitive * 3;
	m_eIndexFormat = D3DFMT_INDEX32;


	m_iNumPrimitive_Local = (m_iLocalVX - 1) * (m_iLocalVZ - 1) * 2;
	m_iNumIndices_Local = m_iNumPrimitive_Local * 3;
	m_iNumVertices_Local = m_iLocalVX * m_iLocalVZ;

	// 2. ���� ���ǵ� ��� ���۸� �����ϰ�.
	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	// 3. ������ ���ۿ� ���� ä���. 

	// For.Vertices
	VTXNORTEX*		pVertices = nullptr;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);

	for (size_t i = 0; i < iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX; ++j)
		{
			_uint iIndex = i * iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j * fInterval, 0.f, i * fInterval);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / _float(iNumVerticesX - 1), i / _float(iNumVerticesZ - 1));
		}
	}


	m_pVBuffer->Unlock();

	_ulong*	pIndices = nullptr;

	_uint		iNumIndices = 0;

	m_pIBuffer->Lock(0, 0, (void**)&pIndices, 0);

	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * iNumVerticesX + j;

			_uint iIndices[4] = {
				{ iIndex + iNumVerticesX },
				{ iIndex + iNumVerticesX + 1},
				{ iIndex + 1 },
				{ iIndex }
			};

			// ������ �� �ﰢ��.
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			// ���� �ϴ� �ﰢ��. 
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	m_pIBuffer->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Ready_Component_Prototype(const _tchar * pHeightMapPath, _float fInterval)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*		pPixels = new _ulong[m_ih.biWidth * m_ih.biHeight];

	ReadFile(hFile, pPixels, sizeof(_ulong) * m_ih.biWidth * m_ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	// 1. � ����, � �ε������� �����Ұ���. 

	// For.Vertices
	m_iStride = sizeof(VTXNORTEX);

	m_iSegX = 16;
	m_iSegZ = 16;
	m_iLocalVX = 129;
	m_iLocalVZ = 129;

	m_iNumVerticesX = m_iSegX * m_iLocalVX - (m_iSegX - 1);		//X���� ����
	m_iNumVerticesY = 1;
	m_iNumVerticesZ = m_iSegZ * m_iLocalVZ - (m_iSegZ - 1);		//Z���� ����

	_float fOffsetY = -0.f;		//Y������ ���� �̵�
	_float fScaleY = 0.25f;		//�� ���� ��
	_int iRpt = 15;				//������ ȿ�� - ������� ����(Default = 0)

	_float fRatioX = m_ih.biWidth / (_float)m_iNumVerticesX;
	_float fRatioZ = m_ih.biHeight / (_float)m_iNumVerticesZ;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	// For.Indices
	m_iIndexSize = sizeof(_ulong);
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPrimitive * 3;
	m_eIndexFormat = D3DFMT_INDEX32;

	m_iNumPrimitive_Local = (m_iLocalVX - 1) * (m_iLocalVZ - 1) * 2;
	m_iNumIndices_Local = m_iNumPrimitive_Local * 3;
	m_iNumVertices_Local = m_iLocalVX * m_iLocalVZ;

	// 2. ���� ���ǵ� ��� ���۸� �����ϰ�.
	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	// 3. ������ ���ۿ� ���� ä���. 

	m_pVBufferSeg.resize(m_iSegZ * m_iSegX);

	// For.Vertices
	VTXNORTEX*		pVertices = nullptr;
	VTXNORTEX**		pVerticesSeg = new VTXNORTEX*[m_iSegZ * m_iSegX]{ nullptr };

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);


	//////////////////LocalIBuffer ����
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexSize * m_iNumIndices_Local, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIBufferSeg, nullptr)))
		return E_FAIL;

	//////////////////LocalVBuffer ����
	for (int iSeg = 0; iSeg < m_iSegX * m_iSegZ; ++iSeg)
	{
		if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iStride * m_iNumVertices_Local, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVBufferSeg[iSeg], nullptr)))
			return E_FAIL;
		m_pVBufferSeg[iSeg]->Lock(0, 0, (void**)&(pVerticesSeg[iSeg]), 0);
	}

	//����Ʈ������ ���� ���� ����
	//////////////////////

	for (size_t i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iVerIndex = i * m_iNumVerticesX + j;
			_uint iIndex = _uint((_float)i * fRatioZ) * m_ih.biWidth + _uint((_float)j * fRatioX);

			pVertices[iVerIndex].vPosition = m_pVerticesPos[iVerIndex] = _float3(j * fInterval, (pPixels[iIndex] & 0x000000ff) / fScaleY + fOffsetY, i * fInterval);
			pVertices[iVerIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iVerIndex].vTexUV = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}

	//ûũ Y�� ����, ���� ���ϱ�, ���׸�Ʈ ����
	_float fChunkYLength = 255.f / fScaleY;
	m_fChunkYPos = fChunkYLength * 0.5f + fOffsetY;
	m_fChunkRadius = sqrtf(powf(fChunkYLength *0.5f, 2.f) + powf((m_iLocalVX >> 1) + 1, 2.f) + powf((m_iLocalVZ >> 1) + 1, 2.f));

	m_p_vwChunkRender.resize(m_iSegX * m_iSegZ, _float4(0.f, 0.f, 0.f, 0.f));
	for (int i = 0; i < m_iSegZ; ++i)
	{
		for (int j = 0; j < m_iSegX; ++j)
		{
			m_p_vwChunkRender[i * m_iSegX + j] = _float4(j * m_iLocalVX + m_iLocalVX * 0.5f, m_fChunkYPos, i * m_iLocalVZ + m_iLocalVZ * 0.5f, 0.f);
		}
	}

	//���� �ε巴��
	_float* fTempY = new _float[m_iNumVertices];
	for (_int n = 0; n < iRpt; ++n)
	{
		for (size_t i = 0; i < m_iNumVerticesZ; ++i)
		{
			for (size_t j = 0; j < m_iNumVerticesX; ++j)
			{
				_uint iIndex = i * m_iNumVerticesX + j;

				if (i > 0 && i < m_iNumVerticesZ - 1 &&
					j > 0 && j < m_iNumVerticesX - 1)
				{
					_float fNewY = m_pVerticesPos[iIndex].y + m_pVerticesPos[iIndex - 1].y + m_pVerticesPos[iIndex + 1].y + m_pVerticesPos[iIndex + m_iNumVerticesX].y + m_pVerticesPos[iIndex - m_iNumVerticesX].y +
						m_pVerticesPos[iIndex - 1 + m_iNumVerticesX].y + m_pVerticesPos[iIndex - 1 - m_iNumVerticesX].y + m_pVerticesPos[iIndex + 1 + m_iNumVerticesX].y + m_pVerticesPos[iIndex + 1 - m_iNumVerticesX].y;
					fNewY /= 9.f;
					fTempY[iIndex] = fNewY;
				}
				else
					pVertices[iIndex].vPosition = m_pVerticesPos[iIndex];

				if (n == iRpt - 1)
				{
					pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
					pVertices[iIndex].vTexUV = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
				}
			}
		}

		for (size_t i = 0; i < m_iNumVerticesZ; ++i)
		{
			for (size_t j = 0; j < m_iNumVerticesX; ++j)
			{
				_uint iIndex = i * m_iNumVerticesX + j;

				pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j * fInterval, fTempY[iIndex], i * fInterval);
			}
		}
	}
	Safe_Delete_Array(fTempY);
	//�ε���, ��� ����
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		_ulong*	pIndices = nullptr;

		_uint		iNumIndices = 0;

		_float3		vSour, vDest, vNormal;

		m_pIBuffer->Lock(0, 0, (void**)&pIndices, 0);

		for (size_t i = 0; i < m_iNumVerticesZ - 1; ++i)
		{
			for (size_t j = 0; j < m_iNumVerticesX - 1; ++j)
			{
				_uint iIndex = i * m_iNumVerticesX + j;

				_uint iIndices[4] = {
					{ iIndex + m_iNumVerticesX },
					{ iIndex + m_iNumVerticesX + 1 },
					{ iIndex + 1 },
					{ iIndex },
				};

				// ������ �� �ﰢ��.
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];

				vSour = pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition;
				vDest = pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition;

				D3DXVec3Cross(&vNormal, &vSour, &vDest);
				D3DXVec3Normalize(&vNormal, &vNormal);

				pVertices[iIndices[0]].vNormal += vNormal;
				pVertices[iIndices[1]].vNormal += vNormal;
				pVertices[iIndices[2]].vNormal += vNormal;

				// ���� �ϴ� �ﰢ��. 
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];

				vSour = pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition;
				vDest = pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition;

				D3DXVec3Cross(&vNormal, &vSour, &vDest);
				D3DXVec3Normalize(&vNormal, &vNormal);

				pVertices[iIndices[0]].vNormal += vNormal;
				pVertices[iIndices[2]].vNormal += vNormal;
				pVertices[iIndices[3]].vNormal += vNormal;
			}
		}

		for (size_t i = 0; i < m_iNumVertices; ++i)
			D3DXVec3Normalize(&pVertices[i].vNormal, &pVertices[i].vNormal);
	}

	//���� ���� ����
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_ppVerticesSegPos.resize(m_iSegX * m_iSegZ);
	for (auto& vPos : m_ppVerticesSegPos)
		vPos.resize(m_iNumVertices_Local, _float3(0.f, 0.f, 0.f));

	for (int i = 0; i < m_iSegZ; ++i)
	{
		for (int j = 0; j < m_iSegX; ++j)
		{
			//m_ppVerticesSegPos.push_back(new _float3[m_iLocalVX * m_iLocalVZ]);

			for (int ii = 0; ii < m_iLocalVZ; ++ii)
			{
				for (int jj = 0; jj < m_iLocalVX; ++jj)
				{
					int iSegIdx = ii * m_iLocalVX + jj;
					pVerticesSeg[j + i * m_iSegX][iSegIdx] = pVertices[(((ii + i * (m_iLocalVZ - 1))* m_iNumVerticesX)/*���� �ϴ� offset*/ + j * (m_iLocalVX - 1)/*���� ���� offset*/ + jj)];
					m_ppVerticesSegPos.back()[iSegIdx] = pVerticesSeg[j + i * m_iSegX][iSegIdx].vPosition;
					//pVerticesSeg[j + i * m_iSegX][iSegIdx].vTexUV = _float2(jj / _float(m_iLocalVX - 1), ii / _float(m_iLocalVZ - 1));
				}
			}
		}
	}

	//���� ���ۿ� �ε��� ����
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		_ulong*	pLocalIndices = nullptr;


		_float3		vSour, vDest, vNormal;

		m_pIBufferSeg->Lock(0, 0, (void**)&pLocalIndices, 0);

		for (int Wi = 0; Wi < m_iSegZ; ++Wi)
		{
			for (int Wj = 0; Wj < m_iSegX; ++Wj)
			{
				_uint		iLocalNumIndices = 0;
				for (int i = 0; i < m_iLocalVZ - 1; ++i)
				{
					for (int j = 0; j < m_iLocalVX - 1; ++j)
					{
						_uint iIndex = i * m_iLocalVX + j;

						_uint iIndices[4] = {
							{ iIndex + m_iLocalVX },
							{ iIndex + m_iLocalVX + 1 },
							{ iIndex + 1 },
							{ iIndex }
						};

						// ������ �� �ﰢ��.
						pLocalIndices[iLocalNumIndices++] = iIndices[0];
						pLocalIndices[iLocalNumIndices++] = iIndices[1];
						pLocalIndices[iLocalNumIndices++] = iIndices[2];

						//vSour = pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition;
						//vDest = pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition;

						//D3DXVec3Cross(&vNormal, &vSour, &vDest);
						//D3DXVec3Normalize(&vNormal, &vNormal);

						//pVertices[iIndices[0]].vNormal += vNormal;
						//pVertices[iIndices[1]].vNormal += vNormal;
						//pVertices[iIndices[2]].vNormal += vNormal;

						// ���� �ϴ� �ﰢ��. 
						pLocalIndices[iLocalNumIndices++] = iIndices[0];
						pLocalIndices[iLocalNumIndices++] = iIndices[2];
						pLocalIndices[iLocalNumIndices++] = iIndices[3];

						vSour = pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition;
						vDest = pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition;

						//D3DXVec3Cross(&vNormal, &vSour, &vDest);
						//D3DXVec3Normalize(&vNormal, &vNormal);

						//pVertices[iIndices[0]].vNormal += vNormal;
						//pVertices[iIndices[2]].vNormal += vNormal;
						//pVertices[iIndices[3]].vNormal += vNormal;
					}
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int iSeg = 0; iSeg < m_iSegX * m_iSegZ; ++iSeg)
		m_pVBufferSeg[iSeg]->Unlock();

	m_pIBufferSeg->Unlock();
	m_pVBuffer->Unlock();
	m_pIBuffer->Unlock();
	Safe_Delete_Array(pPixels);

	//for (int i = 0; i < m_iSegZ * m_iSegX; ++i)
	//	Safe_Delete(pVerticesSeg[i]);
	Safe_Delete_Array(pVerticesSeg);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Ready_Component(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Render_VIBuffer(_int iIdx)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pVBufferSeg[iIdx])
		return E_FAIL;

	// 1. ��ġ��ü�� �����ؾ��� �������۸� ��ġ ������ ���°�� �����ұ��? 
	m_pGraphic_Device->SetStreamSource(0, m_pVBufferSeg[iIdx], 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIBufferSeg);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices_Local, 0, m_iNumPrimitive_Local);

	return S_OK;
}

_bool CVIBuffer_Terrain::Get_TerrainPos(_float3 _vWorldPos, _float3& _vTerrainPos)
{
	//interval�� 1�̴� ���� �����ʿ䰡 ����.
	_int x = (_int)_vWorldPos.x;
	_int z = (_int)_vWorldPos.z;

	_uint iIndex = z * m_iNumVerticesX + x;
	if (m_iNumVerticesX * m_iNumVerticesZ <= iIndex)
		return false;

	_vTerrainPos = m_pVerticesPos[iIndex];

	return true;
}

_bool CVIBuffer_Terrain::Get_Normal(_float3 _vWorldPos, _float3 & _vTerrainNor)
{
	_int iCntX = (_int)m_iNumVerticesX;
	_int iCntZ = (_int)m_iNumVerticesZ;

	_int iIndex = _int(_vWorldPos.x) + _int(_vWorldPos.z) * iCntX;

	_float3 vVec1;
	_float3 vVec2;

	if (_vWorldPos.x < 0.f ||
		_vWorldPos.z < 0.f ||
		_vWorldPos.x > _float(iCntX) ||
		_vWorldPos.z > _float(iCntZ))
		return 0;

	if (_vWorldPos.x + _vWorldPos.z >= _vWorldPos.x + _vWorldPos.z + 1.f)
	{
		//x+z>=2x+itvl : �� �ﰢ��
		iIndex += iCntX + 1;
		vVec1 = m_pVerticesPos[iIndex] - m_pVerticesPos[iIndex - 1];
		vVec2 = m_pVerticesPos[iIndex - iCntX] - m_pVerticesPos[iIndex];
	}
	else
	{
		//x+z<2x+itvl : �Ʒ��ﰢ��
		vVec1 = m_pVerticesPos[iIndex + iCntX] - m_pVerticesPos[iIndex];
		vVec2 = m_pVerticesPos[iIndex + 1] - m_pVerticesPos[iIndex];
	}

	_float3 vNormal;
	D3DXVec3Cross(&vNormal, &vVec1, &vVec2);
	D3DXVec3Normalize(&vNormal, &vNormal);

	_vTerrainNor = vNormal;

	return 1;

	//_matrix rotation;
	//min(-45, max(45, ));

	//yroation
	//xrotaiton += �����ӷ�

	//D3DXMatrixRotationYawPitchRoll(&rotation , D3DXToRadian(xrotaiton), D3DXToRadian(yrotaion), D3DXToRadian(0));


	//100
	//	010
	//	001 * rotation;
	//xyz
}

_bool CVIBuffer_Terrain::Picking(POINT & ptMouse, _matrix WorldMatrix, _float3 * pWorldOut)
{
	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);

	_float4		vMousePos = _float4((_float)ptMouse.x, (_float)ptMouse.y, 0.0f, 0.0f);

	// 2���� ���� �����̽��� ��ȯ�Ѵ�.(0, 0, g_iWinCX, g_iWinCY -> -1, 1, 1, -1)
	vMousePos.x = vMousePos.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = vMousePos.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	if (nullptr == pPipeLine)
		return false;
	Safe_AddRef(pPipeLine);

	_matrix ProjMatrix = pPipeLine->Get_Transform(D3DTS_PROJECTION);
	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	D3DXVec4Transform(&vMousePos, &vMousePos, &ProjMatrix);

	// �佺���̽� ���� ���콺 ����, ���콺�� �������� ���Ѵ�.
	_float3		vMousePivot = _float3(0.f, 0.f, 0.f);
	_float3		vMouseRay = _float3(vMousePos.x, vMousePos.y, vMousePos.z) - vMousePivot;

	_matrix ViewMatrix = pPipeLine->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// ���彺���̽� ���� ���콺 ����, ���콺�� �������� ���Ѵ�.
	D3DXVec3TransformCoord(&vMousePivot, &vMousePivot, &ViewMatrix);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &ViewMatrix);

	// ���÷� ����.
	_matrix	WorldMatrixInverse;
	D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);

	// ���ý����̽� ���� ���콺 ����, ���콺�� �������� ���Ѵ�.
	D3DXVec3TransformCoord(&vMousePivot, &vMousePivot, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &WorldMatrixInverse);

	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	_float fFar = 600.f;

	_int	iNumX = _int(abs(vMouseRay.x * fFar)) + 1;
	_int	iNumZ = _int(abs(vMouseRay.z * fFar)) + 1;

	vector<_int> vectorSpecimen;
	vectorSpecimen.reserve((fFar) * 9);

	for (int i = 0; i <= int(fFar); ++i)
	{
		for (int ii = -1; ii < 2; ++ii)
		{
			for (int jj = -1; jj < 2; ++jj)
			{
				_float3 vTest = vMousePivot + vMouseRay * i;
				_int iIdx = _int(vTest.x) + (_int(vTest.z) + ii) * m_iNumVerticesX + jj;
				if (iIdx > 0 && iIdx < m_iNumVertices)
					vectorSpecimen.push_back(iIdx);
			}
		}
	}

	for (auto iIdx : vectorSpecimen)
	{
		_uint iIndices[4] = {
			{ iIdx + m_iNumVerticesX },
			{ iIdx + m_iNumVerticesX + 1 },
			{ iIdx + 1 },
			{ iIdx },
		};

		_float		fU, fV, fDist;

		// �� ��� �ﰢ��.z
		if (true == D3DXIntersectTri(&m_pVerticesPos[iIndices[1]], &m_pVerticesPos[iIndices[0]], &m_pVerticesPos[iIndices[2]], &vMousePivot, &vMouseRay, &fU, &fV, &fDist))
		{
			_float3 vLocalMouse = vMousePivot + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
			D3DXVec3TransformCoord(pWorldOut, &vLocalMouse, &WorldMatrix);
			Safe_Release(pPipeLine);
			return true;
		}



		// �� �ϴ� �ﰢ��.
		if (true == D3DXIntersectTri(&m_pVerticesPos[iIndices[3]], &m_pVerticesPos[iIndices[2]], &m_pVerticesPos[iIndices[0]], &vMousePivot, &vMouseRay, &fU, &fV, &fDist))
		{
			_float3 vLocalMouse = vMousePivot + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
			D3DXVec3TransformCoord(pWorldOut, &vLocalMouse, &WorldMatrix);
			Safe_Release(pPipeLine);
			return true;

		}
	}


	vectorSpecimen.clear();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//TEMP	for (int iSegZ = 0; iSegZ < m_iSegZ; ++iSegZ)
	//TEMP	{
	//TEMP		for (int iSegX = 0; iSegX < m_iSegX; ++iSegX)
	//TEMP		{
	//TEMP			for (size_t i = 0; i < m_iLocalVZ - 1; ++i)
	//TEMP			{
	//TEMP				for (size_t j = 0; j < m_iLocalVX - 1; ++j)
	//TEMP				{
	//TEMP					_uint iIndex = i * m_iLocalVX + j;
	//TEMP
	//TEMP					_uint iIndices[4] = {
	//TEMP						{ iIndex + m_iLocalVX },
	//TEMP						{ iIndex + m_iLocalVX + 1 },
	//TEMP						{ iIndex + 1 },
	//TEMP						{ iIndex },
	//TEMP					};
	//TEMP					///////////////////////////////////////////////////////////////////
	//TEMP					_float		fU, fV, fDist;
	//TEMP
	//TEMP					// �� ��� �ﰢ��.
	//TEMP					if (true == D3DXIntersectTri(&m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[1]], &m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[0]], &m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[2]], &vMousePivot, &vMouseRay, &fU, &fV, &fDist))
	//TEMP					{
	//TEMP						///////////////////
	//TEMP						_float3 ddd1 = m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[1]];
	//TEMP						_float3 ddd2 = m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[0]];
	//TEMP						_float3 ddd3 = m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[2]];
	//TEMP						///////////////////
	//TEMP						_float3 vLocalMouse = vMousePivot + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
	//TEMP						D3DXVec3TransformCoord(pWorldOut, &vLocalMouse, &WorldMatrix);
	//TEMP						Safe_Release(pPipeLine);
	//TEMP						return true;
	//TEMP					}
	//TEMP
	//TEMP
	//TEMP
	//TEMP					// �� �ϴ� �ﰢ��.
	//TEMP					if (true == D3DXIntersectTri(&m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[3]], &m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[2]], &m_ppVerticesSegPos[iSegX + iSegZ * m_iSegX][iIndices[0]], &vMousePivot, &vMouseRay, &fU, &fV, &fDist))
	//TEMP					{
	//TEMP						_float3 vLocalMouse = vMousePivot + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
	//TEMP						D3DXVec3TransformCoord(pWorldOut, &vLocalMouse, &WorldMatrix);
	//TEMP						Safe_Release(pPipeLine);
	//TEMP						return true;
	//TEMP
	//TEMP					}
	//TEMP				}
	//TEMP
	//TEMP			}
	//TEMP		}
	//TEMP	}


	Safe_Release(pPipeLine);

	return false;
}

CVIBuffer_Terrain::CHUNK_TAG CVIBuffer_Terrain::Get_Idx(_float3 vPos)
{
	_int	iCurSX = _int(vPos.x / (m_iLocalVX - 1));
	_int	iCurSZ = _int(vPos.z / (m_iLocalVZ - 1));

	_bool bOutOfRange = 0;

	if (iCurSX < 0)
	{
		iCurSX = 0;
		bOutOfRange = 1;
	}

	if (iCurSX >= m_iSegX)
	{
		iCurSX = m_iSegX - 1;
		bOutOfRange = 1;
	}

	if (iCurSZ < 0)
	{
		iCurSZ = 0;
		bOutOfRange = 1;
	}

	if (iCurSZ >= m_iSegZ)
	{
		iCurSZ = m_iSegZ - 1;
		bOutOfRange = 1;
	}

	CHUNK_TAG tResult;
	tResult.bOutOfRange = bOutOfRange;
	tResult.iIdx = iCurSX + iCurSZ * m_iSegX;
	tResult.iSegX = iCurSX;
	tResult.iSegZ = iCurSZ;

	return tResult;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(PDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ, _float fInterval)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(iNumVerticesX, iNumVerticesZ, fInterval)))
	{
		MessageBox(0, L"Failed To Creating CVIBuffer_Terrain", L"System Message", MB_OK);

		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(PDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pHeightMapPath, _float fInterval)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(pHeightMapPath, fInterval)))
	{
		MessageBox(0, L"Failed To Creating CVIBuffer_Terrain", L"System Message", MB_OK);

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone_Component(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"Failed To Creating CVIBuffer_Terrain", L"System Message", MB_OK);

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	for (auto& SEG : m_pVBufferSeg)
		Safe_Release(SEG);
	m_pVBufferSeg.clear();

	for (auto SEG : m_ppVerticesSegPos)
		SEG.clear();
	m_ppVerticesSegPos.clear();
	Safe_Release(m_pIBufferSeg);
	CVIBuffer::Free();
}

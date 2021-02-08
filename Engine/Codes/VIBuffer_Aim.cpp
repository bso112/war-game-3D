#include "..\Headers\VIBuffer_Aim.h"

CCVIBuffer_Aim::CCVIBuffer_Aim(PDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CCVIBuffer_Aim::CCVIBuffer_Aim(const CCVIBuffer_Aim & rhs)
	: CVIBuffer(rhs)
{
	m_fRadius = rhs.m_fRadius;
	m_iSegment = rhs.m_iSegment;
	m_iVCnt = rhs.m_iVCnt;
	m_vectorAimPos.reserve(m_iVCnt);
}

HRESULT CCVIBuffer_Aim::Ready_Component_Prototype()
{
	// 1. 어떤 정점, 어떤 인덱스들을 정의할건지. 


	// For.Vertices
	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = m_iVCnt * m_iSegment;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

	// For.Indices
	m_iIndexSize = sizeof(_ushort);
	m_iNumPrimitive = (m_iVCnt - 1)*m_iSegment * 2;
	m_iNumIndices = m_iNumPrimitive * 3;
	m_eIndexFormat = D3DFMT_INDEX16;

	// 2. 위에 정의된 대로 버퍼를 생성하고.
	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	// 3. 생성된 버퍼에 값을 채우고. 

	// For.Vertices
	VTXCUBETEX*		pVertices = nullptr;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);

	for (int i = 0; i < m_iVCnt; ++i)
	{
		for (int j = 0; j < m_iSegment; j++)
		{
			_float3 vTemp = _float3(0.f, 0.f, 0.f);
			vTemp.x += m_fRadius;
			pVertices[m_iSegment * i + j].vPosition = vTemp;
		}

		//vTemp = _float3(0.f, 0.f, 0.f);
		//vTemp.z += m_fRadius;
		//pVertices[m_iSegment * i + 1].vPosition = vTemp;

		//vTemp = _float3(0.f, 0.f, 0.f);
		//vTemp.x -= m_fRadius;
		//pVertices[m_iSegment * i + 2].vPosition = vTemp;

		//vTemp = _float3(0.f, 0.f, 0.f);
		//vTemp.z -= m_fRadius;
		//pVertices[m_iSegment * i + 3].vPosition = vTemp;
	}

	m_pVBuffer->Unlock();

	_ushort*	pIndices = nullptr;

	m_pIBuffer->Lock(0, 0, (void**)&pIndices, 0);

	_uint		iIndex = 0;

	for (int i = 0; i < m_iVCnt - 1; ++i)
	{
		for (int j = 0; j < m_iSegment; ++j)
		{
			if (j < m_iSegment - 1)
			{
				pIndices[iIndex++] = i * m_iSegment + j;		pIndices[iIndex++] = i * m_iSegment + j + 1;			pIndices[iIndex++] = (i + 1) * m_iSegment + j + 1;
				pIndices[iIndex++] = i * m_iSegment + j;	pIndices[iIndex++] = (i + 1) * m_iSegment + j + 1;		pIndices[iIndex++] = (i + 1) * m_iSegment + j;
			}
			else
			{
				pIndices[iIndex++] = i * m_iSegment + j;	pIndices[iIndex++] = i * m_iSegment;			pIndices[iIndex++] = (i + 1) * m_iSegment;
				pIndices[iIndex++] = i * m_iSegment + j;	pIndices[iIndex++] = (i + 1) * m_iSegment;		pIndices[iIndex++] = (i + 1) * m_iSegment + j;
			}
		}
	}

	m_pIBuffer->Unlock();

	return S_OK;
}

HRESULT CCVIBuffer_Aim::Ready_Component(void * pArg)
{
	return S_OK;
}

void CCVIBuffer_Aim::Update_VBuffer()
{
	VTXCUBETEX*		pVertices = nullptr;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);

	for (int i = 0; i < m_iVCnt; ++i)
	{
		_float fRadius = m_fRadius;

		//if (i == 0)
		//	fRadius = 0.f;
		if (i <= 1)
			fRadius = m_fRadius;
		else
			fRadius = m_fRadius - (i - 1) * m_fRadius / (m_iVCnt - 2);

		for (int j = 0; j < m_iSegment; j++)
		{
			_float3 vTemp = m_vectorAimPos[i];

			_float3 vLook;
			if (i == 0)
				vLook = m_vectorAimPos[i + 1] - m_vectorAimPos[i];
			else
				vLook = m_vectorAimPos[i] - m_vectorAimPos[i - 1];

			D3DXVec3Normalize(&vLook, &vLook);
			_float3 vRight;
			D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);
			D3DXVec3Normalize(&vRight, &vRight);
			_float3 vUp;
			D3DXVec3Cross(&vUp, &vLook, &vRight);
			D3DXVec3Normalize(&vUp, &vUp);
			_matrix matRot;
			D3DXMatrixIdentity(&matRot);
			D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(360.f / m_iSegment*j));
			D3DXVec3TransformNormal(&vUp, &vUp, &matRot);
			vUp *= fRadius;
			vTemp += vUp;/////////////////////////위치값 보정x

			pVertices[m_iSegment * i + j].vPosition = vTemp;
			pVertices[m_iSegment * i + j].vTexUV = _float3(0.f, i / _float(m_iVCnt - 1), 0.f);
		}

		//vTemp = m_vectorAimPos[i];
		//vTemp.z += m_fRadius;
		//pVertices[m_iSegment * i + 1].vPosition = vTemp;


		//vTemp = m_vectorAimPos[i];
		//vTemp.x -= m_fRadius;
		//pVertices[m_iSegment * i + 2].vPosition = vTemp;


		//vTemp = m_vectorAimPos[i];
		//vTemp.z -= m_fRadius;
		//pVertices[m_iSegment * i + 3].vPosition = vTemp;
	}

	m_pVBuffer->Unlock();
}

CCVIBuffer_Aim * CCVIBuffer_Aim::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCVIBuffer_Aim*	pInstance = new CCVIBuffer_Aim(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MessageBox(0, L"Failed To Creating CCVIBuffer_Aim", L"System Message", MB_OK);

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCVIBuffer_Aim::Clone_Component(void * pArg)
{
	CCVIBuffer_Aim*	pInstance = new CCVIBuffer_Aim(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"Failed To Creating CCVIBuffer_Aim", L"System Message", MB_OK);

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCVIBuffer_Aim::Free()
{
	CVIBuffer::Free();
}

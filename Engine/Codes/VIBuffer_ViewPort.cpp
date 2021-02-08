#include "VIBuffer_ViewPort.h"

USING(Engine)


//객체만 복사하고 안의 내용물은 얕은복사.
CVIBuffer_ViewPort::CVIBuffer_ViewPort(CVIBuffer_ViewPort& _rhs)
	:CComponent(_rhs),
	m_pVBuffer(_rhs.m_pVBuffer),
	m_pIBuffer(_rhs.m_pIBuffer)

{
	Safe_AddRef(m_pVBuffer);
	Safe_AddRef(m_pIBuffer);
	memcpy(&m_vBasePos, _rhs.m_vBasePos, sizeof(m_vBasePos));
}

HRESULT CVIBuffer_ViewPort::Render()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pVBuffer ||
		nullptr == m_pIBuffer)
		return E_FAIL;

	m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, sizeof(VTXTEX_VIEWPORT));
	m_pGraphic_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1));
	m_pGraphic_Device->SetIndices(m_pIBuffer);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	return S_OK;
}

CVIBuffer_ViewPort * CVIBuffer_ViewPort::Create(PDIRECT3DDEVICE9 _pGraphic_Device)
{
	CVIBuffer_ViewPort* pInstance = new CVIBuffer_ViewPort(_pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Fail to create VIBuffer");
		delete pInstance;
		pInstance = nullptr;
	}
	return pInstance;
}


HRESULT CVIBuffer_ViewPort::Set_Transform(_matrix _matrix)
{
	if (nullptr == m_pVBuffer)
		return E_FAIL;

	VTXTEX_VIEWPORT* pVertices = nullptr;
	//다른 스레드가 접근하지 못하도록 락을 걸고, 락이 걸린 공간에 접근할 수 있는 포인터를 인자로 넘겨줌
	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);


	for (int i = 0; i < 4; ++i)
	{
		D3DXVec4Transform(&pVertices[i].vPosition, &m_vBasePos[i], &_matrix);

	}

	m_pVBuffer->Unlock();
	return S_OK;

}

HRESULT CVIBuffer_ViewPort::Initialize_Prototype()
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	//버텍스 버퍼를 생성
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXTEX_VIEWPORT) * 4, 0, D3DFVF_XYZRHW | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1), D3DPOOL_MANAGED, &m_pVBuffer, nullptr)))
		return E_FAIL;



	VTXTEX_VIEWPORT* pVertices = nullptr;
	//다른 스레드가 접근하지 못하도록 락을 걸고, 락이 걸린 공간에 접근할 수 있는 포인터를 인자로 넘겨줌
	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);

	//버텍스 버퍼에 있는 버텍스 값 셋팅
	m_vBasePos[0] = pVertices[0].vPosition = D3DXVECTOR4(-0.5f, -0.5f, 0.f, 1.f);
	pVertices[0].vTexUV = D3DXVECTOR2(0.f, 0.f);

	m_vBasePos[1] = pVertices[1].vPosition = D3DXVECTOR4(0.5f, -0.5f, 0.f, 1.f);
	pVertices[1].vTexUV = D3DXVECTOR2(1.f, 0.f);

	m_vBasePos[2] = pVertices[2].vPosition = D3DXVECTOR4(0.5f, 0.5f, 0.f, 1.f);
	pVertices[2].vTexUV = D3DXVECTOR2(1.f, 1.f);

	m_vBasePos[3] = pVertices[3].vPosition = D3DXVECTOR4(-0.5f, 0.5f, 0.f, 1.f);
	pVertices[3].vTexUV = D3DXVECTOR2(0.f, 1.f);

	m_pVBuffer->Unlock();

	//인덱스 버퍼를 생성
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(sizeof(_ushort) * 6, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIBuffer, nullptr)))
		return E_FAIL;

	//잠긴 인덱스버퍼에 접근하기 위한 포인터
	_ushort* pIndices = nullptr;
	m_pIBuffer->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_pIBuffer->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_ViewPort::Initialize(void * _pArg)
{
	return S_OK;
}

void CVIBuffer_ViewPort::Free()
{
	Safe_Release(m_pVBuffer);
	Safe_Release(m_pIBuffer);
	CComponent::Free();
}

CComponent * CVIBuffer_ViewPort::Clone_Component(void * _pArg)
{
	CVIBuffer_ViewPort* pInstance = new CVIBuffer_ViewPort(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Fail to clone VIBuffer");
		delete pInstance;
		pInstance = nullptr;
	}
	return pInstance;
}

#include "stdafx.h"
#include "ParticleSystem.h"
#include "Management.h"
#include "PickingMgr.h"
#include <random>

CParticleSystem::CParticleSystem(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CParticleSystem::CParticleSystem(const CParticleSystem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticleSystem::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CParticleSystem::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Ready_ParticleSystem()))
		return E_FAIL;

	if (FAILED(Ready_VIBuffer()))
		return E_FAIL;

	m_pTransformCom->SetUp_Position(m_StateDesc.vPosition);
	m_bStart = false;

	return S_OK;
}

_int CParticleSystem::Update_GameObject(_double _DeltaTime)
{
	if (m_bDead)
		return -1;

	if (FAILED(Kill_Particles()))
		return E_FAIL;


	m_dCurrentDuration += _DeltaTime;
	if (m_StateDesc.dDuration > m_dCurrentDuration)
	{
		switch (m_StateDesc.eType)
		{
		case Client::CParticleSystem::PARTICLE_UP:
			Emit_Particle_UP(_DeltaTime);
			break;
		case Client::CParticleSystem::PARTICLE_SPREAD:
			break;
		case Client::CParticleSystem::PARTICLE_GRAVITY:
			break;
		case Client::CParticleSystem::PARTICLE_END:
			break;
		default:
			break;
		}
	}

	if (m_bStart && End_Check())
 	{
		m_bDead = true;
		return -1;
	}

	if (FAILED(Update_Particles(_DeltaTime)))
		return E_FAIL;

	if (FAILED(Update_VIBuffer()))
		return E_FAIL;

	return NOERROR;
}

_int CParticleSystem::Late_Update_GameObject(_double _DeltaTime)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return -1;

	return NOERROR;
}

HRESULT CParticleSystem::Render_GameObject()
{
	if (nullptr == m_pVBuffer || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(m_StateDesc.iShaderPass);

	if (FAILED(Render_VIBuffer()))
		return E_FAIL;

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CParticleSystem::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Particle", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(m_StateDesc.eTextureScene, m_StateDesc.pTextureKey, L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::Ready_ParticleSystem()
{
	m_ParticleArray = new PARTICLE_INFO[m_StateDesc.iMaxParticleCount];
	ZeroMemory(m_ParticleArray, sizeof(PARTICLE_INFO) * m_StateDesc.iMaxParticleCount);
	if (nullptr == m_ParticleArray)
		return E_FAIL;

	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		m_ParticleArray[i].bActive = false;
	}

	m_iCurrentParticleCount = 0;
	m_dCurrentSpawnTime = 0;
	return S_OK;
}

HRESULT CParticleSystem::Ready_VIBuffer()
{
	// For. Vertices
	m_iStride = sizeof(VTXTEX_PARTICLE);
	m_iNumVertices = m_StateDesc.iMaxParticleCount * 6; // 중복 정점 허용?
	m_iNumPrimitive = m_iNumVertices / 3;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0);

	m_Vertices = new VTXTEX_PARTICLE[m_iNumVertices];
	if (nullptr == m_Vertices)
		return E_FAIL;
	ZeroMemory(m_Vertices, sizeof(VTXTEX_PARTICLE) * m_iNumVertices);

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iStride * m_iNumVertices, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_DEFAULT, &m_pVBuffer, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;
	Safe_AddRef(pManagement);

	matView = pManagement->Get_Transform(D3DTS_VIEW);
	matProj = pManagement->Get_Transform(D3DTS_PROJECTION);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShaderCom->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &matView, sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_matProj", &matProj)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CParticleSystem::Update_Particles(_double _DeltaTime)
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if (m_ParticleArray[i].bActive)
		{
			// 각 파티클들의 업데이트
			switch (m_StateDesc.eType)
			{
			case Client::CParticleSystem::PARTICLE_UP:
				m_ParticleArray[i].dCurrentLifeTime += _DeltaTime;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;
				break;
			case Client::CParticleSystem::PARTICLE_SPREAD:
				break;
			case Client::CParticleSystem::PARTICLE_GRAVITY:
				break;
			case Client::CParticleSystem::PARTICLE_END:
				break;
			default:
				break;
			}
		}
	}
	return S_OK;
}

HRESULT CParticleSystem::Kill_Particles()
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if (true == m_ParticleArray[i].bActive && true == m_StateDesc.dParticleLifeTime <= m_ParticleArray[i].dCurrentLifeTime)
		{
			m_ParticleArray[i].bActive = false;
			--m_iCurrentParticleCount;

			for (int j = i; j < m_StateDesc.iMaxParticleCount - 1; ++j)
			{
				m_ParticleArray[j].bActive = m_ParticleArray[j + 1].bActive;
				m_ParticleArray[j].vPosition = m_ParticleArray[j + 1].vPosition;
				m_ParticleArray[j].vVelocity = m_ParticleArray[j + 1].vVelocity;
				m_ParticleArray[j].dCurrentLifeTime = m_ParticleArray[j + 1].dCurrentLifeTime;
			}
		}
	}

	return S_OK;
}

HRESULT CParticleSystem::Update_VIBuffer()
{
	ZeroMemory(m_Vertices, sizeof(VTXTEX_PARTICLE) * m_iNumVertices);

	// For.Vertices
	VTXTEX_PARTICLE*		pVertices = nullptr;
	int index = 0;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	_matrix InvViewMatrix = {};
	D3DXMatrixInverse(&InvViewMatrix, nullptr, &CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW));
	ZeroMemory((_float3*)InvViewMatrix.m[3], sizeof(_float3));

	for (_int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		/// 오른쪽 위 삼각형
		// Left Top
		_float3 LocalPos = _float3(-0.5f, 0.5f, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexUV = _float2(0.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Right Top
		LocalPos = _float3(0.5f, 0.5f, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexUV = _float2(1.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Right Bottom
		LocalPos = _float3(0.5f, -0.5f, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexUV = _float2(1.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;

		/// 왼쪽 아래 삼각형
		// Left Top
		LocalPos = _float3(-0.5f, 0.5f, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexUV = _float2(0.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Right Bottom
		LocalPos = _float3(0.5f, -0.5f, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexUV = _float2(1.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Left Bottom
		//pVertices[index].vPosition = _float3(m_ParticleArray[i].vPosition.x - m_vParticleScale.x, m_ParticleArray[i].vPosition.y - m_vParticleScale.y, m_ParticleArray[i].vPosition.z);
		LocalPos = _float3(-0.5f, -0.5f, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexUV = _float2(0.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;
	}

	m_pVBuffer->Unlock();

	return S_OK;
}

HRESULT CParticleSystem::Render_VIBuffer()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pVBuffer)
		return E_FAIL;

	// ㅅ그리시위한 버택스 버퍼는 정적버퍼
	// 행렬 테스트 
	// 셋 프리퍼시
	// 셋 스트림
	m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumPrimitive);

	return S_OK;
}

HRESULT CParticleSystem::Emit_Particle_UP(_double _DeltaTime)
{
	m_bStart = true;
	_bool emitParticle = false;
	_bool found = false;
	float positionX, positionY, positionZ, red, green, blue;
	int index, i, j;

	m_dCurrentSpawnTime += _DeltaTime;
	emitParticle = false;

	if (m_dCurrentSpawnTime > (m_StateDesc.dSpawnTime / m_StateDesc.fParticlePerSecond))
	{
		m_dCurrentSpawnTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_iCurrentParticleCount < (m_StateDesc.iMaxParticleCount - 1)))
	{
		m_iCurrentParticleCount++;

		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.x;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.y;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.z;
		_float3 vPosition = _float3(positionX, positionY, positionZ);

		_float3 vVelocity = m_StateDesc.vVelocity * (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.fVelocityDeviation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		_float4 vColor = _float4(red, green, blue, 1.f);

		index = 0;
		found = false;
		while (!found)
		{
			if ((m_ParticleArray[index].bActive == false) || Compute_ViewZ(m_ParticleArray[index].vPosition) < Compute_ViewZ(vPosition))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		i = m_iCurrentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_ParticleArray[i].bActive = m_ParticleArray[j].bActive;
			m_ParticleArray[i].vPosition = m_ParticleArray[j].vPosition;
			m_ParticleArray[i].vVelocity = m_ParticleArray[j].vVelocity;
			m_ParticleArray[i].vColor = m_ParticleArray[j].vColor;
			m_ParticleArray[i].dCurrentLifeTime = m_ParticleArray[j].dCurrentLifeTime;
			i--;
			j--;
		}

		m_ParticleArray[i].bActive = true;
		m_ParticleArray[i].vPosition = vPosition;
		m_ParticleArray[i].vVelocity = vVelocity;
		m_ParticleArray[i].vColor = vColor;
		m_ParticleArray[i].dCurrentLifeTime = 0;
	}

	return S_OK;
}

_float CParticleSystem::Compute_ViewZ(_float3 vWorldPos)
{
	_float3 vCamPosition = CManagement::Get_Instance()->Get_CamPosition();
	_float3	 vDistance = vCamPosition - vWorldPos;
	return D3DXVec3Length(&vDistance);
}

_bool CParticleSystem::End_Check()
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if (m_ParticleArray[i].bActive)
			return false;
	}
	return true;
}

CParticleSystem * CParticleSystem::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CParticleSystem*	pInstance = new CParticleSystem(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CParticleSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticleSystem::Clone_GameObject(void * pArg)
{
	CParticleSystem*	pInstance = new CParticleSystem(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CParticleSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticleSystem::Free()
{
	Safe_Delete_Array(m_ParticleArray);
	Safe_Delete_Array(m_Vertices);

	Safe_Release(m_pVBuffer);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	CGameObject::Free();
}

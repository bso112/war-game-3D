#include "stdafx.h"
#include "..\Headers\Bullet_Trail.h"
#include "Management.h"
#include "PickingMgr.h"

CBullet_Trail::CBullet_Trail(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBullet_Trail::CBullet_Trail(const CBullet_Trail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet_Trail::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Trail::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fAlpha = 1.f;

	_float3 vRight, vUp, vLook;
	_float3 vPosition = (m_StateDesc.vSrc + m_StateDesc.vDst) * 0.5f;

	_float3	 vDir = m_StateDesc.vDst - m_StateDesc.vSrc;
	vUp = _float3(0.f, 1.f, 0.f);

	D3DXVec3Normalize(&vLook, &vDir);
	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	vRight *= m_StateDesc.vScale.x;
	vUp *= m_StateDesc.vScale.y;
	vLook *= D3DXVec3Length(&vDir);

	_matrix StateMatrix;
	D3DXMatrixIdentity(&StateMatrix);
	memcpy(StateMatrix.m[0], &vRight, sizeof(_float3));
	memcpy(StateMatrix.m[1], &vUp, sizeof(_float3));
	memcpy(StateMatrix.m[2], &vLook, sizeof(_float3));
	memcpy(StateMatrix.m[3], &vPosition, sizeof(_float3));

	m_pTransformCom->Set_WorldMatrix(StateMatrix);

	return S_OK;
}

_int CBullet_Trail::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
		return -1;

	if (!m_bActive)
		return S_OK;

	m_fAlpha -= (_float)TimeDelta / m_StateDesc.fLifeTime;
	if (0 > m_fAlpha)
	{
		m_bDead = true;
	}

	return _int();
}

_int CBullet_Trail::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return -1;

	return _int();
}

HRESULT CBullet_Trail::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(2);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CBullet_Trail::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Cube", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_BulletTrail", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Trail::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();

	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();

	m_pShaderCom->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShaderCom->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	m_pShaderCom->Set_Value("g_Alpha", &m_fAlpha, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}


CBullet_Trail * CBullet_Trail::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBullet_Trail*	pInstance = new CBullet_Trail(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBullet_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBullet_Trail::Clone_GameObject(void * pArg)
{
	CBullet_Trail*	pInstance = new CBullet_Trail(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CBullet_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBullet_Trail::Free()
{
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

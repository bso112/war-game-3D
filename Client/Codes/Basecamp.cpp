#include "stdafx.h"
#include "Basecamp.h"

CBasecamp::CBasecamp(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBasecamp::CBasecamp(const CBasecamp & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBasecamp::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBasecamp::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Position(m_StateDesc.vWaypointPos);
	m_pTransformCom->AddForce(m_StateDesc.vOffset);

#pragma region LOOKWAYPOINT
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_float3 vNewRight, vNewUp, vNewLook;
	_float3 vTargetPos = _float3(m_StateDesc.vWaypointPos.x, WorldMatrix.m[3][1], m_StateDesc.vWaypointPos.z);

	vNewLook = vTargetPos - (_float3)WorldMatrix.m[3];
	D3DXVec3Normalize(&vNewLook, &vNewLook);

	D3DXVec3Cross(&vNewRight, &(_float3)WorldMatrix.m[1], &vNewLook);
	D3DXVec3Normalize(&vNewRight, &vNewRight);

	D3DXVec3Cross(&vNewUp, &vNewRight, &vNewLook);
	D3DXVec3Normalize(&vNewUp, &vNewUp);

	memcpy(WorldMatrix.m[0], &(vNewRight * m_StateDesc.vScale.x), sizeof(_float3));
	memcpy(WorldMatrix.m[1], &(vNewUp* m_StateDesc.vScale.y), sizeof(_float3));
	memcpy(WorldMatrix.m[2], &(vNewLook * m_StateDesc.vScale.z), sizeof(_float3));

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
#pragma endregion

	return S_OK;		
}

_int CBasecamp::Update_GameObject(_double TimeDelta)
{
	return _int(0);
}

_int CBasecamp::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (pManagement->Is_InFrustum(m_pTransformCom->Get_WorldPosition(), 30.f))
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;

	Safe_Release(pManagement);

	return _int(0);
}

HRESULT CBasecamp::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(0);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CBasecamp::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Cube", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	CTransform::STATEDESC tTransformDesc;
	tTransformDesc.SpeedPerSec = 5.0;
	tTransformDesc.RotatePerSec = D3DXToRadian(30.f);
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Basecamp", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasecamp::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));


	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}


CBasecamp * CBasecamp::Create_Basecamp_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBasecamp*	pInstance = new CBasecamp(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBasecamp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBasecamp::Clone_GameObject(void * pArg)
{
	CBasecamp*	pInstance = new CBasecamp(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CBasecamp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasecamp::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

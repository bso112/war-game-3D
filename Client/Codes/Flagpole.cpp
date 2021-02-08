#include "stdafx.h"
#include "..\Headers\Flagpole.h"
#include "Flag.h"
#include "PickingMgr.h"

CFlagpole::CFlagpole(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFlagpole::CFlagpole(const CFlagpole & rhs)
	: CGameObject(rhs)
{
}

void CFlagpole::Set_OccupiedRate(_double Rate)
{
	if(nullptr != m_pFlag)
		m_pFlag->Set_OccupiedRate(Rate);
}

HRESULT CFlagpole::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CFlagpole::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPos);

	// For.Layer_Flag
	if (FAILED(Ready_Layer_Flag(L"Layer_Flag")))
		return E_FAIL;

	return S_OK;
}

_int CFlagpole::Update_GameObject(_double TimeDelta)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// Right
	memcpy(&WorldMatrix.m[0], &((_float3)ViewMatrix.m[0] * m_StateDesc.vScale.x), sizeof(_float3));

	// Look
	_float3 vNewLook;
	D3DXVec3Cross(&vNewLook, &(_float3)WorldMatrix.m[0], &(_float3)WorldMatrix.m[1]);
	D3DXVec3Normalize(&vNewLook, &vNewLook);
	memcpy(&WorldMatrix.m[2], &vNewLook, sizeof(_float3));

	// Inverse Matrix
	_matrix InverseParentsMatrix = m_pTransformCom->Get_ParentsMatrixInverse();
	_matrix InverseRevolutionMatrix = m_pTransformCom->Get_RevolutionMatrixInverse();

	WorldMatrix *= InverseParentsMatrix * InverseRevolutionMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	Safe_Release(pManagement);

	return _int(0);
}

_int CFlagpole::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	_float3	vWorldPos = m_pTransformCom->Get_WorldPosition();
	_matrix ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	Compute_ViewZ(_float3(vWorldPos.x, ViewMatrix.m[3][1], vWorldPos.z));

	if (true == pManagement->Is_InFrustum(vWorldPos, m_StateDesc.vScale.x))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
			return -1;
	}

	Safe_Release(pManagement);

	return _int(0);
}

HRESULT CFlagpole::Render_GameObject()
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

HRESULT CFlagpole::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	CTransform::STATEDESC StateDesc;
	StateDesc.SpeedPerSec = 5.0f;
	StateDesc.RotatePerSec = D3DXToRadian(90.0f);
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &StateDesc)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Flagpole", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlagpole::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CFlagpole::Ready_Layer_Flag(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CFlag::STATEDESC StateDesc;
	StateDesc.eFlagID = CFlag::FLAG_RED;
	StateDesc.fMaxHeight = m_StateDesc.vPos.y + m_StateDesc.vScale.y * 0.20f;
	StateDesc.fMinHeight = m_StateDesc.vPos.y - m_StateDesc.vScale.y * 0.15f;
	StateDesc.vScale = _float3(m_StateDesc.vScale.x * 1.5f, m_StateDesc.vScale.x * 1.5f, 1.f);
	StateDesc.vPos = _float3(m_StateDesc.vPos.x, StateDesc.fMaxHeight, m_StateDesc.vPos.z);	// ±Í¥Î ≥Ù¿Ã - ±Íπﬂ ±Ê¿Ã
	m_pFlag = dynamic_cast<CFlag*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Flag", SCENE_STAGE, pLayerTag, &StateDesc));
	if (nullptr == m_pFlag)
		return E_FAIL;

	Safe_AddRef(m_pFlag);

	Safe_Release(pManagement);

	return S_OK;
}

CFlagpole * CFlagpole::Create_Flagpole_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CFlagpole*	pInstance = new CFlagpole(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CFlagpole Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlagpole::Clone_GameObject(void * pArg)
{
	CFlagpole*	pInstance = new CFlagpole(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CFlagpole Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlagpole::Free()
{
	Safe_Release(m_pFlag);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

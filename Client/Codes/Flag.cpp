#include "stdafx.h"
#include "..\Headers\Flag.h"
#include "PickingMgr.h"

CFlag::CFlag(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFlag::CFlag(const CFlag & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlag::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CFlag::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPos);

	m_vOriginalPos = m_StateDesc.vPos;

	return S_OK;
}

_int CFlag::Update_GameObject(_double TimeDelta)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (0x80000000 & Lift_Flag(TimeDelta))
		return -1;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);

	// 카메라 행렬 
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// Right
	memcpy(&WorldMatrix.m[0], &((_float3)ViewMatrix.m[0] * m_StateDesc.vScale.x), sizeof(_float3));

	// Look
	_float3 vNewLook;
	D3DXVec3Cross(&vNewLook, &(_float3)WorldMatrix.m[0], &(_float3)WorldMatrix.m[1]);
	D3DXVec3Normalize(&vNewLook, &vNewLook);
	memcpy(&WorldMatrix.m[2], &vNewLook, sizeof(_float3));

	// Pos
	_float3 vNewPos = _float3(m_vOriginalPos.x - vNewLook.x, WorldMatrix.m[3][1], m_vOriginalPos.z - vNewLook.z);
	memcpy(&WorldMatrix.m[3], &vNewPos, sizeof(_float3));

	// Inverse Matrix
	_matrix InverseParentsMatrix = m_pTransformCom->Get_ParentsMatrixInverse();
	_matrix InverseRevolutionMatrix = m_pTransformCom->Get_RevolutionMatrixInverse();

	WorldMatrix *= InverseParentsMatrix * InverseRevolutionMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	Safe_Release(pManagement);

	return _int(0);
}

_int CFlag::Late_Update_GameObject(_double TimeDelta)
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

	if (true == pManagement->Is_InFrustum(vWorldPos, m_StateDesc.vScale.x * 0.5f))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
			return -1;
	}

	Safe_Release(pManagement);

	return _int(0);
}

HRESULT CFlag::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(8);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CFlag::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Flag", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CFlag::SetUp_ConstantTable()
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

	_bool isBlue = FLAG_BLUE == m_StateDesc.eFlagID ? true : false;
	m_pShaderCom->Set_Bool("g_isBlue", isBlue);

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

_int CFlag::Lift_Flag(_double TimeDelta)
{
	if (m_OldRate == m_OccupiedRate)
		return 0;

	if (0 != m_OccupiedRate)
		int i = 10;

	if (50 <= m_OccupiedRate)
	{
		if (100 == m_OccupiedRate)
			m_StateDesc.eFlagID = FLAG_BLUE;

		// 최고 최저 차이 * 깃발 점령 비율 + 최저 위치
		_float fHeight = ((m_StateDesc.fMaxHeight - m_StateDesc.fMinHeight) * ((_float)m_OccupiedRate - 50.f) / 50.f) + m_StateDesc.fMinHeight;

		_float3 vPos = m_pTransformCom->Get_WorldPosition();
		m_pTransformCom->SetUp_Position(_float3(vPos.x, fHeight, vPos.z));
	}
	else
	{
		if (0 == m_OccupiedRate)
			m_StateDesc.eFlagID = FLAG_RED;

		_float fHeight = ((m_StateDesc.fMaxHeight - m_StateDesc.fMinHeight) * (50.f - (_float)m_OccupiedRate) / 50.f) + m_StateDesc.fMinHeight;

		_float3 vPos = m_pTransformCom->Get_WorldPosition();
		m_pTransformCom->SetUp_Position(_float3(vPos.x, fHeight, vPos.z));
	}

	m_OldRate = m_OccupiedRate;

	return _int(0);
}

CFlag * CFlag::Create_Flag_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CFlag*	pInstance = new CFlag(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CFlag Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlag::Clone_GameObject(void * pArg)
{
	CFlag*	pInstance = new CFlag(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CFlag Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlag::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

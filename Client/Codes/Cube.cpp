#include "stdafx.h"
#include "Cube.h"
#include "Management.h"
#include "PickingMgr.h"

CCube::CCube(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CCube::CCube(const CCube & rhs)
	: CGameObject(rhs)
{

}

HRESULT CCube::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCube::Ready_GameObject(void * pArg)
{
	BASEDESC desc;
	if (nullptr != pArg)
		memcpy(&desc, pArg, sizeof(BASEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Position(desc.vPos);
	m_pTransformCom->SetUp_Scale(desc.vSize);

	return S_OK;
}

_int CCube::Update_GameObject(_double TimeDelta)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement ||
		nullptr == m_pTransformCom)
		return -1;

	if (m_bControl)
	{
		//움직임
		if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_W))
			m_pTransformCom->Go_Straight(TimeDelta);

		if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_A))
			m_pTransformCom->Go_Left(TimeDelta);

		if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_S))
			m_pTransformCom->Go_BackWard(TimeDelta);

		if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_D))
			m_pTransformCom->Go_Right(TimeDelta);

		//회전

		if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_Z))
			m_pTransformCom->Rotation_Axis(_float3(0.f, 1.f, 0.f), TimeDelta);

		if (0x80 & pManagement->Get_DIKeyState((_ubyte)DIKEYBOARD_X))
			m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta);
	}

	return _int(0);
}

_int CCube::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom ||
		nullptr == m_pColliderBoxCom)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (pManagement->Is_InFrustum(m_pTransformCom->Get_Position()))
	{
#ifdef _DEBUG
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return -1;
#endif // _DEBUG

		if (FAILED(CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_BUILDING, this)))
			return -1;
	}

	m_pColliderBoxCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pColliderBoxCom->Render_Collider();
	Safe_Release(pManagement);

	return _int(0);
}

HRESULT CCube::Render_GameObject()
{
#ifdef _DEBUG
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	if (FAILED(m_pColliderBoxCom->Render_Collider()))
		return E_FAIL;
#endif // _DEBUG



	return S_OK;
}

void CCube::OnCollisionEnter(CGameObject * _pOther)
{
	int a = 0;
}

HRESULT CCube::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::STATEDESC tTransformDesc;
	tTransformDesc.SpeedPerSec = 5.0;
	tTransformDesc.RotatePerSec = D3DXToRadian(30.f);
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Component_Texture_Sky", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Cube", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Cube", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Box", L"Com_Collider", (CComponent**)&m_pColliderBoxCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCube::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
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

	Safe_Release(pManagement);

	return S_OK;
}


CCube * CCube::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CCube*	pInstance = new CCube(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCube::Clone_GameObject(void * pArg)
{
	CCube*	pInstance = new CCube(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCube::Free()
{
	Safe_Release(m_pColliderBoxCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}

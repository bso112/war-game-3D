#include "stdafx.h"
#include "BasePlane.h"
#include "PickingMgr.h"

typedef	struct tagRay
{
	D3DXVECTOR3 vOrigin;
	D3DXVECTOR3 vDir;

	tagRay() {};
	tagRay(D3DXVECTOR3 _vOrigin, D3DXVECTOR3 _vDir)
	{
		vOrigin = _vOrigin;
		vDir = _vDir;
	}
}MYRAY;

//마우스 좌표에서부터 레이를 만든다.
MYRAY Make_Ray_InBasePlane(POINT& ptMouse, D3DXMATRIX ProjMatrix, D3DXMATRIX ViewMatrix)
{
	D3DXVECTOR4		vMousePos = D3DXVECTOR4((float)ptMouse.x, (float)ptMouse.y, 0.0f, 0.0f);

	// 2차원 투영 스페이스로 변환한다.(0, 0, g_iWinCX, g_iWinCY -> -1, 1, 1, -1)
	vMousePos.x = vMousePos.x / (g_iToolViewCX * 0.5f) - 1.f;
	vMousePos.y = vMousePos.y / -(g_iToolViewCY * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;


	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	// 뷰스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec4Transform(&vMousePos, &vMousePos, &ProjMatrix);
	D3DXVECTOR3		vMousePivot = D3DXVECTOR3(0.f, 0.f, 0.f);
	D3DXVECTOR3		vMouseRay = D3DXVECTOR3(vMousePos.x, vMousePos.y, vMousePos.z) - vMousePivot;

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// 월드스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec3TransformCoord(&vMousePivot, &vMousePivot, &ViewMatrix);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &ViewMatrix);
	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	return MYRAY(vMousePivot, vMouseRay);
}

CBasePlane::CBasePlane(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBasePlane::CBasePlane(const CBasePlane & rhs)
	: CGameObject(rhs)
{
}

const CBasePlane::WAYPOINTDESC CBasePlane::Get_WaypointDesc()
{
	m_WaypointDesc.isSpecial = m_StateDesc.isSpecial;
	m_WaypointDesc.fRadius = m_StateDesc.vScale.x * 0.5f;
	m_WaypointDesc.vPosition = m_StateDesc.vPos;

	return m_WaypointDesc;
}

const CBasePlane::NATUREDESC CBasePlane::Get_NatureDesc()
{
	m_NatureDesc.iNatureID = m_StateDesc.iNatureID;
	m_NatureDesc.vScale = m_StateDesc.vScale;
	m_NatureDesc.vPos = m_StateDesc.vPos;

	return m_NatureDesc;
}

HRESULT CBasePlane::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBasePlane::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	CPickingMgr::Get_Instance()->Register_Observer(this);

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Rotation(m_StateDesc.vAxis, m_StateDesc.fRadian);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPos);

	return S_OK;
}

_int CBasePlane::Update_GameObject(_double TimeDelta)
{
	return _int(0);
}

_int CBasePlane::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom ||
		nullptr == m_pColliderCom)
		return -1;

	CCollisionMgr::Get_Instance()->Add_CollisionGroup(CCollisionMgr::COL_UNIT, this);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return -1;

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	return _int(0);
}

HRESULT CBasePlane::Render_GameObject()
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

	if (FAILED(m_pColliderCom->Render_Collider()))
		return E_FAIL;

	return S_OK;
}

_bool CBasePlane::Picking(POINT & ptMouse)
{
	if (nullptr == m_pTransformCom)
		return false;

	//레이 생성
	CManagement* pManageMent = CManagement::Get_Instance();
	if (nullptr == pManageMent) return false;
	MYRAY tRay = Make_Ray_InBasePlane(ptMouse, pManageMent->Get_Transform(D3DTS_PROJECTION), pManageMent->Get_Transform(D3DTS_VIEW));

	//레이의 오리진으로 향하는 벡터
	_float3 vToOrigin = tRay.vOrigin - m_pTransformCom->Get_Position();
	//레이의 방향에 투영된 vToOrigin 벡터의 길이 (길이를 구하기 위해선 레이의 방향을 반대로 해야한다)
	_float vProjectedLength = D3DXVec3Dot(&vToOrigin, &(tRay.vDir * -1));

	//수선의 높이
	_float fH = sqrtf(powf(D3DXVec3Length(&vToOrigin), 2) - powf(vProjectedLength, 2));

	//구의 반지름
	_float radius = m_pTransformCom->Get_Scaled().x * 0.5f;
	if (fH <= radius)
		return true;

	return false;
}

HRESULT CBasePlane::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	CTransform::STATEDESC tTransformDesc;
	tTransformDesc.SpeedPerSec = 5.0;
	tTransformDesc.RotatePerSec = D3DXToRadian(90.f);
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &tTransformDesc)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Box", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasePlane::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	Safe_Release(pManagement);

	return S_OK;
}

CBasePlane * CBasePlane::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBasePlane*	pInstance = new CBasePlane(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBasePlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBasePlane::Clone_GameObject(void * pArg)
{
	CBasePlane*	pInstance = new CBasePlane(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CBasePlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBasePlane::Free()
{
	CPickingMgr::Get_Instance()->UnRegister_Observer(this);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

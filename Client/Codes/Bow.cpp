#include "stdafx.h"
#include "..\Headers\Bow.h"
#include "Arrow.h"
#include "PickingMgr.h"
#include "SoundMgr.h"

CBow::CBow(PDIRECT3DDEVICE9 pGraphic_Device)
	: CWeapon(pGraphic_Device)
{
}

CBow::CBow(const CBow & rhs)
	: CWeapon(rhs)
{
	m_fPower_Max = rhs.m_fPower_Max;
	m_fPower_Min = rhs.m_fPower_Min;
	m_fPower_Stack = rhs.m_fPower_Stack;
}

HRESULT CBow::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBow::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	m_StateDesc.iAttack = 5;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->SetUp_Scale(m_StateDesc.vScale);
	m_pTransform->SetUp_Position(m_StateDesc.vPos);

	CKeyMgr::Get_Instance()->RegisterObserver(this);

	if (FAILED(D3DXCreateLine(m_pGraphic_Device, &m_pLine)))
		return E_FAIL;

	m_fPower_Min = 10.f;
	m_fPower_Max = 60.f;
	m_fPower_Stack = 1.f; // 최대값1 기준

	_float3 vUp, vLook, vRight;
	vUp = m_pTransform->Get_WorldState(Engine::CTransform::STATE_UP);
	vLook = -m_pTransform->Get_WorldState(Engine::CTransform::STATE_RIGHT);
	vRight = m_pTransform->Get_WorldState(Engine::CTransform::STATE_LOOK);

	m_pTransform->Set_State(Engine::CTransform::STATE_UP, vUp);
	m_pTransform->Set_State(Engine::CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(Engine::CTransform::STATE_LOOK, vLook);

	return S_OK;
}

_int CBow::Update_GameObject(_double TimeDelta)
{
	if (m_bAiming == 0)
		m_dblHoldingTime = 0.0;
	else
		m_dblHoldingTime += TimeDelta;

	if (m_dblHoldingTime >= m_dblHoldingMax)
		m_dblHoldingTime = 0.0;

	if (true == m_isAttack)
	{
		m_AccTimer += TimeDelta;

		if (0.2 <= m_AccTimer)
		{
			m_isAttack = false;
			m_AccTimer = 0.0;
		}
	}

	if (CONTROL_AI == m_eControlID)
		m_fPower = min(m_fPower + (_float)TimeDelta * 2.f, 2.f);

	return _int(0);
}

_int CBow::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer || nullptr == m_pRenderer)
		return -1;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return -1;

	if (pManagement->Is_InFrustum(m_pTransform->Get_Position()))
	{
		if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
			return -1;
	}

	return _int(0);
}

HRESULT CBow::Render_GameObject()
{
	//////////////////////////////////////궤적
	if (m_bAiming)
	{
		if (FAILED(SetUp_ConstantTable_to_Default_Shader()))
			return E_FAIL;

		m_pDefault_Shader->Begin_Shader();
		m_pDefault_Shader->Begin_Pass(2);//(2)

		m_pCVIBuffer_Aim->Render_VIBuffer();

		m_pDefault_Shader->End_Pass();
		m_pDefault_Shader->End_Shader();

		m_bAiming = 0;
		m_pCVIBuffer_Aim->ClearVector();
	}

	if (nullptr == m_pVIBuffer ||
		nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();
	m_pShader->Begin_Pass(6);

	m_pVIBuffer->Render_VIBuffer();

	if (FAILED(Draw_Line()))
		return E_FAIL;

	m_pShader->End_Pass();
	m_pShader->End_Shader();

	return S_OK;
}

HRESULT CBow::Attack(_double TimeDelta)
{
	if (true == m_isAttack)
		return S_OK;

	// AI 기본 화살 파워
	m_fPower = 1.f;

	if (FAILED(Ready_Layer_Arrow(L"Layer_Arrow")))
		return E_FAIL;

	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	if (nullptr == pSoundMgr) return E_FAIL;
	_tchar* pSoundTags[2] = { L"bow_shoot_2.mp3", L"bow_shoot_4.mp3" };

	pSoundMgr->PlaySound_Overwrite(pSoundTags[rand() % 2], CSoundMgr::CHANNELID::PLAYER);


	m_isAttack = true;

	return S_OK;
}

HRESULT CBow::Attack(_double TimeDelta, _float Power)
{
	if (true == m_isAttack)
		return S_OK;

	// AI 기본 화살 파워
	m_fPower = Power;

	if (FAILED(Ready_Layer_Arrow(L"Layer_Arrow")))
		return E_FAIL;

	m_isAttack = true;

	return S_OK;
}

HRESULT CBow::OnKeyPressing(_int KeyCode)
{
	if (CONTROL_AI == m_eControlID)
		return S_OK;

	if (KeyCode == VK_LBUTTON && false == m_isAttack)
	{
		m_fPower = min(m_fPower + (_float)CManagement::Get_Instance()->Get_TimeDelta(L"Timer_60") * m_fPower_Stack*(m_fPower_Max - m_fPower_Min), m_fPower_Max);
		m_bAiming = 1;
		_float3 vSelfPos = m_pTransform->Get_WorldPosition();
		_float3 vLook = m_pTransform->Get_WorldState(Engine::CTransform::STATE_RIGHT);

		//m_pCVIBuffer_Aim->PushBack(Predict_Position(vSelfPos, vLook, 0.0));
		for (int i = 0; i < m_pCVIBuffer_Aim->Get_m_iVCnt(); ++i)
			m_pCVIBuffer_Aim->PushBack(Predict_Position(vSelfPos, vLook, i * 0.1));
		m_pCVIBuffer_Aim->Update_VBuffer();
		

	}

	return S_OK;
}

HRESULT CBow::OnKeyUp(_int KeyCode)
{
	if (CONTROL_AI == m_eControlID)
		return S_OK;

	// 화살 생성
	if (VK_LBUTTON == KeyCode && false == m_isAttack)
	{
		if (FAILED(Ready_Layer_Arrow(L"Layer_Arrow")))
			return E_FAIL;

		m_isAttack = true;
		m_fPower = m_fPower_Min;
	}

	return S_OK;
}

HRESULT CBow::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Default_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Default", L"Com_Shader_Default", (CComponent**)&m_pDefault_Shader)))
		return E_FAIL;

	// For.Com_VIBuffer_Aim
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Aim", L"Com_VIBuffer_Aim", (CComponent**)&m_pCVIBuffer_Aim)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Bow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBow::Ready_Layer_Arrow(const _tchar * pLayerTag)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CArrow* pArrow = nullptr;

	CArrow::STATEDESC StateDesc;
	StateDesc.vScale = _float3(1.f, 1.f, 1.f);
	StateDesc.vPos = _float3(0.f, 0.f, 0.f);
	StateDesc.iAttack = m_StateDesc.iAttack;
	if (FAILED(pArrow = dynamic_cast<CArrow*>(pManagement->Add_Object_ToLayer(SCENE_STAGE, L"GameObject_Arrow", SCENE_STAGE, pLayerTag, &StateDesc))))
		return E_FAIL;

	_matrix matTransform = m_pTransform->Get_WorldMatrix_Scale1();
	_float3 vRight = matTransform.m[2];
	_float3 vUp = matTransform.m[0];
	_float3 vLook = matTransform.m[1];
	memcpy((_float3*)matTransform.m[0], &vRight, sizeof(_float3));
	memcpy((_float3*)matTransform.m[1], &vUp, sizeof(_float3));
	memcpy((_float3*)matTransform.m[2], &vLook, sizeof(_float3));
	pArrow->Set_WorldMatrix(matTransform);
	//pArrow->Set_Scale(_float3(8.f/6.f, 8.f / 6.f, 8.f / 6.f) * g_fUnitCX);
	//pArrow->Set_Scale(_float3(20.f, 20.f, 20.f) * g_fUnitCX);
	pArrow->Set_Scale(_float3(1.6f, 1.6f, 1.6f) * g_fUnitCX);

	// 부모 행렬
	//_matrix ParentsMatrix = m_pTransform->Get_WorldMatrix_Scale1();
	//pArrow->Set_ParentsMatrix(ParentsMatrix);

	////////////////////////////////

	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	if (nullptr == pSoundMgr) return E_FAIL;
	_tchar* pSoundTags[2] = { L"bow_shoot_2.mp3", L"bow_shoot_4.mp3" };

	pSoundMgr->PlaySound_Overwrite(pSoundTags[rand() % 2], CSoundMgr::CHANNELID::PLAYER);

	///////////////////////////////

	pArrow->Set_Power(m_fPower);
	pArrow->Set_Owner(m_tOwner);

	Safe_Release(pManagement);


	return S_OK;
}

HRESULT CBow::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CPickingMgr* pPickingMgr = CPickingMgr::Get_Instance();
	_float fmDist = pPickingMgr->Get_Blur_mDist();
	_float fMDist = pPickingMgr->Get_Blur_MDist();
	m_pShader->Set_Value("g_fmDist", &fmDist, sizeof(_float));
	m_pShader->Set_Value("g_fMDist", &fMDist, sizeof(_float));
	m_pShader->Set_Value("g_BlurCenter", &_float4(pPickingMgr->Get_Blur_Center(), 1.f), sizeof(_float4));

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pTexture->Set_TextureOnShader(m_pShader, "g_DiffuseTexture");
	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix));
	m_pShader->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pShader->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CBow::SetUp_ConstantTable_to_Default_Shader()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	_matrix matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	//m_pDefault_Shader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMatrix(), sizeof(_matrix));
	m_pDefault_Shader->Set_Value("g_matWorld", &matIdentity, sizeof(_matrix));
	m_pDefault_Shader->Set_Value("g_matView", &pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix));
	m_pDefault_Shader->Set_Value("g_matProj", &pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix));

	_float fTimeRation = m_dblHoldingTime / m_dblHoldingMax;
	m_pDefault_Shader->Set_Value("g_fTimeRation", &fTimeRation, sizeof(_float));

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CBow::Draw_Line()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	// 라인 정점 3개
	_float3 vVertices[] = {
		_float3(-0.05f*m_StateDesc.vScale.z	/* -0.5f*/, m_StateDesc.vScale.y * 0.4f, 0.f) / 6.f * g_fUnitCX ,
		_float3(-0.05f*m_StateDesc.vScale.z +	/* -0.5f **/ (/*1.f + */-(m_fPower - m_fPower_Min) / (m_fPower_Max - m_fPower_Min)/**2.f*/), 0.f, 0.f) / 6.f *g_fUnitCX ,
		_float3(-0.05f*m_StateDesc.vScale.z	/* -0.5f*/, m_StateDesc.vScale.y * -0.4f, 0.f) / 6.f *g_fUnitCX };

	// 월드 행렬(크기 1)
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	_float3 vRight, vUp, vLook, vPos;

	D3DXVec3Normalize(&vRight, &(_float3)WorldMatrix.m[0]);
	D3DXVec3Normalize(&vUp, &(_float3)WorldMatrix.m[1]);
	D3DXVec3Normalize(&vLook, &(_float3)WorldMatrix.m[2]);

	memcpy(WorldMatrix.m[0], &vRight, sizeof(_float3));
	memcpy(WorldMatrix.m[1], &vUp, sizeof(_float3));
	memcpy(WorldMatrix.m[2], &vLook, sizeof(_float3));

	// 라인 두께
	vPos = WorldMatrix.m[3];
	_float3 vCamPos = pManagement->Get_CamPosition();
	_float fLength = D3DXVec3Length(&(vPos - vCamPos));
	m_fWidth = (50.f - fLength) * 0.1f / 6.f * g_fUnitCX;	// 최대거리 50.f
	if (0.f >= m_fWidth)
		m_fWidth = 0.f;

	m_pLine->SetWidth(m_fWidth);

	m_pLine->Begin();
	m_pLine->DrawTransform(vVertices, 3, &(WorldMatrix * pManagement->Get_Transform(D3DTS_VIEW) * pManagement->Get_Transform(D3DTS_PROJECTION)), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	m_pLine->End();

	return S_OK;
}

_float3 CBow::Predict_Position(_float3 vCurPos, _float3 vLook, _double dbleTime)
{
	D3DXVec3Normalize(&vLook, &vLook);
	_float3 vVel = vLook * m_fPower;
	_float3 vV_Hor, vV_Ver;
	_float3 vS_Hor, vS_Ver;
	vV_Ver = _float3(0.f, vVel.y, 0.f);
	vV_Hor = _float3(vVel.x, 0.f, vVel.z);

	vS_Ver = vV_Ver * dbleTime + _float3(0.f, -CPhysicsMgr::Get_Instance()->Get_Gravity() * dbleTime * dbleTime * 0.5f, 0.f);
	vS_Hor = vV_Hor * dbleTime;

	return vCurPos + vS_Hor + vS_Ver;
}

CBow * CBow::Create_Bow_Prototype(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBow* pInstance = new CBow(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBow Prototype");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBow::Clone_GameObject(void * pArg)
{
	CBow* pInstance = new CBow(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CBow Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBow::Free()
{
	CKeyMgr::Get_Instance()->UnRegisterObserver(this);

	Safe_Release(m_pLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pCVIBuffer_Aim);
	Safe_Release(m_pDefault_Shader);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);

	CWeapon::Free();
}

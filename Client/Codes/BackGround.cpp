#include "stdafx.h"
#include "..\Headers\BackGround.h"

CBackGround::CBackGround(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_pTransformCom->SetUp_Scale(m_StateDesc.vScale);
	m_pTransformCom->SetUp_Position(m_StateDesc.vPos);

	return S_OK;
}

_int CBackGround::Update_GameObject(_double TimeDelta)
{
	if (true == m_StateDesc.isMenuColor)
	{
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(g_hWnd, &ptCursor);

		RECT rcBackColor =
		{ _long(m_StateDesc.vPos.x - m_StateDesc.vScale.x * 0.5f),
		  _long(m_StateDesc.vPos.y - m_StateDesc.vScale.y * 0.5f),
		  _long(m_StateDesc.vPos.x + m_StateDesc.vScale.x * 0.5f),
		  _long(m_StateDesc.vPos.y + m_StateDesc.vScale.y * 0.5f) };

		m_hasSelected = PtInRect(&rcBackColor, ptCursor) ? true : false;
	}

	if (SCENE_LOADING == m_StateDesc.eSceneID)
		m_TimeDelta = TimeDelta;

	return _int(0);
}

_int CBackGround::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return -1;

	return _int(0);
}

HRESULT CBackGround::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iPassIndex = m_StateDesc.isMenuColor ? 10 : 2;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CBackGround::Add_Component()
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Rect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(m_StateDesc.eSceneID, m_StateDesc.pTextureTag, L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::SetUp_ConstantTable()
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float3 vPos = (_float3)WorldMatrix.m[3];
	vPos.x -= g_iWinSizeX * 0.5f;
	vPos.y = g_iWinSizeY * 0.5f - vPos.y;
	memcpy(&WorldMatrix.m[3], &vPos, sizeof(_float3));
	m_pShaderCom->Set_Value("g_matWorld", &WorldMatrix, sizeof(_matrix));

	_matrix ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);
	m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_matrix));

	m_pShaderCom->Set_Value("g_matProj", &m_ProjMatrix, sizeof(_matrix));

	if (true == m_StateDesc.isMenuColor)
		m_pShaderCom->Set_Bool("g_isYellow", m_hasSelected);

	if (SCENE_LOADING == m_StateDesc.eSceneID)
	{
		m_FrameNumber += m_TimeDelta * 0.2;

		if (3.0 <= m_FrameNumber)
			m_FrameNumber = 0.0;
	}
	else
		m_FrameNumber = 0.0;

	m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_FrameNumber);

	Safe_Release(pManagement);

	return S_OK;
}

CBackGround * CBackGround::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround*	pInstance = new CBackGround(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBackGround::Clone_GameObject(void * pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Create CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

#include "stdafx.h"
#include "..\Headers\Blur.h"




CBlur::CBlur(PDIRECT3DDEVICE9 pGraphic_Device)
	:CRenderTarget(pGraphic_Device)
{
}

CBlur::CBlur(const CBlur & rhs)
	:CRenderTarget(rhs)
{
}

HRESULT CBlur::Process()
{

	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pTransformCom	||
		nullptr == m_pShaderCom)
		return E_FAIL;


	_float WinSizeX = (_float)m_pTransformCom->Get_Scaled().x;
	_float WinSizeY = (_float)m_pTransformCom->Get_Scaled().y;
	m_pShaderCom->Set_Value("g_fWinCX", &WinSizeX, sizeof(_float));
	m_pShaderCom->Set_Value("g_fWinCY", &WinSizeY, sizeof(_float));


	if (FAILED(m_pVIBufferCom->Set_Transform(m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	//텍스쳐 프로세싱
	if (FAILED(m_pShaderCom->Set_Texture("g_BlurTexture", m_pRawTexture)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin_Shader()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin_Pass(0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->End_Pass()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->End_Shader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlur::Ready_GameObject_Prototype()
{
	
	return S_OK;
}

HRESULT CBlur::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(STATEDESC));

	//Safe_AddRef(m_tDesc.pBlendTexture);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_ViewPort", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Blur", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement) return E_FAIL;

	m_pRawTexture = pManagement->Get_Texture(L"Target_Blur");
	//Safe_AddRef(m_pRawTexture);

	m_pTransformCom->SetUp_Position(m_tDesc.tBaseDesc.vPos);
	m_pTransformCom->SetUp_Scale(m_tDesc.tBaseDesc.vSize);

	
	return S_OK;
}

_int CBlur::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CBlur::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CBlur::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pTransformCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	_float WinSizeX = (_float)m_pTransformCom->Get_Scaled().x;
	_float WinSizeY = (_float)m_pTransformCom->Get_Scaled().y;
	if (FAILED(m_pShaderCom->Set_Value("g_fWinCX", &WinSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fWinCY", &WinSizeY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Texture("g_BlendTexture", m_tDesc.pBlendTexture)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Texture("g_BlurTexture", m_pRawTexture)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Set_Transform(m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin_Shader()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin_Pass(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->End_Pass()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->End_Shader()))
		return E_FAIL;

	return S_OK;
}

CBlur* CBlur::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlur*	pInstance = new CBlur(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CInfantry");
		Safe_Release(pInstance);
	}
	return pInstance;

}

CGameObject * CBlur::Clone_GameObject(void * pArg)
{
	CBlur*	pInstance = new CBlur(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CInfantry");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlur::Free()
{
	//Safe_Release(m_tDesc.pBlendTexture);
	CRenderTarget::Free();
}

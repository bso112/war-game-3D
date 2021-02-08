#include "stdafx.h"
#include "..\Headers\FrameImage.h"
#include "Management.h"

CFrameImage::CFrameImage(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFrameImage::CFrameImage(const CFrameImage & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFrameImage::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CFrameImage::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(_float3(2.f, 2.f, 1.f));
	m_pTransformCom->SetUp_Position(m_StateDesc.vPosition);

	return S_OK;
}

_int CFrameImage::Update_GameObject(_double TimeDelta)
{
	if (m_bDead)
		return -1;

	if (!m_bActive)
		return NOERROR;

	m_fCurrentFrameTime += (_float)TimeDelta;
	if (m_StateDesc.fFrameTime <= m_fCurrentFrameTime)
	{
		m_fCurrentFrameTime = 0.f;
		++m_iCurrentTexture;
		if (m_StateDesc.iTextureNum - 1 <= m_iCurrentTexture)
		{
			m_iCurrentTexture = 0;
		}
	}

	return NOERROR;
}

_int CFrameImage::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return -1;

	return NOERROR;
}

HRESULT CFrameImage::Render_GameObject()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(m_StateDesc.iShaderPass);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CFrameImage::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(m_StateDesc.eTextureScene, m_StateDesc.pTextureKey, L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrameImage::SetUp_ConstantTable()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	
	_matrix World, iden;
	World = m_pTransformCom->Get_WorldMatrix();
	D3DXMatrixIdentity(&iden);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &iden, sizeof(_matrix))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &iden, sizeof(_matrix))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_TextureOnShader(m_pShaderCom, "g_DiffuseTexture", m_iCurrentTexture)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}


CFrameImage * CFrameImage::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CFrameImage*	pInstance = new CFrameImage(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CFrameImage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFrameImage::Clone_GameObject(void * pArg)
{
	CFrameImage*	pInstance = new CFrameImage(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CFrameImage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFrameImage::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}

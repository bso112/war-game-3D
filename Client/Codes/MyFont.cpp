#include "stdafx.h"
#include "..\Headers\MyFont.h"

USING(Client)

CMyFont::CMyFont(PDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMyFont::CMyFont(const CMyFont & _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CMyFont::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CMyFont::Ready_GameObject(void * _pArg)
{
	if (nullptr != _pArg)
		memcpy(&m_StateDesc, _pArg, sizeof(STATEDESC));

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &m_StateDesc.FontDesc, &m_pFont)))
		return E_FAIL;

	if (FAILED(D3DXCreateSprite(m_pGraphic_Device, &m_pSprite)))
		return E_FAIL;

	m_pTransform->SetUp_Scale(m_StateDesc.vMultiScale);
	m_pTransform->SetUp_Position(m_StateDesc.vPosition);

	return S_OK;
}

_int CMyFont::Update_GameObject(_double _TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (m_bDead)
		return -1;

	return _int();
}

_int CMyFont::Late_Update_GameObject(_double _TimeDelta)
{
	if (!m_bActive)
		return S_OK;

	if (nullptr == m_pRenderer)
		return -1;
	
	if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return -1;
	
	return NOERROR;
}

HRESULT CMyFont::Render_GameObject()
{
	m_pSprite->SetTransform(&m_pTransform->Get_WorldMatrix());

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, m_StateDesc.pStringBuf, lstrlen(m_StateDesc.pStringBuf), nullptr, DT_CENTER, D3DXCOLOR(m_StateDesc.vColor.x, m_StateDesc.vColor.y, m_StateDesc.vColor.z, m_StateDesc.fAlpha));

	m_pSprite->End();
	return S_OK;
}

HRESULT CMyFont::Set_Text(const _tchar * pText)
{
	lstrcpy(m_StateDesc.pStringBuf, pText);
	return S_OK;
}

HRESULT CMyFont::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	return S_OK;
}

CMyFont * CMyFont::Create(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CMyFont*	pInstance = new CMyFont(pGraphic_Device);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CMyFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMyFont::Clone_GameObject(void * _pArg)
{
	CMyFont*	pInstance = new CMyFont(*this);
	if (FAILED(pInstance->Ready_GameObject(_pArg)))
	{
		MSG_BOX("Failed To Clone CMyFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyFont::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	CGameObject::Free();
}

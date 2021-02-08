#pragma once
#include "UI.h"
#include "Management.h"

BEGIN(Client)

class CMyImage : public CUI
{
public:
	typedef struct tagStateDesc
	{
		BASEDESC				m_tBaseDesc;
		//키매니저에 등록해야하는데 버튼은 어느씬에든 있을 수 있으니까 인자로 받아야함.
		SCENEID					m_eSceneID;
		const _tchar*			m_pTextureTag = L"";
		_uint					m_iTextureSceneID = SCENE_END;
		_uint					m_iTextureID = 0;
		_uint					m_iShaderPass = 0;
		_uint					m_iDepth = 0;
	}STATEDESC;

protected:
	explicit CMyImage(PDIRECT3DDEVICE9 _pGraphic_Device) :CUI(_pGraphic_Device) {};
	explicit CMyImage(CMyImage& _rhs);
	virtual ~CMyImage() = default;


protected:
	CVIBuffer_ViewPort*			m_pVIBuffer = nullptr;
	CTransform*			m_pTransform = nullptr;
	CTexture*			m_pTexture = nullptr;
	CShader*			m_pShader = nullptr;
	CRenderer*			m_pRenderer = nullptr;

protected:
	STATEDESC		m_tDesc;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	static CMyImage*	Create(PDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};

END
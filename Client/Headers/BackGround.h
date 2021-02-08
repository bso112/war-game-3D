#pragma once
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CBackGround final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		SCENEID			eSceneID = SCENE_END;
		const _tchar*	pTextureTag = L"";
		_float3			vScale;
		_float3			vPos;
		_bool			isMenuColor = false;
	}STATEDESC;

private:
	explicit CBackGround(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	STATEDESC			m_StateDesc;
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pTextureCom2 = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;	

private:
	_matrix		m_ProjMatrix;
	_bool		m_hasSelected = false;
	_double		m_FrameNumber = 0.0;
	_double		m_TimeDelta = 0.0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBackGround* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
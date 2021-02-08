#pragma once
#include "GameObject.h"


#include "Management.h"
#include "Client_Defines.h"


BEGIN(Client)

class CImage3D : public CGameObject
{
public:
	typedef struct tagStatedesc
	{
		BASEDESC			tBaseDesc;
		const _tchar*		pName = nullptr;
		const _tchar*		pTextureTag = nullptr;
		SCENEID				eTextureSceneID = SCENE_END;
		_uint				iTextureID = 0;
		_uint				iShaderPass = 0;
		//컬러렌더링시에 컬러
		_float4				TextureColor;
		_bool				bBillboard = false;
		_double				dLifeTime = 0.0;

	}STATEDESC;
protected:
	explicit CImage3D(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CImage3D(const CImage3D& rhs);
	virtual ~CImage3D() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();


private:

	CTransform*	m_pTransform = nullptr;
	CVIBuffer*	m_pVIBuffer = nullptr;
	CTexture*	m_pTexture = nullptr;
	CRenderer*	m_pRenderer = nullptr;
	CShader*	m_pShader = nullptr;

private:
	_double		m_dDeadTimer = 0.0;



private:
	STATEDESC	m_tDesc;
public:
	static CImage3D* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();


};
END

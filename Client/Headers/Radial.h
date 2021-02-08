#pragma once
#include "RenderTarget.h"
BEGIN(Client)
class CRadial : public CRenderTarget
{
public:
	typedef struct tagSTATEDESC
	{
		BASEDESC			tBaseDesc;
		LPDIRECT3DTEXTURE9	pBlurTexture = nullptr;
	}STATEDESC;
protected:
	explicit CRadial(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CRadial(const CRadial& rhs);
	virtual ~CRadial() = default;

public:
	virtual HRESULT Process();

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	STATEDESC		m_tDesc;

public:
	static CRadial* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	void	Free() override;
};
END

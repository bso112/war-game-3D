#pragma once
#include "RenderTarget.h"
BEGIN(Client)
class CBlend : public CRenderTarget
{
public:
	typedef struct tagSTATEDESC
	{
		BASEDESC			tBaseDesc;
		LPDIRECT3DTEXTURE9	pBlendTexture = nullptr;
	}STATEDESC;
protected:
	explicit CBlend(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBlend(const CBlend& rhs);
	virtual ~CBlend() = default;


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	STATEDESC	m_tDesc;

public:
	static CBlend* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	void	Free() override;



};
END

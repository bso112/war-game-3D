#pragma once
#include "Tool_Defines.h"
#include "Management.h"

BEGIN(Tool)

class CTerrain final : public CGameObject
{
private:
	explicit CTerrain(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool	Get_TerrainPos(_float3 _vWolrdPos, _float3& vTerrainPos);
	_bool	Get_TerrainPos(POINT& ViewPortPt, _float3* vTerrainPos);
	_bool	Get_TerrainNormal(_float3 _vWolrdPos, _float3& vTerrainNor);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;

public:
	static CTerrain* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
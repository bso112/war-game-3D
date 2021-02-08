#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

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
	void	Chunk_Culling();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureMagicCom = nullptr;
	CTexture*				m_pTextureCircleCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	
	_bool					m_bWireframe = 0;
	_bool					m_bBrushOn = false;
	SKILL					m_eSkill;

public:
	void Set_Skill(SKILL _eSkill) { m_eSkill = _eSkill; }
	void Set_BrushOn(_bool _On) { m_bBrushOn = _On; }

public:
	static CTerrain* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
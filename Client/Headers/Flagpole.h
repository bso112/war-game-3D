#pragma once
#include "Client_Defines.h"
#include "Management.h"

BEGIN(Client)

class CFlag;
class CFlagpole final : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		_float3		vScale;
		_float3		vPos;
	}STATEDESC;

private:
	explicit CFlagpole(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFlagpole(const CFlagpole& rhs);
	virtual ~CFlagpole() = default;

public:
	void Set_ParentsMatrix(_matrix ParentsMatrix) { m_pTransformCom->Set_ParentsMatrix(ParentsMatrix); }
	void Set_OccupiedRate(_double Rate);

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
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	_float3				m_vOriginalPos;

private:
	CFlag*				m_pFlag = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Layer_Flag(const _tchar* pLayerTag);

public:
	static CFlagpole* Create_Flagpole_Prototype(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END
#pragma once
#include "Particle.h"

BEGIN(Client)

class CParticleSystem final : public CGameObject
{
public:
	enum PARTICLE_TYPE { PARTICLE_UP, PARTICLE_SPREAD, PARTICLE_GRAVITY, PARTICLE_END };

public:
	typedef struct tagStateDesc
	{
		PARTICLE_TYPE	eType;
		// Particle TextureInfo
		SCENEID			eTextureScene;
		const _tchar*		pTextureKey;
		_uint				iShaderPass;

		// System CreateInfo
		_bool				bActive;
		_double			dDuration;

		// Particle Info
		_float2				vParticleScale;
		_float3				vPosition;
		_float3				vVelocity;
		_float				fVelocityDeviation;


		// Particles Info
		_int				iMaxParticleCount;
		_double			dParticleLifeTime;

		// System SpawnInfo
		_double			dSpawnTime;
		_float				fParticlePerSecond;
		_float3				vParticleDeviation; // 파티클 편차
	}STATEDESC;

private:
	typedef struct tagParticleInfo
	{
		// Particle Info
		_bool			bActive;

		_float3			vVelocity;
		_float3			vPosition;
		_float4			vColor;

		_double		dCurrentLifeTime;
	}PARTICLE_INFO;

private:
	CTransform*			m_pTransformCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

private:
	STATEDESC		m_StateDesc;
	PARTICLE_TYPE	m_eType = PARTICLE_END;

private:
	_bool			m_bStart = false;
	_double		m_dCurrentDuration = 0;
	_double		m_dCurrentSpawnTime = 0;
	_int			m_iCurrentParticleCount = 0;

private:
	PARTICLE_INFO*			m_ParticleArray = nullptr;		// 파티클의 정보들
	VTXTEX_PARTICLE*		m_Vertices = nullptr;			// 정점들의 정보들

protected: // For.VertexBuffer
	PDIRECT3DVERTEXBUFFER9		m_pVBuffer = nullptr;
	_uint			m_iStride = 0;
	_uint			m_iNumVertices = 0;
	_uint			m_iNumPrimitive = 0;
	_ulong		m_dwFVF = 0;

private:
	explicit CParticleSystem(PDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CParticleSystem(const CParticleSystem& _other);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* _pArg);
	virtual _int Update_GameObject(_double _DeltaTime);
	virtual _int Late_Update_GameObject(_double _DeltaTime);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT Ready_ParticleSystem();
	HRESULT Ready_VIBuffer();
	HRESULT SetUp_ConstantTable();
	HRESULT Update_Particles(_double _DeltaTime);
	HRESULT Kill_Particles();
	HRESULT Update_VIBuffer();
	HRESULT Render_VIBuffer();

public:
	HRESULT Emit_Particle_UP(_double _DeltaTime);

private:
	_float Compute_ViewZ(_float3 vWorldPos);
	_bool End_Check();

public:
	static CParticleSystem* Create(PDIRECT3DDEVICE9 _pGraphicDevice);
	virtual CGameObject* Clone_GameObject(void* _pArg);
	virtual void Free();
};

END
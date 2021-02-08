#pragma once
#include "GameObject.h"
#include "Management.h"
#include "Client_Defines.h"
#include "Infantry.h"
#include "Basecamp.h"

#define	SPAWN_SPEED	5.0
BEGIN(Client)

class CFlagpole;
class CFlag;
class CWaypoint : public CGameObject
{
public:
	typedef	struct tagStateDesc
	{
		WAYPOINT	tWaypoint;
	}STATEDESC;

protected:
	explicit CWaypoint(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CWaypoint(const CWaypoint& rhs);
	virtual ~CWaypoint() = default;

public:
	const _float3 Get_Pos() const { return m_tDesc.tWaypoint.vPosition; }
	const _float3 Get_BasecampPos() const { return m_pBasecamp->Get_WorldPos(); }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	WAYPOINT	Get_Waypoint() { return m_tDesc.tWaypoint; }
	void		Occupy(_double dAccupyRate);
	_bool		Get_Occupy() { return m_tDesc.tWaypoint.bOccupied; }
	HRESULT		Spawn_Ally(_float3 _vSpawnPos, _uint iUnitOccupation);
	_bool		IsCommanderInWaypoint();

protected:
	void		OnCommanderInWaypoint();
protected:
	virtual void OnSetActive(_bool bActive) override;
private:
	HRESULT		Spawn_Enemy(_float3 _vSpawnPos);

private: 
	void		OnOccupy();
	//현재 이 웨이포인트에서 생성된 유닛 중 살아남아있는 유닛의 수를 가져온다.
	size_t		Get_CurrEnemyCnt();
	CTransform*	m_pTransform = nullptr;
	CVIBuffer*	m_pVIBuffer = nullptr;
	CTexture*	m_pTexture = nullptr;
	CRenderer*	m_pRenderer = nullptr;
	CShader*	m_pShader = nullptr;

private:
	STATEDESC				m_tDesc;

private:
	_double					m_dSpawnEnemyTimer = FLT_MAX;
	//생성되는 유닛들의 종류
	vector<CInfantry::OCCUPATION>	m_UnitOccs;
	//생성할 유닛의 인덱스
	_uint					m_iSpawnUnitIndex = 0;
	//현재 생성한 유닛의 수
	size_t					m_iSpawnUnitCnt = 0;

private:	// Flag
	CFlagpole*	m_pFlagpole = nullptr;
	_double		m_dOccupyRate = 0.0;	// 0 > 적군, 100 > 아군
	CFlag*		m_pFlag = nullptr;

private:
	CBasecamp*	m_pBasecamp = nullptr;

private:
	HRESULT Ready_Layer_Flagpole(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Flag(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Basecamp(const _tchar* pLayerTag);
	HRESULT	Ready_Setting();

public:
	static CWaypoint* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();


};
END

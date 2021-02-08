#pragma once
#include "Base.h"

BEGIN(Client)
class CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)
public:
	enum CHANNELID { BGM, SKILL ,PLAYER, WEAPON, PLAYER_VOICE, MONSTER, BOSS, EFFECT, UI, MAXCHANNEL };
private:
	explicit CSoundMgr() {};
	virtual ~CSoundMgr() = default;

public:
	void Initialize();

public:
	void PlaySound_Overwrite(TCHAR* pSoundKey, CHANNELID eID);
	void PlaySound(TCHAR * pSoundKey, CHANNELID eID);
	void PlayBGM(TCHAR* pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();
	void Set_Volum(CHANNELID eID, _float fVolume);

private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;


	// CBase을(를) 통해 상속됨
	virtual void Free() override;

};
END

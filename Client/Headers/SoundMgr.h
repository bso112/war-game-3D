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
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;


	// CBase��(��) ���� ��ӵ�
	virtual void Free() override;

};
END

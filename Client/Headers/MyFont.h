#pragma once
#include "GameObject.h"
#include "Component_Manager.h"

BEGIN(Client)

class CMyFont final : public CGameObject
{
public:
	enum FONT_TYPE { FONT_NORMAL, FONT_UI, FONT_END };

	typedef struct tagFontDesc {
		D3DXFONT_DESC	FontDesc;
		FONT_TYPE			eType;
		_tchar					pStringBuf[256];
		_float3					vPosition;
		_float3					vMultiScale;
		_float4					vColor;
		_float					fAlpha;
		RECT					tRect;
		_int					iDepth;
	}STATEDESC;

private:
	LPD3DXFONT		m_pFont = nullptr;
	LPD3DXSPRITE	m_pSprite = nullptr;

	CRenderer*		m_pRenderer = nullptr;
	CTransform*		m_pTransform = nullptr;

	STATEDESC		m_StateDesc;

private:
	explicit CMyFont(PDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMyFont(const CMyFont& _rhs);
	virtual ~CMyFont() = default;

public:
	virtual _int Get_Depth() { return m_StateDesc.iDepth; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* _pArg);
	virtual _int Update_GameObject(_double _TimeDelta);
	virtual _int Late_Update_GameObject(_double _TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	HRESULT Set_Text(const _tchar* pText);

private:
	HRESULT Add_Component(); // 현재 객체에게 필요한 모듈을 가져와서 멤버변수에 보관한다.

public:
	static CMyFont* Create(PDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* _pArg);
	virtual void Free();
};

END
#pragma once
#include "UI_Button_Class.h"
#include "UI_Button_Skill.h"
#include "UI_CrossHair.h"
#include "UI_Frame.h"
#include "UI_HpBar.h"
#include "UI_Icon.h"
#include "UI_Map.h"
#include "UI_Massage.h"
#include "MyFont.h"

BEGIN(Client)

class CUI_Manager : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	enum UI_STATE { UI_FREE, UI_COMMAND, UI_BATTLE, UI_SKILL, UI_NONE, UI_END };
private:
	list<CUI_Button*>							m_pButtons;
	typedef list<CUI_Button*>					BUTTONS;

	map<const _tchar*, CMyFont*>				m_pFonts;
	typedef map<const _tchar*, CMyFont*>		FONTS;

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	map<const _tchar*, CMyFont*>* Get_FontMap() { return &m_pFonts; }

public:
	HRESULT Change_UI_State(UI_STATE _eState);
	_int Update_UI_Manager();

public:
	HRESULT Add_Button(CUI_Button* _pUI);
	_bool Check_In_Mouse_Button();

public:
	HRESULT Add_Font(const _tchar* _pFontTag, CMyFont* _pFont);
	CMyFont* Find_Font(const _tchar* _pFontTag);
	HRESULT Set_Font(const _tchar* _pFontTag, const _tchar* _pText);

public:
	virtual void Free() override;
};

END

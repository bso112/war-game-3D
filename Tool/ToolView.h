#pragma once
#include "Tool_Defines.h"

// CToolView ���Դϴ�.

namespace Tool
{
	class CInfantry;
	class CCube;
	class CCastle;
	class CBasePlane;
}

class CToolView : public CView
{
	DECLARE_DYNCREATE(CToolView)

protected:
	CToolView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CToolView();

public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	CInfantry*	m_pInfantry = nullptr;
	CCube*		m_pCube = nullptr;
	CCastle*	m_pCastle = nullptr;
	CBasePlane*	m_pBasePlane = nullptr;

private:
	HRESULT Ready_Window_Size();
	HRESULT Ready_Layer_Infantry(const wchar_t* pLayerTag);
	HRESULT Ready_Layer_Cube(const wchar_t* pLayerTag);
	HRESULT Ready_Layer_Castle(const wchar_t* pLayerTag);
	HRESULT Ready_Layer_Waypoint(const wchar_t* pLayerTag);
	HRESULT Ready_Layer_Nature(const wchar_t* pLayerTag);
	HRESULT Erase_CurInfantry();
	HRESULT Erase_CurCube();
	HRESULT Erase_PickedInfantry();
	HRESULT Erase_PickedCube();
	HRESULT Erase_PickedCastle();
	HRESULT Erase_PickedBasePlane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
};



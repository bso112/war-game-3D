#pragma once
#include "afxwin.h" 
#include "Tool_Defines.h"


// CControlView 폼 뷰입니다.

class CControlView : public CFormView
{
	DECLARE_DYNCREATE(CControlView)

protected:
	CControlView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CControlView();

public:
	const Tool::OBJECTID Get_ObjectID() const { return m_eObjectID; }
	const D3DXVECTOR3 Get_Scale() const { return D3DXVECTOR3((float)m_iUnitCX, (float)m_iUnitCY, (float)m_iUnitCZ); }
	const FLOAT Get_Height() const { return m_fHeight; }
	const INT Get_UnitOccupation() const;
	const INT Get_ArcherType() const;
	const INT Get_NatureType() const;
	const D3DXVECTOR3 Get_Axis() const { return D3DXVECTOR3(m_fAxisX, m_fAxisY, m_fAxisZ); }
	const FLOAT Get_Radian() { return D3DXToRadian(m_fAngle); }

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTROLVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	Tool::OBJECTID	m_eObjectID = OBJ_END;
	CComboBox		m_ComboBox;
	CButton			m_SelectOcc[3];
	CButton			m_SelectType[2];
	CButton			m_SelectNature[2];
	unsigned int	m_iUnitCX;
	unsigned int	m_iUnitCY;
	unsigned int	m_iUnitCZ;
	float			m_fHeight;
	float			m_fAxisX;
	float			m_fAxisY;
	float			m_fAxisZ;
	float			m_fAngle;

private:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedButtonUnit();
	afx_msg void OnBnClickedButtonCollider();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonSaveInfantry();
	afx_msg void OnBnClickedButtonLoadInfantry();
	afx_msg void OnBnClickedButtonSaveCollider();
	afx_msg void OnBnClickedButtonLoadCollider();
	afx_msg void OnBnClickedButtonSaveWaypoint();
	afx_msg void OnBnClickedButtonLoadWaypoint();
	afx_msg void OnBnClickedButtonSaveNature();
	afx_msg void OnBnClickedButtonLoadNature();
};

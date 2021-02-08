// ControlView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ControlView.h"

// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)
#include "Management.h"
#include "Infantry.h"
#include "Cube.h"
#include "Castle.h"
#include "BasePlane.h"

CControlView::CControlView()
	: CFormView(IDD_CONTROLVIEW)
	, m_iUnitCX(0)
	, m_iUnitCY(0)
	, m_iUnitCZ(0)
	, m_fHeight(0)
	, m_fAxisX(0)
	, m_fAxisY(0)
	, m_fAxisZ(0)
	, m_fAngle(0)
{
}

CControlView::~CControlView()
{
}

const INT CControlView::Get_UnitOccupation() const
{
	for (size_t i = 0; i < 3; i++)
		if (1 == m_SelectOcc[i].GetCheck())
			return i;

	return -1;
}

const INT CControlView::Get_ArcherType() const
{
	for (size_t i = 0; i < 2; i++)
		if (1 == m_SelectType[i].GetCheck())
			return i;

	return -1;
}

const INT CControlView::Get_NatureType() const
{
	for (size_t i = 0; i < 2; i++)
		if (1 == m_SelectNature[i].GetCheck())
			return i;

	return -1;
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_iUnitCX);
	DDX_Text(pDX, IDC_EDIT2, m_iUnitCY);
	DDX_Text(pDX, IDC_EDIT3, m_iUnitCZ);
	DDX_Text(pDX, IDC_EDIT4, m_fHeight);
	DDX_Text(pDX, IDC_EDIT5, m_fAxisX);
	DDX_Text(pDX, IDC_EDIT6, m_fAxisY);
	DDX_Text(pDX, IDC_EDIT7, m_fAxisZ);
	DDX_Text(pDX, IDC_EDIT8, m_fAngle);
	DDX_Control(pDX, IDC_RADIO1, m_SelectOcc[0]);
	DDX_Control(pDX, IDC_RADIO2, m_SelectOcc[1]);
	DDX_Control(pDX, IDC_RADIO3, m_SelectOcc[2]);
	DDX_Control(pDX, IDC_RADIO4, m_SelectType[0]);
	DDX_Control(pDX, IDC_RADIO5, m_SelectType[1]);
	DDX_Control(pDX, IDC_RADIO6, m_SelectNature[0]);
	DDX_Control(pDX, IDC_RADIO7, m_SelectNature[1]);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
}

BEGIN_MESSAGE_MAP(CControlView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CControlView::OnBnClickedButtonUnit)
	ON_BN_CLICKED(IDC_BUTTON2, &CControlView::OnBnClickedButtonCollider)
	ON_BN_CLICKED(IDC_BUTTON3, &CControlView::OnBnClickedButtonSaveInfantry)
	ON_BN_CLICKED(IDC_BUTTON4, &CControlView::OnBnClickedButtonLoadInfantry)
	ON_BN_CLICKED(IDC_BUTTON5, &CControlView::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON6, &CControlView::OnBnClickedButtonSaveCollider)
	ON_BN_CLICKED(IDC_BUTTON7, &CControlView::OnBnClickedButtonLoadCollider)
	ON_BN_CLICKED(IDC_BUTTON8, &CControlView::OnBnClickedButtonSaveWaypoint)
	ON_BN_CLICKED(IDC_BUTTON9, &CControlView::OnBnClickedButtonLoadWaypoint)
	ON_BN_CLICKED(IDC_BUTTON10, &CControlView::OnBnClickedButtonSaveNature)
	ON_BN_CLICKED(IDC_BUTTON11, &CControlView::OnBnClickedButtonLoadNature)
END_MESSAGE_MAP()


// CControlView 진단입니다.

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlView 메시지 처리기입니다.

void CControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// swprintf 실수형 지원, wsprintf 정수형 지원
	_tchar szString[MAX_PATH] = L"";

	wsprintf(szString, L"Infantry");
	m_ComboBox.AddString(szString);

	wsprintf(szString, L"Collider");
	m_ComboBox.AddString(szString);

	wsprintf(szString, L"Castle");
	m_ComboBox.AddString(szString);

	wsprintf(szString, L"WayPoint");
	m_ComboBox.AddString(szString);

	wsprintf(szString, L"Nature");
	m_ComboBox.AddString(szString);

	m_ComboBox.SetCurSel(OBJ_INFANTRY);
	m_eObjectID = OBJ_INFANTRY;
	m_SelectType[0].SetCheck(1);
}

void CControlView::OnBnClickedButtonUnit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ComboBox.SetCurSel(OBJ_INFANTRY);
	m_eObjectID = (Tool::OBJECTID)m_ComboBox.GetCurSel();
}

void CControlView::OnBnClickedButtonCollider()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ComboBox.SetCurSel(OBJ_CUBE);
	m_eObjectID = (Tool::OBJECTID)m_ComboBox.GetCurSel();
}

void CControlView::OnBnClickedButtonUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE); // TRUE == 정보 불러오기, FALSE == 정보 내보내기

	m_eObjectID = (Tool::OBJECTID)m_ComboBox.GetCurSel();
}

void CControlView::OnBnClickedButtonSaveInfantry()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Infantry");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		// Save List Size
		_uint iSize = plistClone->size();
		WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

		// Save Infantry
		for (auto& pInfantry : *plistClone)
			WriteFile(hFile, &dynamic_cast<CInfantry*>(pInfantry)->Get_InfantryDesc(), sizeof(CInfantry::INFANTRYDESC), &dwByte, nullptr);

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonLoadInfantry()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		CInfantry::INFANTRYDESC InfantryDesc;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		// Load List Size
		_uint iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

		// Load Infantry StateDesc
		for (size_t i = 0; i < iSize; i++)
		{
			ReadFile(hFile, &InfantryDesc, sizeof(CInfantry::INFANTRYDESC), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Infantry", SCENE_STATIC, L"Layer_Infantry", &InfantryDesc))
				return;
		}

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonSaveCollider()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Cube");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		for (auto& pCube : *plistClone)
			WriteFile(hFile, &dynamic_cast<CCube*>(pCube)->Get_WorldMatrix(), sizeof(_matrix), &dwByte, nullptr);

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonLoadCollider()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CCube* pCube = nullptr;
		_matrix WorldMatrix;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		while (true)
		{
			ReadFile(hFile, &WorldMatrix, sizeof(_matrix), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			pCube = dynamic_cast<CCube*>(pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_Cube", SCENE_STATIC, L"Layer_Cube"));
			if (nullptr == pCube)
				return;

			pCube->Set_WorldMatrix(WorldMatrix);
		}

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonSaveWaypoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Waypoint");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		for (auto& pWaypoint : *plistClone)
			WriteFile(hFile, &dynamic_cast<CBasePlane*>(pWaypoint)->Get_WaypointDesc(), sizeof(CBasePlane::WAYPOINTDESC), &dwByte, nullptr);

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonLoadWaypoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CBasePlane::WAYPOINTDESC WaypointDesc;
		CBasePlane::STATEDESC StateDesc;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		while (true)
		{
			ReadFile(hFile, &WaypointDesc, sizeof(CBasePlane::WAYPOINTDESC), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			StateDesc.isSpecial = WaypointDesc.isSpecial;
			_float fRadius = WaypointDesc.fRadius;
			StateDesc.vScale = _float3(fRadius * 2.f, fRadius * 2.f, fRadius * 2.f);
			StateDesc.vPos = WaypointDesc.vPosition;
			if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BasePlane", SCENE_STATIC, L"Layer_Waypoint", &StateDesc))
				return;
		}

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonSaveNature()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pManagement->Get_ObjectList(SCENE_STATIC, L"Layer_Nature");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		for (auto& pNature : *plistClone)
			WriteFile(hFile, &dynamic_cast<CBasePlane*>(pNature)->Get_NatureDesc(), sizeof(CBasePlane::NATUREDESC), &dwByte, nullptr);

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

void CControlView::OnBnClickedButtonLoadNature()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CBasePlane::NATUREDESC NatureDesc;
		CBasePlane::STATEDESC StateDesc;

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		while (true)
		{
			ReadFile(hFile, &NatureDesc, sizeof(CBasePlane::NATUREDESC), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			StateDesc.iNatureID = NatureDesc.iNatureID;
			StateDesc.vScale = NatureDesc.vScale;
			StateDesc.vPos = NatureDesc.vPos;
			if (nullptr == pManagement->Add_Object_ToLayer(SCENE_STATIC, L"GameObject_BasePlane", SCENE_STATIC, L"Layer_Nature", &StateDesc))
				return;
		}

		CloseHandle(hFile);
	}

	Safe_Release(pManagement);
}

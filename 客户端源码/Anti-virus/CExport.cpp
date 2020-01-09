// CExport.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CExport.h"
#include "afxdialogex.h"
#include "Tools.h"
#include <atlconv.h>


// CExport 对话框

IMPLEMENT_DYNAMIC(CExport, CDialogEx)

CExport::CExport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXPORT, pParent)
	, m_ExportFOA(_T(""))
	, m_Characteristics(_T(""))
	, m_Base(_T(""))
	, m_Name(_T(""))
	, m_NameString(_T(""))
	, m_NumOfFun(_T(""))
	, m_NumOfName(_T(""))
	, m_AddrOfFun(_T(""))
	, m_AddrOfName(_T(""))
	, m_AddrOfNameOrd(_T(""))
{

}

CExport::~CExport()
{
}

void CExport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT5, m_ExportFOA);
	DDX_Text(pDX, IDC_EDIT2, m_Characteristics);
	DDX_Text(pDX, IDC_EDIT17, m_Base);
	DDX_Text(pDX, IDC_EDIT18, m_Name);
	DDX_Text(pDX, IDC_EDIT6, m_NameString);
	DDX_Text(pDX, IDC_EDIT7, m_NumOfFun);
	DDX_Text(pDX, IDC_EDIT8, m_NumOfName);
	DDX_Text(pDX, IDC_EDIT9, m_AddrOfFun);
	DDX_Text(pDX, IDC_EDIT10, m_AddrOfName);
	DDX_Text(pDX, IDC_EDIT11, m_AddrOfNameOrd);
	DDX_Control(pDX, IDC_LIST1, m_ExportList);
}


BEGIN_MESSAGE_MAP(CExport, CDialogEx)
END_MESSAGE_MAP()


// CExport 消息处理程序


//*****************************************************************************************
// 函数名称: OnInitDialog
// 函数说明: 初始化导出表
// 作    者: lracker
// 时    间: 2019/10/16
// 返 回 值: BOOL
//*****************************************************************************************
BOOL CExport::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	PIMAGE_EXPORT_DIRECTORY pExport = GetExportDirectory();
	// 导出表的FOA
	m_ExportFOA.Format(_T("%XH"), RVATOFOA(m_dwExportRVA));
	// 特征码
	m_Characteristics.Format(_T("%XH"), pExport->Characteristics);
	// 基址
	m_Base.Format(_T("%XH"), pExport->Base);
	// 名称
	m_Name.Format(_T("%XH"), pExport->Name);
	// 名称字串
	// 获取文件载入的va
	DWORD64 dwFile = (DWORD64)pExport - RVATOFOA(m_dwExportRVA);
	DWORD64 tmp = RVATOFOA(pExport->Name)+ dwFile;
	USES_CONVERSION;
	CHAR* sTmp = (CHAR*)tmp;
	m_NameString.Format(_T("%s"), (CString)sTmp);
	// 函数数量
	m_NumOfFun.Format(_T("%XH"), pExport->NumberOfFunctions);
	// 函数名数量
	m_NumOfName.Format(_T("%XH"), pExport->NumberOfNames);
	// 函数地址
	m_AddrOfFun.Format(_T("%XH"), pExport->AddressOfFunctions);
	// 函数名称地址
	m_AddrOfName.Format(_T("%XH"), pExport->AddressOfNames);
	// 函数名序号地址
	m_AddrOfNameOrd.Format(_T("%XH"), pExport->AddressOfNameOrdinals);
	// 初始化导出表列表
	DWORD64 dwOldStyle = m_ExportList.GetExtendedStyle();
	m_ExportList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT);
	CRect cRect;
	m_ExportList.GetClientRect(cRect);
	m_ExportList.InsertColumn(0, L"序号", 0, cRect.Width() / 4);
	m_ExportList.InsertColumn(1, L"RVA", 0, cRect.Width() / 4);
	m_ExportList.InsertColumn(2, L"Offset", 0, cRect.Width() / 4);
	m_ExportList.InsertColumn(3, L"函数名", 0, cRect.Width() / 4);
	// 开始列出导出表里面的函数信息了
	// 导出序号表
	WORD* pEOT = (WORD*)(RVATOFOA(pExport->AddressOfNameOrdinals) + (DWORD64)dwFile);
	// 导出名称表
	DWORD64* pENT = (DWORD64*)(RVATOFOA(pExport->AddressOfNames) + (DWORD64)dwFile);
	// 导出函数表
	DWORD64* pEAT = (DWORD64*)(RVATOFOA(pExport->AddressOfFunctions) + (DWORD64)dwFile);
	for (int i = 0; i < pExport->NumberOfFunctions; i++)
	{
		m_ExportList.InsertItem(i, _T(""));
		CString Buffer;
		// 序号
		Buffer.Format(_T("%XH"), i + pExport->Base);
		m_ExportList.SetItemText(i, 0, Buffer);
		// RVA
		Buffer.Format(_T("%XH"), pEAT[i]);
		m_ExportList.SetItemText(i, 1, Buffer);
		// Offset
		Buffer.Format(_T("%XH"), RVATOFOA(pEAT[i]));
		m_ExportList.SetItemText(i, 2, Buffer);
		for (int j = 0; j < pExport->NumberOfNames; j++)
		{
			if (i == pEOT[j])
			{
				// 函数的名称
				char* FunName = (char*)(RVATOFOA(pENT[j]) + (DWORD64)dwFile);
				m_ExportList.SetItemText(i, 3, (CString)FunName);
				break;
			}
			else if (j == pExport->NumberOfNames - 1)
			{
				// 如果不是名称导出的话，那就填-
				m_ExportList.SetItemText(i, 3, _T("-"));
			}
		}
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

PIMAGE_EXPORT_DIRECTORY CExport::GetExportDirectory()
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_EXPORT_DIRECTORY)(RVATOFOA(m_dwExportRVA) + (DWORD64)pFileBuff);
}

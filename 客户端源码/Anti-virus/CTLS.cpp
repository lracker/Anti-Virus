// CTLS.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CTLS.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "Data.h"


// CTLS 对话框

IMPLEMENT_DYNAMIC(CTLS, CDialogEx)

CTLS::CTLS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TLS, pParent)
	, m_DataBlockStartVA(_T(""))
	, m_DataBlockEndVA(_T(""))
	, m_IndexVA(_T(""))
	, m_CallBackVA(_T(""))
	, m_SizeOfZero(_T(""))
	, m_Characteristics(_T(""))
{

}

CTLS::~CTLS()
{
}

void CTLS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_DataBlockStartVA);
	DDX_Text(pDX, IDC_EDIT18, m_DataBlockEndVA);
	DDX_Text(pDX, IDC_EDIT17, m_IndexVA);
	DDX_Text(pDX, IDC_EDIT5, m_CallBackVA);
	DDX_Text(pDX, IDC_EDIT2, m_SizeOfZero);
	DDX_Text(pDX, IDC_EDIT6, m_Characteristics);
}


BEGIN_MESSAGE_MAP(CTLS, CDialogEx)
END_MESSAGE_MAP()


// CTLS 消息处理程序


BOOL CTLS::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (g_nMachine == 32)
	{
		PIMAGE_TLS_DIRECTORY32 pTls = GetTlsDirectory();
		m_DataBlockStartVA.Format(_T("%XH"), pTls->StartAddressOfRawData);
		m_DataBlockEndVA.Format(_T("%XH"), pTls->EndAddressOfRawData);
		m_IndexVA.Format(_T("%XH"), pTls->AddressOfIndex);
		m_CallBackVA.Format(_T("%XH"), pTls->AddressOfCallBacks);
		m_SizeOfZero.Format(_T("%XH"), pTls->SizeOfZeroFill);
		m_Characteristics.Format(_T("%XH"), pTls->Characteristics);
	}
	else if (g_nMachine == 64)
	{
		PIMAGE_TLS_DIRECTORY64 pTls = GetTlsDirectory64();
		m_DataBlockStartVA.Format(_T("%llXH"), pTls->StartAddressOfRawData);
		m_DataBlockEndVA.Format(_T("%llXH"), pTls->EndAddressOfRawData);
		m_IndexVA.Format(_T("%llXH"), pTls->AddressOfIndex);
		m_CallBackVA.Format(_T("%llXH"), pTls->AddressOfCallBacks);
		m_SizeOfZero.Format(_T("%llXH"), pTls->SizeOfZeroFill);
		m_Characteristics.Format(_T("%llXH"), pTls->Characteristics);
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

PIMAGE_TLS_DIRECTORY32 CTLS::GetTlsDirectory()
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_TLS_DIRECTORY32)(RVATOFOA(m_dwTLSRVA) + (DWORD64)pFileBuff);
}

PIMAGE_TLS_DIRECTORY64 CTLS::GetTlsDirectory64()
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_TLS_DIRECTORY64)(RVATOFOA(m_dwTLSRVA) + (DWORD64)pFileBuff);
}
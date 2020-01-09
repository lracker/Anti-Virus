// CDelay.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CDelay.h"
#include "afxdialogex.h"
#include "Tools.h"


// CDelay 对话框

IMPLEMENT_DYNAMIC(CDelay, CDialogEx)

CDelay::CDelay(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DELAY, pParent)
{

}

CDelay::~CDelay()
{
}

void CDelay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Delay);
}


BEGIN_MESSAGE_MAP(CDelay, CDialogEx)
END_MESSAGE_MAP()


// CDelay 消息处理程序


BOOL CDelay::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化delay表
	PIMAGE_DELAYLOAD_DESCRIPTOR pDelay = GetDelayDirectory();
	CString Buffer;
	CString End;
	Buffer.Format(_T("grAttrs:    0x%X"), pDelay->Attributes);
	Buffer += _T("\r\n");
	End += Buffer;
	Buffer.Format(_T("DLLName RVA:    0x%X"), pDelay->DllNameRVA);
	Buffer += _T("\r\n");
	End += Buffer;
	Buffer.Format(_T("Hmod RVA:    0x%X"), pDelay->ModuleHandleRVA);
	Buffer += _T("\r\n");;
	End += Buffer;
	Buffer.Format(_T("IAT RVA:    0x%X"), pDelay->ImportAddressTableRVA);
	Buffer += _T("\r\n");
	End += Buffer;
	Buffer.Format(_T("INT RVA:    0x%X"), pDelay->ImportNameTableRVA);
	Buffer += _T("\r\n");
	End += Buffer;
	Buffer.Format(_T("BoundIAT RVA:    0x%X"), pDelay->BoundImportAddressTableRVA);
	Buffer += _T("\r\n");
	End += Buffer;
	Buffer.Format(_T("UnLoadIAT RVA:    0x%X"), pDelay->UnloadInformationTableRVA);
	Buffer += L"\r\n";
	End += Buffer;
	Buffer.Format(_T("TimeDateStamp:    0x%X"), pDelay->TimeDateStamp);
	Buffer += _T("\r\n");
	End += Buffer;
	m_Delay.SetWindowText(End);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

PIMAGE_DELAYLOAD_DESCRIPTOR CDelay::GetDelayDirectory()
{

	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_DELAYLOAD_DESCRIPTOR)(RVATOFOA(m_dwDelayRVA) + (DWORD64)pFileBuff);
}
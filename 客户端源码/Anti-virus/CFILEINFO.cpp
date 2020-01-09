// CFILEINFO.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CFILEINFO.h"
#include "afxdialogex.h"
#include "Md5.h"


// CFILEINFO 对话框

IMPLEMENT_DYNAMIC(CFILEINFO, CDialogEx)

CFILEINFO::CFILEINFO(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEINFO, pParent)
	, m_Change(_T(""))
{

}

CFILEINFO::~CFILEINFO()
{
}

void CFILEINFO::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Create);
	DDX_Text(pDX, IDC_EDIT2, m_Change);
	DDX_Text(pDX, IDC_EDIT17, m_Size);
	DDX_Text(pDX, IDC_EDIT5, m_Md5);
}


BEGIN_MESSAGE_MAP(CFILEINFO, CDialogEx)
END_MESSAGE_MAP()


// CFILEINFO 消息处理程序


BOOL CFILEINFO::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 根据文件路径来获取文件的信息
	WIN32_FIND_DATA stcFData = { 0 };
	HANDLE hFind = FindFirstFile(m_FileName, &stcFData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	SYSTEMTIME st = { 0 };
	char buf[64] = { 0 };
	FileTimeToSystemTime(&stcFData.ftCreationTime, &st);
	sprintf_s(buf, "%4d-%02d-%02d %02d:%02d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	m_Create = buf;
	FileTimeToSystemTime(&stcFData.ftLastWriteTime, &st);
	sprintf_s(buf, "%4d-%02d-%02d %02d:%02d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	m_Change = buf;
	m_Size.Format(L"%dKB", stcFData.nFileSizeLow);
	USES_CONVERSION;
	m_Md5 = md5FileValue(T2A(m_FileName));
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

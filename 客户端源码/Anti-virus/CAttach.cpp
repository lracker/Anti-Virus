// CAttach.cpp: 实现文件
//

#include "pch.h"
#include <powrprof.h>
#include "Anti-virus.h"
#include "CAttach.h"
#include "afxdialogex.h"
#include "CFILEINFO.h"



// CAttach 对话框

IMPLEMENT_DYNAMIC(CAttach, CDialogEx)

CAttach::CAttach(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ATTACH, pParent)
{

}

CAttach::~CAttach()
{
}

void CAttach::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAttach, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAttach::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAttach::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAttach::OnBnClickedButton3)

	ON_BN_CLICKED(IDC_BUTTON4, &CAttach::OnBnClickedButton4)

END_MESSAGE_MAP()


// CAttach 消息处理程序


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: 关机
// 作    者: lracker
// 时    间: 2019/10/18
// 返 回 值: void
//*****************************************************************************************
void CAttach::OnBnClickedButton1()
{
	system("shutdown -f");
	//ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, NULL);
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton2
// 函数说明: 重启
// 作    者: lracker
// 时    间: 2019/10/18
// 返 回 值: void
//*****************************************************************************************
void CAttach::OnBnClickedButton2()
{
	system("shutdown -r");
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton3
// 函数说明: 注销
// 作    者: lracker
// 时    间: 2019/10/18
// 返 回 值: void
//*****************************************************************************************
void CAttach::OnBnClickedButton3()
{
	system("shutdown -l");
}




//*****************************************************************************************
// 函数名称: OnBnClickedButton4
// 函数说明: 查看文件信息
// 作    者: lracker
// 时    间: 2019/10/18
// 返 回 值: void
//*****************************************************************************************
void CAttach::OnBnClickedButton4()
{
	static TCHAR BASED_CODE szFilter[] = _T("All Files (*.*)|*.*||");
	CFileDialog File(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szFilter, NULL);
	if (File.DoModal() == IDOK)
	{
		CString FileName;
		FileName = File.GetPathName(); 
		CFILEINFO obj;
		obj.m_FileName = FileName;
		obj.DoModal();
	}
}



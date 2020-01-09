// CSoftware.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CSoftware.h"
#include "afxdialogex.h"


// CSoftware 对话框

IMPLEMENT_DYNAMIC(CSoftware, CDialogEx)

CSoftware::CSoftware(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SOFTWARE, pParent)
{

}

CSoftware::~CSoftware()
{
}

void CSoftware::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSoftware, CDialogEx)
END_MESSAGE_MAP()


// CSoftware 消息处理程序


BOOL CSoftware::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

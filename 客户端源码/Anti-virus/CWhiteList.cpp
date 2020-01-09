// CWhiteList.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CWhiteList.h"
#include "afxdialogex.h"
#include <Shlwapi.h>
#include <xutility>
#include "Data.h"


// CWhiteList 对话框

IMPLEMENT_DYNAMIC(CWhiteList, CDialogEx)

CWhiteList::CWhiteList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WHITELIST, pParent)
{
}

CWhiteList::~CWhiteList()
{
}

void CWhiteList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_WhiteList);
}


BEGIN_MESSAGE_MAP(CWhiteList, CDialogEx)

END_MESSAGE_MAP()


// CWhiteList 消息处理程序

BOOL CWhiteList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化白名单列表
	DWORD dwOldStyle = m_WhiteList.GetExtendedStyle();
	m_WhiteList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect cRect;
	GetClientRect(cRect);
	m_WhiteList.InsertColumn(0, _T("序号"), 0, 100);
	m_WhiteList.InsertColumn(1, _T("进程名"), 0, cRect.Width() - 100);
	int nIndex = 0;
	for (auto& i : g_ProcessVector)
	{
		m_WhiteList.InsertItem(nIndex, _T(""));
		CString Buffer;
		Buffer.Format(_T("%d"), nIndex + 1);
		m_WhiteList.SetItemText(nIndex, 0, Buffer);
		m_WhiteList.SetItemText(nIndex, 1, i);
		nIndex++;
	}



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


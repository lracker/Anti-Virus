// CDoubt.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CDoubt.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include <vector>
#include "Data.h"
using std::vector;

// CDoubt 对话框

IMPLEMENT_DYNAMIC(CDoubt, CDialogEx)

CDoubt::CDoubt(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DOUBT, pParent)
{
	m_Menu.LoadMenuW(IDR_MENU1);  //加载菜单资源
}

CDoubt::~CDoubt()
{
}

void CDoubt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}


BEGIN_MESSAGE_MAP(CDoubt, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDoubt::OnRclickList1)
	ON_COMMAND(ID_32771, &CDoubt::On32771)
END_MESSAGE_MAP()


// CDoubt 消息处理程序


BOOL CDoubt::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	vector<PROCESSENTRY32> newProcess;
	vector<PROCESSENTRY32> doubtProcess;
	// 创建进程快照，和g_ProcessVector里的数据进行比较
	// 获取进程快照
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// 初始化结构体
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	// 开始遍历第一个
	if (!Process32First(hProcessSnap, &pe))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do
	{
		newProcess.push_back(pe);
	} while (Process32Next(hProcessSnap, &pe));
	for (auto& i : newProcess)
	{
		int t = 0;
		for (auto& j : g_ProcessVector)
		{
			if (i.szExeFile == j)
			{
				break;
			}
			if (t == g_ProcessVector.size() - 1)
			{
				doubtProcess.push_back(i);
				break;
			}
			t++;
		}
	}
	// 将可以的程序插入列表中
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect cRect;
	GetClientRect(cRect);
	m_List.InsertColumn(0, _T("可疑的进程名"), 0, cRect.Width() / 2);
	m_List.InsertColumn(1, _T("PID"), 0, cRect.Width() / 2);
	int t = 0;
	for (auto& i : doubtProcess)
	{
		CString Buffer;
		m_List.InsertItem(t, _T(""));
		m_List.SetItemText(t, 0, i.szExeFile);
		Buffer.Format(_T("%d"), i.th32ProcessID);
		m_List.SetItemText(t, 1, Buffer);
		t++;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDoubt::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	m_SelectedItem = pNMItemActivate->iItem;
	CString PID;
	PID = m_List.GetItemText(pNMItemActivate->iItem, 1);
	m_dwPid = _ttoi(PID);
	// 获取当前鼠标相对于屏幕的位置
	POINT Point = { 0 };
	GetCursorPos(&Point);
	// 获取菜单的子菜单
	CMenu* SubMenu = m_Menu.GetSubMenu(0);
	// 弹出窗口
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}


//*****************************************************************************************
// 函数名称: On32771
// 函数说明: 结束进程
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CDoubt::On32771()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPid);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	m_List.DeleteItem(m_SelectedItem);
}


// Anti-virusDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Anti-virus.h"
#include "Anti-virusDlg.h"
#include "afxdialogex.h"
#include "CTaskManager.h"
#include "CMemory.h"
#include "CRubbish.h"
#include "CSERVICE.h"
#include "CPELoader.h"
#include "CKillVirus.h"
#include "CAttach.h"
#include "Tools.h"
#include "Data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAntivirusDlg 对话框



CAntivirusDlg::CAntivirusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANTIVIRUS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 提高权限
	EnableDebugPrivilege(1);
	HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"AntiPID");
	LPVOID lpBuff = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 10);
	CString CurrentPid;
	CurrentPid.Format(_T("%d"),GetCurrentProcessId());
	int size = CurrentPid.GetLength();
	memcpy(lpBuff, CurrentPid, size * 2);
	m_IsWindowHide = TRUE;
}

void CAntivirusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_MyTab);
}

BEGIN_MESSAGE_MAP(CAntivirusDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAntivirusDlg::OnSelchangeTab1)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAntivirusDlg::OnSelchangeTab1)
//ON_WM_INPUT_DEVICE_CHANGE()
ON_WM_INPUT_DEVICE_CHANGE()
END_MESSAGE_MAP()

//*****************************************************************************************
// 函数名称: Protest
// 函数说明: 创建线程一直保护该进程
// 作    者: lracker
// 时    间: 2019/10/18
// 参    数: LPVOID lpParameter
// 返 回 值: DWORD CALLBACK
//*****************************************************************************************
DWORD WINAPI ProtectME(LPVOID lpParameter)
{
	while (TRUE)
	{

		// 初始化的时候就应该HOOK任务管理器的OpenProcess
		DWORD dwNewTaskPid = GetPid(L"Taskmgr.exe");
		// 判断是不是同一个任务管理器
		if(dwNewTaskPid == g_OldTaskPid)
			continue;
		Inject(dwNewTaskPid);
		g_OldTaskPid = dwNewTaskPid;
		Sleep(500);
	}
	return 0;
}

// CAntivirusDlg 消息处理程序

BOOL CAntivirusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// 初始化Tab控件
	m_MyTab.InsertItem(0, _T("任务管理"));
	m_MyTab.InsertItem(1, _T("性能"));
	m_MyTab.InsertItem(2, _T("垃圾清理"));
	m_MyTab.InsertItem(3, _T("服务"));
	m_MyTab.InsertItem(4, _T("PE解析"));
	m_MyTab.InsertItem(5, _T("杀毒"));
	m_MyTab.InsertItem(6, _T("附加功能"));
	// 给子窗口指针赋值
	m_MyTab.m_Dia[0] = new CTaskManager();
	m_MyTab.m_Dia[1] = new CMemory();
	m_MyTab.m_Dia[2] = new CRubbish();
	m_MyTab.m_Dia[3] = new CSERVICE();
	m_MyTab.m_Dia[4] = new CPELoader();
	m_MyTab.m_Dia[5] = new CKillVirus();
	m_MyTab.m_Dia[6] = new CAttach();
	// 创建子窗口
	m_MyTab.m_Dia[0]->Create(IDD_TaskManager, &m_MyTab);
	m_MyTab.m_Dia[1]->Create(IDD_Memory, &m_MyTab);
	m_MyTab.m_Dia[2]->Create(IDD_RUBBISH, &m_MyTab);
	m_MyTab.m_Dia[3]->Create(IDD_SERVICE, &m_MyTab);
	m_MyTab.m_Dia[4]->Create(IDD_PELOADER, &m_MyTab);
	m_MyTab.m_Dia[5]->Create(IDD_KILLVIRUS, &m_MyTab);
	m_MyTab.m_Dia[6]->Create(IDD_ATTACH, &m_MyTab);
	// 获取坐标
	CRect pos = {};
	m_MyTab.GetClientRect(pos);		//转换为客户端坐标
	pos.DeflateRect(2, 30, 3, 2);	// 移动坐标，免得盖住菜单了
	// 移动窗口
	m_MyTab.m_Dia[0]->MoveWindow(pos);
	// 显示窗口
	m_MyTab.m_Dia[0]->ShowWindow(SW_SHOW);
	m_MyTab.m_Dia[1]->ShowWindow(SW_HIDE);
	m_MyTab.m_Dia[2]->ShowWindow(SW_HIDE);
	m_MyTab.m_Dia[3]->ShowWindow(SW_HIDE);
	m_MyTab.m_Dia[4]->ShowWindow(SW_HIDE);
	m_MyTab.m_Dia[5]->ShowWindow(SW_HIDE);
	m_MyTab.m_Dia[6]->ShowWindow(SW_HIDE);
	// 创建一个线程来保护进程
	CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)ProtectME,NULL,0, NULL);
	// 注册全局热键,ctrl+shift+k
	::RegisterHotKey(this->GetSafeHwnd(), 0xa1001, MOD_CONTROL | MOD_SHIFT, 'K'); 
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAntivirusDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAntivirusDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//*****************************************************************************************
// 函数名称: OnSelchangeTab1
// 函数说明: 切换tab控件
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CAntivirusDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CRect cRect;
	GetClientRect(cRect);
	cRect.DeflateRect(2, 30, 3, 2);
	// 获取所选的tab项
	DWORD64 dwSel = m_MyTab.GetCurSel();
	for (DWORD64 i = 0; i < m_MyTab.m_nTabNum; i++)
	{
		if (dwSel == i)
		{
			m_MyTab.m_Dia[i]->MoveWindow(cRect);    //将对话框移动到客户区范围内
			m_MyTab.m_Dia[i]->ShowWindow(SW_SHOW); //只显示选中的
		}
		else
		{
			m_MyTab.m_Dia[i]->ShowWindow(SW_HIDE); //其他都设置为隐藏
		}
	}
	*pResult = 0;
}



//*****************************************************************************************
// 函数名称: PreTranslateMessage
// 函数说明: 老板键
// 作    者: lracker
// 时    间: 2019/10/19
// 参    数: MSG * pMsg
// 返 回 值: BOOL
//*****************************************************************************************
BOOL CAntivirusDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_HOTKEY) && (pMsg->wParam == 0xa1001))
	{
		// 隐藏窗口的代码
		if (m_IsWindowHide == TRUE)
		{
			ShowWindow(SW_HIDE);
			m_IsWindowHide = FALSE;
		}
		// 显示窗口的代码
		else
		{
			ShowWindow(SW_SHOW);
			m_IsWindowHide = TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


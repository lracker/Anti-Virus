// CTaskManager.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CTaskManager.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "CEnumWindow.h"
#include "Data.h"

DWORD WINAPI ProcessThread(LPVOID lpThreadParameter)
{
	while (true)
	{
		CDialogEx* _this = (CDialogEx*)lpThreadParameter;
		// 发送UM_Process消息给窗口
		SendMessage(_this->m_hWnd, UM_Process, NULL, NULL);
		// 缓一缓0.3秒
		Sleep(300);
	}
}

// CTaskManager 对话框

IMPLEMENT_DYNAMIC(CTaskManager, CDialogEx)

CTaskManager::CTaskManager(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TaskManager, pParent)
{
	m_Menu.LoadMenuW(IDR_MENU1);  //加载菜单资源
}

CTaskManager::~CTaskManager()
{
}

void CTaskManager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ProcessList);
	DDX_Control(pDX, IDC_LIST2, m_ThreadList);
	DDX_Control(pDX, IDC_LIST3, m_ModuleList);
}


BEGIN_MESSAGE_MAP(CTaskManager, CDialogEx)
	ON_MESSAGE(UM_Process, &CTaskManager::OnProcess)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CTaskManager::OnClickList1)
	ON_MESSAGE(UM_Thread, &CTaskManager::OnUmThread)
	ON_MESSAGE(UM_Module, &CTaskManager::OnUmModule)
	ON_BN_CLICKED(IDC_BUTTON1, &CTaskManager::OnBnClickedButton1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CTaskManager::OnRclickList1)
	ON_COMMAND(ID_32771, &CTaskManager::EndProcess)
END_MESSAGE_MAP()


// CTaskManager 消息处理程序




BOOL CTaskManager::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 初始化进程列表窗口
	// 设置列表风格
	m_ProcessList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 获取进程窗口的坐标
	CRect ProcssRect;
	// 坐标转换为客户端
	m_ProcessList.GetClientRect(ProcssRect);
	m_ProcessList.InsertColumn(0, L"进程名", 0, ProcssRect.Width() / 3);
	m_ProcessList.InsertColumn(1, L"PID", 0, ProcssRect.Width() / 3);
	m_ProcessList.InsertColumn(2, L"内存占用", 0, ProcssRect.Width() / 3);
	// 遍历一次进程
	FlushProcessList();
	// 创建一个线程来时刻更新进程列表
	CreateThread(NULL, 0, ProcessThread, this, 0, NULL);

	// 初始化线程列表窗口
	// 设置列表风格
	m_ThreadList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 获取线程窗口的坐标
	CRect ThreadRect;
	// 坐标转换为客户端
	m_ThreadList.GetClientRect(ThreadRect);
	m_ThreadList.InsertColumn(0, L"序号", 0, ThreadRect.Width() / 3);
	m_ThreadList.InsertColumn(2, L"TID", 0, ThreadRect.Width() / 3);
	m_ThreadList.InsertColumn(3, L"优先级", 0, ThreadRect.Width() / 3);

	// 初始化模块列表窗口
	m_ModuleList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 获取模块窗口的坐标
	CRect ModuleRect;
	// 坐标转换为客户端
	m_ModuleList.GetClientRect(ModuleRect);
	m_ModuleList.InsertColumn(0, L"模块名", 0, ModuleRect.Width() / 4);
	m_ModuleList.InsertColumn(1, L"加载基址", 0, ModuleRect.Width() / 4);
	m_ModuleList.InsertColumn(2, L"大小", 0, ModuleRect.Width() / 4);
	m_ModuleList.InsertColumn(3, L"路径", 0, ModuleRect.Width() / 4);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//*****************************************************************************************
// 函数名称: FlushProcessList
// 函数说明: 刷新进程列表
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CTaskManager::FlushProcessList()
{
	// 先清空新的列表
	m_NewProcessVector.empty();
	if (!GetAllProcesses(m_NewProcessVector))
		return;
	// 如果是初次遍历进程列表的话，则全部刷新，否则刷新不一样的地方就好了
	if (m_OldProcessVector.empty())
	{
		// 列表的索引
		int nIndex = 0;
		// 用一个buffer来转换PID和内存占用
		CString Buffer;
		for (auto& i : m_NewProcessVector)
		{
			// 插入第nIndex项
			m_ProcessList.InsertItem(nIndex, _T(""));
			// 插入进程的名字
			m_ProcessList.SetItemText(nIndex, 0, i.szExeFile);
			// 进程PID转换成Buffer，插入
			Buffer.Format(_T("%d"), i.th32ProcessID);
			m_ProcessList.SetItemText(nIndex, 1, Buffer);
			// 进程的内存占用转换为Buffer，插入
			Buffer.Format(_T("%.3lfMB"), (double)i.dwMemoryUsage / 1024 / 1024);
			m_ProcessList.SetItemText(nIndex, 2, Buffer);
			nIndex++;
		}
	}
	// 如果不是第一次刷新进程列表的话，查看新旧进程列表的变化
	else
	{
		// 找出退出了的进程
		int nIndex = 0;
		for (auto& i : m_OldProcessVector)
		{
			// 用旧列表里的PID在新列表里寻找,如果返回FALSE则是退出了的程序
			if (!CmpProcIndex(m_NewProcessVector, i.th32ProcessID))
				m_ProcessList.DeleteItem(nIndex);
			else
				// 索引增加
				nIndex++;
		}
		// 找出新启动的程序
		for (auto& i : m_NewProcessVector)
		{
			// 用新列表里的PID在旧列表里面寻找，如果返回FALSE则是新启动的程序
			if (!CmpProcIndex(m_OldProcessVector, i.th32ProcessID))
			{
				// 用一个buffer来转换PID和内存占用
				CString buffer;
				m_ProcessList.InsertItem(nIndex, _T(""));
				// 插入进程名
				m_ProcessList.SetItemText(nIndex, 0, i.szExeFile);
				// 插入进程ID
				buffer.Format(_T("%d"), i.th32ProcessID);
				m_ProcessList.SetItemText(nIndex, 1, buffer);
				// 插入内存占用
				buffer.Format(_T("%.2lfMB"), (double)i.dwMemoryUsage / 1024 / 1024);
				m_ProcessList.SetItemText(nIndex, 2, buffer);
				// 索引增加
				nIndex++;
			}
		}
	}
	//将新的进程列表和旧的交换一遍
	m_OldProcessVector.swap(m_NewProcessVector);
}


//*****************************************************************************************
// 函数名称: OnProcess
// 函数说明: 如果接收到UM_Process消息的话则刷新列表
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CTaskManager::OnProcess(WPARAM wParam, LPARAM lParam)
{
	FlushProcessList();
	return 0;
}


//*****************************************************************************************
// 函数名称: OnClickList1
// 函数说明: 点击列表，线程窗口和模块窗口自动更新
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CTaskManager::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 先获取所选项的PID
	CString PID;
	PID = m_ProcessList.GetItemText(pNMItemActivate->iItem, 1);
	m_dwPid = _ttoi(PID);
	// 发送线程更新消息给窗口
	::SendMessage(m_hWnd, UM_Thread, NULL, NULL);
	// 发送线程模块消息给窗口
	::SendMessage(m_hWnd, UM_Module, NULL, NULL);
	*pResult = 0;
}

//*****************************************************************************************
// 函数名称: OnUmThread
// 函数说明: 收到UM_Thread消息就开始遍历线程了
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CTaskManager::OnUmThread(WPARAM wParam, LPARAM lParam)
{
	// 清空线程列表
	m_ThreadList.DeleteAllItems();
	// 根据PID获取线程
	GetAllThread(m_ThreadVector,m_dwPid);
	// 显示在线程窗口
	int nIndex = 0;
	for (auto& i : m_ThreadVector)
	{
		m_ThreadList.InsertItem(nIndex, _T(""));
		// 用一个Buffer进行转换
		CString Buffer;
		// 插入序号
		Buffer.Format(_T("%d"), nIndex);
		m_ThreadList.SetItemText(nIndex, 0, Buffer);
		// 插入TID
		Buffer.Format(_T("%d"), i.th32ThreadID);
		m_ThreadList.SetItemText(nIndex, 1, Buffer);
		// 插入优先级
		Buffer.Format(_T("%d"), i.tpBasePri);
		m_ThreadList.SetItemText(nIndex, 2, Buffer);
		nIndex++;
	}
	return 0;
}


//*****************************************************************************************
// 函数名称: OnUmModule
// 函数说明: 收到UM_Module消息就开始遍历线程了
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CTaskManager::OnUmModule(WPARAM wParam, LPARAM lParam)
{
	// 清空模块列表
	m_ModuleList.DeleteAllItems();
	// 根据PID获取模块
	GetAllModule(m_ModuleVector, m_dwPid);
	// 显示在模块窗口
	int nIndex = 0;
	for (auto& i : m_ModuleVector)
	{
		m_ModuleList.InsertItem(nIndex, _T(""));
		// 插入模块名
		m_ModuleList.SetItemText(nIndex, 0, i.szModuleName);
		// Buffer用于转换
		CString Buffer;
		Buffer.Format(_T("%08llX"), i.lpBaseOfDll);
		// 插入加载基址
		m_ModuleList.SetItemText(nIndex, 1, Buffer);
		// 插入大小 
		Buffer.Format(_T("%08X"), i.SizeOfImage);
		m_ModuleList.SetItemText(nIndex, 2, Buffer);
		// 插入路径
		m_ModuleList.SetItemText(nIndex, 3, i.szModuleFile);
		nIndex++;
	}		
	return 0;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: 点击桌面窗口
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CTaskManager::OnBnClickedButton1()
{
	CEnumWindow EnumWindow;
	//创建一个模态对话框
	EnumWindow.DoModal();  
}


//*****************************************************************************************
// 函数名称: OnRclickList1
// 函数说明: 右键进程列表
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CTaskManager::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
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
// 函数名称: EndProcess
// 函数说明: 右键，结束进程
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CTaskManager::EndProcess()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPid);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	// 清空线程列表窗口和模块窗口
	m_ThreadList.DeleteAllItems();
	m_ModuleList.DeleteAllItems();
}

// CSERVICE.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CSERVICE.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "Data.h"

DWORD CALLBACK ServiceThread(LPVOID lpThreadParameter)
{
	while (true)
	{
		CDialogEx* _this = (CDialogEx*)lpThreadParameter;
		// 发送UM_Process消息给窗口
		SendMessage(_this->m_hWnd, UM_Service, NULL, NULL);
		// 缓一缓0.5秒
		Sleep(500);
	}
}

// CSERVICE 对话框

IMPLEMENT_DYNAMIC(CSERVICE, CDialogEx)

CSERVICE::CSERVICE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVICE, pParent)
{
	// 加载主菜单
	m_Menu.LoadMenu(IDR_MENU2);

}

CSERVICE::~CSERVICE()
{
}

void CSERVICE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ServiceList);
}


BEGIN_MESSAGE_MAP(CSERVICE, CDialogEx)
	ON_MESSAGE(UM_Service, &CSERVICE::OnUmService)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CSERVICE::OnRclickList1)
	ON_COMMAND(ID_32772, &CSERVICE::On32772)
	ON_COMMAND(ID_32773, &CSERVICE::On32773)
END_MESSAGE_MAP()


// CSERVICE 消息处理程序


BOOL CSERVICE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 获取原风格
	DWORD64 dwOldStyle = m_ServiceList.GetExtendedStyle();
	// 设置风格带有整行选中以及线格
	m_ServiceList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 获取宽度
	CRect cRect;
	m_ServiceList.GetClientRect(cRect);
	// 插入列名
	m_ServiceList.InsertColumn(0, L"名称", 0, cRect.Width() / 4);
	m_ServiceList.InsertColumn(1, L"PID", 0, cRect.Width() / 4);
	m_ServiceList.InsertColumn(2, L"类型", 0, cRect.Width() / 4);
	m_ServiceList.InsertColumn(3, L"状态", 0, cRect.Width() / 4);
	// 刷新一遍服务列表
	FlushServiceList();
	// 创建一个线程来时刻更新服务列表
	CreateThread(NULL, 0, ServiceThread, this, 0, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//*****************************************************************************************
// 函数名称: FlushServiceList
// 函数说明: 刷新服务列表
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CSERVICE::FlushServiceList()
{
	m_NewServiceVector.clear();
	if (!GetALLService(m_NewServiceVector))
		return;
	// 如果是初次遍历进程列表的话，则全部刷新，否则刷新不一样的地方就好了
	if (m_OldServiceVector.empty())
	{
		// 列表的索引
		int nIndex = 0;
		// 用一个buffer来转换PID和内存占用
		CString Buffer;
		for (auto& i : m_NewServiceVector)
		{
			// 插入第nIndex项
			m_ServiceList.InsertItem(nIndex, _T(""));
			// 插入服务的名字
			m_ServiceList.SetItemText(nIndex, 0, i.lpServiceName);
			// 服务PID转换成Buffer，插入
			Buffer.Format(_T("%d"), i.ServiceStatusProcess.dwProcessId);
			m_ServiceList.SetItemText(nIndex, 1, Buffer);
			// 服务的类型转换为Buffer，插入
			Buffer.Empty();
			switch (i.ServiceStatusProcess.dwServiceType)
			{
			case SERVICE_FILE_SYSTEM_DRIVER:
				Buffer = _T("SERVICE_FILE_SYSTEM_DRIVER");
				break;
			case SERVICE_KERNEL_DRIVER:
				Buffer = _T("SERVICE_KERNEL_DRIVER");
				break;
			case SERVICE_WIN32_OWN_PROCESS:
				Buffer = _T("SERVICE_WIN32_OWN_PROCESS");
				break;
			case SERVICE_WIN32_SHARE_PROCESS:
				Buffer = _T("SERVICE_WIN32_SHARE_PROCESS");
				break;
			default:
				break;
			}
			m_ServiceList.SetItemText(nIndex, 2, Buffer);
			// 服务的状态转换为Buffer插入
			Buffer.Empty();
			switch (i.ServiceStatusProcess.dwCurrentState)
			{
			case SERVICE_CONTINUE_PENDING:
				Buffer = _T("SERVICE_CONTINUE_PENDING");
				break;
			case SERVICE_PAUSE_PENDING:
				Buffer = _T("SERVICE_PAUSE_PENDING");
				break;
			case SERVICE_PAUSED:
				Buffer = _T("SERVICE_PAUSED");
				break;
			case SERVICE_RUNNING:
				Buffer = _T("SERVICE_RUNNING");
				break;
			case SERVICE_START_PENDING:
				Buffer = _T("SERVICE_START_PENDING");
				break;
			case SERVICE_STOP_PENDING:
				Buffer = _T("SERVICE_STOP_PENDING");
				break;
			case SERVICE_STOPPED:
				Buffer = _T("SERVICE_STOPPED");
				break;
			default:
				break;
			}
			m_ServiceList.SetItemText(nIndex, 3, Buffer);
			nIndex++;
		}
	}
	// 如果不是第一次刷新服务列表的话，查看新旧服务列表的变化
	else
	{
		// 找出退出了的服务
		int nIndex = 0;
		for (auto& i : m_OldServiceVector)
		{
			// 更新表上的状态
			CString Buffer;
			// 查找状态不一样的
			DWORD64 Status = CmpStatus(i, m_NewServiceVector);
			switch (Status)
			{
			case 0:
				break;
			case SERVICE_CONTINUE_PENDING:
				Buffer = _T("SERVICE_CONTINUE_PENDING");
				// 更新表上的状态
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			case SERVICE_PAUSE_PENDING:
				Buffer = _T("SERVICE_PAUSE_PENDING");
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			case SERVICE_PAUSED:
				Buffer = _T("SERVICE_PAUSED");
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			case SERVICE_RUNNING:
				Buffer = _T("SERVICE_RUNNING");
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			case SERVICE_START_PENDING:
				Buffer = _T("SERVICE_START_PENDING");
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			case SERVICE_STOP_PENDING:
				Buffer = _T("SERVICE_STOP_PENDING");
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			case SERVICE_STOPPED:
				Buffer = _T("SERVICE_STOPPED");
				// 更新表上的状态
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				break;
			default:
				break;
			}
			// 用旧列表里的PID在新列表里寻找,如果返回FALSE则是退出了的程序
			if (!CmpSerName(m_NewServiceVector, i.lpServiceName))
				m_ServiceList.DeleteItem(nIndex);
			else
				// 索引增加
				nIndex++;
		}
		// 找出新启动的服务
		for (auto& i : m_NewServiceVector)
		{
			CString Buffer;
			// 用新列表里的PID在旧列表里面寻找，如果返回FALSE则是新启动的程序
			if (!CmpSerName(m_OldServiceVector, i.lpServiceName))
			{
				// 插入第nIndex项
				m_ServiceList.InsertItem(nIndex, _T(""));
				// 插入服务的名字
				m_ServiceList.SetItemText(nIndex, 0, i.lpServiceName);
				// 服务PID转换成Buffer，插入
				Buffer.Format(_T("%d"), i.ServiceStatusProcess.dwProcessId);
				m_ServiceList.SetItemText(nIndex, 1, Buffer);
				// 服务的类型转换为Buffer，插入
				Buffer.Empty();
				switch (i.ServiceStatusProcess.dwServiceType)
				{
				case SERVICE_FILE_SYSTEM_DRIVER:
					Buffer = _T("SERVICE_FILE_SYSTEM_DRIVER");
					break;
				case SERVICE_KERNEL_DRIVER:
					Buffer = _T("SERVICE_KERNEL_DRIVER");
					break;
				case SERVICE_WIN32_OWN_PROCESS:
					Buffer = _T("SERVICE_WIN32_OWN_PROCESS");
					break;
				case SERVICE_WIN32_SHARE_PROCESS:
					Buffer = _T("SERVICE_WIN32_SHARE_PROCESS");
					break;
				default:
					break;
				}
				m_ServiceList.SetItemText(nIndex, 2, Buffer);
				// 服务的状态转换为Buffer插入
				Buffer.Empty();
				switch (i.ServiceStatusProcess.dwCurrentState)
				{
				case SERVICE_CONTINUE_PENDING:
					Buffer = _T("SERVICE_CONTINUE_PENDING");
					break;
				case SERVICE_PAUSE_PENDING:
					Buffer = _T("SERVICE_PAUSE_PENDING");
					break;
				case SERVICE_PAUSED:
					Buffer = _T("SERVICE_PAUSED");
					break;
				case SERVICE_RUNNING:
					Buffer = _T("SERVICE_RUNNING");
					break;
				case SERVICE_START_PENDING:
					Buffer = _T("SERVICE_START_PENDING");
					break;
				case SERVICE_STOP_PENDING:
					Buffer = _T("SERVICE_STOP_PENDING");
					break;
				case SERVICE_STOPPED:
					Buffer = _T("SERVICE_STOPPED");
					break;
				default:
					break;
				}
				m_ServiceList.SetItemText(nIndex, 3, Buffer);
				nIndex++;
			}
		}
	}
	// 将新的进程列表和旧的交换一遍
	 m_OldServiceVector.swap(m_NewServiceVector);
}



//*****************************************************************************************
// 函数名称: OnUmService
// 函数说明: 收到更新服务列表的消息
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CSERVICE::OnUmService(WPARAM wParam, LPARAM lParam)
{
	FlushServiceList();
	return 0;
}


//*****************************************************************************************
// 函数名称: OnRclickList1
// 函数说明: 右键弹出菜单
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CSERVICE::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_Menu.EnableMenuItem(ID_32772, MF_DISABLED);
	m_Menu.EnableMenuItem(ID_32773, MF_DISABLED);
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POINT Point = { 0 };
	GetCursorPos(&Point);
	// 获取选中项的名字
	m_ServiceName = m_ServiceList.GetItemText(pNMItemActivate->iItem, 0);
	// 判断选中项的状态
	CString Status = m_ServiceList.GetItemText(pNMItemActivate->iItem, 3);
	// 如果当前选中项的状态是已停止的话，那么启动服务按钮亮起来。
	if (Status == _T("SERVICE_STOPPED"))
		m_Menu.EnableMenuItem(ID_32772, MF_ENABLED);
	else if (Status == _T("SERVICE_RUNNING"))
		m_Menu.EnableMenuItem(ID_32773, MF_ENABLED);
	// 获取菜单的子菜单
	CMenu* SubMenu = m_Menu.GetSubMenu(0);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}


//*****************************************************************************************
// 函数名称: On32772
// 函数说明: 启动服务的函数
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CSERVICE::On32772()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hService = OpenService(hSCM, m_ServiceName, SERVICE_START | GENERIC_EXECUTE);
	if (!hSCM)
		return;
	StartService(hService, 0, NULL);
	CloseServiceHandle(hService);
	// 立刻刷新一下列表
	::SendMessage(m_hWnd, UM_Service, NULL, NULL);
}


//*****************************************************************************************
// 函数名称: On32773
// 函数说明: 关闭服务的函数
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CSERVICE::On32773()
{
	SERVICE_STATUS ssStatus;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return;
	SC_HANDLE hService = OpenService(hSCM, m_ServiceName, SERVICE_STOP | GENERIC_EXECUTE);
	ControlService(hService, SERVICE_CONTROL_STOP, &ssStatus);
	CloseServiceHandle(hService);
	// 立刻刷新一下列表
	::SendMessage(m_hWnd, UM_Service, NULL, NULL);
}

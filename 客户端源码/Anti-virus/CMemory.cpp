// CMemory.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CMemory.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "Data.h"

// CMemory 对话框

IMPLEMENT_DYNAMIC(CMemory, CDialogEx)

CMemory::CMemory(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Memory, pParent)
	, m_IntCPU(_T(""))
	, m_InitMem(_T(""))
{

}

CMemory::~CMemory()
{
}


void CMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS2, m_CPU);
	DDX_Text(pDX, IDC_EDIT3, m_IntCPU);
	DDX_Text(pDX, IDC_EDIT4, m_InitMem);
	DDX_Control(pDX, IDC_PROGRESS3, m_Mem);
}


BEGIN_MESSAGE_MAP(CMemory, CDialogEx)
	ON_MESSAGE(UM_CPU, &CMemory::OnUmCpu)
	ON_MESSAGE(UM_MEM, &CMemory::OnUmMem)
	ON_BN_CLICKED(IDC_BUTTON1, &CMemory::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMemory 消息处理程序


//*****************************************************************************************
// 函数名称: FLUSHCPU
// 函数说明: 线程回调函数，获取CPU信息，并且发送消息
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: DWORD CALLBACK
//*****************************************************************************************
DWORD64 CALLBACK FLUSHCPU(WPARAM wParam, LPARAM lParam)
{
	CMemory* _this = (CMemory*)wParam;
	while (TRUE)
	{
		SendMessage(_this->m_hWnd, UM_CPU, GetCPU(), NULL);
		Sleep(300);
	}
}

//*****************************************************************************************
// 函数名称: FLUSHMEM
// 函数说明: 线程回调函数，获取内存信息并且发送消息
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: DWORD CALLBACK
//*****************************************************************************************
DWORD64 CALLBACK FLUSHMEM(WPARAM wParam, LPARAM lParam)
{
	CMemory* _this = (CMemory*)wParam;
	while (TRUE)
	{
		SendMessage(_this->m_hWnd, UM_MEM, GetUsedMem(), NULL);
		Sleep(100);
	}
}

BOOL CMemory::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)FLUSHCPU, this, NULL,NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)FLUSHMEM, this, NULL, NULL);
	// 初始化进度条范围
	m_CPU.SetRange(0, 100);
	m_Mem.SetRange(0, 100);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//*****************************************************************************************
// 函数名称: OnUmCpu
// 函数说明: 更新CPU信息显示
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CMemory::OnUmCpu(WPARAM wParam, LPARAM lParam)
{
	m_CPU.SetPos((int)wParam);
	CString Buffer;
	CString Buffer2 = _T("%");
	Buffer.Format(_T("%d"), (int)wParam);
	Buffer = Buffer + Buffer2;
	m_IntCPU = Buffer;
	UpdateData(FALSE);
	return 0;
}


//*****************************************************************************************
// 函数名称: OnUmMem
// 函数说明: 更新内存信息的显示
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CMemory::OnUmMem(WPARAM wParam, LPARAM lParam)
{
	CString Buffer;
	CString Buffer2 = _T("GB");
	double Mem = (DWORDLONG)wParam / pow(1024, 3);
	Buffer.Format(_T("%.2f"), Mem);
	Buffer = Buffer + Buffer2;
	m_InitMem = Buffer;
	UpdateData(FALSE);
	int MemPercent = Mem * 100 / 32;
	m_Mem.SetPos(MemPercent);
	return 0;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: 内存释放
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CMemory::OnBnClickedButton1()
{
	// 清理内存
	DWORD dwPIDList[1000] = { 0 };
	DWORD BufSize = sizeof(dwPIDList);
	DWORD dwNeedSize = 0;
	// 获取所有进程的标识符
	EnumProcesses(dwPIDList, BufSize, &dwNeedSize);
	// dwNeedSize需要除以DWORD长度才得到个数
	for (DWORD64 i = 0; i < dwNeedSize / sizeof(DWORD64); i++)
	{
		// 使用SetProcessWorkingSetSize设置内存限制必须使用PROCESS_SET_QUOTA。
		HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, FALSE, dwPIDList[i]);
		// 如果dwMinimumWorkingSetSize和dwMaximumWorkingSetSize都具有（SIZE_T）–1 值，
		// 则该函数从指定进程的工作集中删除尽可能多的页面。
		SetProcessWorkingSetSize(hProcess, -1, -1);
	}
}

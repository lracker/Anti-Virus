// CDirectory.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CDirectory.h"
#include "afxdialogex.h"
#include "CExport.h"
#include "CImport.h"
#include "Data.h"
#include "CResource.h"
#include "CTLS.h"
#include "CRelocation.h"
#include "CDelay.h"


// CDirectory 对话框

IMPLEMENT_DYNAMIC(CDirectory, CDialogEx)

CDirectory::CDirectory(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIRECTORY, pParent)
	, m_ExportRVA(_T(""))
	, m_ExportSize(_T(""))
	, m_ImportRVA(_T(""))
	, m_ImportSize(_T(""))
	, m_ResourceRVA(_T(""))
	, m_ResourceSize(_T(""))
	, m_ExceptionRVA(_T(""))
	, m_ExceptionSize(_T(""))
	, m_SecurityRVA(_T(""))
	, m_SecuritySize(_T(""))
	, m_RelocationRVA(_T(""))
	, m_RelocationSize(_T(""))
	, m_DebugRVA(_T(""))
	, m_DebugSize(_T(""))
	, m_CopyrightRVA(_T(""))
	, m_CopyrightSize(_T(""))
	, m_GlobalptrRVA(_T(""))
	, m_GlobalptrSize(_T(""))
	, m_TlsRVA(_T(""))
	, m_TlsSize(_T(""))
	, m_LoadConfigRVA(_T(""))
	, m_LoadConfigSize(_T(""))
	, m_BoundImportRVA(_T(""))
	, m_BoundImportSize(_T(""))
	, m_IATRVA(_T(""))
	, m_IATSize(_T(""))
	, m_DelayImportRVA(_T(""))
	, m_DelayImportSize(_T(""))
	, m_COMRVA(_T(""))
	, m_COMSize(_T(""))
	, m_ReservedRVA(_T(""))
	, m_ReservedSize(_T(""))
{

}

CDirectory::~CDirectory()
{
}

void CDirectory::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ExportRVA);
	DDX_Text(pDX, IDC_EDIT19, m_ExportSize);
	DDX_Text(pDX, IDC_EDIT2, m_ImportRVA);
	DDX_Text(pDX, IDC_EDIT20, m_ImportSize);
	DDX_Text(pDX, IDC_EDIT17, m_ResourceRVA);
	DDX_Text(pDX, IDC_EDIT21, m_ResourceSize);
	DDX_Text(pDX, IDC_EDIT5, m_ExceptionRVA);
	DDX_Text(pDX, IDC_EDIT22, m_ExceptionSize);
	DDX_Text(pDX, IDC_EDIT18, m_SecurityRVA);
	DDX_Text(pDX, IDC_EDIT23, m_SecuritySize);
	DDX_Text(pDX, IDC_EDIT6, m_RelocationRVA);
	DDX_Text(pDX, IDC_EDIT24, m_RelocationSize);
	DDX_Text(pDX, IDC_EDIT7, m_DebugRVA);
	DDX_Text(pDX, IDC_EDIT25, m_DebugSize);
	DDX_Text(pDX, IDC_EDIT8, m_CopyrightRVA);
	DDX_Text(pDX, IDC_EDIT26, m_CopyrightSize);
	DDX_Text(pDX, IDC_EDIT9, m_GlobalptrRVA);
	DDX_Text(pDX, IDC_EDIT27, m_GlobalptrSize);
	DDX_Text(pDX, IDC_EDIT10, m_TlsRVA);
	DDX_Text(pDX, IDC_EDIT28, m_TlsSize);
	DDX_Text(pDX, IDC_EDIT11, m_LoadConfigRVA);
	DDX_Text(pDX, IDC_EDIT29, m_LoadConfigSize);
	DDX_Text(pDX, IDC_EDIT12, m_BoundImportRVA);
	DDX_Text(pDX, IDC_EDIT30, m_BoundImportSize);
	DDX_Text(pDX, IDC_EDIT13, m_IATRVA);
	DDX_Text(pDX, IDC_EDIT31, m_IATSize);
	DDX_Text(pDX, IDC_EDIT14, m_DelayImportRVA);
	DDX_Text(pDX, IDC_EDIT32, m_DelayImportSize);
	DDX_Text(pDX, IDC_EDIT15, m_COMSize);
	DDX_Text(pDX, IDC_EDIT33, m_COMSize);
	DDX_Text(pDX, IDC_EDIT16, m_ReservedRVA);
	DDX_Text(pDX, IDC_EDIT34, m_ReservedSize);
}


BEGIN_MESSAGE_MAP(CDirectory, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDirectory::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON11, &CDirectory::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CDirectory::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CDirectory::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON16, &CDirectory::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CDirectory::OnBnClickedButton17)
END_MESSAGE_MAP()


// CDirectory 消息处理程序


BOOL CDirectory::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 初始化表格里的信息
	// 首先打开内核对象
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	// 将物理地址映射到虚拟内存
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD64)pFileBuff);
	PIMAGE_FILE_HEADER pFile = (PIMAGE_FILE_HEADER)&pNt->FileHeader;
	// 如果是32位的程序的话
	if (g_nMachine == 32)
	{
		// 获取扩展头
		PIMAGE_OPTIONAL_HEADER32 pOpt = (PIMAGE_OPTIONAL_HEADER32)&pNt->OptionalHeader;
		// 开始将扩展头里的目录表内容赋值了
		// 导出表的RVA和大小
		m_ExportRVA.Format(_T("%XH"), pOpt->DataDirectory[0].VirtualAddress);
		m_dwExportRVA = pOpt->DataDirectory[0].VirtualAddress;
		m_ExportSize.Format(_T("%XH"), pOpt->DataDirectory[0].Size);
		// 导入表的RVA和大小
		m_ImportRVA.Format(_T("%XH"), pOpt->DataDirectory[1].VirtualAddress);
		m_dwImportRVA = pOpt->DataDirectory[1].VirtualAddress;
		m_ImportSize.Format(_T("%XH"), pOpt->DataDirectory[1].Size);
		// 资源表
		m_ResourceRVA.Format(_T("%XH"), pOpt->DataDirectory[2].VirtualAddress);
		m_dwResourceRVA = pOpt->DataDirectory[2].VirtualAddress;
		m_ResourceSize.Format(_T("%XH"), pOpt->DataDirectory[2].Size);
		// 例外
		m_ExceptionRVA.Format(_T("%XH"), pOpt->DataDirectory[3].VirtualAddress);
		m_ExceptionSize.Format(_T("%XH"), pOpt->DataDirectory[3].Size);
		// 安全
		m_SecurityRVA.Format(_T("%XH"), pOpt->DataDirectory[4].VirtualAddress);
		m_SecuritySize.Format(_T("%XH"), pOpt->DataDirectory[4].Size);
		// 重定位
		m_RelocationRVA.Format(_T("%XH"), pOpt->DataDirectory[5].VirtualAddress);
		m_dwRelocationRVA = pOpt->DataDirectory[5].VirtualAddress;
		m_RelocationSize.Format(_T("%XH"), pOpt->DataDirectory[5].Size);
		// 调试
		m_DebugRVA.Format(_T("%XH"), pOpt->DataDirectory[6].VirtualAddress);
		m_DebugSize.Format(_T("%XH"), pOpt->DataDirectory[6].Size);
		// 版权
		m_CopyrightRVA.Format(_T("%XH"), pOpt->DataDirectory[7].VirtualAddress);
		m_CopyrightSize.Format(_T("%XH"), pOpt->DataDirectory[7].Size);
		// 全局指针
		m_GlobalptrRVA.Format(_T("%XH"), pOpt->DataDirectory[8].VirtualAddress);
		m_GlobalptrSize.Format(_T("%XH"), pOpt->DataDirectory[8].Size);
		// TLS表
		m_TlsRVA.Format(_T("%XH"), pOpt->DataDirectory[9].VirtualAddress);
		m_dwTLSRVA = pOpt->DataDirectory[9].VirtualAddress;
		m_TlsSize.Format(_T("%XH"), pOpt->DataDirectory[9].Size);
		// 载入配置
		m_LoadConfigRVA.Format(_T("%XH"), pOpt->DataDirectory[10].VirtualAddress);
		m_LoadConfigSize.Format(_T("%XH"), pOpt->DataDirectory[10].Size);
		// 载入范围
		m_BoundImportRVA.Format(_T("%XH"), pOpt->DataDirectory[11].VirtualAddress);
		m_BoundImportSize.Format(_T("%XH"), pOpt->DataDirectory[11].Size);
		// IAT
		m_IATRVA.Format(_T("%XH"), pOpt->DataDirectory[12].VirtualAddress);
		m_IATSize.Format(_T("%XH"), pOpt->DataDirectory[12].Size);
		// 延迟输入
		m_DelayImportRVA.Format(_T("%XH"), pOpt->DataDirectory[13].VirtualAddress);
		m_dwDelayRVA = pOpt->DataDirectory[13].VirtualAddress;
		m_DelayImportSize.Format(_T("%XH"), pOpt->DataDirectory[13].Size);
		// COM
		m_COMRVA.Format(_T("%XH"), pOpt->DataDirectory[14].VirtualAddress);
		m_COMSize.Format(_T("%XH"), pOpt->DataDirectory[14].Size);
		// 保留
		m_ReservedRVA.Format(_T("%XH"), pOpt->DataDirectory[15].VirtualAddress);
		m_ReservedSize.Format(_T("%XH"), pOpt->DataDirectory[15].Size);
	}
	// 如果是64位的程序的话
	else if (g_nMachine == 64)
	{
		// 获取扩展头
		PIMAGE_OPTIONAL_HEADER64 pOpt = (PIMAGE_OPTIONAL_HEADER64)&pNt->OptionalHeader;
		// 开始将扩展头里的目录表内容赋值了
		// 导出表的RVA和大小
		m_ExportRVA.Format(_T("%XH"), pOpt->DataDirectory[0].VirtualAddress);
		m_dwExportRVA = pOpt->DataDirectory[0].VirtualAddress;
		m_ExportSize.Format(_T("%XH"), pOpt->DataDirectory[0].Size);
		// 导入表的RVA和大小
		m_ImportRVA.Format(_T("%XH"), pOpt->DataDirectory[1].VirtualAddress);
		m_dwImportRVA = pOpt->DataDirectory[1].VirtualAddress;
		m_ImportSize.Format(_T("%XH"), pOpt->DataDirectory[1].Size);
		// 资源表
		m_ResourceRVA.Format(_T("%XH"), pOpt->DataDirectory[2].VirtualAddress);
		m_dwResourceRVA = pOpt->DataDirectory[2].VirtualAddress;
		m_ResourceSize.Format(_T("%XH"), pOpt->DataDirectory[2].Size);
		// 例外
		m_ExceptionRVA.Format(_T("%XH"), pOpt->DataDirectory[3].VirtualAddress);
		m_ExceptionSize.Format(_T("%XH"), pOpt->DataDirectory[3].Size);
		// 安全
		m_SecurityRVA.Format(_T("%XH"), pOpt->DataDirectory[4].VirtualAddress);
		m_SecuritySize.Format(_T("%XH"), pOpt->DataDirectory[4].Size);
		// 重定位
		m_RelocationRVA.Format(_T("%XH"), pOpt->DataDirectory[5].VirtualAddress);
		m_dwRelocationRVA = pOpt->DataDirectory[5].VirtualAddress;
		m_RelocationSize.Format(_T("%XH"), pOpt->DataDirectory[5].Size);
		// 调试
		m_DebugRVA.Format(_T("%XH"), pOpt->DataDirectory[6].VirtualAddress);
		m_DebugSize.Format(_T("%XH"), pOpt->DataDirectory[6].Size);
		// 版权
		m_CopyrightRVA.Format(_T("%XH"), pOpt->DataDirectory[7].VirtualAddress);
		m_CopyrightSize.Format(_T("%XH"), pOpt->DataDirectory[7].Size);
		// 全局指针
		m_GlobalptrRVA.Format(_T("%XH"), pOpt->DataDirectory[8].VirtualAddress);
		m_GlobalptrSize.Format(_T("%XH"), pOpt->DataDirectory[8].Size);
		// TLS表
		m_TlsRVA.Format(_T("%XH"), pOpt->DataDirectory[9].VirtualAddress);
		m_dwTLSRVA = pOpt->DataDirectory[9].VirtualAddress;
		m_TlsSize.Format(_T("%XH"), pOpt->DataDirectory[9].Size);
		// 载入配置
		m_LoadConfigRVA.Format(_T("%XH"), pOpt->DataDirectory[10].VirtualAddress);
		m_dwDelayRVA = pOpt->DataDirectory[10].VirtualAddress;
		m_LoadConfigSize.Format(_T("%XH"), pOpt->DataDirectory[10].Size);
		// 载入范围
		m_BoundImportRVA.Format(_T("%xH"), pOpt->DataDirectory[11].VirtualAddress);
		m_BoundImportSize.Format(_T("%xH"), pOpt->DataDirectory[11].Size);
		// IAT
		m_IATRVA.Format(_T("%XH"), pOpt->DataDirectory[12].VirtualAddress);
		m_IATSize.Format(_T("%XH"), pOpt->DataDirectory[12].Size);
		// 延迟输入
		m_DelayImportRVA.Format(_T("%XH"), pOpt->DataDirectory[13].VirtualAddress);
		m_dwDelayRVA = pOpt->DataDirectory[13].VirtualAddress;
		m_DelayImportSize.Format(_T("%XH"), pOpt->DataDirectory[13].Size);
		// COM
		m_COMRVA.Format(_T("%XH"), pOpt->DataDirectory[14].VirtualAddress);
		m_COMSize.Format(_T("%XH"), pOpt->DataDirectory[14].Size);
		// 保留
		m_ReservedRVA.Format(_T("%XH"), pOpt->DataDirectory[15].VirtualAddress);
		m_ReservedSize.Format(_T("%XH"), pOpt->DataDirectory[15].Size);
	}
	UpdateData(FALSE);
	CloseHandle(hFileMap);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: 点开查看输出表详细信息
// 作    者: lracker
// 时    间: 2019/10/16
// 返 回 值: void
//*****************************************************************************************
void CDirectory::OnBnClickedButton1()
{
	if (m_ExportSize == _T("0H"))
	{
		MessageBox(_T("没有输出表"));
		return;
	}
	CExport obj;
	obj.m_dwExportRVA = m_dwExportRVA;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton11
// 函数说明: 点开查看输入表详细信息
// 作    者: lracker
// 时    间: 2019/10/16
// 返 回 值: void
//*****************************************************************************************
void CDirectory::OnBnClickedButton11()
{
	if (m_ImportSize == _T("0H"))
	{
		MessageBox(_T("没有输入表"));
		return;
	}
	CImport obj;
	obj.m_dwImportRVA = m_dwImportRVA;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton12
// 函数说明: 点开查看资源详细信息
// 作    者: lracker
// 时    间: 2019/10/16
// 返 回 值: void
//*****************************************************************************************
void CDirectory::OnBnClickedButton12()
{
	if (m_ResourceSize == _T("0H"))
	{
		MessageBox(_T("没有资源表"));
		return;
	}
	CResource obj;
	obj.m_dwResourceRVA = m_dwResourceRVA;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton13
// 函数说明: 重定位表的
// 作    者: lracker
// 时    间: 2019/10/16
// 返 回 值: void
//*****************************************************************************************
void CDirectory::OnBnClickedButton13()
{
	if (m_RelocationSize == _T("0H"))
	{
		MessageBox(_T("没有重定位表"));
		return;
	}
	CRelocation obj;
	obj.m_dwRelocationRVA = m_dwRelocationRVA;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton16
// 函数说明: 点击查看TLS详情
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CDirectory::OnBnClickedButton16()
{
	if (m_TlsSize == _T("0H"))
	{
		MessageBox(_T("没有TLS表"));
		return;
	}
	CTLS obj;
	obj.m_dwTLSRVA = m_dwTLSRVA;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton17
// 函数说明: 点击查看延迟输入表
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CDirectory::OnBnClickedButton17()
{
	if (m_DelayImportSize == _T("0H"))
	{
		MessageBox(_T("没有延迟表"));
		return;
	}
	CDelay obj;
	obj.m_dwDelayRVA = m_dwDelayRVA;
	obj.DoModal();
}

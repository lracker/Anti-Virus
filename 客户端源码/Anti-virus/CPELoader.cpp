// CPELoader.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CPELoader.h"
#include "afxdialogex.h"
#include "Data.h"
#include <winnt.h>
#include "CSection.h"
#include "CDirectory.h"


// CPELoader 对话框

IMPLEMENT_DYNAMIC(CPELoader, CDialogEx)

CPELoader::CPELoader(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PELOADER, pParent)
	, m_EntryPoint(_T(""))
	, m_ImageBase(_T(""))
	, m_SizeOfImage(_T(""))
	, m_BaseOfCode(_T(""))
	, m_BaseOfData(_T(""))
	, m_SectionAlignment(_T(""))
	, m_FileAlignment(_T(""))
	, m_Magic(_T(""))
	, m_Subsystem(_T(""))
	, m_NumberOfSections(_T(""))
	, m_TimeDateStamp(_T(""))
	, m_SizeOfHeaders(_T(""))
	, m_Characterisics(_T(""))
	, m_Checksum(_T(""))
	, m_SizeOfOptionalHander(_T(""))
	, m_NumOfRvaAndSizes(_T(""))
	, m_FilePATH(_T(""))
	, m_BIT(_T(""))
{
	m_hFileMap = NULL;
}

CPELoader::~CPELoader()
{
}

void CPELoader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_EntryPoint);
	DDX_Text(pDX, IDC_EDIT2, m_ImageBase);
	DDX_Text(pDX, IDC_EDIT3, m_SizeOfImage);
	DDX_Text(pDX, IDC_EDIT4, m_BaseOfCode);
	DDX_Text(pDX, IDC_EDIT5, m_BaseOfData);
	DDX_Text(pDX, IDC_EDIT6, m_SectionAlignment);
	DDX_Text(pDX, IDC_EDIT7, m_FileAlignment);
	DDX_Text(pDX, IDC_EDIT8, m_Magic);
	DDX_Text(pDX, IDC_EDIT9, m_Subsystem);
	DDX_Text(pDX, IDC_EDIT10, m_NumberOfSections);
	DDX_Text(pDX, IDC_EDIT11, m_TimeDateStamp);
	DDX_Text(pDX, IDC_EDIT12, m_SizeOfHeaders);
	DDX_Text(pDX, IDC_EDIT13, m_Characterisics);
	DDX_Text(pDX, IDC_EDIT14, m_Checksum);
	DDX_Text(pDX, IDC_EDIT15, m_SizeOfOptionalHander);
	DDX_Text(pDX, IDC_EDIT16, m_NumOfRvaAndSizes);
	DDX_Text(pDX, IDC_EDIT17, m_FilePATH);
	DDX_Text(pDX, IDC_EDIT18, m_BIT);
}


BEGIN_MESSAGE_MAP(CPELoader, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPELoader::OnBnClickedButton1)
	ON_MESSAGE(UM_PESHOW, &CPELoader::OnUmPeshow)
	ON_BN_CLICKED(IDC_BUTTON2, &CPELoader::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPELoader::OnBnClickedButton3)
END_MESSAGE_MAP()


// CPELoader 消息处理程序


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: 打开PE文件按钮
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CPELoader::OnBnClickedButton1()
{
	// 假如之前已经打开过文件了，那就关闭这个句柄，这样文件的内核对象引用次数减为0
	if (m_hFileMap)
		CloseHandle(m_hFileMap);
	g_PEPath.Empty();
	static TCHAR BASED_CODE szFilter[] = _T("应用文件 (*.exe)|*.exe|")
		_T("链接库文件 (*.dll)|*.dll|")
		_T("驱动文件 (*.sys)|*.sys|All Files (*.*)|*.*||");
	CFileDialog PE(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szFilter, NULL);
	if (PE.DoModal() == IDOK)
	{
		g_PEPath = PE.GetPathName();
		// 首先检查一下是不是PE文件，
		// 两点。MZ和PE两个标志
		// 创建或者打开文件内核对象
		HANDLE hFile = CreateFile(g_PEPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		// 创建一个文件映射内核对象
		// 获取大小
		int nSize = GetFileSize(hFile, NULL);
		m_hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, nSize, _T("PE"));
		// 将物理地址映射到虚拟地址
		m_FileBuff = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		PIMAGE_DOS_HEADER pFile = (PIMAGE_DOS_HEADER)m_FileBuff;
		if (pFile->e_magic != IMAGE_DOS_SIGNATURE)
		{
			MessageBox(L"这不是PE文件");
			CloseHandle(hFile);
			CloseHandle(m_hFileMap);
			m_FileBuff = nullptr;
			pFile = nullptr;
			return;
		}
		// 检查NT头部
		PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFile->e_lfanew + (LONGLONG)pFile);
		if (pNt->Signature != IMAGE_NT_SIGNATURE)
		{
			MessageBox(L"这不是PE文件");
			CloseHandle(hFile);
			CloseHandle(m_hFileMap);
			m_FileBuff = nullptr;
			pFile = nullptr;
			return;
		}
		// 当打开一个PE文件则发送消息让PE有关的信息显示出来
		::SendMessage(m_hWnd, UM_PESHOW, NULL, NULL);
		m_FilePATH = g_PEPath;
		CloseHandle(hFile);
		m_FileBuff = nullptr;
		pFile = nullptr;
		UpdateData(FALSE);
	}
}


//*****************************************************************************************
// 函数名称: OnUmPeshow
// 函数说明: 当打开完一个PE文件，则立刻更新数值显示
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CPELoader::OnUmPeshow(WPARAM wParam, LPARAM lParam)
{
	// 上面已经检测过了，是PE文件了，所以接下来开始分析PE头了
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_FileBuff;
	// 根据DOS头部最后一个参数获取pNt的位置
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD64)pDos);
	// 获取文件头
	PIMAGE_FILE_HEADER pFile = (PIMAGE_FILE_HEADER)&pNt->FileHeader;
	// 获取该PE文件是32位还是64位,.MakeUpper()用于转换字符为大写
	// 32位的情况下
	if (pFile->Machine == IMAGE_FILE_MACHINE_I386)
	{
		// 32位系统
		g_nMachine = 32;
		m_BIT = L"32位程序";
		// 根据Nt头，获取Option头的位置
		PIMAGE_OPTIONAL_HEADER32 pOpt = (PIMAGE_OPTIONAL_HEADER32)&pNt->OptionalHeader;
		// 入口点
		m_EntryPoint.Format(_T("%XH"), pOpt->AddressOfEntryPoint);
		// ImageBase
		m_ImageBase.Format(_T("%XH"), pOpt->ImageBase);
		// SizeOfImage
		m_SizeOfImage.Format(_T("%XH"), pOpt->SizeOfImage);
		// BaseOfCode
		m_BaseOfCode.Format(_T("%XH"), pOpt->BaseOfCode);
		// BaseOfData
		m_BaseOfData.Format(_T("%XH"), pOpt->BaseOfData);
		// SectionAlignment
		m_SectionAlignment.Format(_T("%XH"), pOpt->SectionAlignment);
		// FileAlignment
		m_FileAlignment.Format(_T("%XH"), pOpt->FileAlignment);
		// Magic
		m_Magic.Format(_T("%XH"), pOpt->Magic);
		// 子系统
		m_Subsystem.Format(_T("%XH"), pOpt->Subsystem);
		// 区段数目
		m_NumberOfSections.Format(_T("%XH"), pFile->NumberOfSections);
		// 日期时间标志
		m_TimeDateStamp.Format(_T("%XH"), pFile->TimeDateStamp);
		// 首部大小
		m_SizeOfHeaders.Format(_T("%XH"), pOpt->SizeOfHeaders);
		// 特征值
		m_Characterisics.Format(_T("%XH"), pFile->Characteristics);
		// 校验和
		m_Checksum.Format(_T("%XH"), pOpt->CheckSum);
		// 可选头部大小
		m_SizeOfOptionalHander.Format(_T("%XH"), pFile->SizeOfOptionalHeader);
		// RVA数及大小
		m_NumOfRvaAndSizes.Format(_T("%XH"), pOpt->NumberOfRvaAndSizes);
	}
	// 64位的情况下
	if (pFile->Machine == IMAGE_FILE_MACHINE_IA64 || pFile->Machine == IMAGE_FILE_MACHINE_AMD64)
	{
		// 64位系统
		g_nMachine = 64;
		m_BIT = L"64位程序";
		// 根据Nt头，获取Option头的位置
		PIMAGE_OPTIONAL_HEADER64 pOpt = (PIMAGE_OPTIONAL_HEADER64)&pNt->OptionalHeader;
		// 入口点
		m_EntryPoint.Format(_T("%XH"), pOpt->AddressOfEntryPoint);
		// ImageBase
		m_ImageBase.Format(_T("%llXH"), pOpt->ImageBase);
		// SizeOfImage
		m_SizeOfImage.Format(_T("%XH"), pOpt->SizeOfImage);
		// BaseOfCode
		m_BaseOfCode.Format(_T("%XH"), pOpt->BaseOfCode);
		// BaseOfData,64位PE无BaseOfData
	//	m_BaseOfData.Format(_T("%lxH"), pOpt->BaseOfData);
		// SectionAlignment
		m_SectionAlignment.Format(_T("%XH"), pOpt->SectionAlignment);
		// FileAlignment
		m_FileAlignment.Format(_T("%XH"), pOpt->FileAlignment);
		// Magic
		m_Magic.Format(_T("%XH"), pOpt->Magic);
		// 子系统
		m_Subsystem.Format(_T("%XH"), pOpt->Subsystem);
		// 区段数目
		m_NumberOfSections.Format(_T("%XH"), pFile->NumberOfSections);
		// 日期时间标志
		m_TimeDateStamp.Format(_T("%XH"), pFile->TimeDateStamp);
		// 首部大小
		m_SizeOfHeaders.Format(_T("%XH"), pOpt->SizeOfHeaders);
		// 特征值
		m_Characterisics.Format(_T("%XH"), pFile->Characteristics);
		// 校验和
		m_Checksum.Format(_T("%XH"), pOpt->CheckSum);
		// 可选头部大小
		m_SizeOfOptionalHander.Format(_T("%XH"), pFile->SizeOfOptionalHeader);
		// RVA数及大小
		m_NumOfRvaAndSizes.Format(_T("%XH"), pOpt->NumberOfRvaAndSizes);
	}
	UpdateData(FALSE);
	return 0;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton2
// 函数说明: 单击目录按钮得到区段列表
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CPELoader::OnBnClickedButton2()
{
	if (g_PEPath.IsEmpty())
	{
		MessageBox(L"请先打开文件");
		return;
	}
	CSection obj;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton3
// 函数说明: 目录按钮
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CPELoader::OnBnClickedButton3()
{
	if (g_PEPath.IsEmpty())
	{
		MessageBox(L"请先打开文件");
		return;
	}
	CDirectory obj;
	obj.DoModal();
}

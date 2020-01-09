// CSection.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CSection.h"
#include "afxdialogex.h"
#include <locale.h>
#include <atlconv.h>


// CSection 对话框

IMPLEMENT_DYNAMIC(CSection, CDialogEx)

CSection::CSection(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SECTION, pParent)
{

}

CSection::~CSection()
{
}

void CSection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_SectionList);
}


BEGIN_MESSAGE_MAP(CSection, CDialogEx)
END_MESSAGE_MAP()


// CSection 消息处理程序


BOOL CSection::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 初始化列表
	DWORD64 dwOldStyle = m_SectionList.GetExtendedStyle();
	m_SectionList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT);
	CRect cRect;
	GetClientRect(cRect);
	m_SectionList.InsertColumn(0, L"名称", 0, cRect.Width() / 6);
	m_SectionList.InsertColumn(1, L"VOffset", 0, cRect.Width() / 6);
	m_SectionList.InsertColumn(2, L"VSize", 0, cRect.Width() / 6);
	m_SectionList.InsertColumn(3, L"ROffset", 0, cRect.Width() / 6);
	m_SectionList.InsertColumn(4, L"RSize", 0, cRect.Width() / 6);
	m_SectionList.InsertColumn(5, L"标志", 0, cRect.Width() / 6);
	InsertItem();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//*****************************************************************************************
// 函数名称: InsertItem
// 函数说明: 插入区段信息
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CSection::InsertItem()
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	// 将物理地址映射到虚拟地址
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD64)pFileBuff);
	PIMAGE_FILE_HEADER pFile = &pNt->FileHeader;
	// 获取区段表
	PIMAGE_SECTION_HEADER pSectionHeadr = IMAGE_FIRST_SECTION(pNt);
	for (int i = 0; i < pFile->NumberOfSections; i++)
	{
		m_SectionList.InsertItem(i, _T(""));
		// 插入区段名，注意这个区段是BYTE类型以及不是以'\0'结尾的,8个字节
		CHAR Buffer[IMAGE_SIZEOF_SHORT_NAME + 1] = { 0 };
		memcpy_s(Buffer, 8, pSectionHeadr->Name, 8);
		m_SectionList.SetItemText(i, 0, (CString)Buffer);
		CString Buffer2;
		// 插入VOffset
		Buffer2.Format(_T("%XH"), pSectionHeadr->VirtualAddress);
		m_SectionList.SetItemText(i, 1, Buffer2);
		// 插入VSize 
		Buffer2.Format(_T("%XH"), pSectionHeadr->Misc.VirtualSize);
		m_SectionList.SetItemText(i, 2, Buffer2);
		// 插入ROffset
		Buffer2.Format(_T("%XH"), pSectionHeadr->PointerToRawData);
		m_SectionList.SetItemText(i, 3, Buffer2);
		// 插入RSize
		Buffer2.Format(_T("%XH"), pSectionHeadr->SizeOfRawData);
		m_SectionList.SetItemText(i, 4, Buffer2);
		// 插入属性
		Buffer2.Format(_T("%XH"), pSectionHeadr->Characteristics);
		m_SectionList.SetItemText(i, 5, Buffer2);
		++pSectionHeadr;
	}
	CloseHandle(hFileMap);
}

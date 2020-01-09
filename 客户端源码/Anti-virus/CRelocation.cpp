// CRelocation.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CRelocation.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "Data.h"

// CRelocation 对话框

IMPLEMENT_DYNAMIC(CRelocation, CDialogEx)

CRelocation::CRelocation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RELOCATION, pParent)
{

}

CRelocation::~CRelocation()
{
}

void CRelocation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_SectionList);
	DDX_Control(pDX, IDC_LIST2, m_BlockList);
}


BEGIN_MESSAGE_MAP(CRelocation, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CRelocation::OnClickList1)
//	ON_MESSAGE(WM_BlOCK, &CRelocation::OnBlock)
ON_MESSAGE(UM_BLOCK, &CRelocation::OnUmBlock)
END_MESSAGE_MAP()


// CRelocation 消息处理程序


BOOL CRelocation::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwOldStyle = m_BlockList.GetExtendedStyle();
	m_SectionList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT);
	m_BlockList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT);
	CRect cRect;
	m_SectionList.GetClientRect(cRect);
	m_SectionList.InsertColumn(0, _T("Index"), 0, cRect.Width() / 4);
	m_SectionList.InsertColumn(1, _T("Section"), 0, cRect.Width() / 4);
	m_SectionList.InsertColumn(2, _T("RVA"), 0, cRect.Width() / 4);
	m_SectionList.InsertColumn(3, _T("Items"), 0, cRect.Width() / 4);
	m_BlockList.GetClientRect(cRect);
	m_BlockList.InsertColumn(0, _T("Index"), 0, cRect.Width() / 5);
	m_BlockList.InsertColumn(1, _T("RVA"), 0, cRect.Width() / 5);
	m_BlockList.InsertColumn(2, _T("Offset"), 0, cRect.Width() / 5);
	m_BlockList.InsertColumn(3, _T("Type"), 0, cRect.Width() / 5);
	m_BlockList.InsertColumn(4, _T("Far Address"), 0, cRect.Width() / 5);
	PIMAGE_BASE_RELOCATION pRelc = GetRelocationDirectory();
	int nIndex = 0;
	while (pRelc->SizeOfBlock)
	{
		m_SectionList.InsertItem(nIndex, _T(""));
		CString Buffer;
		Buffer.Format(_T("%d"), nIndex + 1);
		m_SectionList.SetItemText(nIndex, 0, Buffer);
		Buffer.Format(_T("%s"), GetSection(pRelc->VirtualAddress));
		m_SectionList.SetItemText(nIndex, 1, Buffer);
		Buffer.Format(_T("%X"), pRelc->VirtualAddress);
		m_SectionList.SetItemText(nIndex, 2, Buffer);
		DWORD Items = (pRelc->SizeOfBlock - 8) / 2;
		Buffer.Format(_T("%Xh/%dd"), Items, Items);
		m_SectionList.SetItemText(nIndex, 3, Buffer);
		nIndex++;
		pRelc =(PIMAGE_BASE_RELOCATION)((DWORD64)pRelc + pRelc->SizeOfBlock);
	}
	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//*****************************************************************************************
// 函数名称: GetRelocationDirectory
// 函数说明: 获取重定位表
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: PIMAGE_BASE_RELOCATION
//*****************************************************************************************
PIMAGE_BASE_RELOCATION CRelocation::GetRelocationDirectory()
{

	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_BASE_RELOCATION)(RVATOFOA(m_dwRelocationRVA) + (DWORD64)pFileBuff);
}


//*****************************************************************************************
// 函数名称: GetSection
// 函数说明: 通过RVA获取该RVA在哪个区段
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: CString
//*****************************************************************************************
CString CRelocation::GetSection(DWORD64 dwRVA)
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	// 将物理地址映射到虚拟地址
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD64)pFileBuff);
	PIMAGE_FILE_HEADER pFile = &pNt->FileHeader;
	// 获取区段表
	PIMAGE_SECTION_HEADER pSectionHeadr = IMAGE_FIRST_SECTION(pNt);
	DWORD dwCount = pFile->NumberOfSections;
	CString Buffer = _T("0");
	for (int i = 0; i < dwCount; i++)
	{
		if (dwRVA >= pSectionHeadr[i].VirtualAddress && dwRVA < pSectionHeadr[i].VirtualAddress + pSectionHeadr->SizeOfRawData)
		{
			Buffer.Format(_T("%d(\"%s\")"), i + 1, (CString)pSectionHeadr[i].Name);
		}
	}
	CloseHandle(hFileMap);
	return Buffer;
}


//*****************************************************************************************
// 函数名称: OnClickList1
// 函数说明: 单击区段表获取详细信息
// 作    者: lracker
// 时    间: 2019/10/17
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CRelocation::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	PIMAGE_BASE_RELOCATION pRelc = GetRelocationDirectory();
	for (int i = 0; i < pNMItemActivate->iItem; i++)
	{
		pRelc = (PIMAGE_BASE_RELOCATION)((DWORD64)pRelc + pRelc->SizeOfBlock);
	}
	::SendMessage(m_hWnd, UM_BLOCK, (WPARAM)pRelc, NULL);
	*pResult = 0;
}


//*****************************************************************************************
// 函数名称: OnUmBlock
// 函数说明: 获取UM_BLOCK信息，更新Block表
// 作    者: lracker
// 时    间: 2019/10/17
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CRelocation::OnUmBlock(WPARAM wParam, LPARAM lParam)
{
	m_BlockList.DeleteAllItems();
	//每一个重定位数据都是一个结构体，记录类型，与偏移
	typedef struct TYPEOFFSET {
		WORD Offset : 12;			//一页种的偏移
		WORD Type : 4;			//重定位数据类型，3表示这个数据需要重定位
	}*PTYPEOFFSET;
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_BASE_RELOCATION pRelc = (PIMAGE_BASE_RELOCATION)wParam;
	DWORD dwSize = (pRelc->SizeOfBlock - 8) / 2;
	// 获取重定位块
	PTYPEOFFSET pRelocalBlock = (PTYPEOFFSET)(pRelc + 1);
	for (int i = 0; i < dwSize; i++)
	{
		m_BlockList.InsertItem(i, _T(""));
		CString Buffer;
		Buffer.Format(_T("%d"), i + 1);
		m_BlockList.SetItemText(i, 0, Buffer);
		DWORD dwRVA = pRelocalBlock[i].Offset + pRelc->VirtualAddress;
		Buffer.Format(_T("%Xh"), dwRVA);
		m_BlockList.SetItemText(i, 1, Buffer);
		DWORD dwFOA = RVATOFOA(pRelocalBlock[i].Offset + pRelc->VirtualAddress);
		Buffer.Format(_T("%Xh"), dwFOA);
		m_BlockList.SetItemText(i, 2, Buffer);
		switch (pRelocalBlock[i].Type)
		{
		case 0x3:
		{
			Buffer = _T("HIGHLOW (3)");
			m_BlockList.SetItemText(i, 3, Buffer);
			// FarAdress
			PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)(pDos->e_lfanew + (DWORD64)pFileBuff);
			DWORD VA = *(DWORD*)(dwFOA + (DWORD64)pFileBuff) - 0x400000 + pNt->OptionalHeader.ImageBase;
			Buffer.Format(_T("%Xh"), VA);
			m_BlockList.SetItemText(i, 4, Buffer);
			break;
		}
		case 0xa:
		{
			Buffer = _T("DIR64 (10)");
			m_BlockList.SetItemText(i, 3, Buffer);
			PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pDos->e_lfanew + (DWORD64)pFileBuff);
			DWORD64 VA = *(DWORD64*)(dwFOA + (DWORD64)pFileBuff) - 0x140000000 + pNt->OptionalHeader.ImageBase;
			Buffer.Format(_T("%llXh"), VA);
			m_BlockList.SetItemText(i, 4, Buffer);
			break;
		}
		case 00:
		{
			Buffer = _T("ABSOLUTE (0)");
			m_BlockList.SetItemText(i, 0, _T("-"));
			m_BlockList.SetItemText(i, 1, _T("-"));
			m_BlockList.SetItemText(i, 2, _T("-"));
			m_BlockList.SetItemText(i, 3, Buffer);
			m_BlockList.SetItemText(i, 4, _T("-"));
			m_BlockList.SetItemText(i, 5, _T("-"));
			break;
		}
		default:
			break;
		}
	}
	CloseHandle(hFileMap);
	return 0;
}

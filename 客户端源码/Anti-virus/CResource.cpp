// CResource.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CResource.h"
#include "afxdialogex.h"
#include "Data.h"
#include "Tools.h"


// CResource 对话框

IMPLEMENT_DYNAMIC(CResource, CDialogEx)

CResource::CResource(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESOURCE, pParent)
	, m_RNumIDEntries(_T(""))
	, m_RNumNameEntries(_T(""))
	, m_SelectedRVA(_T(""))
	, m_SelectedOffset(_T(""))
	, m_SelectedSize(_T(""))
	, m_SelectedNumIDEntries(_T(""))
	, m_SelectedNumNameEntries(_T(""))
{

}

CResource::~CResource()
{
}

void CResource::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_RNumIDEntries);
	DDX_Text(pDX, IDC_EDIT2, m_RNumNameEntries);
	DDX_Control(pDX, IDC_TREE1, m_ResourceTree);
	DDX_Text(pDX, IDC_EDIT36, m_SelectedRVA);
	DDX_Text(pDX, IDC_EDIT37, m_SelectedOffset);
	DDX_Text(pDX, IDC_EDIT38, m_SelectedSize);
	DDX_Text(pDX, IDC_EDIT35, m_SelectedNumIDEntries);
	DDX_Text(pDX, IDC_EDIT17, m_SelectedNumNameEntries);
}


BEGIN_MESSAGE_MAP(CResource, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CResource::OnSelchangedTree1)
//	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CResource::OnClickTree1)
END_MESSAGE_MAP()


// CResource 消息处理程序

PIMAGE_RESOURCE_DIRECTORY CResource::GetResourceDirectory()
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_RESOURCE_DIRECTORY)(RVATOFOA(m_dwResourceRVA) + (DWORD64)pFileBuff);
}

BOOL CResource::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	// 将物理地址映射到虚拟地址
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_RESOURCE_DIRECTORY pResDirOne = GetResourceDirectory();
	// 第一层: 资源的类型是什么(图标、位图、菜单...)
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResOneEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirOne + 1);
	// 获取名称条目
	m_RNumNameEntries.Format(_T("%d"), pResDirOne->NumberOfNamedEntries);
	// 获取ID条目
	m_RNumIDEntries.Format(_T("%d"), pResDirOne->NumberOfIdEntries);
	DWORD dwOneCount = pResDirOne->NumberOfIdEntries + pResDirOne->NumberOfNamedEntries;
	for (int i = 0; i < dwOneCount; i++)
	{
		HTREEITEM hRootNode = NULL;
		// 判断资源是什么类型
		// 资源是字符串作为标识
		if (pResOneEntry[i].NameIsString)
		{
			PIMAGE_RESOURCE_DIR_STRING_U pName = (PIMAGE_RESOURCE_DIR_STRING_U)(pResOneEntry[i].NameOffset + (DWORD64)pResDirOne);
			CString strTmp = pName->NameString;
			strTmp = strTmp.Left(pName->Length);
			hRootNode = m_ResourceTree.InsertItem(strTmp);
			if (hRootNode != NULL)
				m_ResourceTree.SetItemData(hRootNode, i + 1);
		}
		// 资源是ID作为标识
		else
		{
			// 系统提供的资源类型
			if (pResOneEntry[i].Id <= 16)
			{
				hRootNode = m_ResourceTree.InsertItem(RES[pResOneEntry[i].Id-1]);
				if (hRootNode != NULL)
					m_ResourceTree.SetItemData(hRootNode, i + 1);
			}
			else
			{
				CString Buffer;
				Buffer.Format(_T("%d"), pResOneEntry[i].Id);
				hRootNode = m_ResourceTree.InsertItem(Buffer);
				if (hRootNode != NULL)
					m_ResourceTree.SetItemData(hRootNode, i + 1);
			}
		}
		// 第二层:资源叫什么名字(1.png,2.png)
		// 是否有第二层目录
		if (pResOneEntry[i].DataIsDirectory)
		{
			// 找到第二层目录表
			PIMAGE_RESOURCE_DIRECTORY pResDirTwo =
				(PIMAGE_RESOURCE_DIRECTORY)(pResOneEntry[i].OffsetToDirectory + (DWORD64)pResDirOne);
			// 找到第二层目录项
			PIMAGE_RESOURCE_DIRECTORY_ENTRY pResTwoEntry =
				(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirTwo + 1);
			// 获取这种资源有多少个
			DWORD ResTowCount = pResDirTwo->NumberOfIdEntries + pResDirTwo->NumberOfNamedEntries;
			for (int j = 0; j < ResTowCount; j++)
			{
				HTREEITEM TwoNode = NULL;
				// 资源是字符串作为标识
				if (pResTwoEntry[j].NameIsString)
				{
					// 如果NameIsString为1，则NameOffset有用
					PIMAGE_RESOURCE_DIR_STRING_U pName2 = (PIMAGE_RESOURCE_DIR_STRING_U)(pResTwoEntry[j].NameOffset + (DWORD64)pResDirOne);
					CString strTmp = pName2->NameString;
					strTmp = strTmp.Left(pName2->Length);
					TwoNode = m_ResourceTree.InsertItem(strTmp, hRootNode);
					if (TwoNode != NULL)
						m_ResourceTree.SetItemData(TwoNode, j + 1);
				}
				else
				{
					CString Buffer;
					Buffer.Format(_T("%d"), pResTwoEntry[j].Id);
					TwoNode = m_ResourceTree.InsertItem(Buffer, hRootNode);
					if (TwoNode != NULL)
						m_ResourceTree.SetItemData(TwoNode, j + 1);
				}
			}
		}

	}


	CloseHandle(hFileMap);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//*****************************************************************************************
// 函数名称: OnSelchangedTree1
// 函数说明: 响应点开的信息
// 作    者: lracker
// 时    间: 2019/10/16
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CResource::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = m_ResourceTree.GetSelectedItem();
	DWORD dwIndex = m_ResourceTree.GetItemData(hItem);
	int nCount = 0;
	while (hItem)
	{
		nCount++;
		hItem = m_ResourceTree.GetParentItem(hItem);
	}
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	// 将物理地址映射到虚拟地址
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_RESOURCE_DIRECTORY pResDirOne = GetResourceDirectory();
	// 第一层: 资源的类型是什么(图标、位图、菜单...)
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResOneEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirOne + 1);
	// 如果选择了第一层的，那么显示第二层的名称条目和ID条目
	if (nCount == 1) 
	{
		if (pResOneEntry[dwIndex - 1].DataIsDirectory)
		{
			// 找到第二层目录表
			PIMAGE_RESOURCE_DIRECTORY pResDirTwo =
				(PIMAGE_RESOURCE_DIRECTORY)(pResOneEntry[dwIndex - 1].OffsetToDirectory + (DWORD64)pResDirOne);
			m_SelectedNumIDEntries.Format(_T("%d"), pResDirTwo->NumberOfIdEntries);
			m_SelectedNumNameEntries.Format(_T("%d"), pResDirTwo->NumberOfNamedEntries);
		}
	}
	// 如果选择了第二层，那么显示第三层的RVA和偏移还有size
	if (nCount == 2)
	{
		// 就是第二层的序号
		DWORD dwPIndex = m_ResourceTree.GetItemData(m_ResourceTree.GetParentItem(m_ResourceTree.GetSelectedItem()));
		// 找到第二层目录表
		PIMAGE_RESOURCE_DIRECTORY pResDirTwo =
			(PIMAGE_RESOURCE_DIRECTORY)(pResOneEntry[dwPIndex - 1].OffsetToDirectory + (DWORD64)pResDirOne);
		// 找到第二层目录项
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pResTwoEntry =
			(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirTwo + 1);
		if (pResTwoEntry[dwIndex - 1].DataIsDirectory)
		{
			// 获取第三层目录表
			PIMAGE_RESOURCE_DIRECTORY pResDirThree =
				(PIMAGE_RESOURCE_DIRECTORY)(pResTwoEntry[dwIndex - 1].OffsetToDirectory + (DWORD64)pResDirOne);
			// 获取第三层目录项
			PIMAGE_RESOURCE_DIRECTORY_ENTRY pResThreeEntry =
				(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirThree + 1);
			// 获取资源真正信息
			PIMAGE_RESOURCE_DATA_ENTRY pResData =
				(PIMAGE_RESOURCE_DATA_ENTRY)(pResThreeEntry->OffsetToData + (DWORD64)pResDirOne);
			m_SelectedRVA.Format(_T("%XH"), pResData->OffsetToData);
			m_SelectedOffset.Format(_T("%XH"), RVATOFOA(pResData->OffsetToData));
			m_SelectedSize.Format(_T("%XH"), pResData->Size);
		}
	}
	UpdateData(FALSE);
	*pResult = 0;
}



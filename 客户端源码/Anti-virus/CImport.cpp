// CImport.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CImport.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "Data.h"

// CImport 对话框

IMPLEMENT_DYNAMIC(CImport, CDialogEx)

CImport::CImport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMPORT, pParent)
{

}

CImport::~CImport()
{
}

void CImport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_DllList);
	DDX_Control(pDX, IDC_LIST2, m_FunList);
}


BEGIN_MESSAGE_MAP(CImport, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CImport::OnClickList1)
	ON_MESSAGE(UM_FUNC, &CImport::OnUmFunc)
END_MESSAGE_MAP()


// CImport 消息处理程序


BOOL CImport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化两个列表
	DWORD64 dwOldStyle = m_DllList.GetExtendedStyle();
	m_DllList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT);
	m_FunList.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT);
	// 插入列名
	CRect cRect;
	m_DllList.GetClientRect(cRect);
	m_DllList.InsertColumn(0, L"DllName", 0, cRect.Width()/6);
	m_DllList.InsertColumn(1, L"OriginalFirstThunk", 0, cRect.Width() / 6);
	m_DllList.InsertColumn(2, L"TimeDateStamp", 0, cRect.Width() / 6);
	m_DllList.InsertColumn(3, L"ForwarderChain", 0, cRect.Width() / 6);
	m_DllList.InsertColumn(4, L"Name", 0, cRect.Width() / 6);
	m_DllList.InsertColumn(5, L"FirstThunk", 0, cRect.Width() / 6);
	m_FunList.GetClientRect(cRect);
	m_FunList.InsertColumn(0, L"ThunkRVA", 0, cRect.Width() / 5);
	m_FunList.InsertColumn(1, L"ThunkOffset", 0, cRect.Width() / 5);
	m_FunList.InsertColumn(2, L"ThunkValue", 0, cRect.Width() / 5);
	m_FunList.InsertColumn(3, L"Hint", 0, cRect.Width() / 5);
	m_FunList.InsertColumn(4, L"ApiName", 0, cRect.Width() / 5);
	// 开始遍历导入表
	PIMAGE_IMPORT_DESCRIPTOR pImport = GetImportDirectory();
	// 获取VA
	m_dwFile = (DWORD64)pImport - RVATOFOA(m_dwImportRVA);
	int nIndex = 0;
	while (pImport->OriginalFirstThunk != NULL)
	{
		CString Buffer;
		m_DllList.InsertItem(nIndex, _T(""));
		// 插入DLL名字
		CHAR* DllName = (CHAR*)(RVATOFOA(pImport->Name) + m_dwFile);
		m_DllList.SetItemText(nIndex, 0, (CString)DllName);
		// 插入OriginalFirstThunk
		Buffer.Format(_T("%XH"), pImport->OriginalFirstThunk);
		m_DllList.SetItemText(nIndex, 1, Buffer);
		// 插入TimeDateStamp
		Buffer.Format(_T("%XH"), pImport->TimeDateStamp);
		m_DllList.SetItemText(nIndex, 2, Buffer);
		// 插入ForwarderChain
		Buffer.Format(_T("%XH"), pImport->ForwarderChain);
		m_DllList.SetItemText(nIndex, 3, Buffer);
		// 插入Name
		Buffer.Format(_T("%XH"), pImport->Name);
		m_DllList.SetItemText(nIndex, 4, Buffer);
		// 插入FirstThunk
		Buffer.Format(_T("%XH"), pImport->FirstThunk);
		m_DllList.SetItemText(nIndex, 5, Buffer);
		nIndex++;
		pImport++;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

PIMAGE_IMPORT_DESCRIPTOR CImport::GetImportDirectory()
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);
	return (PIMAGE_IMPORT_DESCRIPTOR)(RVATOFOA(m_dwImportRVA) + (DWORD64)pFileBuff);
}


//*****************************************************************************************
// 函数名称: OnClickList1
// 函数说明: 单击导入表框，发送消息给下面的框框
// 作    者: lracker
// 时    间: 2019/10/16
// 参    数: NMHDR * pNMHDR
// 参    数: LRESULT * pResult
// 返 回 值: void
//*****************************************************************************************
void CImport::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 发送消息给下面的框框进行更新
	::SendMessage(m_hWnd, UM_FUNC, pNMItemActivate->iItem, NULL);
	*pResult = 0;
}


//*****************************************************************************************
// 函数名称: OnUmFunc
// 函数说明: 收到UM_FUNC消息进行更新
// 作    者: lracker
// 时    间: 2019/10/16
// 参    数: WPARAM wParam
// 参    数: LPARAM lParam
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CImport::OnUmFunc(WPARAM wParam, LPARAM lParam)
{
	m_FunList.DeleteAllItems();
	DWORD64 dwIndex = (DWORD64)wParam;
	// 开始遍历导入表
	PIMAGE_IMPORT_DESCRIPTOR pImport = GetImportDirectory();
	for (int i = 0; i < dwIndex; i++)
	{
		pImport++;
	}
	if (g_nMachine == 32)
	{
		PIMAGE_THUNK_DATA32 pIat = (PIMAGE_THUNK_DATA32)(RVATOFOA(pImport->FirstThunk) + (DWORD64)m_dwFile);
		ULONG ThunkRVA = pImport->OriginalFirstThunk;
		int i = 0;
		while (pIat->u1.Ordinal)
		{
			CString Buffer;
			m_FunList.InsertItem(i, _T(""));
			// 插入RVA
			Buffer.Format(_T("%XH"), ThunkRVA);
			m_FunList.SetItemText(i, 0, Buffer);
			// 插入FOA
			Buffer.Format(_T("%XH"), RVATOFOA(ThunkRVA));
			m_FunList.SetItemText(i, 1, Buffer);
			// 插入value
			Buffer.Format(_T("%XH"), pIat->u1.Function);
			m_FunList.SetItemText(i, 2, Buffer);
			// 判断导入的函数是名称导入还是序号导入
			// 判断最高位是0还是1，1的话是序号导入
			// 序号导入
			if (pIat->u1.Ordinal & 0x80000000)
			{
				m_FunList.SetItemText(i, 3, _T("-"));
				Buffer.Format(_T("Ordinal:  %Xh  %dd"), pIat->u1.Function & 0x7FFFFFFF, pIat->u1.Function & 0x7FFFFFFF);
			}
			// 名称导入
			else
			{
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(RVATOFOA(pIat->u1.AddressOfData) + m_dwFile);
				Buffer.Format(_T("%Xh"), pName->Hint);
				m_FunList.SetItemText(i, 3, Buffer);
				m_FunList.SetItemText(i, 4, (CString)(pName->Name));
			}
			pIat++;
			ThunkRVA += 4;
			i++;
		}


	}
	else if (g_nMachine == 64)
	{
		PIMAGE_THUNK_DATA64 pIat = (PIMAGE_THUNK_DATA64)(RVATOFOA(pImport->FirstThunk) + (DWORD64)m_dwFile);
		ULONG ThunkRVA = pImport->OriginalFirstThunk;
		int i = 0;
		while (pIat->u1.Ordinal)
		{
			CString Buffer;
			m_FunList.InsertItem(i, _T(""));
			// 插入RVA
			Buffer.Format(_T("%llXH"), ThunkRVA);
			m_FunList.SetItemText(i, 0, Buffer);
			// 插入FOA
			Buffer.Format(_T("%llXH"), RVATOFOA(ThunkRVA));
			m_FunList.SetItemText(i, 1, Buffer);
			// 插入value
			Buffer.Format(_T("%llXH"), pIat->u1.Function);
			m_FunList.SetItemText(i, 2, Buffer);
			// 判断导入的函数是名称导入还是序号导入
			// 判断最高位是0还是1，1的话是序号导入
			// 序号导入
			if (pIat->u1.Ordinal & 0x8000000000000000)
			{
				CString tmp = _T("-");
				m_FunList.SetItemText(i, 3, tmp);
				Buffer.Format(_T("Ordinal:  %Xh  %dd"), pIat->u1.Function & 0x7FFFFFFFFFFFFFFF, pIat->u1.Function & 0x7FFFFFFFFFFFFFFF);
				m_FunList.SetItemText(i, 4, Buffer);
			}
			// 名称导入
			else
			{
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(RVATOFOA(pIat->u1.AddressOfData) + m_dwFile);
				Buffer.Format(_T("%Xh"), pName->Hint);
				m_FunList.SetItemText(i, 3, Buffer);
				m_FunList.SetItemText(i, 4, (CString)(pName->Name));
			}
			pIat++;
			ThunkRVA += 8;
			i++;
		}
	}
	return 0;
}

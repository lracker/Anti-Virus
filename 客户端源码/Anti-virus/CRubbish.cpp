// CRubbish.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CRubbish.h"
#include "afxdialogex.h"
#include "Tools.h"


// CRubbish 对话框

IMPLEMENT_DYNAMIC(CRubbish, CDialogEx)

CRubbish::CRubbish(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RUBBISH, pParent)
	, m_RecycleBin(_T(""))
	, m_IERubbish(_T(""))
	, m_VSFolder(_T(""))
	, m_VSNum(_T(""))
	, m_suffix(_T(""))
	, m_CustomFolder(_T(""))
	, m_CustomNum(_T(""))
{
	RecycleScan = FALSE;
	IEScan = FALSE;
	VSScan = FALSE;
	VSGetFloder = FALSE;
	CustomFloder = FALSE;
	CustomScan = FALSE;
}

CRubbish::~CRubbish()
{
}

void CRubbish::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_RecycleBin);
	DDX_Text(pDX, IDC_EDIT2, m_IERubbish);
	DDX_Text(pDX, IDC_EDIT3, m_VSFolder);
	DDX_Text(pDX, IDC_EDIT4, m_VSNum);
	DDX_Text(pDX, IDC_EDIT5, m_suffix);
	DDX_Text(pDX, IDC_EDIT6, m_CustomFolder);
	DDX_Text(pDX, IDC_EDIT7, m_CustomNum);
}


BEGIN_MESSAGE_MAP(CRubbish, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON6, &CRubbish::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON2, &CRubbish::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON7, &CRubbish::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON3, &CRubbish::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON9, &CRubbish::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON4, &CRubbish::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, &CRubbish::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON10, &CRubbish::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON5, &CRubbish::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON1, &CRubbish::OnBnClickedButton1)
END_MESSAGE_MAP()


// CRubbish 消息处理程序


//*****************************************************************************************
// 函数名称: OnBnClickedButton6
// 函数说明: 扫描系统垃圾，也就是扫描回收站的垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton6()
{
	// 初始化SHQUERYRBINFO结构体
	SHQUERYRBINFO RecycleBinInformation = { sizeof(SHQUERYRBINFO) };
	// 查询回收站信息,第一个参数为NULL代表了回收站
	if (SHQueryRecycleBin(NULL, &RecycleBinInformation) == S_OK)
	{
		CString Buffer;
		DOUBLE RubbishSize = RecycleBinInformation.i64Size / pow(1024, 2);
		Buffer.Format(_T("回收站垃圾大小:%.3fMB"), RubbishSize);
		m_RecycleBin = Buffer;
		UpdateData(FALSE);
		// 设置为已经扫描过回收站了
		RecycleScan = TRUE;
	}
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton2
// 函数说明: 清空回收站
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton2()
{
	// 需要扫描过以后才能清理空间。
	if (!RecycleScan)
		MessageBox(L"需要扫描以后才能清理！");
	else
	{
		SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
		MessageBox(L"清理成功");
	}
	
	// 重置垃圾大小显示
	m_RecycleBin.Empty();
	UpdateData(FALSE);
	RecycleScan = FALSE;

}



//*****************************************************************************************
// 函数名称: OnBnClickedButton7
// 函数说明: 扫描浏览器的垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton7()
{
	TCHAR IEPath[MAX_PATH] =_T("C:\\Users\\Canary\\AppData\\Local\\Microsoft\\Windows\\INetCache\\IE");
	int nCount = IEClean(IEPath,m_IERubbishPath);
	m_IERubbish.Format(_T("%d个垃圾文件"),nCount);
	UpdateData(FALSE);
	IEScan = TRUE;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton3
// 函数说明: 清理浏览器垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton3()
{
	// 需要扫描过以后才能清理空间。
	if (!IEScan)
		MessageBox(L"需要扫描以后才能清理！");
	else
	{
		for (auto& i : m_IERubbishPath)
			DeleteFile(i);
		MessageBox(L"清理成功");
	}
	// 重置垃圾数目显示
	m_IERubbish.Empty();
	UpdateData(FALSE);
	IEScan = FALSE;

}


//*****************************************************************************************
// 函数名称: OnBnClickedButton9
// 函数说明: 选择目录
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton9()
{
	CFolderPickerDialog fd(NULL, 0, this, 0);
	if (fd.DoModal() == IDOK)
		m_VSFolder = fd.GetPathName();
	UpdateData(FALSE);
	VSGetFloder = TRUE;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton4
// 函数说明: 清理VS垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton4()
{
	// 需要扫描过以后才能清理空间。
	if (!VSScan)
		MessageBox(L"需要扫描以后才能清理！");
	else
	{
		for (auto& i : m_VSpaths)
			DeleteFile(i);
		MessageBox(L"清理成功");
	}
	// 重置显示
	m_VSNum.Empty();
	m_VSFolder.Empty();
	UpdateData(FALSE);
	VSGetFloder = FALSE;
	VSScan = FALSE;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton8
// 函数说明: 根据路径扫描出VS垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton8()
{
	if (!VSGetFloder)
	{
		MessageBox(L"需要选择目录才能扫描");
		return;
	}
	// 首先清空容器
	m_VSpaths.clear();
	// 根据VS的后缀名切割压入容器
	vector<CString> extName;
	CString strSource = _T(".ilk, .pdb, .obj, .log, .pch, .tlog, .lastbuildstate, .sdf, .idb, .ipch, .res, .o, .lst, .knl, .img, .bin, .db");
	// 分隔符
	CString ch = _T(", ");
	CString strTmp;
	int nPos = 0;
	strTmp = strSource.Tokenize(ch, nPos);
	// 将后缀名压入容器
	while (strTmp.Trim() != _T(""))
	{
		extName.push_back(strTmp);
		strTmp = strSource.Tokenize(ch, nPos);
	}
	// 然后根据VS文件夹的路径进行扫描
	GetRubbish(m_VSFolder, m_VSpaths, extName);
	int nSize = m_VSpaths.size();
	m_VSNum.Format(L"%d个垃圾", nSize);
	UpdateData(FALSE);
	VSScan = TRUE;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton10
// 函数说明: 选择目录
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton10()
{
	CFolderPickerDialog fd(NULL, 0, this, 0);
	if (fd.DoModal() == IDOK)
		m_CustomFolder = fd.GetPathName();
	UpdateData(FALSE);
	CustomFloder = TRUE;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton5
// 函数说明: 自定义扫描
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton5()
{
	// 首先先清空容器
	m_CustomPaths.clear();
	UpdateData(TRUE);
	// 根据VS的后缀名切割压入容器
	vector<CString> extName;
	// 分隔符
	CString ch = _T(", ");
	CString strTmp;
	int nPos = 0;
	strTmp = m_suffix.Tokenize(ch, nPos);
	// 将后缀名压入容器
	while (strTmp.Trim() != _T(""))
	{
		extName.push_back(strTmp);
		strTmp = m_suffix.Tokenize(ch, nPos);
	}
	GetRubbish(m_CustomFolder, m_CustomPaths, extName);
	int nSize = m_CustomPaths.size();
	m_CustomNum.Format(L"%d", nSize);
	UpdateData(FALSE);
	CustomScan = TRUE;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: 清理自定义垃圾
// 作    者: lracker
// 时    间: 2019/10/15
// 返 回 值: void
//*****************************************************************************************
void CRubbish::OnBnClickedButton1()
{
	// 需要扫描过以后才能清理空间。
	if (!CustomScan)
		MessageBox(L"需要扫描以后才能清理！");
	else
	{
		for (auto& i : m_CustomPaths)
			DeleteFile(i);
		MessageBox(L"清理成功");
	}
	// 重置显示
	m_CustomNum.Empty();
	m_CustomFolder.Empty();
	UpdateData(FALSE);
	CustomFloder = FALSE;
	CustomScan = FALSE;
}

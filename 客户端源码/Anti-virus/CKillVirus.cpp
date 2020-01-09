// CKillVirus.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CKillVirus.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "Data.h"
#include "CWhiteList.h"
#include "CDoubt.h"
#include "CSocket.h"
#include "Md5.h"
#include <Shlwapi.h>


// CKillVirus 对话框

IMPLEMENT_DYNAMIC(CKillVirus, CDialogEx)

CKillVirus::CKillVirus(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KILLVIRUS, pParent)
	, m_MD5FilePath(_T(""))
	, m_IsVirus(_T(""))
	, m_ALLPath(_T(""))
	, m_VirusNum(_T(""))
{
	// 读取白名单到全局变量里面
	ReadWhiteList();
}

CKillVirus::~CKillVirus()
{
}

void CKillVirus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_MD5FilePath);
	DDX_Text(pDX, IDC_EDIT2, m_IsVirus);
	DDX_Text(pDX, IDC_EDIT35, m_ALLPath);
	DDX_Text(pDX, IDC_EDIT17, m_VirusNum);
}


BEGIN_MESSAGE_MAP(CKillVirus, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CKillVirus::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CKillVirus::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CKillVirus::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON11, &CKillVirus::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON4, &CKillVirus::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CKillVirus::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON12, &CKillVirus::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON6, &CKillVirus::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CKillVirus::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CKillVirus::OnBnClickedButton8)
END_MESSAGE_MAP()


// CKillVirus 消息处理程序


//*****************************************************************************************
// 函数名称: OnBnClickedButton1
// 函数说明: MD5查杀选择文件
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton1()
{
	static TCHAR BASED_CODE szFilter[] = _T("All Files (*.*)|*.*||");
	CFileDialog File(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szFilter, NULL);
	if (File.DoModal() == IDOK)
	{
		m_File = File.GetPathName();
		m_MD5FilePath = m_File;
		UpdateData(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	}
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton2
// 函数说明: 扫毒
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton2()
{
	// 这里应该先打开木马库获取里面的所有特征码
	HANDLE hFile = CreateFile(g_VirusLibrary, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// 获取文件的大小
	DWORD dwSize = GetFileSize(hFile, 0);
	// 动态申请空间,TCHAR占两个字节
	char* Library = new char[dwSize]();
	ReadFile(hFile, Library, dwSize, &dwSize, NULL);
	// 扫描该文件是否有毒
	USES_CONVERSION;
	if (VirusScan(T2A(m_File), Library))
	{
		// 该文件有毒
		m_IsVirus = _T("该文件有毒");
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	}
	else
	{
		m_IsVirus = _T("该文件没有毒");
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
	// 关闭文件句柄
	CloseHandle(hFile);
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton3
// 函数说明: 选择杀毒
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton3()
{
	// 确定删除该文件
	DeleteFile(m_File);
	m_IsVirus.Empty();
	m_MD5FilePath.Empty();
	MessageBox(_T("杀毒完毕"));
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	UpdateData(FALSE);
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton11
// 函数说明: 选择目录
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton11()
{
	CFolderPickerDialog fd(NULL, 0, this, 0);
	if (fd.DoModal() == IDOK)
	{
		m_Folder = fd.GetPathName();
		// 将路径显示出来
		m_ALLPath = m_Folder;
		GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
		UpdateData(FALSE);
	}
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton4
// 函数说明: 开始扫描路径下所有的文件
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton4()
{
	// 这里应该先打开木马库获取里面的所有特征码
	HANDLE hFile = CreateFile(g_VirusLibrary, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// 获取文件的大小
	DWORD dwSize = GetFileSize(hFile, 0);
	// 动态申请空间,TCHAR占两个字节
	char* Library = new char[dwSize]();
	ReadFile(hFile, Library, dwSize, &dwSize, NULL);
	EnumFold(m_Folder, m_FileVector);
	for (auto& i : m_FileVector)
	{
		USES_CONVERSION;
		if (VirusScan(T2A(i), Library))
		{
			// 将有毒的文件压入容器中
			m_Virus.push_back(i);
		}
	}
	if (m_Virus.size())
	{
		// 然后显示数量
		m_VirusNum.Format(_T("病毒数目为:%d"), m_Virus.size());
		GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
	}
	else
	{
		m_VirusNum = _T("病毒数目为:0");
	}
	UpdateData(FALSE);
	return;
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton5
// 函数说明: 全盘的杀毒
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton5()
{
	for (auto& i : m_Virus)
	{
		DeleteFile(i);
	}
	MessageBox(_T("杀毒完毕"));
	m_ALLPath.Empty();
	m_VirusNum.Empty();
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	UpdateData(FALSE);
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton12
// 函数说明: 查看白名单
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton12()
{
	CWhiteList obj;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton6
// 函数说明: 根据白名单进行扫描
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton6()
{
	CDoubt obj;
	obj.DoModal();
}


BOOL CKillVirus::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton7
// 函数说明: 点击下载新的病毒库文件
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton7()
{
	// 连接服务器
	CSocket socket;
	socket.connect();
	// 连接以后要发送请求
	socket.Send(UPDATE);
	// 发送完以后要接收信息
	RECVDATASTRUCT* pRecv = NULL;
	pRecv = socket.Recv();
	char* tmp = pRecv->data;
	tmp += 3;
	CString Buffer(tmp);
	CString End(g_End);
	Buffer = Buffer + '\n' +  End;
	FILE* pFile = NULL;
	USES_CONVERSION;
	fopen_s(&pFile, T2A(g_VirusLibrary), "wb+");
	fwrite(T2A(Buffer), Buffer.GetLength(), 1, pFile);
	fclose(pFile);
	socket.Close();
	MessageBox(L"更新成功");
}


//*****************************************************************************************
// 函数名称: OnBnClickedButton8
// 函数说明: 提交样本
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void CKillVirus::OnBnClickedButton8()
{
	static TCHAR BASED_CODE szFilter[] = _T("All Files (*.*)|*.*||");
	CFileDialog File(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szFilter, NULL);
	if (File.DoModal() == IDOK)
	{
		m_DoubtFile = File.GetPathName();
		UpdateData(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	}
	// 获取文件 MD5
	USES_CONVERSION;
	char* FileMD5 = md5FileValue(T2A(m_DoubtFile));
	WCHAR* tmp = m_DoubtFile.GetBuffer();
	PathStripPath(tmp);
	CString Buffer1(FileMD5);
	CString Buffer2(tmp);
	Buffer1 += '\n' + Buffer2;
	// 连接服务器
	CSocket socket;
	socket.connect();
	// 连接以后要发送请求
	socket.Send(UPLOAD, T2A(Buffer1));
	socket.Close();
	MessageBox(L"提交成功");
}

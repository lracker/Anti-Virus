#pragma once
#include <vector>
using std::vector;

// CKillVirus 对话框

class CKillVirus : public CDialogEx
{
	DECLARE_DYNAMIC(CKillVirus)

public:
	CKillVirus(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CKillVirus();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KILLVIRUS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CString m_DoubtFile;
	CString m_File;
	CString m_MD5FilePath;
	afx_msg void OnBnClickedButton2();
	CString m_IsVirus;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton11();
	CString m_Folder;
	CString m_ALLPath;
	afx_msg void OnBnClickedButton4();
	vector<CString> m_FileVector;
	vector<CString> m_Virus;
	CString m_VirusNum;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton6();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
};

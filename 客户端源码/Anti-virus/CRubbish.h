#pragma once
#include <vector>
using std::vector;

// CRubbish 对话框

class CRubbish : public CDialogEx
{
	DECLARE_DYNAMIC(CRubbish)

private:
	BOOL RecycleScan;
	BOOL IEScan;
	BOOL VSScan;
	BOOL VSGetFloder;
	BOOL CustomFloder;
	BOOL CustomScan;
public:
	CRubbish(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRubbish();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RUBBISH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton6();
	CString m_RecycleBin;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton7();
	CString m_IERubbish;
	vector<CString> m_IERubbishPath;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton9();
	CString m_VSFolder;
	afx_msg void OnBnClickedButton4();
	vector<CString> m_VSpaths;
	CString m_VSNum;
	afx_msg void OnBnClickedButton8();
	CString m_suffix;
	afx_msg void OnBnClickedButton10();
	CString m_CustomFolder;
	afx_msg void OnBnClickedButton5();
	vector<CString> m_CustomPaths;
	CString m_CustomNum;
	afx_msg void OnBnClickedButton1();
};

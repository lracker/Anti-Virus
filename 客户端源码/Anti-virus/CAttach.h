#pragma once


// CAttach 对话框

class CAttach : public CDialogEx
{
	DECLARE_DYNAMIC(CAttach)

public:
	CAttach(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAttach();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATTACH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};

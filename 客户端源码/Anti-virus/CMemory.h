#pragma once


// CMemory 对话框

class CMemory : public CDialogEx
{
	DECLARE_DYNAMIC(CMemory)

public:
	CMemory(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMemory();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Memory };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CProgressCtrl m_CPU;
protected:
	afx_msg LRESULT OnUmCpu(WPARAM wParam, LPARAM lParam);
public:
	CString m_IntCPU;
	CString m_InitMem;
protected:
	afx_msg LRESULT OnUmMem(WPARAM wParam, LPARAM lParam);
public:
	CProgressCtrl m_Mem;
	afx_msg void OnBnClickedButton1();
};

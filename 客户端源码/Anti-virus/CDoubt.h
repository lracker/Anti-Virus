#pragma once
#include <vector>
#include <TlHelp32.h>
using std::vector;


// CDoubt 对话框

class CDoubt : public CDialogEx
{
	DECLARE_DYNAMIC(CDoubt)

public:
	CDoubt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDoubt();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DOUBT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List;
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	CMenu m_Menu;
	DWORD m_dwPid;
	DWORD m_SelectedItem;

	afx_msg void On32771();
};

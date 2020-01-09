#pragma once
#include <vector>
#include <winsvc.h>
using std::vector;

// CSERVICE 对话框

class CSERVICE : public CDialogEx
{
	DECLARE_DYNAMIC(CSERVICE)

public:
	CSERVICE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSERVICE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVICE };
#endif

private:
	vector<ENUM_SERVICE_STATUS_PROCESS> m_NewServiceVector;
	vector<ENUM_SERVICE_STATUS_PROCESS> m_OldServiceVector;
	CMenu m_Menu;
	CString m_ServiceName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ServiceList;
	virtual BOOL OnInitDialog();
	void FlushServiceList();
protected:
	afx_msg LRESULT OnUmService(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32772();
	afx_msg void On32773();
};

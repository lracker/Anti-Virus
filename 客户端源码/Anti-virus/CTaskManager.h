#pragma once
#include <vector>
#include <TlHelp32.h>
#include "Tools.h"
using std::vector;

// CTaskManager 对话框

class CTaskManager : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskManager)
private:
	vector<PROCESSINFO> m_NewProcessVector;	// 新的进程列表容器
	vector<PROCESSINFO> m_OldProcessVector; // 旧的进程列表容器
	vector<THREADENTRY32> m_ThreadVector;	// 线程列表容器
	vector<ModuleInfo> m_ModuleVector;	// 模块列表容器
public:
	CTaskManager(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTaskManager();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TaskManager };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ProcessList;
	CListCtrl m_ThreadList;
	CListCtrl m_ModuleList;
	CMenu m_Menu;
	DWORD64 m_dwPid = 0;
	void FlushProcessList();
protected:
	afx_msg LRESULT OnProcess(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	afx_msg LRESULT OnUmThread(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmModule(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void EndProcess();
};

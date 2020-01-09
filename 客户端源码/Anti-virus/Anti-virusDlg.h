
// Anti-virusDlg.h: 头文件
//

#pragma once
#include "CMyTabCtrl.h"


// CAntivirusDlg 对话框
class CAntivirusDlg : public CDialogEx
{
// 构造
public:
	CAntivirusDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANTIVIRUS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	// 定义变量判断按下热键后是隐藏还是显示窗口
	BOOL m_IsWindowHide;
public:
	CMyTabCtrl m_MyTab;
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

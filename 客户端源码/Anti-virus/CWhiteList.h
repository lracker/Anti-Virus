#pragma once
#include <vector>
using std::vector;

// CWhiteList 对话框

class CWhiteList : public CDialogEx
{
	DECLARE_DYNAMIC(CWhiteList)

public:
	CWhiteList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CWhiteList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WHITELIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_WhiteList;
	virtual BOOL OnInitDialog();
};

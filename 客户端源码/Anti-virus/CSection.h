#pragma once
#include "CPELoader.h"


// CSection 对话框

class CSection : public CDialogEx
{
	DECLARE_DYNAMIC(CSection)

public:
	CSection(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSection();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SectionList;
	virtual BOOL OnInitDialog();
	void InsertItem();
};

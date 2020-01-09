#pragma once


// CEnumWindow 对话框

class CEnumWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CEnumWindow)

public:
	CEnumWindow(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEnumWindow();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EnumWindow };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_WindowsList;
};

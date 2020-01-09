#pragma once


// CRelocation 对话框

class CRelocation : public CDialogEx
{
	DECLARE_DYNAMIC(CRelocation)

public:
	CRelocation(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRelocation();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RELOCATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SectionList;
	CListCtrl m_BlockList;
	virtual BOOL OnInitDialog();
	DWORD m_dwRelocationRVA;
	PIMAGE_BASE_RELOCATION GetRelocationDirectory();
	CString GetSection(DWORD64 dwRVA);
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
protected:
//	afx_msg LRESULT OnBlock(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmBlock(WPARAM wParam, LPARAM lParam);
};

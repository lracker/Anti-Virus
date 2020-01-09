#pragma once


// CImport 对话框

class CImport : public CDialogEx
{
	DECLARE_DYNAMIC(CImport)

public:
	CImport(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CImport();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_DllList;
	CListCtrl m_FunList;
	virtual BOOL OnInitDialog();
	DWORD m_dwImportRVA;
	PIMAGE_IMPORT_DESCRIPTOR GetImportDirectory();
	DWORD64 m_dwFile;
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	afx_msg LRESULT OnUmFunc(WPARAM wParam, LPARAM lParam);
};

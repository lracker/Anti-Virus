#pragma once


// CExport 对话框

class CExport : public CDialogEx
{
	DECLARE_DYNAMIC(CExport)

public:
	CExport(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CExport();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_ExportFOA;
	virtual BOOL OnInitDialog();
	DWORD m_dwExportRVA;
	CString m_Characteristics;
	CString m_Base;
	CString m_Name;
	CString m_NameString;
	PIMAGE_EXPORT_DIRECTORY GetExportDirectory();
	CString m_NumOfFun;
	CString m_NumOfName;
	CString m_AddrOfFun;
	CString m_AddrOfName;
	CString m_AddrOfNameOrd;
	CListCtrl m_ExportList;
};

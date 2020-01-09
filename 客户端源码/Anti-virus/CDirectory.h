#pragma once


// CDirectory 对话框

class CDirectory : public CDialogEx
{
	DECLARE_DYNAMIC(CDirectory)

public:
	CDirectory(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDirectory();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_ExportRVA;
	CString m_ExportSize;
	CString m_ImportRVA;
	CString m_ImportSize;
	CString m_ResourceRVA;
	CString m_ResourceSize;
	CString m_ExceptionRVA;
	CString m_ExceptionSize;
	CString m_SecurityRVA;
	CString m_SecuritySize;
	CString m_RelocationRVA;
	CString m_RelocationSize;
	CString m_DebugRVA;
	CString m_DebugSize;
	CString m_CopyrightRVA;
	CString m_CopyrightSize;
	CString m_GlobalptrRVA;
	CString m_GlobalptrSize;
	CString m_TlsRVA;
	CString m_TlsSize;
	CString m_LoadConfigRVA;
	CString m_LoadConfigSize;
	CString m_BoundImportRVA;
	CString m_BoundImportSize;
	CString m_IATRVA;
	CString m_IATSize;
	CString m_DelayImportRVA;
	CString m_DelayImportSize;
	CString m_COMRVA;
	CString m_COMSize;
	CString m_ReservedRVA;
	CString m_ReservedSize;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	DWORD64 m_dwExportRVA;
	DWORD64 m_dwImportRVA;
	DWORD64 m_dwResourceRVA;
	DWORD64 m_dwTLSRVA;
	DWORD64 m_dwRelocationRVA;
	DWORD64 m_dwDelayRVA;
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();
};

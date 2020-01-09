#pragma once


// CTLS 对话框

class CTLS : public CDialogEx
{
	DECLARE_DYNAMIC(CTLS)

public:
	CTLS(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTLS();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TLS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_DataBlockStartVA;
	CString m_DataBlockEndVA;
	CString m_IndexVA;
	CString m_CallBackVA;
	CString m_SizeOfZero;
	CString m_Characteristics;
	DWORD64 m_dwTLSRVA;
	virtual BOOL OnInitDialog();
	PIMAGE_TLS_DIRECTORY32 GetTlsDirectory();
	PIMAGE_TLS_DIRECTORY64 GetTlsDirectory64();
};

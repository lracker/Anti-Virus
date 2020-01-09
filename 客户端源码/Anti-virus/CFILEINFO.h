#pragma once


// CFILEINFO 对话框

class CFILEINFO : public CDialogEx
{
	DECLARE_DYNAMIC(CFILEINFO)

public:
	CFILEINFO(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFILEINFO();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Create;
	CString m_Change;
	CString m_Size;
	CString m_Md5;
	CString m_FileName;
	virtual BOOL OnInitDialog();
};

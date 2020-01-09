#pragma once


// CPELoader 对话框

class CPELoader : public CDialogEx
{
	DECLARE_DYNAMIC(CPELoader)

public:
	CPELoader(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPELoader();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PELOADER };
#endif
private:
	LPVOID m_FileBuff;
	HANDLE m_hFileMap;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
protected:
	afx_msg LRESULT OnUmPeshow(WPARAM wParam, LPARAM lParam);
public:
	CString m_EntryPoint;
	CString m_ImageBase;
	CString m_SizeOfImage;
	CString m_BaseOfCode;
	CString m_BaseOfData;
	CString m_SectionAlignment;
	CString m_FileAlignment;
	CString m_Magic;
	CString m_Subsystem;
	CString m_NumberOfSections;
	CString m_TimeDateStamp;
	CString m_SizeOfHeaders;
	CString m_Characterisics;
	CString m_Checksum;
	CString m_SizeOfOptionalHander;
	CString m_NumOfRvaAndSizes;

	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CString m_FilePATH;
	CString m_BIT;
};

#pragma once


// CResource 对话框

class CResource : public CDialogEx
{
	DECLARE_DYNAMIC(CResource)

public:
	CResource(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CResource();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESOURCE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_RNumIDEntries;
	CString m_RNumNameEntries;
	PIMAGE_RESOURCE_DIRECTORY CResource::GetResourceDirectory();
	DWORD m_dwResourceRVA;
	CTreeCtrl m_ResourceTree;
	CString m_SelectedRVA;
	CString m_SelectedOffset;
	CString m_SelectedSize;
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	CString m_SelectedNumIDEntries;
	CString m_SelectedNumNameEntries;
};

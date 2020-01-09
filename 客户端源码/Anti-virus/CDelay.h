#pragma once


// CDelay 对话框

class CDelay : public CDialogEx
{
	DECLARE_DYNAMIC(CDelay)

public:
	CDelay(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDelay();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DELAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Delay;
	DWORD m_dwDelayRVA;
	virtual BOOL OnInitDialog();
	PIMAGE_DELAYLOAD_DESCRIPTOR GetDelayDirectory();
};

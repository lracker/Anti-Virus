#pragma once


// CMyTabCtrl

class CMyTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CMyTabCtrl)



public:
	CMyTabCtrl();
	virtual ~CMyTabCtrl();
	CDialogEx* m_Dia[7];
	int m_nTabNum;
protected:
	DECLARE_MESSAGE_MAP()
};



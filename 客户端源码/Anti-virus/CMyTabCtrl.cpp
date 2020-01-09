// CMyTabCtrl.cpp: 实现文件
//

#include "pch.h"
#include "Anti-virus.h"
#include "CMyTabCtrl.h"


// CMyTabCtrl

IMPLEMENT_DYNAMIC(CMyTabCtrl, CTabCtrl)

CMyTabCtrl::CMyTabCtrl()
{
	m_nTabNum = 7;
}

CMyTabCtrl::~CMyTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
END_MESSAGE_MAP()



// CMyTabCtrl 消息处理程序



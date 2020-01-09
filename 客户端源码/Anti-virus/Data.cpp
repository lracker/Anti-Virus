#include "pch.h"
#include "Data.h"
int g_nMachine = 0;
CString g_PEPath;
const TCHAR* RES[20] = {
	L"光标",
	L"位图",
	L"图标",
	L"菜单",
	L"对话框",
	L"字符串列表",
	L"字体目录",
	L"字体",
	L"快捷键",
	L"非格式化资源",
	L"消息列表",
	L"鼠标指针数组",
	L"NULL",
	L"图标组",
	L"NULL",
	L"版本信息",
};

TCHAR g_VirusLibrary[] = _T("File\\VirusLibrary.txt");
char g_WhiteList[] = "File\\WhiteList.txt";
char* g_End = "--------------------------------";
vector<CString> g_ProcessVector;
extern DWORD g_OldTaskPid = 0;
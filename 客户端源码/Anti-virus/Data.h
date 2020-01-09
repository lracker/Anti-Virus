#pragma once
#include <vector>
using std::vector;
#define UM_Process WM_USER + 100
#define UM_Thread  WM_USER + 101
#define UM_Module  WM_USER + 102
#define UM_Service WM_USER + 103
#define UM_PESHOW  WM_USER + 104
#define UM_CPU	   WM_USER + 105 	 
#define UM_MEM	   WM_USER + 106
#define UM_FUNC    WM_USER + 107
#define UM_BLOCK   WM_USER + 108
# define DLLPATH L"C:\\Users\\Canary\\source\\repos\\VirusDLL\\x64\\Debug\\VirusDLL.dll"

extern int g_nMachine;
extern CString g_PEPath;
extern const TCHAR* RES[20];
extern TCHAR g_VirusLibrary[68];
extern char* g_End;
extern vector<CString> g_ProcessVector;
extern char g_WhiteList[64];
extern DWORD g_OldTaskPid;



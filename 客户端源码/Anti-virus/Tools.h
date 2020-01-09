#pragma once
#include <TlHelp32.h>
#include <vector>
#include <tchar.h>
#include <Psapi.h>
#include <winsvc.h>
using std::vector;

// 进程结构体，这个包含了内存的使用率
struct PROCESSINFO :PROCESSENTRY32 
{
	DWORD64 dwMemoryUsage;  //内存使用率
};
struct ModuleInfo :MODULEINFO
{
	TCHAR szModuleFile[MAX_PATH];
	TCHAR szModuleName[MAX_PATH];
};
typedef struct _SOFTINFO
{
	WCHAR szSoftName[50];	// 软件名称
	WCHAR szSoftVer[50];	// 软件版本号
	WCHAR szSoftData[20];	// 软件安装日期
	WCHAR szSoftSize[MAX_PATH];	// 软件大小
	WCHAR strSoftInsPath[MAX_PATH];	// 软件安装路径
	WCHAR strSoftUniPath[MAX_PATH];	// 软件安装路径
	WCHAR strSoftVenRel[50];	// 软件发布厂商
	WCHAR strSoftIco[MAX_PATH];	// 软件图标路径
}SOFTINFO, * PSOFTINFO;

DWORD64 CmpStatus(ENUM_SERVICE_STATUS_PROCESS OldServer, vector<ENUM_SERVICE_STATUS_PROCESS> m_NewServiceVector);
BOOL CmpSerName(_In_ vector<ENUM_SERVICE_STATUS_PROCESS> ProcessVector, CString ServiceName);
BOOL CmpProcIndex(_In_ vector<PROCESSINFO> ProcessVector, DWORD64 dwPid);
BOOL GetAllProcesses(_Out_ vector<PROCESSINFO>& ProcessVector);
BOOL GetAllThread(_Out_ vector<THREADENTRY32>& ThreadVector, DWORD64 dwPid);
BOOL GetAllModule(_Out_ vector<ModuleInfo>& ModuleVector, DWORD64 dwPid);
int GetCPU();
DWORDLONG GetUsedMem();
INT IEClean(TCHAR* IEPath1, vector<CString>& m_IERubbishPath);
BOOL GetRubbish(CString dir, vector<CString>& paths, vector<CString>& extName);
BOOL GetALLService(_Out_ vector<ENUM_SERVICE_STATUS_PROCESS>& ServiceVector);
BOOL EnableDebugPrivilege(BOOL fEnable); //提升为调试权限
DWORD RVATOFOA(DWORD dwRVA);	// RVA转换为FOA
BOOL VirusScan(char* FilePath, char* Library);		// 将文件和病毒库里的特征码进行比较
ULARGE_INTEGER EnumFold(CString FoldPath, vector<CString>& FileVector);
void ReadWhiteList();
DWORD GetPid(CString ProcessName);
BOOL Inject(DWORD dwPid);

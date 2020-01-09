#include "pch.h"
#include "Tools.h"
#include <Psapi.h>
#include <tchar.h>
#include "Md5.h"
#include "Data.h"
#include <fstream>
using std::fstream;
using std::ios;


//*****************************************************************************************
// 函数名称: GetAllProcesses
// 函数说明: 遍历进程并且放到容器中
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: _Out_ vector<PROCESSENTRY32> & ProcessVector
// 返 回 值: BOOL
//*****************************************************************************************
BOOL GetAllProcesses(_Out_ vector<PROCESSINFO>& ProcessVector)
{
	// 首先清空容器
	ProcessVector.clear();
	// 获取进程快照
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// 初始化结构体
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	// 开始遍历第一个
	if (!Process32First(hProcessSnap, &pe))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}	
	do 
	{
		// 获取进程的内存占用
		PROCESSINFO pInfo = *(PROCESSINFO*)&pe;
		// 打开进程
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
		// 如果进程打开失败，则进程的内存占用置为0
		if (!hProcess)
			pInfo.dwMemoryUsage = 0;
		else
		{
			PROCESS_MEMORY_COUNTERS pmc = { sizeof(PROCESS_MEMORY_COUNTERS) };
			GetProcessMemoryInfo(hProcess, &pmc, sizeof(PROCESS_MEMORY_COUNTERS));
			pInfo.dwMemoryUsage = pmc.WorkingSetSize;  //获取内存占用
		}
		ProcessVector.push_back(pInfo);
	} while (Process32Next(hProcessSnap,&pe));
	return TRUE;
}

//*****************************************************************************************
// 函数名称: CmpStatus
// 函数说明: 根据服务名来找到他的状态
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: CString Service
// 返 回 值: BOOL
//*****************************************************************************************
DWORD64 CmpStatus(ENUM_SERVICE_STATUS_PROCESS OldServer, vector<ENUM_SERVICE_STATUS_PROCESS> m_NewServiceVector)
{
	for (auto& i : m_NewServiceVector)
	{
		// 在新的表里面找到了该服务
		if (!_tcscmp(OldServer.lpServiceName, i.lpServiceName))
		{
			if (OldServer.ServiceStatusProcess.dwCurrentState != i.ServiceStatusProcess.dwCurrentState)
				return i.ServiceStatusProcess.dwCurrentState;
			else
				return 0;
		}
	}
	return 0;
}

//*****************************************************************************************
// 函数名称: CmpSerIndex
// 函数说明: 比较服务列表里的PID
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: _In_ vector<ENUM_SERVICE_STATUS_PROCESS> ProcessVector
// 参    数: DWORD dwPid
// 返 回 值: BOOL
//*****************************************************************************************
BOOL CmpSerName(_In_ vector<ENUM_SERVICE_STATUS_PROCESS> ProcessVector, CString ServiceName)
{
	for (auto& i : ProcessVector)
	{
		// 根据名字去找
		if (i.lpServiceName == ServiceName)
			return TRUE;
	}
	return FALSE;
}

//*****************************************************************************************
// 函数名称: CmpIndex
// 函数说明: 在新列表里查找PID是否存在
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: _In_ vector<PROCESSINFO> ProcessVector
// 参    数: DWORD dwPid
// 返 回 值: BOOL
//*****************************************************************************************

BOOL CmpProcIndex(_In_ vector<PROCESSINFO> ProcessVector, DWORD64 dwPid)
{
	for (auto& i : ProcessVector)
	{
		if (i.th32ProcessID == dwPid)
			return TRUE;
	}
	return FALSE;
}

//*****************************************************************************************
// 函数名称: GetAllThread
// 函数说明: 根据PID获取该进程的所有线程
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: _Out_ vector<THREADENTRY32> & ThreadVector
// 返 回 值: BOOL
//*****************************************************************************************
BOOL GetAllThread(_Out_ vector<THREADENTRY32>& ThreadVector, DWORD64 dwPid)
{
	// 清空线程容器
	ThreadVector.clear();
	// 创建线程快照
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	// 初始化结构体
	THREADENTRY32 ThreadInfo = { sizeof(THREADENTRY32) };
	// 开始遍历第一个
	if (!Thread32First(hThreadSnap, &ThreadInfo))
	{
		CloseHandle(hThreadSnap);
		return FALSE;
	}
	do 
	{
		// 如果该线程的拥有者和进程的PID一样的话，则放入容器中
		if (ThreadInfo.th32OwnerProcessID == dwPid)
			ThreadVector.push_back(ThreadInfo);
	} while (Thread32Next(hThreadSnap, &ThreadInfo));
	CloseHandle(hThreadSnap);
	return TRUE;
}

//*****************************************************************************************
// 函数名称: GetAllModule
// 函数说明: 获取进程的所有模块
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: _Out_ vector<ModuleInfo> & ModuleVector
// 参    数: DWORD dwPid
// 返 回 值: BOOL
//*****************************************************************************************
BOOL GetAllModule(_Out_ vector<ModuleInfo>& ModuleVector, DWORD64 dwPid)
{
	// 清空模块容器
	ModuleVector.clear();
	// 获取进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	// 首先获取所用到的缓冲区的大小
	DWORD dwBuffSize = 0;
	EnumProcessModulesEx(hProcess, NULL, 0, &dwBuffSize, LIST_MODULES_ALL);
	// 申请动态内存空间
	HMODULE* pModuleHandleArr = (HMODULE*)new char[dwBuffSize];
	// 再次调用函数就能够获取模块句柄
	EnumProcessModulesEx(hProcess, pModuleHandleArr, dwBuffSize, &dwBuffSize, LIST_MODULES_ALL);
	// 由于dwBuffSize是字节数，所以要除以每个句柄的字节数才可以得到模块句柄的个数
	for (int i = 0; i < dwBuffSize / sizeof(HMODULE); ++i)
	{
		ModuleInfo stcModuleInfo = { };
		// 根据进程句柄和模块句柄来获取模块信息的函数
		GetModuleInformation(hProcess, pModuleHandleArr[i], &stcModuleInfo,sizeof(MODULEINFO));
		// 根据进程句柄和模块句柄来获取模块的路径(包括模块名)
		GetModuleFileNameEx(hProcess, pModuleHandleArr[i], stcModuleInfo.szModuleFile, MAX_PATH);
		GetModuleFileNameEx(hProcess, pModuleHandleArr[i], stcModuleInfo.szModuleName, MAX_PATH);
		// 通过文件路径函数获取文件名
		PathStripPath(stcModuleInfo.szModuleName);
		ModuleVector.push_back(stcModuleInfo);
	}
	CloseHandle(hProcess);
	return TRUE;
}

//*****************************************************************************************
// 函数名称: GetCPU
// 函数说明: 获取CPU使用率
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: int
//*****************************************************************************************
int GetCPU()
{
	// 第一次获取处理器时间
	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	// 创建内核对象 
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 等待1000毫秒
	WaitForSingleObject(hEvent, 1000);
	// 第二次获取处理器时间
	FILETIME preidleTime, prekernelTime, preuserTime;
	GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);
	// 转换上面所有的时间，将获取的时间变量都转换
	double ChangeidleTime1 = (double)idleTime.dwHighDateTime * 4.294967296E9 + (double)idleTime.dwLowDateTime;
	double ChangekernelTime1 = (double)kernelTime.dwHighDateTime * 4.294967296E9 + (double)kernelTime.dwLowDateTime;
	double ChangeuserTime1 = (double)userTime.dwHighDateTime * 4.294967296E9 + (double)userTime.dwLowDateTime;
	double ChangeidleTime2 = (double)preidleTime.dwHighDateTime * 4.294967296E9 + (double)preidleTime.dwLowDateTime;
	double ChangekernelTime2 = (double)prekernelTime.dwHighDateTime * 4.294967296E9 + (double)prekernelTime.dwLowDateTime;
	double ChangeuserTime2 = (double)preuserTime.dwHighDateTime * 4.294967296E9 + (double)preuserTime.dwLowDateTime;
	return (int)(100.0 - (ChangeidleTime2 - ChangeidleTime1) / (ChangekernelTime2 - ChangekernelTime1 + ChangeuserTime2 - ChangeuserTime1) * 100.0);
}

//*****************************************************************************************
// 函数名称: GetUsedMem
// 函数说明: 获取已经使用的内存
// 作    者: lracker
// 时    间: 2019/10/14
// 返 回 值: DWORDLONG
//*****************************************************************************************
DWORDLONG GetUsedMem()
{
	// 获取当前的内存状态
	MEMORYSTATUSEX stcMemStatusEx = { 0 };
	stcMemStatusEx.dwLength = sizeof(stcMemStatusEx);
	GlobalMemoryStatusEx(&stcMemStatusEx);
	return stcMemStatusEx.ullTotalPhys - stcMemStatusEx.ullAvailPhys;
}

//*****************************************************************************************
// 函数名称: IEClean
// 函数说明: 清理IE浏览器的垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: TCHAR * IEPath1
// 参    数: vector<CString> & m_IERubbishPath
// 返 回 值: INT
//*****************************************************************************************
INT IEClean(TCHAR* IEPath1, vector<CString>& m_IERubbishPath)
{
	CString temp = _T("\\*.*");
	CString IEPath = IEPath1 + temp;
	CFileFind tempFind;
	BOOL IsFinded = tempFind.FindFile(IEPath);
	INT nCount = 0;
	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFileW();
		if (!tempFind.IsDots())
		{
			TCHAR sFoundFileName[200] = { 0 };
			_tcscpy_s(sFoundFileName, tempFind.GetFileName().GetBuffer(200));
			if (tempFind.IsDirectory())
			{
				TCHAR sTempDir[200] = { 0 };
				wsprintf(sTempDir, _T("%s\\%s"), IEPath1, sFoundFileName);
				nCount += IEClean(sTempDir, m_IERubbishPath); //删除文件夹下的文件
				RemoveDirectory(sTempDir); //移除空文件
			}
			else

			{
				TCHAR sTempFileName[200] = { 0 };
				wsprintf(sTempFileName, _T("%s\\%s"), IEPath1, sFoundFileName);
				m_IERubbishPath.push_back(sTempFileName);
				nCount++;
			}
		}
	}
	tempFind.Close();
	return nCount;
}

//*****************************************************************************************
// 函数名称: GetVsRubbish
// 函数说明: 根据后缀获取垃圾
// 作    者: lracker
// 时    间: 2019/10/14
// 参    数: CString dir
// 参    数: std::vector<CString> & paths
// 返 回 值: bool
//*****************************************************************************************
BOOL GetRubbish(CString dir, vector<CString>& paths, vector<CString>& extName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile = FindFirstFile(dir + "\\*", &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE)
		return false;
	do
	{
		if (!lstrcmp(FindFileData.cFileName, L".") || !lstrcmp(FindFileData.cFileName, L".."))
			continue;
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  //如果是个目录的话
			GetRubbish(dir + L"\\" + FindFileData.cFileName, paths, extName);
		else
		{
			// 判断是否是vs垃圾文件的后缀
			for (auto& i : extName)
			{
				TCHAR* pExtName = FindFileData.cFileName + wcslen(FindFileData.cFileName);
				while (pExtName != FindFileData.cFileName & *pExtName != '.')
					pExtName--;
				if (!_wcsicmp(pExtName, i))//如果是垃圾后缀的话，那就压入
					paths.push_back(dir + L"\\" + FindFileData.cFileName);
			}
		}
	} while (FindNextFile(hListFile, &FindFileData));
}

//*****************************************************************************************
// 函数名称: GetALLService
// 函数说明: 获取所有服务
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: _Out_ vector<ENUM_SERVICE_STATUS_PROCESSA> & ServiceVector
// 返 回 值: BOOL
//*****************************************************************************************
BOOL GetALLService(_Out_ vector<ENUM_SERVICE_STATUS_PROCESS>& ServiceVector)
{
	// 这个需要提权的
	// 打开源程计算机服务控制管理器
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return FALSE;
	// 第一次调用，获取需要的内存大小
	DWORD dwServiceNum = 0;
	DWORD dwSize = 0;
	EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &dwSize, &dwServiceNum, NULL, NULL);
	if (!dwSize)
		return FALSE;
	// 申请需要的内存，第二次调用
	LPENUM_SERVICE_STATUS_PROCESS pEnumService = (LPENUM_SERVICE_STATUS_PROCESS)new char[dwSize];
	// 第二次枚举
	bool bStatus = FALSE;
	bStatus = EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (PBYTE)pEnumService, dwSize, &dwSize, &dwServiceNum, NULL, NULL);
	for (DWORD64 i = 0; i < dwServiceNum; i++)
	{
		// 获取基础信息
		ServiceVector.push_back(pEnumService[i]);
	}
	return TRUE;
}

//*****************************************************************************************
// 函数名称: EnableDebugPrivilege
// 函数说明: 调试的时候提高权限
// 作    者: lracker
// 时    间: 2019/10/15
// 参    数: BOOL fEnable
// 返 回 值: BOOL
//*****************************************************************************************
BOOL EnableDebugPrivilege(BOOL fEnable) //提升为调试权限
{
	BOOL fOk = FALSE;
	HANDLE hToken;
	// 以修改权限的方式，打开进程的令牌
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		// 令牌权限结构体
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		// 获得LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		// 提升权限
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL); // 修改权限
		fOk = (GetLastError() == ERROR_SUCCESS);
		
		CloseHandle(hToken);
	}
	return fOk;
}

//*****************************************************************************************
// 函数名称: RVATOFOA
// 函数说明: RVA转换为FOA
// 作    者: lracker
// 时    间: 2019/10/16
// 参    数: DWORD dwRVA
// 返 回 值: DWORD
//*****************************************************************************************
DWORD RVATOFOA(DWORD dwRVA)
{
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _T("PE"));
	LPVOID pFileBuff = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD64)pDos);
	PIMAGE_FILE_HEADER pFile = (PIMAGE_FILE_HEADER)&pNt->FileHeader;
	DWORD dwNum = pFile->NumberOfSections;
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNt);
	DWORD dwOffset = 0;
	for (int i = 0; i < dwNum; i++)
	{
		// RVA大于该区段所在的RVA&&<=区段在文件中的大小+区段的RVA
		if (dwRVA >= pSectionHeader[i].VirtualAddress&& dwRVA <= pSectionHeader[i].VirtualAddress + pSectionHeader[i].SizeOfRawData)
		{
			 dwOffset = dwRVA - pSectionHeader[i].VirtualAddress + pSectionHeader[i].PointerToRawData;
			 break;
		}
	}
	CloseHandle(hFileMap);
	return dwOffset;
}

//*****************************************************************************************
// 函数名称: VirusScan
// 函数说明: 扫描该文件是否有毒，计算出文件的md5和本地的病毒库里的md5进行比较
// 作    者: lracker
// 时    间: 2019/10/17
// 参    数: char * FilePath
// 返 回 值: BOOL
//*****************************************************************************************
BOOL VirusScan(char* FilePath, char* Library)
{
	// 获取文件 MD5
	char* FileMD5 = md5FileValue(FilePath);
	// 开始在Library里面搜索
	char Librarytmp[33] = {};
	memcpy_s(Librarytmp, 32, Library, 32);
	// 这两者比较比较，如果不相等的话
	while (strcmp(Librarytmp,g_End))
	{
		// 如果相等的话
		if (!strcmp(FileMD5, Librarytmp))
		{
			// 找到就返回真
			return TRUE;
		}
		// 跳过第一个比较的md5
		while (*Library != 0x0a)
			Library++;
		// 跳过 \n
		Library++;
		// 跳过名字
		while (*Library != 0x0a)
			Library++;
		// 跳过\n
		Library++;
		memcpy_s(Librarytmp, 32, Library, 32);
	}
	// 找不到返回假
	return FALSE;
}

//*****************************************************************************************
// 函数名称: EnumFold
// 函数说明: 遍历文件夹所有的文件，并且压入容器
// 作    者: lracker
// 时    间: 2019/10/17
// 参    数: CString FoldPath
// 参    数: vector<CString> & FileVector
// 返 回 值: ULARGE_INTEGER
//*****************************************************************************************
ULARGE_INTEGER EnumFold(CString FoldPath, vector<CString>& FileVector)
{
	// 获取第一个文件的信息
	// 文件总大小
	ULARGE_INTEGER qwFileTotalSize = {};
	WIN32_FIND_DATA w32FindData = {};
	CString FoldPath2 = FoldPath + _T("\\*");
	HANDLE hFindFile = FindFirstFile(FoldPath2, &w32FindData);
	// 循环遍历获取当前目录中的文件信息
	do 
	{
		// 去掉两个特殊目录
		if ((!_tcscmp(w32FindData.cFileName, _T("."))) || (!_tcscmp(w32FindData.cFileName, _T(".."))))
			continue;
		// 如果是文件夹的话，那就递归遍历进去
		if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString SubFoldPath = FoldPath + _T("\\") + w32FindData.cFileName;
			qwFileTotalSize = EnumFold(SubFoldPath, FileVector);
		}
		// 如果是文件的话，那么就压入容器里
		else
		{
			CString FilePath = FoldPath + _T("\\") + w32FindData.cFileName;
			FileVector.push_back(FilePath);
		}
	} while (FindNextFile(hFindFile, &w32FindData));
	return qwFileTotalSize;
}

//*****************************************************************************************
// 函数名称: ReadWhiteList
// 函数说明: 读取白名单
// 作    者: lracker
// 时    间: 2019/10/17
// 返 回 值: void
//*****************************************************************************************
void ReadWhiteList()
{
	char buffer[256];

	fstream outFile;
	outFile.open(g_WhiteList, ios::in);

	while (!outFile.eof())
	{
		outFile.getline(buffer, 256, '\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
		USES_CONVERSION;
		g_ProcessVector.push_back(A2W(buffer));
	}
	outFile.close();
}

//*****************************************************************************************
// 函数名称: GetPid
// 函数说明: 通过进程名来获取进程PID
// 作    者: lracker
// 时    间: 2019/10/18
// 参    数: CString ProcessName
// 返 回 值: DWORD
//*****************************************************************************************
DWORD GetPid(CString ProcessName)
{
	// 获取进程快照
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// 初始化结构体
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	// 开始遍历第一个
	if (!Process32First(hProcessSnap, &pe))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do
	{
		if (pe.szExeFile == ProcessName)
			return pe.th32ProcessID;
	} while (Process32Next(hProcessSnap, &pe));
	return 0;
}

//*****************************************************************************************
// 函数名称: Inject
// 函数说明: 远程注入
// 作    者: lracker
// 时    间: 2019/10/18
// 参    数: DWORD dwPid
// 返 回 值: BOOL
//*****************************************************************************************
BOOL Inject(DWORD dwPid)
{
	// 打开进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	// 在进程空间里申请空间
	LPVOID lpPathAddr = VirtualAllocEx(hProcess, 0, wcslen(DLLPATH) * 2 + 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	// 在目标进程写入DLL
	DWORD64 dwWriteSize = 0;
	WriteProcessMemory(hProcess, lpPathAddr, DLLPATH, wcslen(DLLPATH) * 2 + 2, &dwWriteSize);
	// 在目标进程中创建线程
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (PTHREAD_START_ROUTINE)LoadLibraryW, lpPathAddr, NULL, NULL);
	// 等待线程结束
	WaitForSingleObject(hThread, -1);
	// 清理环境
	VirtualFreeEx(hProcess, lpPathAddr, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return 0;
}

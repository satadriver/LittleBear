#include <stdio.h>
#include "api.h"
#include "PEParser.h"
#include <windows.h>
#include "MapPE.h"
#include "ImportFunTable.h"
#include "Reloc.h"
#include "main.h"
#include "FileHelper.h"
#include "Crypto.h"
#include "Public.h"

int PEParser::isPE(const char * data) {
	char szpedoshdr[] = { 'M','Z' };
	if (memcmp(data, szpedoshdr,2) == 0)
	{
		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)data;
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(data + dos->e_lfanew);
		unsigned char szpehdr[] = { 'P','E',0,0 };
		if (memcmp((char*)&nt->Signature, szpehdr,4) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}



int PEParser::getMachine(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->Machine;
}

int PEParser::getNumberOfSections(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->NumberOfSections;
}

int PEParser::getTimeDateStamp(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->TimeDateStamp;
}

int PEParser::getPointerToSymbolTable(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->PointerToSymbolTable;
}

int PEParser::getNumberOfSymbols(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->NumberOfSymbols;
}

int PEParser::getSizeOfOptionalHeader(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->SizeOfOptionalHeader;
}


int PEParser::getCharacteristics(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->Characteristics;
}



PIMAGE_OPTIONAL_HEADER PEParser::getOptionalHeader(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_OPTIONAL_HEADER opt = &nt->OptionalHeader;
	return opt;
}
PIMAGE_FILE_HEADER PEParser::getNtHeader(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh;
}



FARPROC PEParser::getProcAddr(HMODULE pmodule, const char * funname, int funnamelen)
{
#ifndef _WIN64
	FARPROC pGetProcAddr = 0;
	__asm
	{
		pushad

		mov edi, pmodule

		mov eax, [edi + 3ch]

		mov edx, [edi + eax + 78h]	//import table base

		add edx, edi			//import table

		mov ecx, [edx + 18h]; number of functions

		mov ebx, [edx + 20h]

		add ebx, edi; AddressOfName

		__search :

		dec ecx
			cmp ecx, 0xffffffff
			jz __funcend

			mov esi, [ebx + ecx * 4]

			add esi, edi

			push edi
			push esi
			push ecx
			mov ecx, funnamelen
			mov edi, funname
			cld

			repz cmpsb
			jnz _tonotfound

			jecxz __foundname

			_tonotfound :
		pop ecx
			pop esi
			pop edi
			jmp __search



			__foundname :
		pop ecx
			pop esi
			pop edi

		mov ebx, [edx + 24h]

			add ebx, edi; index address

			movzx ecx, word ptr[ebx + ecx * 2]

			mov ebx, [edx + 1ch]

			add ebx, edi

			mov eax, [ebx + ecx * 4]

			add eax, edi

			mov pGetProcAddr, eax

			__funcend :
		popad
	}

	return  pGetProcAddr;

#else
	return (FARPROC)GetProcAddress;
#endif
}

HMODULE PEParser::GetDllKernel32Base()
{
#ifndef _WIN64
	HMODULE hKernel32 = 0;
	__asm
	{
		//assume fs:nothing
		push esi
		xor eax, eax
		mov eax, fs:[eax + 30h]		; 指向PEB的指针
		mov eax, [eax + 0ch]		; 指向PEB_LDR_DATA的指针
		mov esi, [eax + 0ch]		; 根据PEB_LDR_DATA得出InLoadOrderModuleList的Flink字段
		lodsd
		mov eax, [eax]				; 指向下一个节点
		mov eax, [eax + 18h]		; Kernel.dll的基地址
		mov hKernel32, eax
		pop esi
	}

	return hKernel32;
#else
	char szKernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	return GetModuleHandleA(szKernel32);
#endif
}

int PEParser::getBaseApi(HMODULE * kernel32,DWORD * getprocaddr,DWORD * loadlib) {
	char szLoadLibraryA[] = { 'L','o','a','d','L','i','b','r','a','r','y','A',0 };
	char szGetProcAddress[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };

	*kernel32 = (HMODULE)GetDllKernel32Base();
	if (kernel32 == 0)
	{
		return FALSE;
	}

	*getprocaddr = (DWORD)getProcAddr(*kernel32, szGetProcAddress, lstrlenA(szGetProcAddress));
	*loadlib = (DWORD)getProcAddr(*kernel32, szLoadLibraryA, lstrlenA(szLoadLibraryA));

	return TRUE;
}

DWORD PEParser::checksumPE(unsigned char * data, int datasize) {
	DWORD checksum = 0;
	__asm {
		pushad
		xor eax, eax
		mov ebx, datasize
		mov ecx, ebx
		push ecx
		shr ecx, 1
		mov esi, data
		clc
		__cal_checksum :
		adc ax, word ptr[esi]
			inc esi
			inc esi
			loop __cal_checksum
			adc ax, 0

			pop ecx
			test ecx, 1
			jz __end
			xor edi, edi
			movzx di, byte ptr[esi]
			clc
			add ax, di
			__end :
		add eax, ebx;
		mov checksum, eax
		popad
	}
	return checksum;
}



DWORD PEParser::GetSizeOfImage(char* pFileBuff)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFileBuff + pDos->e_lfanew);
	DWORD dwSizeOfImage = pNt->OptionalHeader.SizeOfImage;

	return dwSizeOfImage;
}


DWORD PEParser::GetImageBase(char* pFileBuff)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFileBuff + pDos->e_lfanew);
	DWORD imagebase = pNt->OptionalHeader.ImageBase;

	return imagebase;
}

//why need to modify imagebase？
bool PEParser::SetImageBase(char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	pNt->OptionalHeader.ImageBase = (ULONG32)chBaseAddress;

	return TRUE;
}


int PEParser::CallConsoleEntry(char* chBaseAddress)
{
	int ret = 0;

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	glpmain = (ptrmain)(chBaseAddress + pNt->OptionalHeader.AddressOfEntryPoint);

	char szparams[64][256] = { 0 };	
	int iArgc = 0;
	wchar_t * *wszparams = lpCommandLineToArgvW(lpGetCommandLineW(), &iArgc);
	for (int i = 0; i < iArgc; i++)
	{
		char szparam[256] = { 0 };
		ret = WideCharToMultiByte(CP_ACP, 0, wszparams[i], -1, szparam, 256, 0, 0);
		if (ret > 0)
		{
			lstrcpyA(szparams[i], szparam);
		}
	}

	ret = glpmain(iArgc, (char**)szparams);

	return ret;
}


int PEParser::CallDllEntry(char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	glpDllMainEntry = (ptrDllMainEntry)(chBaseAddress + pNt->OptionalHeader.AddressOfEntryPoint);

	int ret = glpDllMainEntry((DWORD)chBaseAddress, DLL_PROCESS_ATTACH, 0);

	Public::writelog("dll entry\r\n");

	return ret;
}


//MajorOperatingSystemVersion&&MinorOperatingSystemVersion 5.01	= xp
//MajorLinkerVersion和MinorLinkerVersion是链接器版本的高位和低位 14.0 = vs2015
//MajorSubsystemVersion、MinorSubsystemVersion 5.01 = xp
//Subsystem IMAGE_SUBSYSTEM_WINDOWS_CUI=3  IMAGE_SUBSYSTEM_WINDOWS_GUI =2
int PEParser::CallExeEntry(char* chBaseAddress)
{
	int callret = 0;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	glpWinMain = (ptrWinMain)(chBaseAddress + pNt->OptionalHeader.AddressOfEntryPoint);
// 	__asm
// 	{
// 		mov eax, glpWinMain
// 		jmp eax
// 		mov callret,eax
// 	}

	__try {
		callret = glpWinMain((HINSTANCE)chBaseAddress, ghprevInstance, glpCmdLine, gnShowCmd);
	}
	__except (1) {
		printf("main process exception\r\n");
	}

	return callret;
}





int PEParser::loadPE()
{
	int ret = 0;

	char * data = 0;
	int fs = 0;
	ret = FileHelper::fileReader("test.dat", &data, &fs);

	unsigned char * key = (unsigned char*)data + fs - CRYPT_KEY_SIZE;

	Crypto::CryptData((unsigned char*)data, fs, key, CRYPT_KEY_SIZE, (unsigned char*)data, fs);

	DWORD dwSizeOfImage = GetSizeOfImage(data);

	DWORD imagebase = GetImageBase(data);

	char* chBaseAddress = (char*)lpVirtualAlloc((DWORD)imagebase, dwSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (NULL == chBaseAddress)
	{
		chBaseAddress = (char*)lpVirtualAlloc(0, dwSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (NULL == chBaseAddress)
		{
			delete data;
			return NULL;
		}
	}

	Public::writelog("image size:%u,base:%x\r\n", dwSizeOfImage, imagebase);

	RtlZeroMemory(chBaseAddress, dwSizeOfImage);

	ret = MapPE::MapFile(data, chBaseAddress);
	delete data;

	ret = ImportFunTable::recover((DWORD)chBaseAddress);

	ret = Reloc::recovery((DWORD)chBaseAddress);

// 	DWORD dwOldProtect = 0;
// 	if (FALSE == lpVirtualProtect(chBaseAddress, dwSizeOfImage, PAGE_EXECUTE_READWRITE, &dwOldProtect))
// 	{
// 		lpVirtualFree(chBaseAddress, dwSizeOfImage, MEM_DECOMMIT);
// 		lpVirtualFree(chBaseAddress, 0, MEM_RELEASE);
// 
// 		Public::writelog("lpVirtualProtect size:%u,base:%x error\r\n", dwSizeOfImage, chBaseAddress);
// 		return NULL;
// 	}

	ret = SetImageBase(chBaseAddress);

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(chBaseAddress + dos->e_lfanew);
	if (nt->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	{

	}
	else if (nt->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
	{
		gType = 3;
		ghPEModule = (HMODULE)chBaseAddress;
		gPEImageSize = dwSizeOfImage;

		ret = CallConsoleEntry(chBaseAddress);

		lpVirtualFree(chBaseAddress, dwSizeOfImage, MEM_DECOMMIT);
		lpVirtualFree(chBaseAddress, 0, MEM_RELEASE);
		return ret;
	}

	if (nt->FileHeader.Characteristics & 0x2000)
	{
		gType = 2;
		gPEImageSize = dwSizeOfImage;
		ghPEModule = (HMODULE)chBaseAddress;

		ret = CallDllEntry(chBaseAddress);
		return ret;
	}
	else {
		gType = 1;
		ghPEModule = (HMODULE)chBaseAddress;
		gPEImageSize = dwSizeOfImage;

		ret = CallExeEntry(chBaseAddress);

		lpVirtualFree(chBaseAddress, dwSizeOfImage, MEM_DECOMMIT);
		lpVirtualFree(chBaseAddress, 0, MEM_RELEASE);
		return ret;
	}

	return TRUE;
}
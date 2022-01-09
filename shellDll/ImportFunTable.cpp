
#include "ImportFunTable.h"
#include "ExportFunTable.h"

#include "api.h"

//每个dll有一个IMAGE_IMPORT_DESCRIPTOR
//2个IMAGE_THUNK_DATA
//IMAGE_THUNK_DATA指向IMAGE_IMPORT_BY_NAME
//导入地址数组是IAT
//IMAGE_DIRECTORY_ENTRY_IAT = IAT
DWORD ImportFunTable::recover(DWORD chBaseAddress) {

	// 	char szGetModuleHandleA[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A',0 };
	// 	char szGetModuleHandleW[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','W',0 };
	// 	char szInitializeSListHead[] = { 'I','n','i','t','i','a','l','i','z','e','S','L','i','s','t','H','e','a','d',0 };

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);

	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pDos + pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	while (TRUE)
	{
		if (0 == pImportTable->OriginalFirstThunk)
		{
			break;
		}

		char *lpDllName = (char *)((DWORD)pDos + pImportTable->Name);
		HMODULE hDll = (HMODULE)lpGetModuleHandleA((LPSTR)lpDllName);
		if (NULL == hDll)
		{
			hDll = lpLoadLibraryA(lpDllName);
			if (NULL == hDll)
			{
				pImportTable++;
				continue;
			}
		}

		DWORD i = 0;

		PIMAGE_THUNK_DATA lpImportNameArray = (PIMAGE_THUNK_DATA)((DWORD)pDos + pImportTable->OriginalFirstThunk);

		PIMAGE_THUNK_DATA lpImportFuncAddrArray = (PIMAGE_THUNK_DATA)((DWORD)pDos + pImportTable->FirstThunk);
		while (TRUE)
		{
			if (0 == lpImportNameArray[i].u1.AddressOfData)
			{
				break;
			}

			FARPROC lpFuncAddress = NULL;

			if (0x80000000 & lpImportNameArray[i].u1.Ordinal)
			{
				lpFuncAddress = (FARPROC)lpGetProcAddress(hDll, (LPSTR)(lpImportNameArray[i].u1.Ordinal & 0x0000FFFF));
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME lpImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pDos + lpImportNameArray[i].u1.AddressOfData);

				lpFuncAddress = (FARPROC)lpGetProcAddress(hDll, (LPSTR)lpImportByName->Name);
			}

			if (lpFuncAddress > 0)
			{
				lpImportFuncAddrArray[i].u1.Function = (DWORD)lpFuncAddress;
			}

			i++;
		}

		pImportTable++;
	}

	return TRUE;
}



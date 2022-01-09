
#include "MapPE.h"

#include "Public.h"



bool MapPE::MapFile(char* pFileBuff, char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFileBuff + pDos->e_lfanew);

	memcpy(chBaseAddress, pFileBuff, pNt->OptionalHeader.SizeOfHeaders);

	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	int nNumerOfSections = pNt->FileHeader.NumberOfSections;
	for (int i = 0; i < nNumerOfSections; i++, pSection++)
	{
		if ((0 == pSection->VirtualAddress) || (0 == pSection->SizeOfRawData))
		{
			continue;
		}

		char* chDestMem = (char*)((DWORD)chBaseAddress + pSection->VirtualAddress);
		char* chSrcMem = (char*)((DWORD)pFileBuff + pSection->PointerToRawData);

		memcpy(chDestMem, chSrcMem, pSection->SizeOfRawData);

		Public::writelog("map section:%s,size:%x,mem base:%x,file base:%x\r\n",
			pSection->Name, pSection->SizeOfRawData, pSection->VirtualAddress, pSection->PointerToRawData);
	}

	return TRUE;
}

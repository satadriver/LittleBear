
#include <windows.h>
#include "api.h"
#include "Debug.h"
#include "MapPE.h"
#include "escape.h"
#include "ImportFunTable.h"
#include "Public.h"
#include "Reloc.h"
#include "main.h"
#include "PEParser.h"

int			gType			= 0;
int			gPEImageSize	= 0;
HMODULE		ghPEModule		= 0;

DWORD		ghThisHandle	= 0;
HINSTANCE	ghprevInstance	= 0;
LPSTR		glpCmdLine		= 0;
int			gnShowCmd		= 0;

ptrmain				glpmain			= 0;
ptrDllMainEntry		glpDllMainEntry = 0;
ptrWinMain			glpWinMain		= 0;


int __stdcall DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved) {

	if (_Reason == DLL_PROCESS_ATTACH)
	{
		int ret = 0;

		ret = getapi();

		ghThisHandle = (DWORD)_DllHandle;

		ret = PEParser::loadPE();
	}

	return TRUE;
}
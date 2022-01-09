#pragma once

#include <windows.h>

extern int gType;
extern int gPEImageSize;
extern HMODULE ghPEModule;
extern DWORD ghThisHandle;


typedef int(__stdcall *ptrDllMainEntry)(DWORD, DWORD, DWORD);
typedef int(__stdcall *ptrWinMain)(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd);
typedef int(*ptrmain)(int argc, char ** argv);



extern HINSTANCE ghprevInstance;
extern LPSTR glpCmdLine;
extern int gnShowCmd;

extern ptrmain glpmain;
extern ptrDllMainEntry glpDllMainEntry;
extern ptrWinMain glpWinMain;

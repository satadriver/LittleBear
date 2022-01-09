#pragma once

#ifndef LNKPROC_H_H_H
#define LNKPROC_H_H_H

DWORD OpenRecycle();
DWORD HideRecycleOnDesktop();
DWORD HideComputerOnDesktop() ;
DWORD OpenMyComputer();

DWORD HideIEOnDesktop();

DWORD __stdcall LnkProc(char * szDstPePath,char * szSysDir,char * strUserName,char * strDataPath);
DWORD PretendOnDestTop(wchar_t * strCommand,wchar_t * strArguments,wchar_t * strDataPath,wchar_t * strIcon,wchar_t *strLnkName);

#endif
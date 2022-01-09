#pragma once
#ifndef GETANTIVIRUSSOFTINFO_H_H_H
#define GETANTIVIRUSSOFTINFO_H_H_H

int GainAdminPrivileges(char * strApp,UINT idd);
int SetRunAsAdmin();
int GetAntivirusSoftInfo();
BOOL IsAdministratorUser(HMODULE lpDllShell32);
int EnableDebugPrivilege(HANDLE hProc,BOOL bFlag);
int IsUACEnable(int iCpuBits,int * iEnableLUA,int * iConsentPromptBehaviorAdmin,int * iPromptOnSecureDesktop);
BOOL IsProcessAdmin();
DWORD GetProcessIdByName(char * szProcessName);
DWORD FindPidByName(char * lpProcessName);

int CloseProcessByName(char * procname);
int CloseProcByName(char * pname);
#endif

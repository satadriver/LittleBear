#pragma once

#include <windows.h>

#ifndef GETDLLKERNEL32BASE_H_H_H
#define GETDLLKERNEL32BASE_H_H_H

HMODULE GetDllKernel32Base();
int  PebNtGlobalFlagsApproach();
FARPROC getGetProcAddress(HMODULE hKernel32);

HANDLE  CheckInstanceExist(char * lpmutexname, BOOL * exist);

int InitDisplayParam(int * JPG_STREAM_SIZE, int * BMP_STREAM_SIZE, int * REMOTECONTROL_BUFFER_SIZE);

int getRunningPath();

int CheckAndCreateDataPath();

int InitWindowsSocket();
#endif

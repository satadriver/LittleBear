#pragma once
#ifndef LITTLEBEAR_H_H_H
#define LITTLEBEAR_H_H_H


void* DllTestLoadLibrary(char * lpdll);
int DllTestFreeLibrary(void * hm);
void* DllTestGetProcAddress(void* hm,char *szfunc);
int DllTestMsgBox(char * lptext,char * lpcap);
int DllTestMsgBox(int hwnd, char * lptext, char * lpcap, int mode);
void DllTestCreateThread();

extern "C" __declspec(dllexport) int __stdcall LittleBear();



#endif
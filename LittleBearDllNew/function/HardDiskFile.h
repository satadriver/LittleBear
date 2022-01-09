#pragma once
#ifndef DISKFILE_H_H_H
#define DISKFILE_H_H_H

int __stdcall DeleteAllFilesInDir(char * PreStrPath);
DWORD WINAPI DesignateVolumeWatcher(char * strFilePath);

int __stdcall GetHardDiskAllFiles();

int __stdcall FindFilesInDir(char * PreStrPath, int iLayer,HANDLE hfile);


#endif
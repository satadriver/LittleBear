
#include <windows.h>

char * GetCPUBrand(char * strCpuBrand);
int GetUserAndComputerName(char * strUserName,char * strComputerName);
int GetDiskVolumeSerialNo(DWORD * lpulVolumeSerialNo);
int GetParentProcName(char * szParentProcName);
int GetHostName(char *szHostName);
int GetDiskSpaceInfo(char * szBuf);
int GetCpuBits();
int GetWindowsVersion(char * strSysVersion);

int GetSystemDir(char * sysdir);


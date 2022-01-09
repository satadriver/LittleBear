#include "PublicVar.h"
#include <windows.h>


DWORD __SetRegistryKeyValueChar(HKEY hMainKey,char * szSubKey,char * szKeyName,char * szKeyValue,int iCpuBits);
DWORD __SetRegistryKeyValueDword(HKEY hMainKey,char * szSubKey,char * szKeyName,DWORD dwKeyValue,int iCpuBits);

int SetBootAutoRunInRegistryRun(HKEY hMainKey,char * strPEResidence,int iCpuBits);
DWORD QueryRegistryValue(HKEY hMainKey,char * szSubKey,char * szKeyName,unsigned char * szKeyValue,int *buflen,int iCpuBits);
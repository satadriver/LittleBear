
#pragma once
#ifndef GETINFORMATION_H_H_H
#define GETINFORMATION_H_H_H

void checkMacAddr(unsigned char *macaddr);
int GetInetIPAddress(char * szInetIP);
int GetNetCardInfo(char * strIP,unsigned char * cMac,char * strMac,char * szInetIP,char * strGateWayIP,char * strGateWayMac);
BOOL GetHardDiskSerialNo(int driver,char * szHDSerialNumber,char * szHDModelNumber);
char * ConvertToString(DWORD dwDiskData[256], int nFirstIndex, int nLastIndex);
BOOL  DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,PSENDCMDOUTPARAMS pSCOP,BYTE btIDCmd,BYTE btDriveNum,PDWORD pdwBytesReturned);
int __stdcall GetMachineInfo();

#endif
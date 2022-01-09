
#include "zipUtil.h"
#include <windows.h>



int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {



	HANDLE hf = CreateFileA("test.zip", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD filesize = GetFileSize(hf, 0);

	char * lpdata = new char[filesize];

	DWORD dwcnt = 0;
	int ret = ReadFile(hf, lpdata, filesize, &dwcnt, 0);
	CloseHandle(hf);

	DWORD outlen = filesize << 6;
	char * lpout = new char[outlen];

	ZipUtil ziputil;
	ret = ziputil.unzip((unsigned char*)lpout, &outlen, (unsigned char*)lpdata, filesize);


	HANDLE hfout = CreateFileA("out.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	ret = WriteFile(hfout, lpout, outlen, &dwcnt, 0);
	CloseHandle(hfout);
	return TRUE;
}
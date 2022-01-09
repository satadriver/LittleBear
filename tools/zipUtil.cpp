


#include <windows.h>
#include "zipUtil.h"
#include "zconf.h"
#include "zlib.h"
#pragma comment(lib,"zlib.lib")




int ZipUtil::unzip(unsigned char * out, unsigned long *lpoutsize, unsigned char * in, unsigned long insize) {
	int iRet = uncompress((Bytef*)out, lpoutsize, in, insize);
	if (iRet != Z_OK)
	{
		return FALSE;
	}
	return TRUE;
}



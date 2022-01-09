#pragma once
#include <windows.h>

class ZipUtil {
public:
	static int unzip(unsigned char * out, unsigned long *lpoutsize, unsigned char * in, unsigned long insize);
};

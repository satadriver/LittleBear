
#include "crypto.h"
#include "FileHelper.h"
#include "compress.h"
#include "PEParser.h"
#include "api.h"
#include <DbgHelp.h>

#pragma comment(lib,"dbghelp.lib")



void Crypto::CryptData(unsigned char * src, int size, unsigned char * psrckey, int keylen, unsigned char * dst, int dstsize) {

	unsigned char * pkey = new unsigned char[keylen];
	for (int i = 0; i < keylen; i++)
	{
		pkey[i] = (~psrckey[i]) ^ 0x2b;
	}

	for (int i = 0, j = 0; i < size;)
	{
		dst[i] = src[i] ^ pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}

	delete pkey;
}


void Crypto::revertkey(unsigned char * key) {
	for (int i = 0; i < CRYPT_KEY_SIZE; i++)
	{
		key[i] = ~key[i];
	}
}
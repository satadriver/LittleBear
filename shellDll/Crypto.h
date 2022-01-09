#pragma once

#include <windows.h>

#include <iostream>

using namespace std;

typedef unsigned int uint;

#define CRYPT_KEY_SIZE 16

class Crypto {
public:

	static void Crypto::CryptData(unsigned char * src, int size, unsigned char * pkey, int keylen, unsigned char * dst, int dstsize);

	static void Crypto::revertkey(unsigned char * key);


};

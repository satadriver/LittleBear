#pragma once

int Base64Encode(char * base64code, const unsigned char * src, int src_len);
int Base64Decode(char * buf, const char * base64code, int src_len);
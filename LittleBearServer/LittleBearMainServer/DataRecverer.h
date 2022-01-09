

#pragma once
#ifndef DATARECVPROC_H_H_H
#define DATARECVPROC_H_H_H

#include <windows.h>
#include "LittleBearMainServer.h"

class DataRecvers {
public:
	static int __stdcall DataRecverProc(LPNETWORKPROCPARAM lpParam);
	static int __stdcall DataRecvers::DataRecver(NETWORKPROCPARAM stParam, char ** lpBuf, int recvlen);

	static void myeException(struct _EXCEPTION_RECORD *ExceptionRecord,

		void * EstablisherFrame,

		struct _CONTEXT *ContextRecord,

		void * DispatcherContext);

	static int DataRecvers::DataRecverProcess(LPNETWORKPROCPARAM lpParam, LPNETWORKPACKETHEADER hdr,
		char * lpZlibBuf, int lZlibBufLen, char * szuser);
};



#endif
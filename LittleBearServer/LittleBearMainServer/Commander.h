
#include "LittleBearMainServer.h"
#include "Public.h"
#include <windows.h>
#include <iostream>

using namespace std;


class Commander {
public:
	static int SendCmdPacket(SOCKET s, int cmd);
	static int RecvCmdPacket(SOCKET s);
	static int SendCmdPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, char * lpdata, int datasize, int cmd);
	static int CommandWithoutParam(char * szCurrentDir, int cmd, char * cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam);
	static int CommandWithString(char * szCurrentDir, int cmd, char * cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam);
	static int CommandWith2String(char * szCurrentDir, int cmd, char * cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam);
	static string Commander::makeCmdFileName(char * szCurrentDir, char * cmdfn, LPUNIQUECLIENTSYMBOL lpUnique);
};

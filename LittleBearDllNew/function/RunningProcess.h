
#pragma pack(1)
typedef struct
{
	char pname[256];
	DWORD pid;
}RUNNINGPROCESS, *LPRUNNINGPROCESS;

#pragma pack()

int GetRunningProcessInfo();
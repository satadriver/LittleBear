#pragma once
#ifndef REMOTECONTROLPROC_H_H_H
#define REMOTECONTROLPROC_H_H_H

#include <windows.h>
#include "LittleBearMainServer.h"
#include "Public.h"




// #define CLIENT_CONNECT_ERROR	WM_USER + 0x100
// #define WM_USER_KEY_HOOK		WM_USER + 0x200;
// #define WM_USER_MOUSE_HOOK	WM_USER + 0x300;
// #define REMOTE_HEARTBEAT_INTERVAL	3000
// #define DOUBLECLICK_MAX_INTERVAL		500




class RemoteControler {
public:
	static LRESULT CALLBACK RemoteControlWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static BOOL __stdcall RemoteControlWindow(LPREMOTE_CONTROL_PARAM lpparam);

	static int RemoteControl(REMOTE_CONTROL_PARAM stParam, char **lpRecvBuf, int BufSize);
	static int __stdcall RemoteControlProc(LPREMOTE_CONTROL_PARAM lpParam);
};






#endif
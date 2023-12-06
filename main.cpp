#include "main.h"
#include <vector>
#include <windows.h>
#include <iostream>

BYTE setToggleableKeys = (1<<5) | (1<<4) | (1<<3) | (1<<2) | (0<<1) | (0<<0);//numlock, capslock, scrolllock, first whether to change state, second whether to change state to

void toggleTopmost() {
	HWND window = GetForegroundWindow();
	if (window == NULL)
		return;
	if (GetWindowLong(window, GWL_EXSTYLE) & WS_EX_TOPMOST) 
		SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
LRESULT CALLBACK keyboardHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	static bool pauseKeyHook = false;
	if (nCode != HC_ACTION)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	bool eatKey = false;
	static bool releaseAltOnCapslockRelease = false;
	if (pauseKeyHook) {
		if (((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) && (p->vkCode == VK_ESCAPE))
			goto VK_ESCAPE_PRESSED_LABEL;
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
	switch (wParam) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch(p->vkCode) {
		case 220://backslash
			if (GetKeyState(VK_LCONTROL) && GetKeyState(VK_LSHIFT) && GetKeyState(VK_LMENU) && GetKeyState(VK_LWIN)) {
				eatKey = true;
				toggleTopmost();
			}
			break;//break from vkCode switch
		case VK_ESCAPE:
			VK_ESCAPE_PRESSED_LABEL:
			if (GetKeyState(VK_LCONTROL) && GetKeyState(VK_LSHIFT) && GetKeyState(VK_LMENU)) {
				eatKey = true;
				if (GetKeyState(VK_LWIN))
					PostQuitMessage(0);
				else
					pauseKeyHook = !pauseKeyHook;
			}
			break;//break from vkCode switch
		case VK_NUMLOCK:
			if ((setToggleableKeys & 32) && (bool(GetKeyState(VK_NUMLOCK) & 1) == bool(setToggleableKeys & 4))) {//if should change state and state is wrong (actually matching b/c reasons?) then send key
				keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
			break;//break from vkCode switch
		case VK_CAPITAL:
			if ((setToggleableKeys & 16) && (bool(GetKeyState(VK_CAPITAL) & 1) == bool(setToggleableKeys & 2))) {//if should change state and state is wrong (actually matching b/c reasons?) then send key
				keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
			/*if (blender) {//if blender active window rebind to alt
				eatKey = true;
				releaseAltOnCapslockRelease = true;
				send alt down
			}*/
			break;//break from vkCode switch
		case VK_SCROLL:
			if ((setToggleableKeys & 8) && (bool(GetKeyState(VK_SCROLL) & 1) == bool(setToggleableKeys & 1))) {//if should change state and state is wrong (actually matching b/c reasons?) then send key
				keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
			break;//break from vkCode switch
		}
		break;//break from wParam switch
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (p->vkCode == VK_CAPITAL) {
			if (releaseAltOnCapslockRelease) {
				releaseAltOnCapslockRelease = false;
				//send alt up
			}
		}
		break;
	}
	return eatKey ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
}
#include <thread>
BOOL WINAPI consoleHandler(DWORD signal) {
	static bool alreadyDone = false;
	if (!alreadyDone && (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT || signal == CTRL_LOGOFF_EVENT || signal == CTRL_SHUTDOWN_EVENT)) {
		alreadyDone = true;
		bool topmostFound = false;
		HWND window;
		goThroughWindowLoop:
		window = GetTopWindow(GetDesktopWindow());
		do {
			if (GetWindowLong(window, GWL_EXSTYLE) & WS_EX_TOPMOST) {
				SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				topmostFound = true;
			}
			else
				break;
		} while ((window = GetWindow(window, GW_HWNDNEXT)));
		if (topmostFound) {
			topmostFound = false;
			goto goThroughWindowLoop;
		}
	}
	return TRUE;
}
int main(int argc, char* argv[]) {
	SetConsoleCtrlHandler(consoleHandler, TRUE);
	CreateEvent(NULL, FALSE, FALSE, L"Local\\mousewheel_yay_single_instance_event_09fc877b5d5d4835abf85f657e635fc032b4238158e641e7807701fb74c0a1a2245b4f28b9fc420ca5b92db856a30d8ddb7fd431865847a7acbcd66418ed9819d4579055c06a4d04b6b0dc12f6174991");//let the OS nuke it, it's not like this is being called multiple times
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;
	SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHandler, 0, 0);//let the OS nuke it, it's not like this is being called multiple times
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL) > 0) {//use PostQuitMessage to exit loop
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	consoleHandler(CTRL_CLOSE_EVENT);
}
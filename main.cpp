/*todo
mouse hook
commands
	cmda
	setNumLockState 0/1/2 | n/t/y
	setCapsLockState 0/1/2 | n/t/y
	setScrollLockState 0/1/2 | n/t/y
*/

#include <vector>
#include <windows.h>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <thread>

BYTE setToggleableKeys = (1<<5) | (1<<4) | (1<<3) | (1<<2) | (0<<1) | (0<<0);//numlock, capslock, scrolllock, first whether to change state, second what to change state to, have option in menu to change these (get rid of option to pause keyboard hook?)
std::wstring focusedWindowName = L"";

void interpret(std::string& buffer) {//spawn new threads here, can't keep keyboard hook paused for too long



	buffer.clear();
}
char getCharFromKey(DWORD key) {
	switch(key) {
	
	default:
		return 0;
	}
}
void commandMeDaddy(DWORD key) {
	return;

	std::cout << getCharFromKey(key) << std::endl;
	return;

	static bool listening = false;
	static std::string buffer;
	if (focusedWindowName == L"Gw2-64.exe" || focusedWindowName == L"OxygenNotIncluded.exe" /* || focusedWindowName == L"mousewheel yay.exe" */) {//don't want to forget about a half typed command
		listening = false;
		buffer.clear();
		return;
	}
	if (GetKeyState(VK_CONTROL) || GetKeyState(VK_MENU) || GetKeyState(VK_LWIN) || GetKeyState(VK_RWIN)) return;
	if (key == VK_F1)
		if(!(listening = !listening))
			interpret(buffer);
	if (!listening) return;
	switch (key) {
	case VK_ESCAPE://look at python script regarding index and whatnot
		listening = false;
		buffer.clear();
		break;
	case VK_BACK:
		if (buffer.size() > 0)
			buffer.pop_back();
		break;
	case VK_RETURN:
		interpret(buffer);
		listening = false;
		break;
	default:
		if (char c = getCharFromKey(key))
			buffer.push_back(c);
		break;
	}
}
LRESULT __stdcall keyboardHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode != HC_ACTION)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	static bool releaseAltOnCapslockRelease = false;
	switch (wParam) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (p->vkCode) {
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
			if (focusedWindowName == L"blender.exe") {
				releaseAltOnCapslockRelease = true;
				keybd_event(VK_LMENU, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
				return 1;
			}
			break;//break from vkCode switch
		case VK_SCROLL:
			if ((setToggleableKeys & 8) && (bool(GetKeyState(VK_SCROLL) & 1) == bool(setToggleableKeys & 1))) {//if should change state and state is wrong (actually matching b/c reasons?) then send key
				keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
			break;//break from vkCode switch
		case VK_ESCAPE:
			if (GetKeyState(VK_LCONTROL) && GetKeyState(VK_LSHIFT) && GetKeyState(VK_LMENU) && GetKeyState(VK_LWIN)) {
				PostQuitMessage(0);
				return 1;
			}
			commandMeDaddy(VK_ESCAPE);
			break;//break from vkCode switch
		case 0x43://c key
			if (focusedWindowName == L"Warframe.x64.exe" && GetKeyState(VK_LMENU)) {
				std::thread([] {
					for (int i = 0; i < 14 && focusedWindowName == L"Warframe.x64.exe"; ++i) {
						keybd_event(VK_LCONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
						std::this_thread::sleep_for(std::chrono::milliseconds(25));
						keybd_event(VK_LCONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
						std::this_thread::sleep_for(std::chrono::milliseconds(225));
					}
					}).detach();//thread handle may need to be moved to vector to be joined before program exit, for now assume thread will finish before program exit
					return 1;
					break;
			}
			commandMeDaddy(0x43);
			break;
		case 0x45://e key
			if (focusedWindowName == L"Warframe.x64.exe") {
				std::thread([] {
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					while (focusedWindowName == L"Warframe.x64.exe") {
						keybd_event(0x45, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
						std::this_thread::sleep_for(std::chrono::milliseconds(5));
						keybd_event(0x45, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
						std::this_thread::sleep_for(std::chrono::milliseconds(45));
					}
					}).detach();//thread handle may need to be moved to vector to be joined before program exit, for now assume thread will finish before program exit
			}
			commandMeDaddy(0x45);
			break;//break from vkCode switch
		case 220://backslash
			if (GetKeyState(VK_LCONTROL) && GetKeyState(VK_LSHIFT) && GetKeyState(VK_LMENU) && GetKeyState(VK_LWIN)) {
				HWND window = GetForegroundWindow();
				if (window == NULL)
					break;
				if (GetWindowLong(window, GWL_EXSTYLE) & WS_EX_TOPMOST)
					SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				else
					SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				return 1;
			}
			commandMeDaddy(220);
			break;//break from vkCode switch
		default:
			commandMeDaddy(p->vkCode);//everything else should be related to keylogger commands, move it to that function because this one is getting cluttered
			break;//break from vkCode switch
		}
			
		break;//break from wParam switch
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (p->vkCode == VK_CAPITAL && releaseAltOnCapslockRelease) {
				releaseAltOnCapslockRelease = false;
				keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
		break;//break from wParam switch
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}



LRESULT __stdcall mouseHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (wParam == WM_XBUTTONDOWN) {
		std::cout << "xbutton\n";

		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, ((p->mouseData == 65536) ? 1 : -1) * 500, 0);

		return 1;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}




LRESULT __stdcall mouseHandleraaaaaaaaaaaaaaaaaa(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	static bool scroll = false, x1Queued = false;
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (wParam == WM_XBUTTONDOWN) {
		if (p->mouseData == 65536) {//xbutton1
			x1Queued = true;
			std::cout << "x1Queued\n";
			return 1;
		}
		else {
			if (x1Queued) {
				scroll = !scroll;
				x1Queued = false;
				std::cout << "scroll: " << scroll << "\n";
				return 1;
			}
			if (scroll) {
//				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120, 0);
				wParam = WM_MOUSEHWHEEL;
				p->mouseData = -500;
				std::cout << "scroll down\n";
			}
			std::cout << "x2 pressed\n";
		}
	}
	else if (wParam == WM_RBUTTONUP && p->mouseData == 65536) {
		if (!x1Queued) {
			std::cout << "x1 released but canceled\n";
			return 1;
		}
		if (scroll) {
			std::cout << "scroll up\n";
//			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 120, 0);
			wParam = WM_MOUSEHWHEEL;
			p->mouseData = 500;
		}
		else {
			mouse_event(MOUSEEVENTF_XDOWN, 0, 0, 65536, 0);
			std::cout << "x1 pressed\n";
		}
		std::cout << "x1 released\n";
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/*LRESULT CALLBACK mouseHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	static bool scroll = false, x1Queued = false;
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (wParam == WM_XBUTTONDOWN) {
		if (p->mouseData == 65536) {//xbutton1
			x1Queued = true;
			std::cout << "x1Queued\n";
			return 1;
		}
		else {
			if (x1Queued) {
				scroll = !scroll;
				x1Queued = false;
				std::cout << "scroll: " << scroll << "\n";
				return 1;
			}
			if (scroll) {
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120, 0);
				std::cout << "scroll down\n";
				return 1;
			}
			std::cout << "x2 pressed\n";
		}
	}
	else if (wParam == WM_RBUTTONUP && p->mouseData == 65536) {
		if (!x1Queued) {
			std::cout << "x1 released but canceled\n";
			return 1;
		}
		if (scroll) {
			std::cout << "scroll up\n";
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 120, 0);
			return 1;
		}
		else {
			mouse_event(MOUSEEVENTF_XDOWN, 0, 0, 65536, 0);
			std::cout << "x1 pressed\n";
		}
		std::cout << "x1 released\n";
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}*/

/*LRESULT CALLBACK mouseHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	static bool scroll = false;
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (wParam == WM_XBUTTONDOWN) {
		if (p->mouseData == 65536) {//xbutton1
			GetAsyncKeyState(VK_XBUTTON2);
			std::thread([] {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				if (GetAsyncKeyState(VK_XBUTTON2)) {
					scroll = !scroll;
					std::cout << "scroll: " << scroll << "\n";
				}
				else {
					if (scroll) {
						mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 120, 0);
				//		std::cout << "scroll down\n";
					}
					else {
						mouse_event(MOUSEEVENTF_XDOWN, 0, 0, 65536, 0);
					//	std::cout << "x1 down\n";
					}
				}
			}).detach();
			return 1;
		}
		else {
			if (GetKeyState(VK_XBUTTON1))
				return 1;
			if (scroll) {
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120, 0);
			//	std::cout << "scroll up\n";
				return 1;
			}
		//	std::cout << "x2 down\n";
		}
	}*/
	/*else if (wParam == WM_XBUTTONUP) {
		if (p->mouseData == 65536) {//xbutton1
			std::cout << "x1 up\n";
		}
		else {
			std::cout << "x2 up\n";
		}
	}*/
//	return CallNextHookEx(NULL, nCode, wParam, lParam);
//}
/*LRESULT CALLBACK mouseHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	static bool scroll = false;
	static char x1DownQueued = 0, x1UpQueued = 0;
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (wParam == WM_XBUTTONDOWN) {
		if (p->mouseData == 65536) {//xbutton1
			++x1DownQueued;
			std::thread([] {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));//TODO tune this
				if (!x1DownQueued)
					return;
				--x1DownQueued;//so long as the time between comparison and decrement is less than amount of time between clicks this should be fine
				std::cout << "x1 down\n";
				if (scroll)
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 120, 0);
				else
					mouse_event(MOUSEEVENTF_XDOWN, 0, 0, 65536, 0);//press xbutton1 down
				if (x1UpQueued) {
					--x1UpQueued;
					std::cout << "x1 up\n";
					mouse_event(MOUSEEVENTF_XUP, 0, 0, 65536, 0);//release xbutton1
				}
			}).detach();
			return 1;
		}
		else {
			if (x1DownQueued) {
				--x1DownQueued;
				scroll = !scroll;
				std::cout << "swap\n";
				return 1;
			}
			if (scroll) {
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120, 0);
				return 1;
			}
			std::cout << "x2 down\n";
		}
	}
	else if (wParam == WM_XBUTTONUP) {
		if (p->mouseData == 65536) {//xbutton1
			if (x1DownQueued) {
				++x1UpQueued;
				return 1;
			}
			std::cout << "x1 up\n";
		}
		else {
			std::cout << "x2 up\n";
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}*/
/*LRESULT CALLBACK mouseHandler(int nCode, WPARAM wParam, LPARAM lParam) {
//	bool eatKey = false;
	static bool scroll = false;
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	if (wParam == WM_XBUTTONDOWN) {
		if (p->mouseData == 65536) {//xbutton1
			if (GetKeyState(VK_XBUTTON2)) {
				scroll = !scroll;
				return 1;
			}
			if (scroll) {
//				eatKey = true;
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 120, 0);
				return 1;
			}
		}
		else {
			if (GetKeyState(VK_XBUTTON1)) {
				scroll = !scroll;
				return 1;
			}
			if (scroll) {
//				eatKey = true;
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120, 0);
				return 1;
			}
		}
	}
	else if (wParam == WM_XBUTTONUP) {
		if (p->mouseData == 65536) {//xbutton1
			
		}
		else {

		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
	//return eatKey ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
}*/
void __stdcall foregroundWindowHandler(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND foregroundWindowHWND, long idObject, long idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	DWORD processID;
	GetWindowThreadProcessId(foregroundWindowHWND, &processID);
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameEx(process, NULL, buffer, MAX_PATH);
	CloseHandle(process);
	focusedWindowName = std::wstring(buffer);
	focusedWindowName = focusedWindowName.substr(focusedWindowName.find_last_of(L"\\") + 1);
}
int main(int argc, char* argv[]) {
	CreateEvent(NULL, FALSE, FALSE, L"Local\\mousewheel_yay_single_instance_event_09fc877b5d5d4835abf85f657e635fc032b4238158e641e7807701fb74c0a1a2245b4f28b9fc420ca5b92db856a30d8ddb7fd431865847a7acbcd66418ed9819d4579055c06a4d04b6b0dc12f6174991");//let the OS nuke it, it's not like this is being called multiple times
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;
	SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHandler, 0, 0);//let the OS nuke it, it's not like this is being called multiple times
	SetWindowsHookEx(WH_MOUSE_LL, mouseHandler, 0, 0);
	SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, foregroundWindowHandler, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL) > 0) {//use PostQuitMessage to exit loop
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
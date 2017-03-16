#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SPYMSGWND_H
#define HS_HEADER_DEF_SPYMSGWND_H
//////////////////////////////////////////////////////////////////////////

#define SPY_HOOK_SIGNAL			TEXT("Hook.Signal")
#define SPY_BASE_SIGNAL			TEXT("Base.Signal")

//////////////////////////////////////////////////////////////////////////

#define CreateSpyMsgWnd(_hInstance) \
CreateWindow(STR_SPYMSGWND_CLASS, NULL, \
WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE, NULL, _hInstance, NULL);

//////////////////////////////////////////////////////////////////////////

#define ID_CMD_SPYTARGET		1
#define ID_TRAYICON_MAIN		1
#define ID_HOTKEY_SPYIT			1000

#define WM_MY_PRINT_SPYWNDINFO	(WM_USER_FIRST_WND_BKMSGWND + 1)
#define WM_MY_NOTIFYICON		(WM_USER_FIRST_WND_BKMSGWND + 2)
#define WM_MY_COLORSET			(WM_USER_FIRST_WND_BKMSGWND + 10)



#define SPY_CHECK_WND_TIMER_INTERVAL		3000
//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc_MsgBackWnd(HWND hwnd, 
									UINT message, 
									WPARAM wParam, 
									LPARAM lParam);


//////////////////////////////////////////////////////////////////////////
#define TIMER_SHOW_LOADING		100
#define TIMER_PROGRESS_EFFECT	1001
#define WM_MY_LOADING_STATE			(WM_USER_FIRST_WND_BKMSGWND + 50)

//////////////////////////////////////////////////////////////////////////

#define WndSpy_SpyTargetWnd(_hwndMsg, _hwndTag) \
PostMessage(_hwndMsg, WM_COMMAND, MAKEWPARAM(ID_CMD_SPYTARGET, 0), (LPARAM)_hwndTag)

///////////////////////////////////////////////////////////////////////////

//MACRO OpenLoadingBar()
#define LoadingBar_Open() \
SetTimer(g_hwndBK, TIMER_SHOW_LOADING, USER_TIMER_MINIMUM, NULL); \
Sleep(100)
//End OpenLoadingBar()

//MACRO LoadingBar_GetProgressBar()
#define LoadingBar_GetProgressBar() ((HWND)g_hTarget)
//End LoadingBar_GetProgressBar()

//MACRO LoadingBar_AutoStep()
#define LoadingBar_AutoStep() \
SendMessage(g_hwnd_TaskModal, WM_MY_LOADING_STATE, 0, TRUE)
//End LoadingBar_AutoStep()

//MACRO LoadingBar_Close()
#define LoadingBar_Close() \
SendMessage(g_hwnd_TaskModal, WM_MY_LOADING_STATE, 0, FALSE)
//End LoadingBar_Close()

BOOL CALLBACK DlgProc_Loading(HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_SPYMSGWND_H
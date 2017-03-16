#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_TASKBARFUNC
#define HS_INC_HEADER_TASKBARFUNC

INT CheckWndSpecClassAndDoRefresh(HWND hwnd);
BOOL CALLBACK RefreashWndEnumProc(HWND hwnd, LPARAM lParam);
void DoRefreshIfWndIsTaskBar(HWND hwnd);

#endif //HS_INC_HEADER_TASKBARFUNC
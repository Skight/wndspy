#ifndef HS_HEADER_DEF_SPYTOOLCONTAINNER_H
#define HS_HEADER_DEF_SPYTOOLCONTAINNER_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <windows.h>

//////////////////////////////////////////////////////////////////////////

#define WM_MY_TOOL_CLOSE  (WM_USER_FIRST_TOOLCONTAINER+1)

#ifdef _DEBUG
#define MAX_WAITLOOP	500
#else
#define MAX_WAITLOOP	15
#endif

//////////////////////////////////////////////////////////////////////////

HWND GetToolContainerWindow(VOID);

BOOL IsToolWindowExists(VOID);

LRESULT CALLBACK WndProc_ToolContainer(HWND hwnd, 
									   UINT message, 
									   WPARAM wParam, 
									   LPARAM lParam);

BOOL DoWaitForAWhile(BOOL* lpBoolSignal, INT iMaxLoopTime);
//////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_SPYTOOLCONTAINNER_H

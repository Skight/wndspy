#ifndef HS_HEADER_DEF_DLGWNDMODIFIER_H
#define HS_HEADER_DEF_DLGWNDMODIFIER_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <windows.h>

//////////////////////////////////////////////////////////////////////////

#define TIMER_MOUSEMOVE		1
#define TIMER_CHECK_EXIST	2

BOOL CALLBACK DlgProc_WndModifier(HWND hwnd, 
							   UINT message, 
							   WPARAM wParam, 
							   LPARAM lParam);

BOOL IsValidWindowTarget(HWND hwndTag);
//////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_DLGWNDMODIFIER_H
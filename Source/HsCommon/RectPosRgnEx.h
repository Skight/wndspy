#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_RECTPOSRGNEX
#define HS_INC_HEADER_RECTPOSRGNEX

//////////////////////////////////////////////////////////////////////////
VOID ScreenToWindowRect(HWND hwnd, RECT *pRect);
VOID WindowToScreenRect(HWND hwnd, RECT *pRect);
//////////////////////////////////////////////////////////////////////////

BOOL IsWndOverWnd(HWND hwnd1, HWND hwnd2);
BOOL IsWndInWnd(HWND hwnd1, HWND hwnd2);
BOOL IsPtInWnd(POINT* pPt, HWND hwnd);
BOOL IsWndRectEmpty(HWND hwnd);

//////////////////////////////////////////////////////////////////////////

HWND FindNextRealWindowFromPoint(HWND hwndStart, POINT pt);

//////////////////////////////////////////////////////////////////////////

#define IsRealChildWindow(_hwnd)  \
(GetParent(_hwnd) && !GetWindow(_hwnd, GW_OWNER) && \
WS_CAPTION!=(GetWindowLong(_hwnd, GWL_STYLE)&WS_CAPTION))

HWND DescendantWindowFromPoint(HWND hwndChild, POINT pt, BOOL IsDeepFind);

////////////////////////////////////////////////////////////////////////////
// FindRealWindowFromPointEx();
// hwndSkip ->handle to a Top Window to skip... 
// pt -> In Screen coordinates...
// dwFlag -> Combo switches:
// #define FW_IS_DEEPFIND		LOWORD(dwFlag)	
// #define FW_IS_NOINVISIBLE	HIWORD(dwFlag)
//////////////////////////////////////////////////////////////////////////
HWND FindRealWindowFromPointEx(HWND hwndSkip, POINT pt, DWORD dwFlag);

//////////////////////////////////////////////////////////////////////////

LPRECT AdjustRectInScreen(LPRECT pRect);
void MoveWndToWndCenter(HWND hwndMoving, HWND hwndDest);

//////////////////////////////////////////////////////////////////////////

#endif //HS_INC_HEADER_RECTPOSRGNEX

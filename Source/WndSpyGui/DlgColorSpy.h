#ifndef HS_HEADER_DEF_DLGCOLORSPY_H
#define HS_HEADER_DEF_DLGCOLORSPY_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <windows.h>
//////////////////////////////////////////////////////////////////////////

#define WM_MY_GETCOLOR			(WM_USER_FIRST_DLG_COLORSPY+1)
#define WM_MY_UPDATECOLORINFO	(WM_USER_FIRST_DLG_COLORSPY+2)

//////////////////////////////////////////////////////////////////////////

VOID OnDrawColorBtn(LPARAM lParam, COLORREF clr);

BOOL CALLBACK DlgProc_ColorSpy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_DLGCOLORSPY_H
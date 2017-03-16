#ifndef HS_HEADER_DEF_DLGABOUT_H
#define HS_HEADER_DEF_DLGABOUT_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <windows.h>
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_AboutWS(HWND hwnd, 
							   UINT message, 
							   WPARAM wParam, 
							   LPARAM lParam);

//////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_DLGABOUT_H
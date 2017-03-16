#ifndef HS_HEADER_DEF_DLGPSWSNIFFER_H
#define HS_HEADER_DEF_DLGPSWSNIFFER_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <windows.h>
//////////////////////////////////////////////////////////////////////////

DWORD_PTR ListView_FN_GetItemData(HWND hwndLV, INT iIndex);

INT ListView_FN_GetItemText(HWND hwndLV, INT iItem, INT iSubItem, LPTSTR szTextBuf, INT iMaxBufLen);

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_PswSniffer(HWND hwnd, 
							   UINT message, 
							   WPARAM wParam, 
							   LPARAM lParam);

//////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_DLGPSWSNIFFER_H
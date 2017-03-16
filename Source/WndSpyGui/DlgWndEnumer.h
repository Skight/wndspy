#ifndef HS_HEADER_DEF_DLGWNDENUMER_H
#define HS_HEADER_DEF_DLGWNDENUMER_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <windows.h>
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_WndEnumer(HWND hwnd, 
							   UINT message, 
							   WPARAM wParam, 
							   LPARAM lParam);

VOID SendCtrlNotifyMessage(HWND hwndParent, HWND hwndFrom, 
						   UINT idFrom, UINT NotifyCode, LPARAM lParam);

HTREEITEM TreeView_FindString(HWND hwndTV, HTREEITEM hTItem, 
							  LPTSTR szStrFind, BOOL IsMCase, TVITEM* pTvi);

//////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_DLGWNDENUMER_H
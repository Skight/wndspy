#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_MANTAB_3_H
#define HS_HEADER_DEF_MANTAB_3_H
//////////////////////////////////////////////////////////////////////////


BOOL CALLBACK TabDlgProc_Class (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);

LPTSTR OnGetClassHCursorSymbol(LPTSTR szTextBuf);
LPTSTR OnGetClassHBrushSymbol(LPTSTR szTextBuf);
void OnFilterClassStyle(HWND hwndComBox, DWORD dwCS);

//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_MANTAB_3_H
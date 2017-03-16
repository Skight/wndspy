#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_MANTAB_2_H
#define HS_HEADER_DEF_MANTAB_2_H
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK TabDlgProc_Styles (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);

void OnInitAliasList(HWND hwndComBox);

LONG GetKnownClassID(LPTSTR szClassName);

LONG SetSelKnownClassIndex(HWND hwndDlg, INT iComboBoxId, LPTSTR szClassName);

LRESULT DoFilterSymbol(SYMBOL_FILTER_PARAM* sfParam);

LRESULT DoFilterSpecCtrlExStyle(SYMBOL_FILTER_PARAM* pSFP);

void OnFilterWndStyle(HWND hwndCombo, 
					  HWND hwndlist, HWND hwndlistEx,
					  DWORD dwVal, DWORD dwValEx);

//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_MANTAB_2_H

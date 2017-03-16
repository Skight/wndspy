#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_STRFUNCEX
#define HS_INC_HEADER_STRFUNCEX

//////////////////////////////////////////////////////////////////////////

INT GetWindowInteger(HWND hwnd, BOOL bIsHex);

#define GetWindowHex(_hwnd)		\
GetWindowInteger(_hwnd, TRUE)

#define GetDlgItemHex(_hDlg, _idCtrl) \
GetWindowHex(GetDlgItem(_hDlg, _idCtrl))

#define GetWindowInt(_hwnd)		\
GetWindowInteger(_hwnd, FALSE)

#define GetWindowDec(_hwnd)	 GetWindowInt(_hwnd)

BOOL IsWndTextIntStr(HWND hwnd, BOOL bIsHex);

#define IsWndTextHexStr(_hwnd) IsWndTextIntStr(_hwnd, TRUE)

#define IsWndTextDecStr(_hwnd) IsWndTextIntStr(_hwnd, FALSE)


INT CopyWndTextToWnd(HWND hwndFrom, HWND hwndTo, INT iMaxLen, 
					 LPTSTR szSuffix, HINSTANCE hInstance, UINT uStrResID);
////////////////////////////////////////////////////////////////////////

INT GetPasswordEditText(HWND hwndPWEdit, LPTSTR szPassword);
//////////////////////////////////////////////////////////////////////////

INT GetAllTextsFromListWnd(LPTSTR lpsTextBuf, INT iMaxBufLen, 
							HWND hwndList, UINT uMsgGetCount, UINT uMsgGetText, UINT uMsgGetTextLen);

INT GetAllTextsFromStatusBar(HWND hwndSB, LPTSTR lpszTextBuf);

INT GetAllTextsFromHeaderCtrl(HANDLE hProcess, HWND hwndHeader, LPTSTR szPrefix, LPTSTR lpszTextBuf, BOOL isGrouping);
INT GetAllTextsFromListViewCtrl(HWND hwndLV, LPTSTR lpszTextBuf, 
								HWND hwndMsgOnwer, HWND hwndProgress, BOOL isGrouping);

INT GetAllTextsFromTabCtrl(HWND hwndTab, LPTSTR lpszTextBuf);

INT GetAllTextsFromTreeView(HWND hwndTV, LPTSTR lpszTextBuf, HWND hwndProgress);
//////////////////////////////////////////////////////////////////////////

HFONT DoCreateDefaultFont(LPTSTR szFontName);

//////////////////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_STRFUNCEX